#include "Frustum.h"


NW_NAMESPACE_BEGIN

void Frustum::Update(const nix::Matrix4x4& _matrix)
{
	const nix::Vector4 viewProjMatrixAxisX = _matrix[0];
	const nix::Vector4 viewProjMatrixAxisY = _matrix[1];
	const nix::Vector4 viewProjMatrixAxisZ = _matrix[2];
	const nix::Vector4 viewProjMatrixAxisW = _matrix[3];


	const nix::Vector4 addWX = viewProjMatrixAxisW + viewProjMatrixAxisX;
	const nix::Vector4 subWX = viewProjMatrixAxisW - viewProjMatrixAxisX;
	const nix::Vector4 addWY = viewProjMatrixAxisW + viewProjMatrixAxisY;
	const nix::Vector4 subWY = viewProjMatrixAxisW - viewProjMatrixAxisY;
	const nix::Vector4 addWZ = viewProjMatrixAxisW + viewProjMatrixAxisZ;
	const nix::Vector4 subWZ = viewProjMatrixAxisW - viewProjMatrixAxisZ;


	//
	// normals
	m_planes.m_normals[EFrustumPlane::EFrustumPlane_Left] = addWX;
	m_planes.m_normals[EFrustumPlane::EFrustumPlane_Right] = subWX;
	m_planes.m_normals[EFrustumPlane::EFrustumPlane_Top] = subWY;
	m_planes.m_normals[EFrustumPlane::EFrustumPlane_Bottom] = addWY;
	m_planes.m_normals[EFrustumPlane::EFrustumPlane_Far] = addWZ;
	m_planes.m_normals[EFrustumPlane::EFrustumPlane_Near] = subWZ;

	//
	// distance
	m_planes.m_distances[EFrustumPlane::EFrustumPlane_Left] = nix::Scalar(nix::MathFunctions::Swizzle<nix::W, nix::W, nix::W, nix::W>(addWX));
	m_planes.m_distances[EFrustumPlane::EFrustumPlane_Right] = nix::Scalar(nix::MathFunctions::Swizzle<nix::W, nix::W, nix::W, nix::W>(subWX));

	m_planes.m_distances[EFrustumPlane::EFrustumPlane_Top] = nix::Scalar(nix::MathFunctions::Swizzle<nix::W, nix::W, nix::W, nix::W>(subWY));
	m_planes.m_distances[EFrustumPlane::EFrustumPlane_Bottom] = nix::Scalar(nix::MathFunctions::Swizzle<nix::W, nix::W, nix::W, nix::W>(addWY));

	m_planes.m_distances[EFrustumPlane::EFrustumPlane_Far] = nix::Scalar(nix::MathFunctions::Swizzle<nix::W, nix::W, nix::W, nix::W>(addWZ));
	m_planes.m_distances[EFrustumPlane::EFrustumPlane_Near] = nix::Scalar(nix::MathFunctions::Swizzle<nix::W, nix::W, nix::W, nix::W>(subWZ));

	for (uint8 i = 0; i < EFrustumPlane::EFrustumPlane_Count; ++i)
	{
		const nix::Vector4 len4 = m_planes.m_normals[i].Length();
		const nix::Scalar len(len4);

		m_planes.m_normals[i] /= len4;
		m_planes.m_distances[i] = m_planes.m_distances[i] / len;
	}
}

bool Frustum::CheckSphere(const nix::Vector4& _pos, const nix::Scalar& _radius)
{
	for (uint8 i = 0; i < EFrustumPlane::EFrustumPlane_Count; ++i)
	{
		nix::Scalar distanceCheck = nix::Scalar(m_planes.m_normals[i].Dot(_pos));
		distanceCheck = distanceCheck + m_planes.m_distances[i];
		nix::Boolean result = distanceCheck <= _radius;
		if (result.IsTrue())
		{
			return false;
		}
	}

	return true;
}

bool Frustum::CheckSphere(const nix::Vector4& _pos, float _radius)
{
	return CheckSphere(_pos, nix::Scalar(_radius));
}

NW_NAMESPACE_END