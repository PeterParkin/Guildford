#pragma once

#include "../Engine/GameInterface.h"
#include "../Engine/Coords.h"
#include "../Engine/Engine.h"
#include <vector>
#include <DirectXMath.h>

namespace SpinningCube
{
	class ViewSpinningCube : public Eng::IGameView
	{
	public:
		ViewSpinningCube(IGameInstance* game_instance);
		~ViewSpinningCube();
		// <IGameView>
		void VirtualKey(Eng::EVirtualKey key, bool down, Eng::Point p, int index = 0) override;
		void MouseMove(float dx, float dy) override;
		void FocusLost() override;
		void FocusGained() override;
		void Update(const Eng::GameTime& game_time) override;
		void ApplyControls(const Eng::GameTime& game_time);
		void MoveCamera(const Eng::GameTime& game_time);
		void DrawScene(const Eng::GameTime& game_time);
		void TextInput(const wchar_t* text) override;
		void SetRenderTarget(ID3D11Texture2D* texture, ID3D11RenderTargetView* view) override;
		IGameInstance* GetGameInstance() override
		{
			return _GameInstance;
		}
		// </IGameView>
	private:
		IGameInstance* _GameInstance;
		float _CameraPitch, _CameraYaw;
		Eng::Vector3 _CameraVel; // In Right/Up/Forward local camera coordinates.
		float _Acceleration;
		float _Braking;
		float _MaxSpeed;
		float _MouseRadiansPerCm;
		DirectX::XMMATRIX _CameraMatrix;

		float _FOVY;
		unsigned _ForwardDown : 1;
		unsigned _BackwardDown : 1;
		unsigned _LeftDown : 1;
		unsigned _RightDown : 1;
		unsigned _UpDown : 1;
		unsigned _DownDown : 1;
		float _MouseX;
		float _MouseY;
	};
	struct SpinningCubeSharedData;
	class InstanceSpinningCube : public ::IGameInstance
	{
	public:
		InstanceSpinningCube();
		~InstanceSpinningCube();
		// <IGameInstance>
		bool Update(const Eng::GameTime& game_time) override;
		Eng::IGameView* CreateGameView(ID3D11Texture2D* texture, ID3D11RenderTargetView* view, const char* name = nullptr) override;
		void GameViewDestroyed(Eng::IGameView* game_view) override;
		// </IGameInstance>
	private:
		Eng::Vector3 _CubePos;
		Eng::Vector3 _CubeRollPitchYaw;
		DirectX::XMMATRIX _CubeMatrix;
		std::vector<Eng::IGameView*> _GameViewArray;
		static SpinningCubeSharedData* _Shared; // Cube mesh and shaders.
		static int _SharedRefCount;
	};
}
