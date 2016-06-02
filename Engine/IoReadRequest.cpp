#include "EnginePCH.h"

#include "IoReadRequest.h"
#include "TaskGraph.h"

namespace Eng
{
	IoReadRequest::IoReadRequest(const char* filename, std::function<void(std::shared_ptr<IoReadData>)>& read_func)
		: IoRequest(filename)
		, _ReadFunc(std::move(read_func))
	{
		ENGINE_LOG(Comment, "%s - Queued for loading", _Filename);
	}

	void IoReadRequest::Begin()
	{
		PreBegin();
		_FileHandle = CreateFile(_Filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
		unsigned file_size = 0;
		if (_FileHandle == INVALID_HANDLE_VALUE || (file_size = GetFileSize(_FileHandle, nullptr)) == 0)
		{
			ENGINE_LOG(Comment, "%s - Cannot load", _Filename);
			_ReadData = std::shared_ptr<IoReadData>(new IoReadData(0));
			End(false);
		}
		else
		{
			TaskGraph::AssociateWithRead(_FileHandle);
			DWORD page_size = GetPageSize();
			// IoReadData allocates with page-sized granularity, so using file_size is good.
			// It helps prevent processing of data past the end of file.
			_ReadData = std::shared_ptr<IoReadData>(new IoReadData(file_size));
			unsigned load_size = (file_size + page_size - 1) / page_size * page_size;
			DWORD bytes_read = 0;
			// However, you must ensure the load_size is a multiple of the page size or the call will fail.
			BOOL read_file_result = ReadFile(_FileHandle, _ReadData->_Address, load_size, &bytes_read, static_cast<OVERLAPPED*>(this));
			if (!read_file_result)
			{
				DWORD error = GetLastError();
				if (error != ERROR_IO_PENDING)
				{
					ENGINE_LOG(Error, "Something went wrong loading a file with error code %d", error);
				}
			}
			ENGINE_LOG(Comment, "%s - Loading", _Filename);
		}
	}
	void IoReadRequest::End(bool success)
	{
		PreEnd();
		if (success)
		{
			ENGINE_LOG(Comment, "%s - Loaded", _Filename);
		}
		else
		{
			ENGINE_LOG(Comment, "%s - Failed loading", _Filename);
		}
		if (!success)
		{
			_ReadData->Clear(); // Probably should make a function like Destroy and have the destructor call that.
		}
		try
		{
			_ReadFunc(_ReadData);
		}
		catch (...)
		{
			Engine::_ExceptionContainer += std::current_exception();
		}
		PostEnd();
		delete this;
	}
}