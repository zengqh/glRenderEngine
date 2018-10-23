/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/03
* File: asset_importer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "enn.h"

using namespace enn;

float defaultTicksPerSecond_ = 24.0f;

Root* root = 0;
const aiScene* scene_ = 0;
aiNode* rootNode_ = 0;
enn::set<aiAnimation*>::type allAnimations_;
enn::vector<aiAnimation*>::type sceneAnimations_;

struct OutModel
{
	OutModel() :
	root_bone_(0),
	total_vertices_(0),
	total_indices_(0)
	{

	}

	String											out_name_;
	aiNode*											root_node_;
	enn::set<uint32>::type							mesh_indices_;
	enn::vector<aiMesh*>::type						meshes_;
	enn::vector<aiMaterial*>::type					mtrs_;
	enn::vector<aiNode*>::type						mesh_nodes_;
	enn::vector<aiNode*>::type						bones_;
	enn::vector<aiAnimation*>::type					animations_;
	enn::vector<float>::type						bone_rad_ii_;
	enn::vector<AxisAlignedBox>::type				bone_hit_boxes_;
	aiNode*											root_bone_;
	uint32											total_vertices_;
	uint32											total_indices_;
	enn::map<String, Animation*>::type				mesh_anims_;
};

void create_root();

void destroy_root();

void export_model(const String& file_name);

void collect_meshes(OutModel& model, aiNode* node);

void collect_mtrs(OutModel& model);

unsigned GetNumValidFaces(aiMesh* mesh);

void collect_bones(OutModel& model);

aiNode* GetNode(const String& name, aiNode* rootNode, bool caseSensitive = true);

void collect_bones_final(enn::vector<aiNode*>::type& dest, const enn::set<aiNode*>::type& necessary, aiNode* node);

void build_bone_collision_info(OutModel& model);

unsigned GetBoneIndex(OutModel& model, const String& boneName);

Vector3 ToVector3(const aiVector3D& vec);

Vector2 ToVector2(const aiVector2D& vec);

Quaternion ToQuaternion(const aiQuaternion& quat);

void collect_animations(OutModel* model);

void build_and_save_model(OutModel& model);

void build_material(SubMaterial* sub_mtr, aiMaterial* mtr);

void build_animations(OutModel* model);

unsigned GetElementMask(aiMesh* mesh);

void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale);

aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* modelRootNode);

aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode);

aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* node, aiNode* rootNode);

void WriteShortIndices(unsigned short*& dest, aiMesh* mesh, unsigned index, unsigned offset);

void WriteVertex(float*& dest, aiMesh* mesh, unsigned index, unsigned elementMask, AxisAlignedBox& box,
				 const Matrix4& vertexTransform, const Matrix3& normalTransform,
				 enn::vector<enn::vector<uchar>::type>::type blend_indices,
				 enn::vector<enn::vector<float>::type>::type blend_weight);

String SanitateAssetName(const String& name);

void GetBlendData(OutModel& model, aiMesh* mesh, enn::vector<uint32>::type& bone_mappings,
				  enn::vector<enn::vector<uchar>::type>::type& blend_indices,
				  enn::vector<enn::vector<float>::type>::type& blend_weights);

enn::Matrix4 GetOffsetMatrix(OutModel& model, const String& boneName);

enn::Matrix4 ToMatrix4(const aiMatrix4x4& mat);

int main()
{
	create_root();

	uint32 flags = 
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_LimitBoneWeights |
		aiProcess_ImproveCacheLocality |
		aiProcess_FixInfacingNormals |
		aiProcess_FindInstances |
		aiProcess_FlipUVs;

	const String& root_path = ROOTPTR->getRootCfg().file_system_path;
	String model_name = root_path + String("/mesh/") + String("sponza_obj/sponza.obj");

	scene_ = aiImportFile(model_name.c_str(), flags);
	if (!scene_)
	{
		ENN_ASSERT(0);
		return 0;
	}

	String rootNodeName;
	rootNode_ = scene_->mRootNode;

	
	export_model("");

	destroy_root();
}

