#include "EnginePCH.h"
#include "Engine.h"
#include "Task.h"
#include "IoReadRequest.h"
#include "IoWriteRequest.h"
#include "Dispatcher.h"
#include "GraphicsManager.h"
#include <d3d11_1.h>
#include <dxgi.h>

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment( lib, "dxguid.lib")

#if ENGINE_USE_DEBUG_ALLOCATOR
static const size_t nNoMansLandSize = 4;
// PEPARKIN: The structure of _CrtMemBlockHeader before the user data.
// Used to transform the user data pointer into a non-opaque structure.
// Copied, pasted and tweeked from the debug library source code.
#define nNoMansLandSize 4

struct MyCrtMemBlockHeader
{
	MyCrtMemBlockHeader * pBlockHeaderNext;
	MyCrtMemBlockHeader * pBlockHeaderPrev;
	char *                      szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	* and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	* maintained in the debug heap.
	*/
	int                         nBlockUse;
	size_t                      nDataSize;
#else  /* _WIN64 */
	size_t                      nDataSize;
	int                         nBlockUse;
#endif  /* _WIN64 */
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	*  unsigned char           data[nDataSize];
	*  unsigned char           anotherGap[nNoMansLandSize];
	*/
} _CrtMemBlockHeader;
#endif

#include <algorithm> // sort
#define PROJECTNAME "Guildford"
__declspec(thread) Eng::EThreadType _ThreadType = Eng::EThreadType::UnknownThread;
__declspec(thread) unsigned _ThreadId = -1;

