#pragma once

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class NW_DLL nix::Vector4;

class NW_DLL AABB final
{
public:
	AABB() : m_min(nix::kPlusInf), m_max(nix::kMinusInf) {}
	AABB(const nix::Vector4& _min, const nix::Vector4& _max) : m_min(_min), m_max(_max) {}
	~AABB() {}

	NW_INLINE nix::Vector4 GetMin() const { return m_min; }
	NW_INLINE nix::Vector4 GetMax() const { return m_max; }
	NW_INLINE nix::Vector4 GetHalfExtent() const { return (m_max - m_min) * 0.5f; }
	NW_INLINE nix::Vector4 GetCenter() const { return (m_max + m_min) * 0.5f; }
	NW_INLINE nix::Vector4 GetSize() const { return m_max - m_min; }

	bool IsInfinite() const;
	bool IsZero() const;
	bool IsInitialized() const;
	bool IsValid() const;

	void MergePoint(const nix::Vector4& _point);
	void Expande(const AABB& _bbox);
	void Expande(const nix::Vector4& _min, const nix::Vector4& _max);

	void GetCorners(nix::Vector4(&_corners)[8]) const;

private:
	nix::Vector4 m_min;
	nix::Vector4 m_max;
};

NW_NAMESPACE_END