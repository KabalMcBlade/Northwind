#pragma once

#include "Node.h"
#include "Transform.h"

#include "../Geometry/AABB.h"
#include "../Geometry/OBB.h"


NW_NAMESPACE_BEGIN

class NW_DLL Entity : public Node
{
public:
	Entity() : Node() {}
	explicit Entity(const char* _name) : Node(_name) {}
	Entity(const nwString& _name) : Node(_name) {}
	virtual ~Entity() {}

	NW_INLINE const Transform& GetTransform() const { return m_transform; }
	NW_INLINE Transform& GetTransform() { return m_transform; }

	// just to keep the same, both return copy
	AABB GetAABB() const;
	OBB GetOBB() const;

private:
	Transform m_transform;
	AABB m_aabb;
};

NW_NAMESPACE_END