void create_root()
{
	/** init root */
	RootConfig root_config;
	root_config.window_handle = (void*)0;
	root_config.window_width = 0;
	root_config.window_height = 0;
	root_config.file_system_path = "F:/GE/enn/Media";

	root = ENN_NEW Root;
	root->init(root_config);
	root->start_console();
}

void destroy_root()
{
	if (!root)
	{
		return;
	}

	root->uninit();

	ENN_SAFE_DELETE(root);
}

void export_model(const String& file_name)
{
	OutModel model;

	model.root_node_ = rootNode_;
	model.out_name_ = file_name;

	collect_meshes(model, model.root_node_);

	collect_bones(model);

	collect_animations(&model);

	build_animations(&model);

	collect_mtrs(model);

	build_and_save_model(model);
}

void collect_meshes(OutModel& model, aiNode* node)
{
	for (uint32 i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene_->mMeshes[node->mMeshes[i]];

		for (uint32 j = 0; j < model.meshes_.size(); ++j)
		{
			if (mesh == model.meshes_[j])
			{
				ENN_ASSERT(0);
			}
		}
		
		model.mesh_indices_.insert(node->mMeshes[i]);
		model.meshes_.push_back(mesh);
		model.mesh_nodes_.push_back(node);
		model.total_vertices_ += mesh->mNumVertices;
		model.total_indices_ += GetNumValidFaces(mesh) * 3;
	}

	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		collect_meshes(model, node->mChildren[i]);
	}
}

void collect_mtrs(OutModel& model)
{
	for (unsigned i = 0; i < scene_->mNumMaterials; ++i)
	{
		aiMaterial* mtr = scene_->mMaterials[i];
		model.mtrs_.push_back(mtr);
	}
}

unsigned GetNumValidFaces(aiMesh* mesh)
{
	unsigned ret = 0;

	for (unsigned j = 0; j < mesh->mNumFaces; ++j)
	{
		if (mesh->mFaces[j].mNumIndices == 3)
			++ret;
	}

	return ret;
}

void collect_bones(OutModel& model)
{
	enn::set<aiNode*>::type necessary;
	enn::set<aiNode*>::type root_nodes;

	for (uint32 i = 0; i < model.meshes_.size(); ++i)
	{
		aiMesh* mesh = model.meshes_[i];
		aiNode* mesh_node = model.mesh_nodes_[i];
		aiNode* mesh_parent_node = mesh_node->mParent;
		aiNode* rootNode = 0;

		for (uint32 j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			String bone_name = bone->mName.data;

			aiNode* bone_node = GetNode(bone_name, scene_->mRootNode, true);

			if (!bone_node)
			{
				ENN_ASSERT(0);
			}

			necessary.insert(bone_node);
			rootNode = bone_node;

			for (; ;)
			{
				bone_node = bone_node->mParent;
				if (!bone_node || bone_node == mesh_node || bone_node == mesh_parent_node)
				{
					break;
				}

				rootNode = bone_node;
				necessary.insert(bone_node);
			}

			if (root_nodes.find(rootNode) == root_nodes.end())
			{
				root_nodes.insert(rootNode);
			}
		}
	}

	if (root_nodes.size() > 1)
	{
		ENN_ASSERT(0);
	}

	if (root_nodes.empty())
	{
		return;
	}

	model.root_bone_ = *root_nodes.begin();

	collect_bones_final(model.bones_, necessary, model.root_bone_->mChildren[0]);

	/** Initialize the bone collision info */
	model.bone_rad_ii_.resize(model.bones_.size());
	model.bone_hit_boxes_.resize(model.bones_.size());

	for (uint32 i = 0; i < model.bones_.size(); ++i)
	{
		model.bone_rad_ii_[i] = 0.0f;
		model.bone_hit_boxes_[i] = AxisAlignedBox::BOX_NULL;
	}
}

