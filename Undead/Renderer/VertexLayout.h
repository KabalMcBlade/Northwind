#pragma once

#include "../Dependencies/Nix/Nix/Nix.h"

#include "../Core/BasicTypes.h"

#define UD_NORMALIZED_VERTEX_DIV_WEIGHT_BYTE	0.0039215686274509803921568627450980392156862745098039215686f	// 1.0f / 255.0f
#define UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT	0.0000152590218966964217593652246890974288548104066529335469f	// 1.0f / 65535.0f

#define UD_VERTEX_DIV_RANGE_BYTE                0.0078431372549019607843137254901960784313725490196078431372f	// 2.0f / 255.0f
#define UD_VERTEX_DIV_RANGE_SHORT               0.0000305180437933928435187304493781948577096208133058670939f	// 2.0f / 65535.0f

#define UD_VERTEX_BYTE_TO_FLOAT(x)				( (x) * ( UD_VERTEX_DIV_RANGE_BYTE ) - 1.0f )
#define UD_VERTEX_SHORT_TO_FLOAT(x)				( (x) * ( UD_VERTEX_DIV_RANGE_SHORT ) - 1.0f )




UD_NAMESPACE_BEGIN

enum EVertexLayout
{
	EVertexLayout_Full,
	EVertexLayout_ColorOnly,
	EVertexLayout_TextureOnly
};

/*
// 12 + 12 + 16 + 8 + 8 + 16 + 16 + 8 = 96
float			m_position[3];
float			m_normal[3];
float			m_tangent[4];
float			m_texCoord0[2];
float			m_texCoord1[2];
float			m_color[4];
float			m_weights[4];
uint16			m_joints[4];
*/

