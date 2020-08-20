#include "Transform.h"


NW_NAMESPACE_BEGIN

nix::Matrix4x4 Transform::LookAt(const nix::Vector4& _eye, const nix::Vector4& _center, const nix::Vector4& _up)
{
	// make rotation matrix
	nix::Vector4 z = (_eye - _center).Normalize3();
	nix::Vector4 y = _up.Normalize3();
	nix::Vector4 x = y.Cross(z);
	y = z.Cross(x);

	// cross product gives area of parallelogram, which is < 1.0 for non-perpendicular unit-length vectors; so normalize x, y here
	x = x.Normalize3();
	y = y.Normalize3();

	// Slow, but for test I just want to see if is working right.
	const float xx = nix::MathFunctions::ExtractX(x);
	const float xy = nix::MathFunctions::ExtractY(x);
	const float xz = nix::MathFunctions::ExtractZ(x);

	const float yx = nix::MathFunctions::ExtractX(y);
	const float yy = nix::MathFunctions::ExtractY(y);
	const float yz = nix::MathFunctions::ExtractZ(y);

	const float zx = nix::MathFunctions::ExtractX(z);
	const float zy = nix::MathFunctions::ExtractY(z);
	const float zz = nix::MathFunctions::ExtractZ(z);

	const float eye_x = nix::MathFunctions::ExtractX(_eye);
	const float eye_y = nix::MathFunctions::ExtractY(_eye);
	const float eye_z = nix::MathFunctions::ExtractZ(_eye);

	return nix::Matrix4x4(
		xx, xy, xz, -xx * eye_x - xy * eye_y - xz * eye_z,
		yx, yy, yz, -yx * eye_x - yy * eye_y - yz * eye_z,
		zx, zy, zz, -zx * eye_x - zy * eye_y - zz * eye_z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

nix::Matrix4x4 Transform::PerspectiveProjection(float _fov, float _aspect, float _zNear, float _zFar)
{
	const float f = _zFar;
	const float n = _zNear;

	const float t = n * std::tanf(_fov * 0.5f);
	const float b = -t;

	const float l = b * _aspect;
	const float r = t * _aspect;

	return nix::Matrix4x4(
		(2.0f * n) / (r - l),			0.0f,					0.0f,				0.0f,
		0.0f,							-(2.0f * n) / (t - b),	0.0f,				0.0f,
		(r + l) / (r - l),				(t + b) / (t - b),		-(f) / (f - n),		-1.0f,
		0.0f,							0.0f,					(f * n) / (n - f),	0.0f
	);
}

nix::Matrix4x4 Transform::OrthographicProjection(float _left, float _right, float _bottom, float _top, float _zNear, float _zFar)
{
	return nix::Matrix4x4(
		2.0f / (_right - _left),		0.0f,							0.0f,						-(_right + _left) / (_right - _left),
		0.0f,							2.0f / (_top - _bottom),		0.0f,						-(_top + _bottom) / (_top - _bottom),
		0.0f,							0.0f,							-2.0f / (_zFar - _zNear),	-(_zFar + _zNear) / (_zFar - _zNear),
		0.0f,							0.0f,							0.0,						1.0f
	);
}


Transform::Transform(const nix::Vector4& _position, float _scale, const nix::Quaternion& _rotation)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_rotation);
}

Transform::Transform(const nix::Vector4& _position, float _scale, float _radians, const nix::Vector4& _axis)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_radians, _axis);
}

Transform::Transform(const nix::Vector4& _position, const nix::Vector4& _scale, const nix::Quaternion& _rotation)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_rotation);
}

Transform::Transform(const nix::Vector4& _position, const nix::Vector4& _scale, float _radians, const nix::Vector4& _axis)
{
    SetPosition(_position);
    SetScale(_scale);
    SetRotation(_radians, _axis);
}

void Transform::UpdateMatrix(const nix::Matrix4x4& _parentMatrix/* = kIdentity*/)
{
	if (m_dirty)
	{
		static const nix::Matrix4x4 identity;

		m_rotation = m_rotation.Normalize();
		const nix::Matrix4x4 rotate = m_rotation.ToMatrix();
		const nix::Matrix4x4 translate = identity.Translate(m_position);
		m_matrixLS = rotate * translate;

		m_rotation.SetFromMatrix(m_matrixLS);

		const nix::Matrix4x4 scale = identity.Scale(m_scale);
		m_matrixLS = scale * m_matrixLS;
		m_matrixWS = m_matrixLS * _parentMatrix;	// was in the SetMatrixWS

		m_dirty = false;
	}
}

void Transform::SetPosition(const nix::Vector4& _position)
{
    m_dirty = true;
    m_position = _position;
}

void Transform::SetPosition(float _x, float _y, float _z)
{
    m_dirty = true;
    m_position = nix::MathFunctions::Set(_x, _y, _z, 0.0f);
}

void Transform::SetScale(float _scale)
{
    m_dirty = true;
    m_scale = nix::MathFunctions::Splat(_scale);
}

void Transform::SetScale(const nix::Vector4& _scale)
{
    m_dirty = true;
    m_scale = _scale;
}

void Transform::SetScale(float _x, float _y, float _z)
{
    m_dirty = true;
    m_scale = nix::MathFunctions::Set(_x, _y, _z, 0.0f);
}

void Transform::SetRotation(const nix::Quaternion& _rotation)
{
    m_dirty = true;
    m_rotation = _rotation;
}

void Transform::SetRotation(float _radians, const nix::Vector4& _axis)
{
    m_dirty = true;
    m_rotation.SetFromAngleAxis(_radians, _axis);
}

void Transform::SetRotation(float _radians, float _x, float _y, float _z)
{
    m_dirty = true;
    m_rotation.SetFromAngleAxis(_radians, _x, _y, _z);
}

void Transform::SetRotation(float _pitch, float _yaw, float _roll)
{
    m_dirty = true;
    m_rotation.SetFromPitchYawRoll(_pitch, _yaw, _roll);
}

NW_NAMESPACE_END