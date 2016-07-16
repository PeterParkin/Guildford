#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Engine/Engine.h"
#include "../Engine/ExceptionHelper.h"
#include "../Engine/Task.h"
#include "../Engine/Config.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EssayTest
{
	using namespace Eng;
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
		class LoadTestObject : public Delegate<std::shared_ptr<IoReadData>>
		{
		public:

		private:
			void operator()(std::shared_ptr<IoReadData> read_data)
			{
				// Load completes in worker thread.
				if (read_data->_Size != 0)
				{
					ENGINE_LOG(Comment, "Load complete with size %d", read_data->_Size);
				}
				else
				{
					ENGINE_LOG(Comment, "Load failed - zero bytes");
				}
			}
		};
		TEST_METHOD(Save)
		{
			ExceptionHelper eh;
			try
			{
				Engine e;
				static unsigned file_size = 256;
				std::shared_ptr<IoReadData> data(new IoReadData(file_size));
				uint8_t* target = static_cast<uint8_t*>(data->_Address);
				unsigned iter = 0;
				for (; iter < file_size-2; ++iter)
				{
					unsigned index = iter % 28;
					switch (index)
					{
						case 26:
							target[iter] = '\r';
						break;
						case 27:
							target[iter] = '\n';
						break;
						default:
							target[iter] = 'A' + index;
					}
				}
				Engine::Save("Test.txt", data);
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
		TEST_METHOD(ConfigTest)
		{
			ExceptionHelper eh;
			try
			{
				Engine e;
				Config cfg("ConfigTest.cfg");
				cfg["FullScreen"] = true;
				cfg["lowercase"] = 1;
				cfg["UPPERCASE"] = true;
				cfg["FloatyMcFloatFace"] = "3.14159";
				{
					std::lock_guard<Config> lock(cfg);
					bool fullscreen = cfg["FullScreen"];
					float pie = cfg["FloatyMcFloatFace"];
					Assert::IsTrue(cfg["FullScreen"]);
				}
				const Config& const_cfg = cfg;
				int num = const_cfg["lowercase"];
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
		TEST_METHOD(EventTest)
		{
			ExceptionHelper eh;

			class PrintFloat_Delegate : public Delegate<float>
			{
				void operator()(float f)
				{
					ENGINE_LOG(Comment, "float f: %f", f);
				}
			};
			class Sum_Delegate : public Delegate<int, float, double>
			{
				void operator()(int i, float f, double d)
				{
					float result = (float)(d + f + i);
					ENGINE_LOG(Comment, "Result: %f", result);
				}
			};
			class Bonkers_Delegate
				: public Delegate<int, float, double>
				, public Delegate<float>
			{
				void operator()(int i, float f, double d)
				{
					ENGINE_LOG(Comment, "Received %d, %f, %lf", i, f, d);
				}
				void operator()(float f)
				{
					ENGINE_LOG(Comment, "Received %f", f);
				}
			};
			try
			{
				Engine e;
				PrintFloat_Delegate d;
				Sum_Delegate d2;
				Bonkers_Delegate bd;
				Event<float> float_event;
				Event<int, float, double> complex_event;
				float_event += d;
				float_event += bd;
				complex_event += d2;
				complex_event += bd;
				float_event(5);
				complex_event(1, 2.f, 3.0);
				float_event -= d;
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