namespace Eng
{
	static HANDLE LogFileHandle = INVALID_HANDLE_VALUE;
	static std::string _MemoryLeakString;
	ExceptionContainer Engine::_ExceptionContainer;
	ID3D11Device1* Engine::_Device = nullptr;
	ID3D11DeviceContext* Engine::_ImmediateContext = nullptr;
	IDXGISwapChain* Engine::_SwapChain = nullptr;
	ID3D11Texture2D* Engine::_BackBufferTexture = nullptr;
	ID3D11RenderTargetView* Engine::_BackBufferView = nullptr;
	GraphicsManager* Engine::_GraphicsManager = nullptr;
#if ENGINE_USE_DEBUG_ALLOCATOR
	static _CrtMemState _MemStateOnConstruction;
	static void DumpClientFunction(void* user_data, size_t block_size)
	{
		auto header = reinterpret_cast<MyCrtMemBlockHeader*>((size_t)user_data-sizeof(MyCrtMemBlockHeader));
		char buffer[LocalBufferSize];
		sprintf_s(buffer, "%s(%d): %Iu\r\n", header->szFileName, header->nLine, header->nDataSize);
		_MemoryLeakString += buffer;
	}
#endif
	static void DeleteOldLogs()
	{
		WIN32_FIND_DATA fd;
		HANDLE find_handle = FindFirstFile(PROJECTNAME "_*.log", &fd);
		static const int temp = sizeof(long);
		struct FileData
		{
			char Filename[MAX_PATH];
			unsigned __int64 Time;
			FileData(const WIN32_FIND_DATA& fd)
			{
				memcpy(Filename, fd.cFileName, MAX_PATH);
				Time = _strtoui64(fd.cFileName + sizeof(PROJECTNAME "_") - 1, nullptr, 10);
			}
			bool operator<(const FileData& other) const
			{
				return Time < other.Time;
			}
		};
		std::vector<FileData> file_data_array;
		file_data_array.reserve(5);
		if (find_handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				file_data_array.push_back(fd);
			} while (FindNextFile(find_handle, &fd));
			FindClose(find_handle);
		}
		if (file_data_array.size() > 4)
		{
			std::sort(file_data_array.begin(), file_data_array.end());
			for (int index = 0; index < (int)file_data_array.size() - 4; ++index)
			{
				DeleteFile(file_data_array[index].Filename);
			}
		}
	}

	static void CreateLog()
	{
		DeleteOldLogs();
		char buffer[LocalBufferSize];
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf_s(buffer, PROJECTNAME "_%04d%02d%02d%2d%02d.log", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
		LogFileHandle = CreateFile(buffer, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
	}

	static void CloseLog()
	{
		if (LogFileHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(LogFileHandle);
			LogFileHandle = INVALID_HANDLE_VALUE;
		}
	}
	void Engine::CreateDeviceAndSwapChain(void* hwnd)
	{
		unsigned flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if ENGINE_USE_DEBUG_DIRECTX
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL feature_levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
		};
		static const int num_feature_levels = sizeof(feature_levels) / sizeof(D3D_FEATURE_LEVEL);

		IDXGIFactory1* factory;
		HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
		if (!SUCCEEDED(hr))
		{
			ENGINE_LOG(Error, "CreateDXGIFactory1 failed");
		}
		IDXGIAdapter1* adapter = nullptr;
		int adapter_index = 0;
		while ((hr = factory->EnumAdapters1(adapter_index++, &adapter)) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);
			if (desc.AdapterLuid.LowPart != 39912 || desc.AdapterLuid.HighPart != 0)
			{
				// Not the Microsoft Basic Render Driver
				ID3D11Device* device;
				ID3D11DeviceContext* immediate_context;
				hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, feature_levels, num_feature_levels, D3D11_SDK_VERSION, &device, nullptr, &immediate_context);
				SAFE_RELEASE(adapter);
				if (hr == S_OK)
				{
					hr = device->QueryInterface(__uuidof(ID3D11Device1), (void**)&_Device);
					SAFE_RELEASE(device);
					if (hr == S_OK)
					{
						hr = immediate_context->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&_ImmediateContext);
						SAFE_RELEASE(immediate_context);
						if (hr == S_OK)
						{
							break;
						}
						else
						{
							SAFE_RELEASE(factory);
							ENGINE_LOG(Error, "ID3D11DeviceContext1 cannot be found through QueryInterface")
						}
					}
					else
					{
						SAFE_RELEASE(factory);
						ENGINE_LOG(Error, "ID3D11Device1 cannot be found through QueryInterface");
					}
				}
			}
			SAFE_RELEASE(adapter);
		}
		if (hr == DXGI_ERROR_NOT_FOUND)
		{
			SAFE_RELEASE(factory);
			ENGINE_LOG(Error, "Unable to find a DX11 adapter");
		}
		DXGI_SWAP_CHAIN_DESC scd{ 0 };
		scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		scd.SampleDesc.Count = 1;
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 2;
		scd.OutputWindow = (HWND)hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		hr = factory->CreateSwapChain(_Device, &scd, &_SwapChain);
		SAFE_RELEASE(factory);
		if (!SUCCEEDED(hr))
		{
			ENGINE_LOG(Error, "Unable to create swap chain: HRESULT 0x%08x", hr);
			return;
		}
		// Now get the back buffer
		hr = _SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&_BackBufferTexture);
		if (hr != S_OK)
		{
			SAFE_RELEASE(_SwapChain);
			ENGINE_LOG(Error, "Unable to get the back buffer texture from the swapchain: HRESULT 0x%08x.", hr);
			return;
		}
		hr = _Device->CreateRenderTargetView(_BackBufferTexture, nullptr, &_BackBufferView);
		if (hr != S_OK)
		{
			SAFE_RELEASE(_BackBufferTexture);
			SAFE_RELEASE(_SwapChain);
			ENGINE_LOG(Error, "CreateRenderTargetView failed: HRESULT 0x%08x.", hr);
		}
		ENGINE_SET_DEBUG_OBJECT_NAME(_Device, "Device");
		ENGINE_SET_DEBUG_OBJECT_NAME(_ImmediateContext, "ImmediateContext");
		ENGINE_SET_DEBUG_OBJECT_NAME(_SwapChain, "SwapChain");
		ENGINE_SET_DEBUG_OBJECT_NAME(_BackBufferTexture, "BackBufferTexture");
		ENGINE_SET_DEBUG_OBJECT_NAME(_BackBufferView, "BackBufferView");

		_GraphicsManager = new GraphicsManager();
	}

	void Engine::ReleaseDirectX()
	{
		SAFE_DELETE(_GraphicsManager);
		SAFE_RELEASE(_BackBufferView);
		SAFE_RELEASE(_BackBufferTexture);
		SAFE_RELEASE(_SwapChain);
		if (_ImmediateContext)
		{
			_ImmediateContext->Flush();
			_ImmediateContext->Release();
			_ImmediateContext = nullptr;
		}
		if (_Device)
		{
			_Device->Release();
			_Device = nullptr;
		}
/*		if (_Device)
		{
			ID3D11Debug* debug = nullptr;
			HRESULT hr = _Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
			if (hr == S_OK)
			{
				debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
				debug->Release();
			}
			_Device->Release();
			_Device = nullptr;
		} */
	}
