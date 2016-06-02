#include "GameSpinningCubePCH.h"
#include "GameSpinningCube.h"
namespace Shaders
{
#pragma push_macro("BYTE")
#define BYTE uint8_t
#include "VertexShader.h"
#include "PixelShader.h"
#pragma pop_macro("BYTE")
}
#include "../Engine/Engine.h"
#include <d3d11_1.h>

#pragma comment( lib, "dxguid.lib")

extern "C" __declspec(dllexport) DXGI_FORMAT GetRequiredTextureFormat()
{
	return DXGI_FORMAT_B8G8R8A8_UNORM;
}
extern "C" __declspec(dllexport) IGameInstance* CreateGameInstance(ID3D11Texture2D* texture, ID3D11RenderTargetView* view)
{
	return new SpinningCube::InstanceSpinningCube();
}
extern "C" __declspec(dllexport) void DestroyGameInstance(IGameInstance* instance)
{
	delete static_cast<SpinningCube::InstanceSpinningCube*>(instance);
}

namespace SpinningCube
{
	using namespace Eng;
	using namespace DirectX;
	SpinningCubeSharedData* InstanceSpinningCube::_Shared = nullptr;
	int InstanceSpinningCube::_SharedRefCount = 0;
	ViewSpinningCube::ViewSpinningCube(IGameInstance* game_instance)
		: _GameInstance(game_instance)
		, _CameraPitch(0)
		, _CameraYaw(0)
		, _CameraVel(0,0,0)
		, _Acceleration(3.f)
		, _Braking(10.f)
		, _MaxSpeed(10.f)
		, _MouseRadiansPerCm(XM_PI / 4.f ) // 45 degrees per cm motion.
		, _FOVY(Deg2Rad(75.f))
		, _ForwardDown(0)
		, _BackwardDown(0)
		, _LeftDown(0)
		, _RightDown(0)
		, _UpDown(0)
		, _DownDown(0)
	{
		_CameraMatrix = XMMatrixTranslation(0, 1, -4);
	}

	ViewSpinningCube::~ViewSpinningCube()
	{
		_GameInstance->GameViewDestroyed(this);
	}

	void ViewSpinningCube::VirtualKey(EVirtualKey key, bool down, Point point, int index)
	{
		unsigned state = down ? 1 : 0;
		switch (key)
		{
			case EVirtualKey::W:
				_ForwardDown = state;
				break;
			case EVirtualKey::S:
				_BackwardDown = state;
				break;
			case EVirtualKey::A:
				_LeftDown = state;
				break;
			case EVirtualKey::D:
				_RightDown = state;
				break;
			case EVirtualKey::Space:
				_UpDown = state;
				break;
			case EVirtualKey::Shift:
				_DownDown = state;
				break;
		}
	}
	void ViewSpinningCube::MouseMove(float dx, float dy)
	{
		_MouseX += dx;
		_MouseY += dy;
	}

	void ViewSpinningCube::FocusLost()
	{
		_ForwardDown = 0;
		_BackwardDown = 0;
		_LeftDown = 0;
		_RightDown = 0;
		_UpDown = 0;
		_DownDown = 0;
		_MouseX = 0;
		_MouseY = 0;
	}
	void ViewSpinningCube::FocusGained()
	{

	}

	void ViewSpinningCube::Update(const GameTime& game_time)
	{
		float rgba[4] = { 0.f, 0.5f, 1.f, 1.f };
		Engine::_ImmediateContext->ClearRenderTargetView(Engine::_BackBufferView, rgba);
		ApplyControls(game_time);
		MoveCamera(game_time);
		DrawScene(game_time);
		Engine::_SwapChain->Present(1, 0);
	}

	inline void Accelerate(float& velocity, float acceleration, float time, float limit)
	{
		velocity = CLAMP(velocity + acceleration * time, -limit, limit);
	}
	inline void Brake(float& velocity, float braking, float time)
	{
		velocity = velocity > 0 ? MAX(velocity - braking * time, 0) : MIN(velocity + braking * time, 0);
	}

