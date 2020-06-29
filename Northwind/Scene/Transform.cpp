#include "Transform.h"


NW_NAMESPACE_BEGIN

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