aiNode* GetNode(const String& name, aiNode* rootNode, bool caseSensitive)
{
	if (!rootNode)
	{
		return 0;
	}

	if (name.compare(rootNode->mName.data) == 0)
	{
		return rootNode;
	}

	for (uint32 i = 0; i < rootNode->mNumChildren; ++i)
	{
		aiNode* found = GetNode(name, rootNode->mChildren[i], caseSensitive);
		if (found)
		{
			return found;
		}
	}

	return 0;
}

void collect_bones_final(enn::vector<aiNode*>::type& dest, const enn::set<aiNode*>::type& necessary, aiNode* node)
{
	if (necessary.find(node) != necessary.end())
	{
		dest.push_back(node);
		for (unsigned i = 0; i < node->mNumChildren; ++i)
		{
			collect_bones_final(dest, necessary, node->mChildren[i]);
		}
	}
}

void build_bone_collision_info(OutModel& model)
{
	for (uint32 i = 0; i < model.meshes_.size(); ++i)
	{
		aiMesh* mesh = model.meshes_[i];
		for (uint32 j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];

			String bone_name = bone->mName.data;

			uint32 bone_idx = GetBoneIndex(model, bone_name);

			if (bone_idx == Math::ENN_MAX_UNSIGNED)
			{
				continue;
			}

			for (uint32 k = 0; k < bone->mNumWeights; ++k)
			{
				float weight = bone->mWeights[k].mWeight;

				/** Require skinning weight to be sufficiently large before vertex contributes to bone hitbox */
				if (weight > 0.33f)
				{
					aiVector3D vertexBoneSpace = bone->mOffsetMatrix * mesh->mVertices[bone->mWeights[k].mVertexId];
					Vector3 vertex = ToVector3(vertexBoneSpace);

					float radius = vertex.length();
					if (radius > model.bone_rad_ii_[bone_idx])
					{
						model.bone_rad_ii_[bone_idx] = radius;
					}

					model.bone_hit_boxes_[bone_idx].merge(vertex);
				}
			}
		}
	}
}

unsigned GetBoneIndex(OutModel& model, const String& boneName)
{
	for (unsigned i = 0; i < model.bones_.size(); ++i)
	{
		if (boneName == model.bones_[i]->mName.data)
			return i;
	}

	return Math::ENN_MAX_UNSIGNED;
}

Vector3 ToVector3(const aiVector3D& vec)
{
	return Vector3(vec.x, vec.y, vec.z);
}

Vector2 ToVector2(const aiVector2D& vec)
{
	return Vector2(vec.x, vec.y);
}

Quaternion ToQuaternion(const aiQuaternion& quat)
{
	return Quaternion(quat.w, quat.x, quat.y, quat.z);
}

void collect_animations(OutModel* model)
{
	const aiScene* scene = scene_;
	for (unsigned i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];
		if (allAnimations_.find(anim) != allAnimations_.end())
		{
			continue;
		}

		if (model)
		{
			bool modelBoneFound = false;
			for (unsigned j = 0; j < anim->mNumChannels; ++j)
			{
				aiNodeAnim* channel = anim->mChannels[j];
				String channelName = (channel->mNodeName.data);
				if (GetBoneIndex(*model, channelName) != Math::ENN_MAX_UNSIGNED)
				{
					modelBoneFound = true;
					break;
				}

			}

			if (modelBoneFound)
			{
				model->animations_.push_back(anim);
				allAnimations_.insert(anim);
			}
		}
		else
		{
			sceneAnimations_.push_back(anim);
			allAnimations_.insert(anim);
		}
	}
}

