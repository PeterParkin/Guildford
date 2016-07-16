#pragma once
//
// Engine.h, 
// EngineCore.h, 
// TaskGraph.h,
// are the only header files from the Engine module you should include outside the Engine module.
// The lifetime of these classes is determined by the lifetime of the Engine object, which
// is responsible for the handling of exceptions.
//
#ifndef ENGINE_SHIPPING // Defined in the project settings for Shipping and Profiling configurations.
// ENGINE_LOG throws an exception when used with the Error setting.
#define ENGINE_LOG(severity, text, ...) ::Eng::Engine::Log(__FILE__, __FUNCTION__, __LINE__, severity, text, __VA_ARGS__);
#define ENGINE_ASSERT(condition) if(!(condition)) ::Eng::Engine::Log(__FILE__, __FUNCTION__, __LINE__, ::Eng::Error, #condition);
#else
#define ENGINE_LOG(severity, text, ...)
#define ENGINE_ASSERT(condition)
#endif

#include "../Engine/EngineTypes.h"

struct ID3D11Device1;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DeviceChild;

namespace Eng
{
	enum ESeverity
	{
		Comment,
		Warning,
		Error
	};
	enum class EThreadType : uint8_t
	{
		UnknownThread,
		MainThread,
		TaskGraphThread,
	};
	class Engine
	{
	public:
		ENGINE_API Engine();
		ENGINE_API ~Engine() noexcept(false); // Destructor throws if there's a memory leak.
		ENGINE_API static void Log(const char* file, const char* function, int line, ESeverity severity, const char* format, ...);
		ENGINE_API static void SetThreadName(const char* name, EThreadType thread_type = EThreadType::UnknownThread);
		ENGINE_API static void TestCreateMemoryLeak();
		ENGINE_API static void TestPlacementNew();
		ENGINE_API static ExceptionContainer _ExceptionContainer;
		ENGINE_API static bool HasExceptions();
		ENGINE_API static void ThrowException();
		ENGINE_API static void Async(std::function<void()> function); // Must include lambda capture.
		ENGINE_API static void Load(const char* filename, std::function<void(std::shared_ptr<IoReadData>)> function);
		ENGINE_API static void Save(const char* filename, std::shared_ptr<IoReadData> data);
		ENGINE_API static void Dispatch(std::function<void()> function);
		ENGINE_API static EThreadType GetThreadType();
		ENGINE_API static ID3D11Device1* _Device;
		ENGINE_API static ID3D11DeviceContext* _ImmediateContext;
		ENGINE_API static IDXGISwapChain* _SwapChain;
		ENGINE_API static ID3D11Texture2D* _BackBufferTexture;
		ENGINE_API static ID3D11RenderTargetView* _BackBufferView;
		ENGINE_API static void CreateDeviceAndSwapChain(void* hwnd);
		static uint64_t CreateHash64(const char* text)
		{
			static const uint64_t FNV_Prime = 0x100000001b3;
			static const uint64_t FNV_Offset = 0xcbf29ce484222325;
			uint64_t result = FNV_Offset;
			int iter = 0;
			for (; text[iter] != NULL && iter < ENGINE_MAX_PATH; ++iter)
			{
				result ^= text[iter];
				result *= FNV_Prime;
			}
			ENGINE_ASSERT(iter != ENGINE_MAX_PATH);
			return result;
		}
		static uint32_t CreateHash32(const char* text)
		{
			union
			{
				uint64_t hash;
				struct
				{
					uint32_t first;
					uint32_t second;
				};
			};
			hash = CreateHash64(text);
			return first ^ second;
		}
		ENGINE_API static GraphicsManager* _GraphicsManager;
	private:
		static void ReleaseDirectX();
	};
}