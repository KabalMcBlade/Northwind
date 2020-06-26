#include "OBB.h"

#include "AABB.h"


NW_NAMESPACE_BEGIN

OBB::OBB(const AABB& _aabb, const nix::Matrix4x4& _matrix)
{
	m_min = _aabb.GetMin();
	m_max = _aabb.GetMax();

	nix::Vector4 corners[8];
	_aabb.GetCorners(corners);

	for (uint8 i = 0; i < 8; ++i)
	{
		nix::Vector4 t = _matrix * corners[i];

		m_min = m_min.Min(t);
		m_max = m_max.Max(t);
	}
}


bool OBB::IsInfinite() const
{
	const nix::Boolean sel1 = m_min == nix::Vector4(nix::kMinusInf);
	const nix::Boolean sel2 = m_max == nix::Vector4(nix::kPlusInf);

	return sel1.IsTrue() && sel2.IsTrue();
}

bool OBB::IsZero() const
{
	const nix::Boolean sel1 = m_min == nix::Vector4(nix::kZero);
	const nix::Boolean sel2 = m_max == nix::Vector4(nix::kZero);

	return sel1.IsTrue() && sel2.IsTrue();
}

bool OBB::IsInitialized() const
{
	const nix::Boolean sel1 = m_min == nix::Vector4(nix::kPlusInf);
	const nix::Boolean sel2 = m_max == nix::Vector4(nix::kMinusInf);

	return sel1.IsTrue() && sel2.IsTrue();
}

bool OBB::IsValid() const
{
	return !IsInitialized() && !IsZero() && !IsInfinite();
}

void OBB::MergePoint(const nix::Vector4& _point)
{
	m_min = m_min.Min(_point);
	m_max = m_max.Max(_point);
}

void OBB::Expande(const OBB& _bbox)
{
	m_min = m_min.Min(_bbox.GetMin());
	m_max = m_max.Max(_bbox.GetMax());
}

void OBB::Expande(const nix::Vector4& _min, const nix::Vector4& _max)
{
	m_min = m_min.Min(_min);
	m_max = m_max.Max(_max);
}

void OBB::GetCorners(nix::Vector4(&_corners)[8]) const
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
