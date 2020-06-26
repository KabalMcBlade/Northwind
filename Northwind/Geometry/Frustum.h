#pragma once

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"


NW_NAMESPACE_BEGIN

class NW_DLL nix::Vector4;
class NW_DLL nix::Matrix4x4;

class NW_DLL Frustum final
{
private:
	enum EFrustumPlane
	{
		EFrustumPlane_Near = 0,
		EFrustumPlane_Far = 1,
		EFrustumPlane_Left = 2,
		EFrustumPlane_Right = 3,
		EFrustumPlane_Top = 4,
		EFrustumPlane_Bottom = 5,
		EFrustumPlane_Count
	};

	struct Planes
	{
		nix::Vector4 m_normals[EFrustumPlane::EFrustumPlane_Count];
		nix::Scalar m_distances[EFrustumPlane::EFrustumPlane_Count];
	};


public:
	Frustum() {}
	~Frustum() {}

	void Update(const nix::Matrix4x4& _matrix);
	bool CheckSphere(const nix::Vector4& _pos, const nix::Scalar& _radius);
	bool CheckSphere(const nix::Vector4& _pos, float _radius);

private:
	Planes m_planes;
};

NW_NAMESPACE_END