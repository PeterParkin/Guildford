#pragma once

#include <DirectXMath.h>

namespace Eng
{
	using namespace DirectX;
	enum EConstantBuffer
	{
		ECB_VertexShader0,
		ECB_VertexShader1,
		ECB_PixelShader0,
		ECB_Num,
	};
	struct alignas(16) VS_0
	{
		float _TextureWidth, _TextureHeight, _Seconds, _DeltaTime;
		uint32_t _FrameIndex;
		VS_0()
			: _TextureWidth(0)
			, _TextureHeight(0)
			, _Seconds(0)
			, _DeltaTime(0)
			, _FrameIndex(0)
		{
		}
	};
	struct alignas(16) VS_1
	{
		XMVECTOR _World[3];
		XMMATRIX _WorldViewProjection;
		VS_1()
		{
			_World[0] = XMVectorSet(1, 0, 0, 1);
			_World[1] = XMVectorSet(0, 1, 0, 1);
			_World[2] = XMVectorSet(0, 0, 1, 1);
			_WorldViewProjection = XMMatrixIdentity();
		}
	};
	struct alignas(16) PS_0
	{
		XMMATRIX _InverseViewProjection; // clip space to world space.
		XMMATRIX _InverseProjection; // clip space to Camera space.
		PS_0()
		{
			_InverseViewProjection = XMMatrixIdentity();
			_InverseProjection = XMMatrixIdentity();
		}
	};
}