void build_and_save_model(OutModel& model)
{
	if (!model.root_node_)
	{
		ENN_ASSERT(0);
		return;
	}

	String root_node_name = model.root_node_->mName.data;

	if (!model.meshes_.size())
	{
		ENN_ASSERT(0);
		return;
	}

	Material* mat = ENN_NEW Material;
	for (unsigned i = 0; i < model.mtrs_.size(); ++i)
	{
		aiMaterial* mtr = model.mtrs_[i];

		SubMaterial* sub_mtr = mat->createSubMaterial(i);

		build_material(sub_mtr, mtr);
	}

	Mesh* mesh = ENN_NEW Mesh();
	mesh->setMaterial(mat);

	AxisAlignedBox box;

	for (unsigned i = 0; i < model.meshes_.size(); ++i)
	{
		aiMesh* ai_mesh = model.meshes_[i];
		unsigned elementMask = GetElementMask(ai_mesh);
		unsigned validFaces = GetNumValidFaces(ai_mesh);
		int mtr_idx = ai_mesh->mMaterialIndex;

		if (!validFaces)
		{
			continue;
		}

		SubMesh* sub_mesh = mesh->createNewSubMesh(i);

		sub_mesh->setUseMemory(true);
		sub_mesh->setPrimType(PRIM_TRIANGLELIST);

		sub_mesh->setMatIdx(mtr_idx);

		SubMaterial* sub_mtr = sub_mesh->getMaterial();
		ENN_ASSERT(sub_mtr);

		/** if there is no texture, disable uv1 */
		if (!(sub_mtr->getTexDiff() || sub_mtr->getTexNormal() ||
			sub_mtr->getTexSpecular() || sub_mtr->getTexEmissive()))
		{
			elementMask &= ~MASK_TEXCOORD1;
		}

		sub_mesh->setVertexType(elementMask);
		sub_mesh->setVertexCount(ai_mesh->mNumVertices);

		sub_mesh->setIdxType(IDX_16);
		sub_mesh->setIndexCount(validFaces * 3);

		vec3f pos, scale;
		Quaternion rot;
		GetPosRotScale(GetMeshBakingTransform(model.mesh_nodes_[i], model.root_node_), pos, rot, scale);

		Matrix4 vertex_transform;
		Matrix3 normal_transform;

		vertex_transform.makeTransform(pos, scale, rot);
		rot.toRotationMatrix(normal_transform);

		uchar* vertexData = (uchar*)sub_mesh->getVertexData();
		uchar* indexData = (uchar*)sub_mesh->getIndexData();

		/** write index data */
		{
			ushort* dest = (ushort*)indexData;
			for (unsigned f = 0; f < ai_mesh->mNumFaces; ++f)
			{
				WriteShortIndices(dest, ai_mesh, f, 0);
			}
		}

		/** build vertex data */
		{
			float* dest = (float*)vertexData;
			enn::vector<enn::vector<uchar>::type>::type blend_indices;
			enn::vector<enn::vector<float>::type>::type blend_weights;
			enn::vector<uint32>::type bone_mappings;

			if (model.bones_.size())
			{
				GetBlendData(model, ai_mesh, bone_mappings,
					blend_indices, blend_weights);
			}

			for (unsigned v = 0; v < ai_mesh->mNumVertices; ++v)
			{
				WriteVertex(dest, ai_mesh, v, elementMask, box, 
					vertex_transform, normal_transform, blend_indices, blend_weights);
			}
		}
	}

	/** save skeleton */
	if (model.bones_.size() && model.root_bone_)
	{
		Skeleton* skel = mesh->createSkeleton();

		for (uint32 i = 0; i < model.bones_.size(); ++i)
		{
			aiNode* bone_node = model.bones_[i];
			String bone_name(bone_node->mName.data);
			BoneNode* bn = skel->createBoneNode(bone_name, i);
			bn->getOffsetMatrix() = GetOffsetMatrix(model, bone_name);

			aiMatrix4x4 transform = bone_node->mTransformation;
			if (bone_node == model.root_bone_)
			{
				transform = GetDerivedTransform(bone_node, model.root_node_);
			}

			GetPosRotScale(transform, bn->getInitPos(), bn->getInitRot(), bn->getInitScale());

			bn->setParent(0);
		}

		for (uint32 i = 1; i < model.bones_.size(); ++i)
		{
			String parent_name = model.bones_[i]->mParent->mName.data;

			for (uint32 j = 0; j < (uint32)skel->getBoneNodeCount(); ++j)
			{
				BoneNode* bnj = skel->getBoneNode(j);
				BoneNode* bni = skel->getBoneNode(i);

				if (bnj->getName() == parent_name)
				{
					bni->setParent(bnj);
				}
			}
		}

		/* animation */
		enn::map<String, Animation*>::type::iterator it = model.mesh_anims_.begin();
		for (; it != model.mesh_anims_.end(); ++it)
		{
			String anim_name = it->first;
			Animation* anim = it->second;

			skel->insertAnimation(anim_name, anim);
		}
	}

	mesh->updateAABB(true);

	MeshManager* mesh_manager = Root::getSingletonPtr()->get_mesh_mgr();

	mesh_manager->saveMesh(mesh, "sponza.mesh");
}

