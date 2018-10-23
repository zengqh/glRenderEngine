//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/09/23
// File: enn_collision_shape.cpp
//

#include "enn_collision_shape.h"
#include "enn_physics_world.h"
#include "enn_rigid_body.h"
#include "enn_mesh.h"
#include "enn_scene_node.h"
#include "enn_root.h"
#include "hull.h"

#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>

namespace enn
{

static const float DEFAULT_COLLISION_MARGIN = 0.04f;

static const btVector3 WHITE(1.0f, 1.0f, 1.0f);
static const btVector3 GREEN(0.0f, 1.0f, 0.0f);


static const char* typeNames[] = 
{
	"Box",
	"Sphere",
	"StaticPlane",
	"Cylinder",
	"Capsule",
	"Cone",
	"TriangleMesh",
	"ConvexHull",
	"Terrain",
	0
};

//////////////////////////////////////////////////////////////////////////
class TriangleMeshInterface : public btTriangleIndexVertexArray
{
public:
	TriangleMeshInterface(Mesh* mesh) : btTriangleIndexVertexArray()
	{
		uint32 sub_mesh_cnt = mesh->getSubMeshCount();

		for (uint32 i = 0; i < sub_mesh_cnt; ++i)
		{
			SubMesh* sub_mesh = mesh->getSubMesh(i);

			if (!sub_mesh)
			{
				ENN_ASSERT(0);
				continue;
			}

			VertexBuffer* vb = sub_mesh->getVB();
			IndexBuffer* ib = sub_mesh->getIB();

			if (!vb || !ib)
			{
				ENN_ASSERT(0);
				continue;
			}

			void* vb_data = vb->getData();
			void* ib_data = ib->getData();

			if (!vb_data || ib_data)
			{
				ENN_ASSERT(0);
				continue;
			}

			btIndexedMesh meshIndex;

			meshIndex.m_numTriangles = sub_mesh->getIndexCount() / 3;
			meshIndex.m_triangleIndexBase = static_cast<const unsigned char*>(ib_data);
			meshIndex.m_triangleIndexStride = 3 * sub_mesh->getIdxSize();
			meshIndex.m_numVertices = 0;
			meshIndex.m_vertexBase = static_cast<const unsigned char*>(vb_data);
			meshIndex.m_vertexStride = sub_mesh->getVertexSize();
			meshIndex.m_indexType = PHY_SHORT;
			meshIndex.m_vertexType = PHY_FLOAT;

			m_indexedMeshes.push_back(meshIndex);
		}
	}
};

//////////////////////////////////////////////////////////////////////////
TriangleMeshData::TriangleMeshData(Mesh* mesh)
{
	model_name_ = mesh->getName();
	mesh_interface_ = ENN_NEW TriangleMeshInterface(mesh);
	shape_ = new btBvhTriangleMeshShape(mesh_interface_, true, true);
	info_map_ = new btTriangleInfoMap();

	btGenerateInternalEdgeInfo(shape_, info_map_);
}

TriangleMeshData::~TriangleMeshData()
{
	delete shape_;
	shape_ = 0;

	ENN_SAFE_DELETE(mesh_interface_);
	
	delete info_map_;
	info_map_ = 0;
}

//////////////////////////////////////////////////////////////////////////
ConvexData::ConvexData(Mesh* mesh)
{
	model_name_ = mesh->getName();

	uint32 sub_mesh_cnt = mesh->getSubMeshCount();
	enn::vector<Vector3>::type vertices;

	for (uint32 i = 0; i < sub_mesh_cnt; ++i)
	{
		SubMesh* sub_mesh = mesh->getSubMesh(i);

		if (!sub_mesh)
		{
			ENN_ASSERT(0);
			continue;
		}

		VertexBuffer* vb = sub_mesh->getVB();
		IndexBuffer* ib = sub_mesh->getIB();

		if (!vb || !ib)
		{
			ENN_ASSERT(0);
			continue;
		}

		void* vb_data = vb->getData();
		void* ib_data = ib->getData();

		if (!vb_data || ib_data)
		{
			ENN_ASSERT(0);
			continue;
		}

		uint32 vertex_count = sub_mesh->getVertexCount();
		uint32 vertex_size = sub_mesh->getVertexSize();

		for (uint32 j = 0; j < vertex_count; ++j)
		{
			const Vector3& v = (*(const Vector3*)((uchar*)vb_data + j * vertex_size));
			vertices.push_back(v);
		}
	}

	buildHull(vertices);
}

ConvexData::~ConvexData()
{
	
}

void ConvexData::buildHull(const enn::vector<Vector3>::type& vertices)
{
	if (vertices.size())
	{
		StanHull::HullDesc desc;

		desc.SetHullFlag(StanHull::QF_TRIANGLES);
		desc.mVcount = vertices.size();
		desc.mVertices = &vertices[0].x;
		desc.mVertexStride = 3 * sizeof(float);
		desc.mSkinWidth = 0.0f;

		StanHull::HullLibrary lib;
		StanHull::HullResult result;
		lib.CreateConvexHull(desc, result);

		vertexCount_ = result.mNumOutputVertices;
		vertexData_ = ENN_NEW_ARRAY_T(Vector3, vertexCount_);
		indexCount_ = result.mNumIndices;
		indexData_ = ENN_NEW_ARRAY_T(uint32, indexCount_);

		memcpy(vertexData_, result.mOutputVertices, vertexCount_ * sizeof(Vector3));
		memcpy(indexData_, result.mIndices, indexCount_ * sizeof(unsigned));

		lib.ReleaseResult(result);
	}
	else
	{
		vertexCount_ = 0;
		indexCount_ = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
HeightfieldData::HeightfieldData()
{
	
}

HeightfieldData::~HeightfieldData()
{

}

//////////////////////////////////////////////////////////////////////////
CollisionShape::CollisionShape()
: shape_(0)
, position_(Vector3::ZERO)
, rotation_(Quaternion::IDENTITY)
, size_(Vector3::UNIT_SCALE)
, margin_(DEFAULT_COLLISION_MARGIN)
, recreateShape_(true)
, node_(0)
, rigid_body_(0)
{
	physics_world_ = ROOTPTR->getSceneManager()->getPhysicsWorld();
}

CollisionShape::~CollisionShape()
{
	releaseShape();
	if (physics_world_)
	{
		physics_world_->removeCollisionShape(this);
	}
}

void CollisionShape::setBox(const Vector3& size, const Vector3& position, 
							const Quaternion& rotation)
{
	shape_type_ = SHAPE_BOX;
	size_ = size;
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setSphere(float diameter, const Vector3& position,
	const Quaternion& rotation)
{
	shape_type_ = SHAPE_SPHERE;
	size_ = Vector3(diameter, diameter, diameter);
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setStaticPlane(const Vector3& position, 
	const Quaternion& rotation)
{
	shape_type_ = SHAPE_STATICPLANE;
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setCylinder(float diameter, float height, const Vector3& position, 
	const Quaternion& rotation)
{
	shape_type_ = SHAPE_CYLINDER;
	size_ = Vector3(diameter, height, diameter);
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setCapsule(float diameter, float height, const Vector3& position, 
	const Quaternion& rotation)
{
	shape_type_ = SHAPE_CAPSULE;
	size_ = Vector3(diameter, height, diameter);
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setCone(float diameter, float height, const Vector3& position, 
	const Quaternion& rotation)
{
	shape_type_ = SHAPE_CONE;
	size_ = Vector3(diameter, height, diameter);
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setTriangleMesh(Mesh* mesh, const Vector3& scale, 
	const Vector3& position, 
	const Quaternion& rotation)
{
	if (!mesh)
	{
		ENN_ASSERT(0);
		return;
	}

	shape_type_ = SHAPE_TRIANGLEMESH;
	model_.attach(mesh);
	size_ = scale;
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setConvexHull(Mesh* mesh, const Vector3& scale, 
	const Vector3& position, 
	const Quaternion& rotation)
{
	if (!mesh)
	{
		ENN_ASSERT(0);
		return;
	}

	shape_type_ = SHAPE_CONVEXHULL;
	model_.attach(mesh);
	size_ = scale;
	position_ = position;
	rotation_ = rotation;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setTerrain()
{
	shape_type_ = SHAPE_TERRAIN;

	updateShape();
	notifyRigidBody();
}

void CollisionShape::setShapeType(ShapeType type)
{
	if (type != shape_type_)
	{
		shape_type_ = type;

		updateShape();
		notifyRigidBody();
	}
}

void CollisionShape::setSize(const Vector3& size)
{
	if (size != size_)
	{
		size_ = size;

		updateShape();
		notifyRigidBody();
	}
}

void CollisionShape::setPosition(const Vector3& position)
{
	if (position != position_)
	{
		position_ = position;

		updateShape();
		notifyRigidBody();
	}
}

void CollisionShape::setRotation(const Quaternion& rotation)
{
	if (rotation != rotation_)
	{
		rotation_ = rotation;

		updateShape();
		notifyRigidBody();
	}
}

void CollisionShape::setTransform(const Vector3& position, const Quaternion& rotation)
{
	if (position != position_ || rotation != rotation_)
	{
		position_ = position;
		rotation_ = rotation;

		updateShape();
		notifyRigidBody();
	}
}

void CollisionShape::setMargin(float margin)
{
	margin = Math::maxVal(margin, 0.0f);

	if (margin != margin_)
	{
		margin_ = margin;
		if (shape_)
		{
			shape_->setMargin(margin_);
		}
	}
}

btCollisionShape* CollisionShape::getCollisionShape() const
{
	return shape_;
}

CollisionGeometryData* CollisionShape::getGeometryData() const
{
	return geometry_;
}

PhysicsWorld* CollisionShape::getPhysicsWorld() const
{
	return physics_world_;
}

ShapeType CollisionShape::getShapeType() const
{
	return shape_type_;
}

const Vector3& CollisionShape::getSize() const
{
	return size_;
}

const Vector3& CollisionShape::getPosition() const
{
	return position_;
}

const Quaternion& CollisionShape::getRotation() const
{
	return rotation_;
}

float CollisionShape::getMargin() const
{
	return margin_;
}

Mesh* CollisionShape::getModel() const
{
	return model_.get();
}

AxisAlignedBox CollisionShape::getWorldAABB() const
{
	ENN_ASSERT(0);
	return AxisAlignedBox::BOX_UNIT;
}

void CollisionShape::notifyRigidBody(bool updataMass)
{
	btCompoundShape* compound = getParentCompoundShape();
	if (node_ && shape_ && compound)
	{
		// Remove the shape first to ensure it is not added twice
		compound->removeChildShape(shape_);

		Vector3 position = position_;

		btTransform offset;
		offset.setOrigin(ToBtVector3(node_->getDerivedMatrix().getScaleVector() * position));
		offset.setRotation(ToBtQuaternion(rotation_));

		compound->addChildShape(offset, shape_);
	}

	if (updataMass)
	{
		rigid_body_->updateMass();
	}
}

void CollisionShape::releaseShape()
{
	btCompoundShape* compound = getParentCompoundShape();
	if (shape_ && compound)
	{
		compound->removeChildShape(shape_);
		rigid_body_->updateMass();
	}

	delete shape_;
	shape_ = 0;
}

void CollisionShape::setNode(SceneNode* node)
{
	node_ = node;
	physics_world_->addCollisionShape(this);
}

void CollisionShape::updateShape()
{
	releaseShape();
	if (!physics_world_)
	{
		return;
	}

	if (node_)
	{
		Vector3 newWorldScale = node_->getDerivedMatrix().getScaleVector();

		switch (shape_type_)
		{
		case SHAPE_BOX:
			shape_ = new btBoxShape(ToBtVector3(size_ * 0.5f));
			shape_->setLocalScaling(ToBtVector3(newWorldScale));
			break;

		case SHAPE_SPHERE:
			shape_ = new btSphereShape(size_.x * 0.5f);
			shape_->setLocalScaling(ToBtVector3(newWorldScale));
			break;

		case SHAPE_STATICPLANE:
			shape_ = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0.0f);
			break;

		case SHAPE_CYLINDER:
			shape_ = new btCylinderShape(btVector3(size_.x * 0.5f, size_.y * 0.5f, size_.x * 0.5f));
			shape_->setLocalScaling(ToBtVector3(newWorldScale));
			break;

		case SHAPE_CAPSULE:
			shape_ = new btCapsuleShape(size_.x * 0.5f, Math::maxVal(size_.y  - size_.x, 0.0f));
			shape_->setLocalScaling(ToBtVector3(newWorldScale));
			break;

		case SHAPE_CONE:
			shape_ = new btConeShape(size_.x * 0.5f, size_.y);
			shape_->setLocalScaling(ToBtVector3(newWorldScale));
			break;

		case SHAPE_TRIANGLEMESH:
			size_ = Vector3(Math::fabs(size_.x), Math::fabs(size_.y), Math::fabs(size_.z));
			if (model_)
			{
				// Check the geometry cache
				String id = "TriMesh_" + model_->getName();

				PhysicsWorld::GeometryCacheMap& cache_map = physics_world_->getGeometryCache();

				PhysicsWorld::GeometryCacheMap::iterator it = cache_map.find(id);
				if (it != cache_map.end())
				{
					geometry_ = it->second;
				}
				else
				{
					geometry_ = ENN_NEW TriangleMeshData(model_.get());
					cache_map[id] = geometry_;
				}

				TriangleMeshData* triMesh = static_cast<TriangleMeshData*>(geometry_);

				shape_ = new btScaledBvhTriangleMeshShape(triMesh->shape_, ToBtVector3(newWorldScale * size_));
			}
			break;

		case SHAPE_CONVEXHULL:
			if (model_)
			{
				String id = "Convex_" + model_->getName();

				PhysicsWorld::GeometryCacheMap& cache_map = physics_world_->getGeometryCache();

				PhysicsWorld::GeometryCacheMap::iterator it = cache_map.find(id);
				if (it != cache_map.end())
				{
					geometry_ = it->second;
				}
				else
				{
					geometry_ = ENN_NEW ConvexData(model_.get());
					cache_map[id] = geometry_;
				}

				ConvexData* convex = static_cast<ConvexData*>(geometry_);
				shape_ = new btConvexHullShape((btScalar*)convex->vertexData_, convex->vertexCount_, sizeof(Vector3));
				shape_->setLocalScaling(ToBtVector3(newWorldScale * size_));
			}
			break;

		case SHAPE_TERRAIN:
			ENN_ASSERT(0);
			break;

		default:
			ENN_ASSERT(0);
			break;
		}

		if (shape_)
		{
			shape_->setUserPointer(this);
			shape_->setMargin(margin_);
		}
	}

	recreateShape_ = false;
}

btCompoundShape* CollisionShape::getParentCompoundShape()
{
	if (!rigid_body_)
	{
		rigid_body_ = node_->getRigidBody();
	}

	return rigid_body_ ? rigid_body_->getCompoundShape() : 0;
}

}