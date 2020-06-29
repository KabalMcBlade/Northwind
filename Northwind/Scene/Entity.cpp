#include "Entity.h"


NW_NAMESPACE_BEGIN

AABB Entity::GetAABB() const
{
	return m_aabb;
}

OBB Entity::GetOBB() const
{
	OBB obb(m_aabb, m_transform.GetMatrixLS());
	return obb;
}


NW_NAMESPACE_END