void build_material(SubMaterial* sub_mtr, aiMaterial* mtr)
{
	aiString matNameStr;
	mtr->Get(AI_MATKEY_NAME, matNameStr);
	String matName = SanitateAssetName(matNameStr.data);

	sub_mtr->setName(matName);
	sub_mtr->enableLighting(true);
	sub_mtr->setCullMode(CULL_CCW);

	String diffuseTexName;
	String normalTexName;
	String specularTexName;
	String lightmapTexName;
	String emissiveTexName;

	Color diffuseColor;
	Color specularColor;
	Color ambientColor;
	Color emissiveColor;
	float specPower = 1.0f;
	bool hasAlpha = false;


	aiString stringVal;
	float floatVal;
	int intVal;
	aiColor3D colorVal;

	intVal;

	/** tex */
	if (mtr->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), stringVal) == AI_SUCCESS)
	{
		diffuseTexName = stringVal.data;
	}

	if (mtr->Get(AI_MATKEY_TEXTURE(aiTextureType_NORMALS, 0), stringVal) == AI_SUCCESS)
	{
		normalTexName = stringVal.data;
	}

	if (mtr->Get(AI_MATKEY_TEXTURE(aiTextureType_SPECULAR, 0), stringVal) == AI_SUCCESS)
	{
		specularTexName = stringVal.data;
	}

	if (mtr->Get(AI_MATKEY_TEXTURE(aiTextureType_LIGHTMAP, 0), stringVal) == AI_SUCCESS)
	{
		lightmapTexName = stringVal.data;
	}

	if (mtr->Get(AI_MATKEY_TEXTURE(aiTextureType_EMISSIVE, 0), stringVal) == AI_SUCCESS)
	{
		emissiveTexName = stringVal.data;
	}

	/** color */
	if (mtr->Get(AI_MATKEY_COLOR_DIFFUSE, colorVal) == AI_SUCCESS)
	{
		diffuseColor = Color(colorVal.r, colorVal.g, colorVal.b);
	}

	if (mtr->Get(AI_MATKEY_COLOR_SPECULAR, colorVal) == AI_SUCCESS)
	{
		specularColor = Color(colorVal.r, colorVal.g, colorVal.b);
	}

	if (mtr->Get(AI_MATKEY_COLOR_AMBIENT, colorVal) == AI_SUCCESS)
	{
		ambientColor = Color(colorVal.r, colorVal.g, colorVal.b);
	}

	if (mtr->Get(AI_MATKEY_COLOR_EMISSIVE, colorVal) == AI_SUCCESS)
	{
		emissiveColor = Color(colorVal.r, colorVal.g, colorVal.b);
	}

	if (mtr->Get(AI_MATKEY_COLOR_EMISSIVE, colorVal) == AI_SUCCESS)
	{
		emissiveColor = Color(colorVal.r, colorVal.g, colorVal.b);
	}

	if (mtr->Get(AI_MATKEY_SHININESS, floatVal) == AI_SUCCESS)
	{
		specPower = floatVal;
		specularColor.a = specPower;
	}

	/** set texure */
	if (!diffuseTexName.empty())
	{
		TextureUnit* tu_diff = sub_mtr->useTexDiff();
		tu_diff->setTexNameNoLoading(diffuseTexName);
	}
	
	if (!normalTexName.empty())
	{
		TextureUnit* tu_normal = sub_mtr->useTexNormal();
		tu_normal->setTexNameNoLoading(normalTexName);
	}

	if (!specularTexName.empty())
	{
		TextureUnit* tu = sub_mtr->useTexSpecular();
		tu->setTexNameNoLoading(specularTexName);
	}

	if (!emissiveTexName.empty())
	{
		TextureUnit* tu = sub_mtr->useTexEmissive();
		tu->setTexNameNoLoading(emissiveTexName);
	}

	if (!lightmapTexName.empty())
	{
		TextureUnit* tu = sub_mtr->useTexDiff2();
		tu->setTexNameNoLoading(lightmapTexName);
	}

	/** set light about attr */
	sub_mtr->setDiffuse(diffuseColor);
	sub_mtr->setSpecularShiness(specularColor);
	sub_mtr->setEmissive(emissiveColor);
}

