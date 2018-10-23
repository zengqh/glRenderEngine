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
// File: enn_collision_shape.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_physics_utils.h"
#include "enn_mesh.h"

class btBvhTriangleMeshShape;
class btCollisionShape;
class btCompoundShape;
class btTriangleMesh;
struct btTriangleInfoMap;

namespace enn
{

class PhysicsWorld;
class RigidBody;
class TriangleMeshInterface;
class Mesh;
class SceneNode;

enum ShapeType
{
	SHAPE_BOX = 0,
	SHAPE_SPHERE,
	SHAPE_STATICPLANE,
	SHAPE_CYLINDER,
	SHAPE_CAPSULE,
	SHAPE_CONE,
	SHAPE_TRIANGLEMESH,
	SHAPE_CONVEXHULL,
	SHAPE_TERRAIN
};

// Base class for collision shape geometry data.
struct CollisionGeometryData : public AllocatedObject
{
	String model_name_;
};

// Triangle mesh geometry data.
class TriangleMeshData : public CollisionGeometryData
{
public:
	TriangleMeshData(Mesh* mesh);
	~TriangleMeshData();

	TriangleMeshInterface*		mesh_interface_;
	btBvhTriangleMeshShape*		shape_;
	btTriangleInfoMap*			info_map_;
};

// Convex hull geometry data
struct ConvexData : public CollisionGeometryData
{
	ConvexData(Mesh* mesh);
	~ConvexData();

	void buildHull(const enn::vector<Vector3>::type& vertices);

	Vector3*	vertexData_;
	uint32*		indexData_;
	uint32		vertexCount_;
	uint32		indexCount_;
};

// Heightfield geometry data.
struct HeightfieldData : public CollisionGeometryData
{
	HeightfieldData();
	~HeightfieldData();

	float*		heightData_;
	Vector3		spacing_;
	nv::vec2i	size_;
	float		minHeight_;
	float		maxHeight_;
};

//////////////////////////////////////////////////////////////////////////
class CollisionShape : public AllocatedObject
{
public:
	CollisionShape();
	virtual ~CollisionShape();

public:
	void setBox(const Vector3& size, const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setSphere(float diameter, const Vector3& position = Vector3::ZERO,
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setStaticPlane(const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setCylinder(float diameter, float height, const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setCapsule(float diameter, float height, const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setCone(float diameter, float height, const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setTriangleMesh(Mesh* mesh, const Vector3& scale = Vector3::UNIT_SCALE, 
		const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setConvexHull(Mesh* mesh, const Vector3& scale = Vector3::UNIT_SCALE, 
		const Vector3& position = Vector3::ZERO, 
		const Quaternion& rotation = Quaternion::IDENTITY);

	void setTerrain();

	void setShapeType(ShapeType type);

	void setSize(const Vector3& size);

	void setPosition(const Vector3& position);

	void setRotation(const Quaternion& rotation);

	void setTransform(const Vector3& position, const Quaternion& rotation);

	void setMargin(float margin);

	btCollisionShape* getCollisionShape() const;

	CollisionGeometryData* getGeometryData() const;

	PhysicsWorld* getPhysicsWorld() const;

	ShapeType getShapeType() const;

	const Vector3& getSize() const;

	const Vector3& getPosition() const;

	const Quaternion& getRotation() const;

	float getMargin() const;

	Mesh* getModel() const;

	AxisAlignedBox getWorldAABB() const;

	void notifyRigidBody(bool updataMass = true);

	void releaseShape();

public:
	void setNode(SceneNode* node);

protected:
	void updateShape();
	btCompoundShape* getParentCompoundShape();

protected:
	PhysicsWorld*				physics_world_;
	RigidBody*					rigid_body_;
	MeshPtr						model_;
	CollisionGeometryData*		geometry_;
	btCollisionShape*			shape_;
	ShapeType					shape_type_;
	Vector3						position_;
	Quaternion					rotation_;
	Vector3						size_;
	float						margin_;
	SceneNode*					node_;
	bool						recreateShape_;
};

}