#if ENGINE_USE_DEBUG_ALLOCATOR
	static void ReportMemoryLeaks()
	{
		_CrtMemState current_state;
		_CrtMemState difference;
		_CrtMemCheckpoint(&current_state);
		size_t leak_in_client_block = 0;
		if (_CrtMemDifference(&difference, &_MemStateOnConstruction, &current_state))
		{
			leak_in_client_block = difference.lSizes[_CLIENT_BLOCK];
			if (leak_in_client_block)
			{
				_CrtMemDumpAllObjectsSince(&_MemStateOnConstruction); // Calls into DumpClientFunction, extracting files and line numbers into a global variable, _MemoryLeakString.
				char buffer[LocalBufferSize];
				sprintf_s(buffer, "Memory leak total: %Iu bytes\r\n", leak_in_client_block);
				_MemoryLeakString += buffer;
				ENGINE_LOG(Comment, "Engine Finalized - Leaks detected!\r\n%s", _MemoryLeakString.c_str());
				if (!std::uncaught_exception())
				{
					throw std::exception(_MemoryLeakString.c_str());
				}
			}
		}
		if (leak_in_client_block == 0)
		{
			ENGINE_LOG(Comment, "Engine Finalized - no memory leaks");
		}
	}
#endif
	Engine::Engine()
	{
		SetThreadName("MainThread", EThreadType::MainThread);
		CreateLog();
#if ENGINE_USE_DEBUG_ALLOCATOR
		_CrtSetDumpClient(DumpClientFunction);
		_CrtMemCheckpoint(&_MemStateOnConstruction);
//		_crtBreakAlloc = 258;
#endif
		TaskAllocator::Initialize();
		TaskGraph::Initialize();
		IoRequest::Initialize();
		char buffer[MAX_PATH];
		GetCurrentDirectoryA(sizeof(buffer), buffer);
		ENGINE_LOG(Comment, "Engine Initialized in directory %s", buffer);
	}

	Engine::~Engine() noexcept(false)
	{
		// IoRequests will no longer call their functions.
		TaskGraph::Join(); // Finish the current TaskGraph if you've forgotten to already.
		IoRequest::Finalize(); // Await outstanding IO requests.
		TaskGraph::Finalize();
		TaskAllocator::Finalize();
		ReleaseDirectX();
#if ENGINE_USE_DEBUG_ALLOCATOR
		ReportMemoryLeaks();
#else
		ENGINE_LOG(Comment, "Engine Finalized");
#endif
		CloseLog();
	}

	void Engine::Log(const char* file, const char* function, int line, ESeverity severity, const char* format, ...)
	{
		// Find the last '\' character in file.
		int last_slash_index = -1;
		for (int index = 0; file[index] != NULL && index < MAX_PATH; ++index)
		{
			if (file[index] == '\\')
			{
				last_slash_index = index;
			}
		}
		if (last_slash_index != -1)
		{
			file = file + last_slash_index + 1;
		}
		char buffer[LocalBufferSize];
		SYSTEMTIME time;
		GetLocalTime(&time);
		const char* type_name[3] = { "Comment", "Warning", "Error" };
		va_list args;
		va_start(args, format);
		int num_chars = sprintf_s(buffer, "%s(%d): %02d:%02d:%02d.%03d: %s: ", file, line, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, type_name[severity]);
		const char* throw_chars = buffer + num_chars;
		num_chars += vsprintf_s(buffer + num_chars, LocalBufferSize - num_chars, format, args);
		va_end(args);

		num_chars = MIN(num_chars, LocalBufferSize - 3);
		buffer[num_chars++] = '\r';
		buffer[num_chars++] = '\n';
		buffer[num_chars] = 0;
		DWORD bytes_written = 0;
		if (LogFileHandle != INVALID_HANDLE_VALUE)
		{
			if (WriteFile(LogFileHandle, buffer, num_chars, &bytes_written, nullptr) == FALSE)
			{
				throw Exception("Unable to write to log file.", __FILE__, __FUNCTION__, __LINE__);
			}
		}
		OutputDebugString(buffer);
		if (severity != Comment )
		{
			if (!std::uncaught_exception())
			{
				throw Exception(throw_chars, file, function, line);
			}
			else
			{
				try
				{
					throw Exception(throw_chars, file, function, line);
				}
				catch (...)
				{
					_ExceptionContainer += std::current_exception();
				}
			}
		}
	}

	void Engine::SetThreadName(const char* name, EThreadType thread_type)
	{
		const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
		} THREADNAME_INFO;