void build_animations(OutModel* model)
{
	const enn::vector<aiAnimation*>::type& animations = model ? model->animations_ : sceneAnimations_;

	for (unsigned i = 0; i < animations.size(); ++i)
	{
		aiAnimation* anim = animations[i];

		String animName = anim->mName.data;
		
		Animation* mesh_anim = ENN_NEW Animation(i);

		float ticksPerSecond = (float)anim->mTicksPerSecond;

		if (ticksPerSecond < Math::ENN_EPSILON)
		{
			ticksPerSecond = defaultTicksPerSecond_;
		}

		float tickConversion = 1.0f / ticksPerSecond;

		mesh_anim->setLength((float)anim->mDuration * tickConversion);

		enn::vector<NodeAniTrack>::type tracks;

		for (unsigned j = 0; j < anim->mNumChannels; ++j)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			String channelName = (channel->mNodeName.data);
			aiNode* boneNode = 0;
			bool isRootBone = false;

			if (model)
			{
				unsigned boneIndex = GetBoneIndex(*model, channelName);
				if (boneIndex == Math::ENN_MAX_UNSIGNED)
				{
					ENN_ASSERT(0);
					continue;
				}

				boneNode = model->bones_[boneIndex];
				isRootBone = boneIndex == 0;
			}
			else
			{
				boneNode = GetNode(channelName, scene_->mRootNode);

				if (!boneNode)
				{
					ENN_ASSERT(0);

					continue;
				}
			}
			
			NodeAniTrack* curr_track = mesh_anim->createNodeTrack(j);

			uint32 key_frames = channel->mNumPositionKeys;
			if (channel->mNumRotationKeys > key_frames)
			{
				key_frames = channel->mNumRotationKeys;
			}
			if (channel->mNumScalingKeys > key_frames)
			{
				key_frames = channel->mNumScalingKeys;
			}

			for (uint32 k = 0; k < key_frames; ++k)
			{
				float key_frame_time_pos = 0.0f;

				if (k < channel->mNumPositionKeys)
				{
					key_frame_time_pos = float(channel->mPositionKeys[k].mTime * tickConversion);
				}
				else if (k < channel->mNumRotationKeys)
				{
					key_frame_time_pos = float(channel->mRotationKeys[k].mTime * tickConversion);
				}
				else if (k < channel->mNumScalingKeys)
				{
					key_frame_time_pos = float(channel->mScalingKeys[k].mTime * tickConversion);
				}

				NodeKeyFrame* kf = static_cast<NodeKeyFrame*>(curr_track->createKeyFrame(key_frame_time_pos));

				// Start with the bone's base transform
				aiMatrix4x4 boneTransform = boneNode->mTransformation;
				aiVector3D pos, scale;
				aiQuaternion rot;
				boneTransform.Decompose(scale, rot, pos);

				if (k < channel->mNumPositionKeys)
				{
					pos = channel->mPositionKeys[k].mValue;
				}
				if (k < channel->mNumRotationKeys)
				{
					rot = channel->mRotationKeys[k].mValue;
				}
				if (k < channel->mNumScalingKeys)
				{
					scale = channel->mScalingKeys[k].mValue;
				}

				kf->setPosition(ToVector3(pos));
				kf->setRotation(ToQuaternion(rot));
				kf->setScale(ToVector3(scale));
			}
		}

		model->mesh_anims_.insert(enn::map<enn::String, enn::Animation*>::type::value_type(
			animName, mesh_anim));
	}
}

