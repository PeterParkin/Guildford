#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "../Engine/EngineCore.h"
#include "../Engine/Engine.h"
#include "../Engine/GameInterface.h"
#include "CommandLineInfo.h"
#include "../Engine/TaskGraph.h"
#include "../Engine/GameInstanceData.h"
#include <d3d11_1.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;

#pragma region Function Declarations
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void RegisterWindowClass(const char* window_classname, WNDPROC window_proc, HINSTANCE hInstance);
static void TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static HWND CreateGameWindow(HINSTANCE hInstance, const CommandLineInfo& cli);
static void RegisterForRawInput(HWND hwnd);
static int MessageLoop();
#pragma endregion

static UINT_PTR ModalTickTimer = 0;
static IGameInstance* _GameInstance = nullptr;
Eng::IGameView* _GameView = nullptr;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	using namespace Eng;
	int result = 0;
	HWND hwnd = nullptr;
	try
	{
		Engine e; // Destructor throws on memory leak in the CRT if an exception isn't in flight.
		try
		{
			CommandLineInfo cli(lpCmdLine);
			const char* module_name = cli.Get<const char*>("Module", "GameSpinningCube");
			GameInstanceData gid(module_name);
			hwnd = CreateGameWindow(hInstance, cli);
			RegisterForRawInput(hwnd);
			Engine::CreateDeviceAndSwapChain(hwnd);
			ShowWindow(hwnd, nCmdShow);
			_GameInstance = gid.CreateGameInstance();
			if (_GameInstance == nullptr)
			{
				ENGINE_LOG(Eng::Error, "Unable to create game instance");
			}
			_GameView = _GameInstance->CreateGameView(Engine::_BackBufferTexture, Engine::_BackBufferView);
			if (_GameView == nullptr)
			{
				ENGINE_LOG(Eng::Error, "Unable to create game view");
			}
			result = MessageLoop();
		}
		catch (Exception& e)
		{
			MessageBox(nullptr, e.what(), nullptr, 0);
		}
	}
	catch (Exception& e)
	{
		MessageBox(nullptr, e.what(), nullptr, 0);
	}
	catch (...)
	{
		MessageBox(nullptr, "Unknown exception", nullptr, 0);
	}
	return result;
}

static void RegisterWindowClass(const char* window_classname, WNDPROC window_proc, HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = window_classname;

	// register the window class
	RegisterClassEx(&wc);
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	using namespace Eng;
	switch (message)
	{
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_ERASEBKGND:
			// Prevent the background being erased.  Prevents flickering.
			return 0;

			// Painting handled by DirectX graphics, so implement a stub so the system believes the message was properly acted on.
			// Prevents wasteful rendering.
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			EVirtualKey key = static_cast<EVirtualKey>(wParam);
//			Engine::VirtualKeyDown(key);
		}
		break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			EVirtualKey key = static_cast<EVirtualKey>(wParam);
//			Engine::VirtualKeyUp(key);
		}
		break;

		case WM_CHAR:
//			Engine::CharacterInput((char)wParam);
			break;

		case WM_ACTIVATE:
			if ((wParam & 0xFFFF) == 0)
			{
//				Engine::SetBackground();
			}
			else
			{
//				Engine::SetForeground();
			}
			break;

		case WM_ENTERSIZEMOVE:
			ModalTickTimer = SetTimer(nullptr, 0, USER_TIMER_MINIMUM, (TIMERPROC)TimerProc);
			break;

		case WM_EXITSIZEMOVE:
			KillTimer(nullptr, ModalTickTimer);
			break;

		case WM_INPUT:
		{
			UINT dwSize = 40;
			static BYTE lpb[40];
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			RAWINPUT* raw = (RAWINPUT*)lpb;
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				float dx, dy;
				dx = (float)raw->data.mouse.lLastX;
				dy = (float)raw->data.mouse.lLastY;
				_GameView->MouseMove(dx, dy);
			}
			break;
		}

	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

static void TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	using namespace Eng;
//	Engine::Tick();
}

static HWND CreateGameWindow(HINSTANCE hInstance, const CommandLineInfo& cli)
{
	static const char* window_class_name = "GuildfordWindowClassName";
	static const char* title = "Title";
	
	auto width = cli.Get<int>("Width", 1024);
	auto height = cli.Get<int>("Height", 768);
	auto module_name = cli.Get<const char*>("Module", "GameSpinningCube");

	RegisterWindowClass(window_class_name, WindowProc, hInstance);
	RECT wr = { 0, 0, width, height };    // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size
														  // create the window and use the result as the handle
	HWND hWnd = CreateWindowEx(NULL,
		window_class_name,    // name of the window class
		module_name,   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		CW_USEDEFAULT,    // x-position of the window
		CW_USEDEFAULT,    // y-position of the window
		wr.right - wr.left,    // width of the window
		wr.bottom - wr.top,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL
	return hWnd;
}

void RegisterForRawInput(HWND hwnd)
{
	RAWINPUTDEVICE rid;
	rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid.usUsage = HID_USAGE_GENERIC_MOUSE;
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = hwnd;
	BOOL result = RegisterRawInputDevices(&rid, 1, sizeof(rid));
	if (result == FALSE)
	{
		ENGINE_LOG(Eng::Warning, "Unable to register for raw mouse movement");
	}
}

static int MessageLoop()
{
	MSG msg;
	Eng::GameTime game_time;
	for (;;)
	{
		// Check to see if any messages are waiting in the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);

			// check to see if it's time to quit
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else if (msg.message == WM_TIMER)
			{
				game_time.Update();
				if (_GameInstance->Update(game_time))
				{
					PostQuitMessage(-1);
				}
			}
		}
		else
		{
			game_time.Update();
			if (_GameInstance->Update(game_time))
			{
				PostQuitMessage(-1);
			}
		}
	}
	// return this part of the WM_QUIT message to Windows
	return (int)msg.wParam;
}
