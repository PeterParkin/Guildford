#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Engine/Engine.h"
#include "../Engine/ExceptionHelper.h"
#include "../Engine/Task.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EssayTest
{
	using namespace Eng;
	int Global = 0;
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(InitializeFinalize)
		{
			ExceptionHelper eh;
			try
			{
				Engine e;
			}
			catch (Exception& e)
			{
				eh = e;
			}
			catch (std::exception& e)
			{
				eh = e;
			}
			if (eh.IsValid())
			{
				VSLineInfo* pvsli = static_cast<VSLineInfo*>(&eh);
				auto li = reinterpret_cast<Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pvsli);
				Assert::Fail(eh._What, li);
			}
			try
			{
				Engine e;
			}
			catch (Exception& e)
			{
				eh = e;
			}
			catch (std::exception& e)
			{
				eh = e;
			}
			if (eh.IsValid())
			{
				VSLineInfo* pvsli = static_cast<VSLineInfo*>(&eh);
				auto li = reinterpret_cast<Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pvsli);
				Assert::Fail(eh._What, li);
			}
		}
		TEST_METHOD(MemoryLeakThrowsException)
		{
#ifdef _DEBUG
			Assert::ExpectException<std::exception>([]()
			{
				Engine e;
				Engine::TestCreateMemoryLeak();
			});
#else
#endif
		}
		
		TEST_METHOD(TestTask)
		{
			ExceptionHelper eh;
			try
			{
				Engine e;
				auto t1 = Task<>::Create([]() -> int
				{
					ENGINE_LOG(Comment, "Entered int task");
					return 1;
				});
				auto t2 = Task<>::Create([]() -> float
				{
					ENGINE_LOG(Comment, "Entered float task");
					return 2.f;
				});
				auto t3 = Task<int, float>::Create([](const int& i, const float& f) -> double
				{
					ENGINE_LOG(Comment, "Int+Float task");
					return i + f;
				}, t1, t2);
				TaskGraph::Join();
			}
			catch (Exception& e)
			{
				eh = e;
			}
			catch (std::exception& e)
			{
				eh = e;
			}
			if (eh.IsValid())
			{
				VSLineInfo* pvsli = static_cast<VSLineInfo*>(&eh);
				auto li = reinterpret_cast<Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pvsli);
				Assert::Fail(eh._What, li);
			}
		}
		TEST_METHOD(Load)
		{
			ExceptionHelper eh;
			try
			{
				Engine e;
				Engine::Load("Test.dat", [](std::shared_ptr<IoReadData> data)
				{
					ENGINE_LOG(Comment, "Success");
				});
			}
			catch (Exception& e)
			{
				eh = e;
			}
			catch (std::exception& e)
			{
				eh = e;
			}
			if (eh.IsValid())
			{
				VSLineInfo* pvsli = static_cast<VSLineInfo*>(&eh);
				auto li = reinterpret_cast<Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo*>(pvsli);
				Assert::Fail(eh._What, li);
			}
		}
	};
}