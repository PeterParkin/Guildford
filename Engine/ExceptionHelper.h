#pragma once
//
// Used to help process Exceptions from the Engine for Visual Studio's native test system.
//
#include <exception>

namespace Eng
{
	class WideStringData
	{
	public:
		WideStringData()
			: _Data(nullptr)
		{}
		WideStringData(const char* data)
		{
			int array_size = (int)strlen(data) + 1;
			_Data = new wchar_t[array_size];
			swprintf_s(_Data, array_size, L"%hs", data);
		}
		~WideStringData()
		{
			if (_Data != nullptr)
			{
				delete[] _Data;
				_Data = nullptr;
			}
		}
		WideStringData(WideStringData&& other)
			: _Data(other._Data)
		{
			other._Data = nullptr;
		}
		WideStringData& operator=(const WideStringData& other) = delete; // Remove assignment operator.
		WideStringData& operator=(const char* data)
		{
			int array_size = (int)strlen(data) + 1;
			_Data = new wchar_t[array_size];
			swprintf_s(_Data, array_size, L"%hs", data);
			return *this;
		}

		const wchar_t* GetData() { return _Data; }
	private:
		wchar_t* _Data;
	};
	// Duplicate of Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo
	struct VSLineInfo
	{
		const wchar_t* pszFile; 
		const char* pszFunc;
		int line;
		VSLineInfo()
			: pszFile(nullptr)
			, pszFunc(nullptr)
			, line(-1)
		{}
	};
	// Used to help create the VSLineInfo data, and maintain the contents for the lifetime of the ExceptionHelper.
	struct ExceptionHelper : public VSLineInfo
	{
		WideStringData _WideFile;
		WideStringData _WideWhat;
		const wchar_t* _What;
		ExceptionHelper()
			: VSLineInfo()
			, _WideFile()
			, _WideWhat()
			, _What(nullptr)
		{}
		ExceptionHelper& operator=(const Exception& e)
		{
			_WideFile = e._File;
			_WideWhat = e.what();
			_What = _WideWhat.GetData();
			pszFile = _WideFile.GetData();
			pszFunc = e._Func;
			line = e._Line;
			return *this;
		}
		ExceptionHelper& operator=(const std::exception& e)
		{
			_WideFile = "Unknown File";
			_WideWhat = e.what();
			_What = _WideWhat.GetData();
			pszFile = _WideFile.GetData();
			pszFunc = "Unknown Function";
			line = 0;
			return *this;
		}
		bool IsValid()
		{
			return line != -1;
		}
	};
}