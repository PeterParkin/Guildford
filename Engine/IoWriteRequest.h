#pragma once

#include "IoRequest.h"
#include "Event.h"

#include <memory> // std::shared_ptr
#include <functional> // std::function

namespace Eng
{
	class IoWriteRequest final : public IoRequest
	{
	public:
		IoWriteRequest(const char* filename, std::shared_ptr<IoReadData> data);
		void Begin();
		void End(bool success);
	private:
		std::shared_ptr<IoReadData> _WriteData;
	};
}