#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h> // DWORD, LPOVERLAPPED, HANDLE. OnComplete function must be visible to derived classes.
#include <list> // std::list
#include <mutex>

namespace Eng
{
	class IoRequest : public OVERLAPPED
	{
	public:
		IoRequest();
		IoRequest(IoRequest* prev, IoRequest* next); // For construction of doubly-linked list.
		IoRequest(const char* filename);
		void PreBegin();
		void PreEnd();
		void PostEnd();

		static void Initialize();
		static void Finalize();
		static void WaitComplete();
	protected:
		DWORD GetPageSize();
		~IoRequest();

		HANDLE _FileHandle;
		char _Filename[MAX_PATH];

	private:
		IoRequest* _Prev;
		IoRequest* _Next;
		static SpinLock _Mutex; // For accessing _InProgressStart & _InProgressEnd
		static IoRequest _InProgressStart, _InProgressEnd;
		static HANDLE _CompleteEvent;
		static std::atomic<int> _OutstandingIoRequests;
	};
}