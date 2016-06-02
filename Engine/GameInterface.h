#pragma once
#include <dxgiformat.h>
#include <memory>
#include "../Engine/VirtualKey.h"
#include "../Engine/Coords.h"
#include "../Engine/Engine.h"

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11CommandList;
class IGameInstance;

namespace Eng
{
	class IGameView
	{
	public:
		virtual ~IGameView() {}
		// Virtual key presses include mouse input.
		virtual void VirtualKey(Eng::EVirtualKey key, bool down, Eng::Point pos, int index = 0) = 0;
		virtual void MouseMove(float dx, float dy) = 0;
		virtual void FocusLost() = 0;
		virtual void FocusGained() = 0;
		// Usually just a single character, or a delete/backspace.
		// May also be used for automation.
		virtual void TextInput(const wchar_t* text) = 0;

		// Change the render target if we've just resized.
		virtual void SetRenderTarget(ID3D11Texture2D* texture, ID3D11RenderTargetView* view) = 0;
		virtual IGameInstance* GetGameInstance() = 0;
		virtual void Update(const GameTime& game_time) = 0;
	};
}

// No namespace for C linkage.
class IGameInstance
{
public:
	virtual ~IGameInstance() {}

	// Return true to request exit.
	virtual bool Update(const Eng::GameTime& game_time) = 0;

	// A single game can have multiple views.
	virtual Eng::IGameView* CreateGameView(ID3D11Texture2D* texture, ID3D11RenderTargetView* view, const char* name = nullptr ) = 0;
	virtual void GameViewDestroyed(Eng::IGameView* game_view) = 0;
};

// Each Game Module must implement these functions with C-style linkage so I can
// find them by text using the GetProcAddress on the loaded dll module.
typedef IGameInstance*(*CreateGameInstanceType)();
typedef void(*DestroyGameInstanceType)(IGameInstance*);
