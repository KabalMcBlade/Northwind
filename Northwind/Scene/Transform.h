#pragma once

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/BasicDefines.h"
#include "../Core/BasicTypes.h"
#include "../Core/MemoryWrapper.h"


NW_NAMESPACE_BEGIN

class NW_DLL Transform final
{
public:
	static nix::Matrix4x4 LookAt(const nix::Vector4& _eye, const nix::Vector4& _center, const nix::Vector4& _up);
	static nix::Matrix4x4 PerspectiveProjection(float _fov, float _aspect, float _zNear, float _zFar);
	static nix::Matrix4x4 OrthographicProjection(float _left, float _right, float _bottom, float _top, float _zNear, float _zFar);

public:
	static const nix::Matrix4x4 kIdentity;

	Transform() : m_rotation(), m_scale(1.0f, 1.0f, 1.0f, 0.0f), m_position(0.0f, 0.0f, 0.0f, 1.0f), m_dirty(true) {}
	~Transform() {}

    Transform(const nix::Vector4& _position, float _scale, const nix::Quaternion& _rotation);
    Transform(const nix::Vector4& _position, float _scale, float _radians, const nix::Vector4& _axis);

    Transform(const nix::Vector4& _position, const nix::Vector4& _scale, const nix::Quaternion& _rotation);
    Transform(const nix::Vector4& _position, const nix::Vector4& _scale, float _radians, const nix::Vector4& _axis);

    //void SetMatrixWS(const nix::Matrix4x4& _matrixWS) { m_matrixWS = _matrixWS; }

	void UpdateMatrix(const nix::Matrix4x4& _parentMatrix = kIdentity);

	NW_INLINE const nix::Matrix4x4& GetMatrixWS() const { return m_matrixWS; }
	NW_INLINE const nix::Matrix4x4& GetMatrixLS() const { return m_matrixLS; }

	NW_INLINE const nix::Vector4& GetPosition() const { return m_position; }
	NW_INLINE const nix::Vector4& GetScale() const { return m_scale; }
	NW_INLINE const nix::Quaternion& GetRotation() const { return m_rotation; }

    void SetPosition(const nix::Vector4& _position);
    void SetPosition(float _x, float _y, float _z);
    void SetScale(float _scale);
    void SetScale(float _x, float _y, float _z);
    void SetScale(const nix::Vector4& _scale);
    void SetRotation(const nix::Quaternion& _rotation);
    void SetRotation(float _radians, const nix::Vector4& _axis);
    void SetRotation(float _radians, float _x, float _y, float _z);
    void SetRotation(float _pitch, float _yaw, float _roll);

private:
    Transform(const Transform& _orig) = delete;    
    Transform& operator = (const Transform&) = delete;

	nix::Matrix4x4 m_matrixWS;
	nix::Matrix4x4 m_matrixLS;
	nix::Vector4 m_position;
	nix::Vector4 m_scale;
	nix::Quaternion m_rotation;

    bool m_dirty;
};

NW_NAMESPACE_END