// 96
UD_MEMORY_ALIGNED struct Vertex
{
	nix::Vector4    m_position;		// 16 byte
	nix::Vector4    m_texUV;		// 16 byte	// UV is UV0 and UV1
	nix::Vector4    m_normal;		// 16 byte
	nix::Vector4	m_tangent;		// 16 byte
	nix::Vector4	m_color;		// 16 byte
	uint16			m_weights[4];	// 8 byte
	uint16			m_joints[4];	// 8 byte

	Vertex() {}

	UD_INLINE void Clear()
	{
		m_position = nix::Vector4(nix::kZero);
		m_texUV = nix::Vector4(nix::kZero);
		m_normal = nix::Vector4(nix::kZero);
		m_tangent = nix::Vector4(nix::kZero);
		m_color = nix::Vector4(nix::kZero);
		memset(m_weights, 0, sizeof(m_weights));
		memset(m_joints, 0, sizeof(m_joints));
	}

	UD_INLINE nix::Vector4 GetPosition() const { return m_position; }
	UD_INLINE nix::Vector4 GetNormal() const { return m_normal; }
	UD_INLINE nix::Vector4 GetTangent() const { return m_tangent; }
	UD_INLINE float GetBiTangentSign() const { return nix::MathFunctions::ExtractW(m_tangent) < 0.0f ? -1.0f : 1.0f; }
	UD_INLINE nix::Vector4 GetBiTangent() const
	{
		const nix::Vector4& v = GetNormal();
		nix::Vector4 b = v.Cross(GetTangent());
		b *= GetBiTangentSign();
		return b;
	}
	UD_INLINE nix::Vector4 GetColor() const { return m_color; }
	UD_INLINE nix::Vector4 GetTexUVUV() const { return m_texUV; }
	UD_INLINE nix::Vector4 GetWeights() const 
	{ 
		return nix::Vector4(UD_VERTEX_SHORT_TO_FLOAT(m_weights[0]), UD_VERTEX_SHORT_TO_FLOAT(m_weights[1]), UD_VERTEX_SHORT_TO_FLOAT(m_weights[2]), UD_VERTEX_SHORT_TO_FLOAT(m_weights[3]));
	}

	UD_INLINE int16 GetJoint0() { return m_joints[0]; }
	UD_INLINE int16 GetJoint1() { return m_joints[1]; }
	UD_INLINE int16 GetJoint2() { return m_joints[2]; }
	UD_INLINE int16 GetJoint3() { return m_joints[3]; }

	UD_INLINE void SetPosition(const nix::Vector4& _position) { m_position = _position; }
	UD_INLINE void SetPosition(float _x, float _y, float _z) { m_position = nix::Vector4(_x, _y, _z, 1.0f); }
	UD_INLINE void SetNormal(const nix::Vector4& _normal) { m_normal = _normal; }
	UD_INLINE void SetNormal(float _x, float _y, float _z) { m_normal = nix::Vector4(_x, _y, _z, 1.0f); }
	UD_INLINE void SetTangent(const nix::Vector4& _tangent) { m_tangent = _tangent; }
	UD_INLINE void SetTangent(float _x, float _y, float _z) { m_tangent = nix::Vector4(_x, _y, _z, 1.0f); }
	UD_INLINE void SetColor(const nix::Vector4& _color) { m_color = _color; }
	UD_INLINE void SetColor(float _r, float _g, float _b, float _a) { m_color = nix::Vector4(_r, _g, _b, _a); }
	UD_INLINE void SetWeights(float _x, float _y, float _z, float _w) 
	{ 
		m_weights[0] = static_cast<uint16>(_x / UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT);
		m_weights[1] = static_cast<uint16>(_y / UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT);
		m_weights[2] = static_cast<uint16>(_z / UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT);
		m_weights[3] = static_cast<uint16>(_w / UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT);
	}
	UD_INLINE void SetBiTangentSign(float _sign) { nix::MathFunctions::InsertOnW(m_tangent, _sign < 0.0f ? 0.0f : 1.0f); }
	UD_INLINE void SetBiTangent(const nix::Vector4& _tangent)
	{
		const nix::Vector4& v = GetNormal();
		nix::Vector4 b = v.Cross(GetTangent());
		SetBiTangentSign(nix::MathFunctions::ExtractX(b.Dot3(_tangent)));
	}
	UD_INLINE void SetBiTangent(float _x, float _y, float _z)
	{
		nix::Vector4 v(_x, _y, _z, 0.0f);
		SetBiTangent(v);
	}
	UD_INLINE void SetTexUVUV(const nix::Vector4& _uvuv) { m_texUV = _uvuv; }
	UD_INLINE void SetTexUV0(float _u, float _v)
	{
		static const nix::Vector4 maskIn(1.0f, 1.0f, 0.0f, 0.0f);
		static const nix::Vector4 maskOut(0.0f, 0.0f, 1.0f, 1.0f);

		nix::Vector4 v(_u, _v, 0.0f, 0.0f);
		m_texUV = v * maskIn + m_texUV * maskOut;
	}
	UD_INLINE void SetTexUV0(const nix::Vector4& _uv0)
	{
		static const nix::Vector4 maskIn(1.0f, 1.0f, 0.0f, 0.0f);
		static const nix::Vector4 maskOut(0.0f, 0.0f, 1.0f, 1.0f);

		m_texUV = _uv0 * maskIn + m_texUV * maskOut;
	}
	UD_INLINE void SetTexUV1(float _u, float _v)
	{
		static const nix::Vector4 maskIn(0.0f, 0.0f, 1.0f, 1.0f);
		static const nix::Vector4 maskOut(1.0f, 1.0f, 0.0f, 0.0f);

		nix::Vector4 v(0.0f, 0.0f, _u, _v);
		m_texUV = v * maskIn + m_texUV * maskOut;
	}
	UD_INLINE void SetTexUV1(const nix::Vector4& _uv1)
	{
		static const nix::Vector4 maskIn(0.0f, 0.0f, 1.0f, 1.0f);
		static const nix::Vector4 maskOut(1.0f, 1.0f, 0.0f, 0.0f);

		m_texUV = _uv1 * maskIn + m_texUV * maskOut;
	}
	UD_INLINE void SetJoint0(uint16 _joint) { m_joints[0] = _joint; }
	UD_INLINE void SetJoint1(uint16 _joint) { m_joints[1] = _joint; }
	UD_INLINE void SetJoint2(uint16 _joint) { m_joints[2] = _joint; }
	UD_INLINE void SetJoint3(uint16 _joint) { m_joints[3] = _joint; }
	UD_INLINE void SetJoint(uint16 _joint0, uint16 _joint1, uint16 _joint2, uint16 _joint3)
	{
		SetJoint0(_joint0);
		SetJoint1(_joint1);
		SetJoint2(_joint2);
		SetJoint3(_joint3);
	}

	UD_INLINE void Lerp(const Vertex& _a, const Vertex& _b, const float _t)
	{
		m_position = _a.GetPosition().LerpTo(_b.GetPosition(), _t);
		m_texUV = _a.GetTexUVUV().LerpTo(_b.GetTexUVUV(), _t);
		m_color = _a.GetColor().LerpTo(_b.GetColor(), _t);

		m_normal = (_a.GetNormal().LerpTo(_b.GetNormal(), _t).Normalize());
		m_tangent = (_a.GetTangent().LerpTo(_b.GetTangent(), _t).Normalize());
		
		m_weights[0] = (uint16)(_a.m_weights[0] + _t * (_b.m_weights[0] - _a.m_weights[0]));
		m_weights[1] = (uint16)(_a.m_weights[1] + _t * (_b.m_weights[1] - _a.m_weights[1]));
		m_weights[2] = (uint16)(_a.m_weights[2] + _t * (_b.m_weights[2] - _a.m_weights[2]));
		m_weights[3] = (uint16)(_a.m_weights[3] + _t * (_b.m_weights[3] - _a.m_weights[3]));

		m_joints[0] = (uint16)(_a.m_joints[0] + _t * (_b.m_joints[0] - _a.m_joints[0]));
		m_joints[1] = (uint16)(_a.m_joints[1] + _t * (_b.m_joints[1] - _a.m_joints[1]));
		m_joints[2] = (uint16)(_a.m_joints[2] + _t * (_b.m_joints[2] - _a.m_joints[2]));
		m_joints[3] = (uint16)(_a.m_joints[3] + _t * (_b.m_joints[3] - _a.m_joints[3]));

		SetBiTangent(m_tangent);
	}

	// 4 joints only
	UD_INLINE Vertex GetSkinnedVertex(const nix::Matrix4x4* _joints)
	{
		if (_joints == nullptr)
		{
			return *this;
		}

		const nix::Matrix4x4& j0 = _joints[static_cast<uint32>(m_joints[0])];
		const nix::Matrix4x4& j1 = _joints[static_cast<uint32>(m_joints[1])];
		const nix::Matrix4x4& j2 = _joints[static_cast<uint32>(m_joints[2])];
		const nix::Matrix4x4& j3 = _joints[static_cast<uint32>(m_joints[3])];

		const float w0 = m_weights[0] * UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT;
		const float w1 = m_weights[1] * UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT;
		const float w2 = m_weights[2] * UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT;
		const float w3 = m_weights[3] * UD_NORMALIZED_VERTEX_DIV_WEIGHT_SHORT;

		nix::Matrix4x4 accum = j0 * w0;
		accum += j1 * w1;
		accum += j2 * w2;
		accum += j3 * w3;

		Vertex result;
		result.m_position = accum * m_position;

		result.SetTexUVUV(GetTexUVUV());
		result.SetNormal(accum * GetNormal());
		result.SetTangent(accum * GetTangent());
		result.SetBiTangent(result.GetTangent());

		for (uint32 i = 0; i < 4; ++i)
		{
			result.m_joints[i] = m_joints[i];
			result.m_weights[i] = m_weights[i];
		}
		return result;
	}
};

UD_NAMESPACE_END