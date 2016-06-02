#include "EnginePCH.h"
#include "IoRequest.h"

namespace Eng
{
	IoRequest IoRequest::_InProgressStart(nullptr, &_InProgressEnd);
	IoRequest IoRequest::_InProgressEnd(&_InProgressStart, nullptr);
	std::atomic<int> IoRequest::_OutstandingIoRequests = 0;
	HANDLE IoRequest::_CompleteEvent = INVALID_HANDLE_VALUE;

	IoRequest::IoRequest()
		: OVERLAPPED{ 0 }
		, _FileHandle(INVALID_HANDLE_VALUE)
		, _Prev(nullptr)
		, _Next(nullptr)
	{
		_Filename[0] = NULL;
	}

	IoRequest::IoRequest(const char* filename)
		: OVERLAPPED{ 0 }
		, _FileHandle(INVALID_HANDLE_VALUE)
		, _Prev(nullptr)
		, _Next(nullptr)
	{
		strcpy_s(_Filename, filename);
		if (0 == _OutstandingIoRequests.fetch_add(1, std::memory_order_relaxed))
		{
			ResetEvent(_CompleteEvent);
		}
	}

	IoRequest::IoRequest(IoRequest* prev, IoRequest* next)
		: OVERLAPPED{ 0 }
		, _Prev(prev)
		, _Next(next)
	{
	}

	void IoRequest::PreBegin()
	{
		_Prev = _InProgressEnd._Prev;
		_Next = &_InProgressEnd;
		_Prev->_Next = this;
		_Next->_Prev = this;
	}
	void IoRequest::PreEnd()
	{
		_Prev->_Next = _Next;
		_Next->_Prev = _Prev;
		_Next = nullptr;
		_Prev = nullptr;
	}
	void IoRequest::PostEnd()
	{
		if (1 == _OutstandingIoRequests.fetch_sub(1, std::memory_order_relaxed))
		{
			SetEvent(_CompleteEvent);
		}
	}

	void IoRequest::Initialize()
	{
		_CompleteEvent = CreateEvent(nullptr, TRUE, TRUE, nullptr); // Manual reset, initially set.
	}

	void IoRequest::Finalize()
	{
		WaitForSingleObject(_CompleteEvent, INFINITE);
		CloseHandle(_CompleteEvent);
		_CompleteEvent = INVALID_HANDLE_VALUE;
	}

	IoRequest::~IoRequest()
	{
		if (_FileHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(_FileHandle);
		}
	}

	DWORD IoRequest::GetPageSize()
	{
		static DWORD _PageSize = 0;
		if (_PageSize == 0)
		{
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			_PageSize = si.dwPageSize;
		}
		return _PageSize;
	}

	void IoRequest::WaitComplete()
	{
		WaitForSingleObject(_CompleteEvent, INFINITE); // No waking up and throwing exceptions!
	}
}