unsigned GetElementMask(aiMesh* mesh)
{
	unsigned elementMask = MASK_POSITION;
	if (mesh->HasNormals())
		elementMask |= MASK_NORMAL;
	if (mesh->HasTangentsAndBitangents())
		elementMask |= MASK_TANGENT;
	/** disable color channel.
	if (mesh->GetNumColorChannels() > 0)
		elementMask |= MASK_COLOR;
	*/
	if (mesh->GetNumUVChannels() > 0)
		elementMask |= MASK_TEXCOORD1;
	if (mesh->GetNumUVChannels() > 1)
		elementMask |= MASK_TEXCOORD2;
	if (mesh->HasBones())
		elementMask |= (MASK_BLENDWEIGHTS | MASK_BLENDINDICES);
	return elementMask;
}

void GetPosRotScale(const aiMatrix4x4& transform, Vector3& pos, Quaternion& rot, Vector3& scale)
{
	aiVector3D aiPos;
	aiQuaternion aiRot;
	aiVector3D aiScale;
	transform.Decompose(aiScale, aiRot, aiPos);
	pos = ToVector3(aiPos);
	rot = ToQuaternion(aiRot);
	scale = ToVector3(aiScale);
}


aiMatrix4x4 GetDerivedTransform(aiNode* node, aiNode* rootNode)
{
	return GetDerivedTransform(node->mTransformation, node, rootNode);
}

aiMatrix4x4 GetDerivedTransform(aiMatrix4x4 transform, aiNode* node, aiNode* rootNode)
{
	// If basenode is defined, go only up to it in the parent chain
	while (node && node != rootNode)
	{
		node = node->mParent;
		if (node)
		{
			transform = node->mTransformation * transform;
		}
	}
	return transform;
}

void WriteShortIndices(unsigned short*& dest, aiMesh* mesh, unsigned index, unsigned offset)
{
	if (mesh->mFaces[index].mNumIndices == 3)
	{
		*dest++ = mesh->mFaces[index].mIndices[0] + offset;
		*dest++ = mesh->mFaces[index].mIndices[1] + offset;
		*dest++ = mesh->mFaces[index].mIndices[2] + offset;
	}
}

void WriteVertex(float*& dest, aiMesh* mesh, unsigned index, unsigned elementMask, AxisAlignedBox& box,
				 const Matrix4& vertexTransform, const Matrix3& normalTransform,
				 enn::vector<enn::vector<uchar>::type>::type blend_indices,
				 enn::vector<enn::vector<float>::type>::type blend_weight)
{
	Vector3 vertex = vertexTransform * ToVector3(mesh->mVertices[index]);
	box.merge(vertex);

	*dest++ = vertex.x;
	*dest++ = vertex.y;
	*dest++ = vertex.z;

	if (elementMask & MASK_NORMAL)
	{
		Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);
		*dest++ = normal.x;
		*dest++ = normal.y;
		*dest++ = normal.z;
	}

	if (elementMask & MASK_COLOR)
	{
		*dest++ = mesh->mColors[0][index].r;
		*dest++ = mesh->mColors[0][index].g;
		*dest++ = mesh->mColors[0][index].b;
		*dest++ = mesh->mColors[0][index].a;
	}

	if (elementMask & MASK_TEXCOORD1)
	{
		Vector3 texCoord = ToVector3(mesh->mTextureCoords[0][index]);
		*dest++ = texCoord.x;
		*dest++ = texCoord.y;
	}

	if (elementMask & MASK_TEXCOORD2)
	{
		Vector3 texCoord = ToVector3(mesh->mTextureCoords[1][index]);
		*dest++ = texCoord.x;
		*dest++ = texCoord.y;
	}

	if (elementMask & MASK_TANGENT)
	{
		Vector3 tangent = normalTransform * ToVector3(mesh->mTangents[index]);
		Vector3 normal = normalTransform * ToVector3(mesh->mNormals[index]);
		Vector3 bitangent = normalTransform * ToVector3(mesh->mBitangents[index]);

		// Check handedness
		float w = 1.0f;
		if ((tangent.crossProduct(normal)).dotProduct(bitangent) < 0.5f)
		{
			w = -1.0f;
		}

		*dest++ = tangent.x;
		*dest++ = tangent.y;
		*dest++ = tangent.z;
		*dest++ = w;
	}

	if (elementMask & MASK_BLENDWEIGHTS)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (i < (int)blend_weight[index].size())
			{
				*dest++ = blend_weight[index][i];
			}
			else
			{
				*dest++ = 0.0f;
			}
		}
	}

	if (elementMask & MASK_BLENDINDICES)
	{
		uchar* destBytes = (uchar*)dest;
		++dest;
		for (uint32 i = 0; i < 4; ++i)
		{
			if (i < blend_indices[index].size())
			{
				*destBytes++ = blend_indices[index][i];
			}
			else
			{
				*destBytes++ = 0;
			}
		}
	}
}

