#include "EnginePCH.h"

#include "IoWriteRequest.h"
#include "TaskGraph.h"

namespace Eng
{
	IoWriteRequest::IoWriteRequest(const char* filename, std::shared_ptr<IoReadData> data)
		: IoRequest(filename)
		, _WriteData(data)
	{
		ENGINE_LOG(Comment, "%s - Queued for saving", _Filename);
	}

	void IoWriteRequest::Begin()
	{
		PreBegin();
		_FileHandle = CreateFile(_Filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, NULL);
		unsigned file_size = 0;
		if (_FileHandle == INVALID_HANDLE_VALUE )
		{
			ENGINE_LOG(Comment, "%s - Cannot save", _Filename);
			End(false);
		}
		else
		{
			TaskGraph::AssociateWithWrite(_FileHandle);
			DWORD bytes_written = 0;
			unsigned page_size = GetPageSize();
			unsigned total_file_size = (_WriteData->_Size + page_size - 1) / page_size * page_size;
			BOOL write_file_result = WriteFile(_FileHandle, _WriteData->_Address, total_file_size, &bytes_written, static_cast<OVERLAPPED*>(this));
			if (!write_file_result)
			{
				DWORD error = GetLastError();
				if (error != ERROR_IO_PENDING)
				{
					ENGINE_LOG(Error, "Something went wrong saving a file with error code %d", error);
				}
			}
			ENGINE_LOG(Comment, "%s - Saving", _Filename);
		}
	}
	void IoWriteRequest::End(bool success)
	{
		PreEnd();
		if (success)
		{
			if (SetFilePointer(_FileHandle, _WriteData->_Size, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
			{
				ENGINE_LOG(Error, "Unable to set the file pointer: %s", _Filename);
			}
			if (SetEndOfFile(_FileHandle) == FALSE)
			{
				ENGINE_LOG(Error, "Unable to set the end of file: %s", _Filename);
			}
			ENGINE_LOG(Comment, "%s - Saved", _Filename);
		}
		else
		{
			ENGINE_LOG(Warning, "%s - Failed saving", _Filename);
		}
		PostEnd();
		delete this;
	}
}