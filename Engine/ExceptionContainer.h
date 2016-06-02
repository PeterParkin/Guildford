#pragma once
//
// Included in Engine.h
// Allows transport of exceptions from any module or thread
//
#include <vector>
#include <exception>
#include <mutex>

namespace Eng
{
	class ExceptionContainer
	{
	private:
		std::mutex _Mutex;
		std::vector<std::exception_ptr> _ExceptionPtrArray;
	public:
		bool HasExceptions()
		{
			std::lock_guard<std::mutex> guard(_Mutex);
			return _ExceptionPtrArray.size() > 0;
		}
		ExceptionContainer& operator+=(const std::exception_ptr& e)
		{
			std::lock_guard<std::mutex> guard(_Mutex);
			_ExceptionPtrArray.push_back(e);
			return *this;
		}
		void ThrowException()
		{
			std::exception_ptr e;
			{
				std::lock_guard<std::mutex> guard(_Mutex);
				if (_ExceptionPtrArray.size() == 0)
				{
					// No exception to throw
					return;
				}
				e = _ExceptionPtrArray[0];
				_ExceptionPtrArray.erase(_ExceptionPtrArray.begin());
			}
			std::rethrow_exception(e);
		}
	};
}