String SanitateAssetName(const String& name)
{
	String fixedName = name;
	enn::replace_str(fixedName, "<", "");
	enn::replace_str(fixedName, ">", "");
	enn::replace_str(fixedName, "?", "");
	enn::replace_str(fixedName, "*", "");
	enn::replace_str(fixedName, ":", "");
	enn::replace_str(fixedName, "\"", "");
	enn::replace_str(fixedName, "/", "");
	enn::replace_str(fixedName, "\\", "");
	enn::replace_str(fixedName, "|", "");

	return fixedName;
}

void GetBlendData(OutModel& model, aiMesh* mesh, enn::vector<uint32>::type& bone_mappings,
				  enn::vector<enn::vector<uchar>::type>::type& blend_indices,
				  enn::vector<enn::vector<float>::type>::type& blend_weights)
{
	blend_indices.resize(mesh->mNumVertices);
	blend_weights.resize(mesh->mNumVertices);

	bone_mappings.clear();

	if (model.bones_.size() > MAX_BONES_NUM)
	{
		ENN_ASSERT(0);
	}
	else
	{
		for (uint32 i = 0; i < mesh->mNumBones; ++i)
		{
			aiBone* bone = mesh->mBones[i];
			String bone_name = bone->mName.data;

			uint32 global_idx = GetBoneIndex(model, bone_name);
			if (global_idx == Math::ENN_MAX_UNSIGNED)
			{
				ENN_ASSERT(0);
			}

			for (uint32 j = 0; j < bone->mNumWeights; ++j)
			{
				uint32 vertex = bone->mWeights[j].mVertexId;
				blend_indices[vertex].push_back(global_idx);
				blend_weights[vertex].push_back(bone->mWeights[j].mWeight);

				if (blend_weights[vertex].size() > 4)
				{
					ENN_ASSERT(0);
				}
			}
		}
	}
}


enn::Matrix4 GetOffsetMatrix(OutModel& model, const String& boneName)
{
	for (unsigned i = 0; i < model.meshes_.size(); ++i)
	{
		aiMesh* mesh = model.meshes_[i];
		aiNode* node = model.mesh_nodes_[i];

		for (unsigned j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone = mesh->mBones[j];
			if (boneName == bone->mName.data)
			{
				aiMatrix4x4 offset = bone->mOffsetMatrix;

				aiMatrix4x4 nodeDerivedInverse = GetMeshBakingTransform(node, model.root_node_);
				nodeDerivedInverse.Inverse();
				offset *= nodeDerivedInverse;
				return ToMatrix4(offset);
			}
		}
	}

	return Matrix4::IDENTITY;
}

aiMatrix4x4 GetMeshBakingTransform(aiNode* meshNode, aiNode* modelRootNode)
{
	if (meshNode == modelRootNode)
	{
		return aiMatrix4x4();
	}
	else
	{
		return GetDerivedTransform(meshNode, modelRootNode);
	}
}

enn::Matrix4 ToMatrix4(const aiMatrix4x4& mat)
{
	enn::Matrix4 ret;
	memcpy(&ret.m[0][0], &mat.a1, sizeof(enn::Matrix4));
	return ret;
}
