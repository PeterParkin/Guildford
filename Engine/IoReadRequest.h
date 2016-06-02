#pragma once

#include "IoRequest.h"
#include "IoTypes.h" // IoReadData

#include <memory> // std::shared_ptr
#include <functional> // std::function

namespace Eng
{
	class IoReadRequest final : public IoRequest
	{
	public:
		IoReadRequest(const char* filename, std::function<void(std::shared_ptr<IoReadData>)>& read_func);
		void Begin();
		void End(bool success);
	private:
		std::shared_ptr<IoReadData> _ReadData;
		std::function<void(std::shared_ptr<IoReadData>)> _ReadFunc;
	};
}