	void ViewSpinningCube::ApplyControls(const GameTime& game_time)
	{
		// Acceleration first.
		Vector3 acceleration;
		acceleration._m[0] = (float)(_RightDown - _LeftDown);
		acceleration._m[1] = (float)(_UpDown - _DownDown);
		acceleration._m[2] = (float)(_ForwardDown - _BackwardDown);
		acceleration.Normalize();
	}
	void ViewSpinningCube::MoveCamera(const GameTime& game_time)
	{
		// Pitch up doesn't affect the X vector.
		XMMATRIX yaw = XMMatrixRotationY(_CameraYaw);
		XMMATRIX pitch = XMMatrixRotationX(_CameraPitch);
		XMMATRIX new_rotation = XMMatrixMultiply(yaw, pitch); // Pitch first, then yaw.
		XMVECTOR& right = new_rotation.r[0];
		XMVECTOR& up = new_rotation.r[1];
		XMVECTOR& forward = new_rotation.r[2];
		XMVECTOR delta_position = right * _CameraVel._m[0]
			+ up * _CameraVel._m[1]
			+ forward * _CameraVel._m[2];
		XMVECTOR& camera_position = _CameraMatrix.r[3];
		_CameraMatrix.r[0] = new_rotation.r[0];
		_CameraMatrix.r[1] = new_rotation.r[1];
		_CameraMatrix.r[2] = new_rotation.r[2];
		_CameraMatrix.r[3] += delta_position;
		Engine::_GraphicsManager->SetCamera(_CameraMatrix);
	}
	void ViewSpinningCube::DrawScene(const GameTime& game_time)
	{

	}
	void ViewSpinningCube::TextInput(const wchar_t * text)
	{
	}
	void ViewSpinningCube::SetRenderTarget(ID3D11Texture2D * texture, ID3D11RenderTargetView * view)
	{
	}
	struct VertexStruct
	{
		float _X, _Y, _Z;
		unsigned _RGBA;
	};
	struct SpinningCubeSharedData
	{
		ID3D11Buffer* _VertexBuffer;
		ID3D11Buffer* _IndexBuffer;
		ID3D11VertexShader* _VertexShader;
		ID3D11PixelShader* _PixelShader;
		SpinningCubeSharedData()
			: _VertexBuffer(nullptr)
			, _IndexBuffer(nullptr)
			, _VertexShader(nullptr)
			, _PixelShader(nullptr)
		{
			HRESULT hr;
			hr = Engine::_Device->CreateVertexShader(Shaders::VertexShader, sizeof(Shaders::VertexShader), nullptr, &_VertexShader);
			if (FAILED(hr))
			{
				ENGINE_THROW("Failed to create vertex shader");
			}
			ENGINE_SET_DEBUG_OBJECT_NAME(_VertexShader, "VertexShader");
			hr = Engine::_Device->CreatePixelShader(Shaders::PixelShader, sizeof(Shaders::PixelShader), nullptr, &_PixelShader);
			if (FAILED(hr))
			{
				ENGINE_THROW("Failed to create pixel shader");
			}
			ENGINE_SET_DEBUG_OBJECT_NAME(_PixelShader, "PixelShader");
			VertexStruct vertex_buffer_data[] =
			{
				{ -1,  1, -1, 0xFF000000 },
				{  1,  1, -1, 0xFFFFFF00 },
				{  1,  1,  1, 0x00FF0000 },
				{ -1,  1,  1, 0xFFFF0000 },
				{ -1, -1, -1, 0xFF00FF00 },
				{  1, -1, -1, 0x0000FF00 },
				{  1, -1,  1, 0x00FFFF00 },
				{ -1, -1,  1, 0x00000000 },
			};
			D3D11_BUFFER_DESC vertex_buffer_desc = { 0 };
			vertex_buffer_desc.ByteWidth = sizeof(vertex_buffer_data);
			vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertex_buffer_desc.CPUAccessFlags = 0;
			vertex_buffer_desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertex_buffer_subresource = { 0 };
			vertex_buffer_subresource.pSysMem = vertex_buffer_data;
			hr = Engine::_Device->CreateBuffer(&vertex_buffer_desc, &vertex_buffer_subresource, &_VertexBuffer);
			ENGINE_SET_DEBUG_OBJECT_NAME(_VertexBuffer, "VertexBuffer");
			if (FAILED(hr))
			{
				ENGINE_THROW("Failed to create vertex buffer");
			}
			uint16_t index_buffer_data[] =
			{
				0, 1, 2, 0, 2, 3,
				7, 4, 0, 7, 0, 3,
				4, 5, 1, 4, 1, 0,
				5, 6, 2, 5, 2, 1,
				6, 7, 3, 6, 3, 2,
				7, 6, 5, 7, 5, 4,
			};
			D3D11_BUFFER_DESC index_buffer_desc = { 0 };
			index_buffer_desc.ByteWidth = sizeof(index_buffer_data);
			index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			index_buffer_desc.CPUAccessFlags = 0;
			index_buffer_desc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA index_buffer_subresource = { 0 };
			index_buffer_subresource.pSysMem = index_buffer_data;
			hr = Engine::_Device->CreateBuffer(&index_buffer_desc, &index_buffer_subresource, &_IndexBuffer);
			if (FAILED(hr))
			{
				ENGINE_THROW("Failed to create index buffer");
			}
		}
		~SpinningCubeSharedData()
		{
			SAFE_RELEASE(_VertexBuffer);
			SAFE_RELEASE(_IndexBuffer);
			SAFE_RELEASE(_VertexShader);
			SAFE_RELEASE(_PixelShader);
		}
	};
	InstanceSpinningCube::InstanceSpinningCube()
		: _CubeMatrix( XMMatrixIdentity() )
	{
		if (Engine::GetThreadType() != EThreadType::MainThread)
		{
			ENGINE_LOG(Warning, "Do not create InstanceSpinningCube outside the main thread");
		}
		if (_SharedRefCount++ == 0)
		{
			_Shared = new SpinningCubeSharedData();
		}
	}
	InstanceSpinningCube::~InstanceSpinningCube()
	{
		if (--_SharedRefCount == 0)
		{
			delete _Shared;
			_Shared = nullptr;
		}
	}
	bool InstanceSpinningCube::Update(const GameTime& game_time)
	{
		// Rotate the cube.

		return false;
	}
	IGameView* InstanceSpinningCube::CreateGameView(ID3D11Texture2D * texture, ID3D11RenderTargetView * view, const char * name)
	{
		IGameView* new_view = new ViewSpinningCube(this);
		_GameViewArray.push_back(new_view);
		return new_view;
	}
	void InstanceSpinningCube::GameViewDestroyed(IGameView* game_view)
	{
		if (_GameViewArray.size() == 0)
		{
			ENGINE_LOG(Error, "Attempt to remove GameView from empty array");
		}
		for (auto iter = _GameViewArray.begin(); iter != _GameViewArray.end(); ++iter)
		{
			if (*iter == game_view)
			{
				if (iter == _GameViewArray.end() - 1)
				{
					_GameViewArray.erase(iter);
				}
				else
				{
					*iter = _GameViewArray.back();
					_GameViewArray.erase(_GameViewArray.end() - 1);
				}
				return;
			}
		}
		ENGINE_LOG(Error, "Unable to find GameView in array");
	}
}

