#pragma once

#include <DirectXMath.h>

namespace Eng
{
	using namespace DirectX;
	struct Vector3
	{
		Vector3() {}
		Vector3(float x, float y, float z)
			: _m{ x, y, z }
		{
		}
		Vector3& operator+=(const Vector3& other)
		{
			XMVECTORF32 left = { _m[0], _m[1], _m[2], 0 };
			XMVECTORF32 right = { other._m[0], other._m[1], other._m[2], 0 };
			XMVECTOR result = XMVectorAdd(left, right);
			_m[0] = result.m128_f32[0];
			_m[1] = result.m128_f32[1];
			_m[2] = result.m128_f32[2];
		}
		Vector3 operator*(float param)
		{
			XMVECTOR current = { _m[0], _m[1], _m[2], 0 };
			XMVECTOR result = XMVectorScale(current, param);
			return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
		}
		void Normalize()
		{
			XMVECTOR result = { _m[0], _m[1], _m[2], 0 };
			XMVector3Normalize(result);
			_m[0] = result.m128_f32[0];
			_m[1] = result.m128_f32[1];
			_m[2] = result.m128_f32[2];
		}
		float _m[3];
	};
	struct Quat
	{
		Quat() {}
		Quat(float x, float y, float z, float w)
			: _m{ x, y, z, w }
		{
		}
		Quat& operator*=(const Quat& other)
		{
			XMVECTORF32 left = { _m[0], _m[1], _m[2], _m[3] };
			XMVECTORF32 right = { other._m[0], other._m[1], other._m[2], other._m[3] };
			XMVECTOR result = XMVectorMultiply(left, right);
			_m[0] = result.m128_f32[0];
			_m[1] = result.m128_f32[1];
			_m[2] = result.m128_f32[2];
			_m[3] = result.m128_f32[3];
		}
		float _m[4];
	};
	struct Point
	{
		float _X, _Y;
	};
	struct Rect : Point
	{
		float _Width, _Height;
	};
}