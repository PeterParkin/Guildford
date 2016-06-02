#pragma once

#include <functional>

namespace Eng
{
	class Dispatcher
	{
	public:
		static void Dispatch(std::function<void()>& function);
		static void ExecuteAll();
		static void DeleteAll();
	};
}