#include "AABB.h"


NW_NAMESPACE_BEGIN

bool AABB::IsInfinite() const
{
	const nix::Boolean sel1 = m_min == nix::Vector4(nix::kMinusInf);
	const nix::Boolean sel2 = m_max == nix::Vector4(nix::kPlusInf);

	return sel1.IsTrue() && sel2.IsTrue();
}

bool AABB::IsZero() const
{ 
	const nix::Boolean sel1 = m_min == nix::Vector4(nix::kZero);
	const nix::Boolean sel2 = m_max == nix::Vector4(nix::kZero);

	return sel1.IsTrue() && sel2.IsTrue();
}

bool AABB::IsInitialized() const
{ 
	const nix::Boolean sel1 = m_min == nix::Vector4(nix::kPlusInf);
	const nix::Boolean sel2 = m_max == nix::Vector4(nix::kMinusInf);

	return sel1.IsTrue() && sel2.IsTrue();
}

bool AABB::IsValid() const
{
	return !IsInitialized() && !IsZero() && !IsInfinite();
}

void AABB::MergePoint(const nix::Vector4& _point)
{
	m_min = m_min.Min(_point);
	m_max = m_max.Max(_point);
}

void AABB::Expande(const AABB& _bbox)
{
	m_min = m_min.Min(_bbox.GetMin());
	m_max = m_max.Max(_bbox.GetMax());
}

void AABB::Expande(const nix::Vector4& _min, const nix::Vector4& _max)
{
	m_min = m_min.Min(_min);
	m_max = m_max.Max(_max);
}

void AABB::GetCorners(nix::Vector4(&_corners)[8]) const
{
	const nix::float128 minX_maxX_minY_maxY = nix::MathFunctions::Permute<nix::A_X, nix::B_X, nix::A_Y, nix::B_Y>(m_min, m_max);
	const nix::float128 minZ_maxZ_minW_maxW = nix::MathFunctions::Permute<nix::A_Z, nix::B_Z, nix::A_W, nix::B_W>(m_min, m_max);

	_corners[0] = m_min;
	_corners[1] = nix::MathFunctions::Permute<nix::A_W, nix::A_X, nix::B_W, nix::B_X>(minX_maxX_minY_maxY, minZ_maxZ_minW_maxW);
	_corners[2] = nix::MathFunctions::Permute<nix::A_W, nix::A_X, nix::B_W, nix::B_Y>(minX_maxX_minY_maxY, minZ_maxZ_minW_maxW);
	_corners[3] = nix::MathFunctions::Permute<nix::A_W, nix::A_X, nix::B_Z, nix::B_Y>(minX_maxX_minY_maxY, minZ_maxZ_minW_maxW);
	_corners[4] = m_max;
	_corners[5] = nix::MathFunctions::Permute<nix::A_W, nix::A_Y, nix::B_W, nix::B_X>(minX_maxX_minY_maxY, minZ_maxZ_minW_maxW);
	_corners[6] = nix::MathFunctions::Permute<nix::A_W, nix::A_Y, nix::B_Z, nix::B_X>(minX_maxX_minY_maxY, minZ_maxZ_minW_maxW);
	_corners[0] = nix::MathFunctions::Permute<nix::A_W, nix::A_Y, nix::B_Z, nix::B_Y>(minX_maxX_minY_maxY, minZ_maxZ_minW_maxW);
}

NW_NAMESPACE_END
