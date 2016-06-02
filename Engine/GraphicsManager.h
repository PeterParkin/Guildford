#pragma once

#include "../Engine/Engine.h"
#include "../Engine/ConstantBuffers.h"

#include <DirectXMath.h>

struct ID3D11Buffer;

namespace Eng
{
	using namespace DirectX;
	class GraphicsManager
	{
	public:
		void SetGameTime(const GameTime& game_time)
		{
			_VertexShader0._DeltaTime = (float)game_time.GetDeltaTime();
			_VertexShader0._Seconds = game_time.GetTime();
			_VertexShader0._FrameIndex = game_time.GetFrameIndex();
			_VertexShader0Dirty = 1;
		}
		void SetWidthHeightFOV(float width, float height, float fov_y)
		{
			_VertexShader0._TextureWidth = width;
			_VertexShader0._TextureHeight = height;
			_VertexShader0Dirty = 1;
			_Projection = XMMatrixPerspectiveFovLH(fov_y, height / width, 0.001f, 5000.f);
			_ProjectionDirty = 1;
		}
		void SetCamera(FXMMATRIX matrix)
		{
			_InverseView = matrix;
			_InverseViewDirty = 1;
		}
		void SetWorld(FXMMATRIX matrix)
		{
			_World = matrix;
			_WorldDirty = 1;
		}
		ENGINE_API void CommitConstantBuffers();
		GraphicsManager();
		~GraphicsManager();
	private:
		ID3D11Buffer* ConstantBuffer[ECB_Num];
		VS_0 _VertexShader0;
		VS_1 _VertexShader1;
		PS_0 _PixelShader0;
		XMMATRIX _World;
		XMMATRIX _View;
		XMMATRIX _InverseView;
		XMMATRIX _Projection;
		unsigned _VertexShader0Dirty : 1;
		unsigned _WorldDirty : 1;
		unsigned _ViewDirty : 1;
		unsigned _InverseViewDirty : 1;
		unsigned _ProjectionDirty : 1;
	};
}