#pragma pack(pop)
		THREADNAME_INFO info = { 0x1000UL, name, 0xFFFFFFFFUL, 0UL };
		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
		_ThreadType = thread_type;
	}

	void Engine::TestCreateMemoryLeak()
	{
		new int;
	}

	void Engine::TestPlacementNew()
	{
		struct MyStruct
		{
			int z;
			MyStruct(int x, int y) : z(x + y) {}
			~MyStruct() { z = 0; }
		};
		char buffer[sizeof(MyStruct)];
		MyStruct* new_struct = pnew<MyStruct>(buffer, 1, 2);
		new_struct->~MyStruct();
	}

	bool Engine::HasExceptions()
	{
		return _ExceptionContainer.HasExceptions();
	}

	void Engine::ThrowException()
	{
		_ExceptionContainer.ThrowException();
	}

	void Engine::Async(std::function<void()> function)
	{
		TaskGraph::Async(function);
	}
	void Engine::Load(const char* filename, std::function<void(std::shared_ptr<IoReadData>)> function)
	{
		if (_ThreadType == EThreadType::MainThread)
		{
			// Main Thread - don't stall opening files.
			TaskGraph::BeginRead(new IoReadRequest(filename, function));
		}
		else
		{
			// Worker thread - stall opening files.
			try
			{
				IoReadRequest* read_request = new IoReadRequest(filename, function);
				read_request->Begin();
			}
			catch (...)
			{
				_ExceptionContainer += std::current_exception();
			}
		}
	}
	void Engine::Save(const char* filename, std::shared_ptr<IoReadData> data)
	{
		if (_ThreadType == EThreadType::MainThread)
		{
			// Main Thread - don't stall opening files.
			TaskGraph::BeginWrite(new IoWriteRequest(filename, data));
		}
		else
		{
			// Worker thread - stall opening files.
			try
			{
				IoWriteRequest* write_request = new IoWriteRequest(filename, data);
				write_request->Begin();
			}
			catch (...)
			{
				_ExceptionContainer += std::current_exception();
			}
		}

	}
	void Engine::Dispatch(std::function<void()> function)
	{
		Dispatcher::Dispatch(function);
	}

	EThreadType Engine::GetThreadType()
	{
		return _ThreadType;
	}
}