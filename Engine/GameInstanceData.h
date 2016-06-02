#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../Engine/GameInterface.h"
#include "../Engine/Engine.h"

namespace Eng
{
	class GameInstanceData
	{
	public:
		GameInstanceData(const GameInstanceData&) = delete; // Disable copy constructor.
		GameInstanceData(const char* dll_name)
			: _Module(nullptr)
			, _CreateGameInstance(nullptr)
			, _DestroyGameInstance(nullptr)
		{
			_Module = LoadLibrary(dll_name);
			if (_Module == nullptr)
			{
				ENGINE_LOG(Error, "Unable to load %s", dll_name);
			}
			_CreateGameInstance = reinterpret_cast<CreateGameInstanceType>(GetProcAddress((HMODULE)_Module, "CreateGameInstance"));
			_DestroyGameInstance = reinterpret_cast<DestroyGameInstanceType>(GetProcAddress((HMODULE)_Module, "DestroyGameInstance"));
			if (_CreateGameInstance == nullptr || _DestroyGameInstance == nullptr)
			{
				ENGINE_LOG(Error, "Create/DestroyGameInstance missing for %s", dll_name);
			}
		}
		GameInstanceData(GameInstanceData&& other) // Move only.
			: _Module(other._Module)
			, _CreateGameInstance(other._CreateGameInstance)
			, _DestroyGameInstance(other._DestroyGameInstance)
		{
			other._Module = nullptr;
			other._CreateGameInstance = nullptr;
			other._DestroyGameInstance = nullptr;
		}
		~GameInstanceData() noexcept(false)
		{
			_CreateGameInstance = nullptr;
			_DestroyGameInstance = nullptr;
			if (_Module != nullptr)
			{
				FreeLibrary(_Module);
			}
			_Module = nullptr;
		}
		IGameInstance* CreateGameInstance() { return _CreateGameInstance(); }
	private:
		HMODULE _Module;
		CreateGameInstanceType _CreateGameInstance;
		DestroyGameInstanceType _DestroyGameInstance;
	};
}