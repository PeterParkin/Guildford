#pragma once

#include <exception>

#define ENGINE_THROW(what, ...) \
{ \
	char buffer[512]; \
	sprintf_s(buffer, what, __VA_ARGS__ ); \
	throw new Exception(buffer, __FILE__, __FUNCTION__, __LINE__); \
}

namespace Eng
{
	class Exception : public std::exception
	{
		friend struct ExceptionHelper;
	private:
		const char* _File;
		const char* _Func;
		int _Line;
	public:
		Exception(const char* what_param)
			: std::exception(what_param)
			, _File(nullptr)
			, _Func(nullptr)
			, _Line(-1)
		{}
		Exception(const char* what_param, const char* file, const char* func, int line)
			: std::exception(what_param)
			, _File(file)
			, _Func(func)
			, _Line(line)
		{}
		const char* GetFile() const
		{
			return _File;
		}
		const char* GetFunction() const
		{
			return _Func;
		}
		int GetLine() const
		{
			return _Line;
		}
	};
}