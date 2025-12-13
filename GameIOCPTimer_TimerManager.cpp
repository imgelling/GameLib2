#pragma warning(disable : 4100)
#include "GameIOCP.h" //Timer_TimerManager.h"

namespace game
{
	namespace IOCP
	{
		namespace Timer
		{
			TimerManager::TimerManager()
			{
				_iocp = INVALID_HANDLE_VALUE;
				_nextId = 0;
				_stop = false;
			}

			TimerManager::~TimerManager()
			{
				Shutdown();
			}

			bool TimerManager::Initialize(game::IOCP::IOCPManager& iocpManager)
			{
				if (!_stop)
				{
					_iocp = iocpManager.GetCompeltionPort();
					if (_iocp == INVALID_HANDLE_VALUE)
					{
						return false;
					}
					_nextId = 0;
					_stop = false;
					auto callback = [&](int32_t result, game::IOCP::PER_IO_DATA* ioData) 
						{
							_DoWork(result, ioData);
						};
					iocpManager.SetTimerFunction(callback);
					_OnTimer = std::bind(&TimerManager::_OnTimerDefault, this, std::placeholders::_1);
					_worker = std::thread([this] { _Loop(); });
					_memoryPool.Initialize(sizeof(PER_IO_DATA_TIMER), 2);
					return true;
				}
				return false;
			}

			void TimerManager::Shutdown()
			{
				if (!_stop)
				{
					_stop = true;
					_cv.notify_all();
					if (_worker.joinable()) _worker.join();
					{
						std::lock_guard<std::mutex> lock(_mtx);
						while (!_queue.empty())
						{
							PER_IO_DATA_TIMER* t = _queue.top();
							_queue.pop();
							if (t->cancelled)
							{
								_timers.erase(t->id);
								_memoryPool.Deallocate(t);
							}
						}
						for (auto& a : _timers)
						{
							if (a.second != nullptr)
							{
								_memoryPool.Deallocate(a.second);
								a.second = nullptr;
							}
						}
						_timers.clear();
					}
					_memoryPool.PrintStats("Timer memory pool");
				}
			}

			uint64_t TimerManager::AddTimer(const std::chrono::milliseconds interval, const uint32_t executions, const std::function<void(const PER_IO_DATA_TIMER& task)> callback, const TimerMode mode)
			{
				const uint64_t id = _nextId++;

				PER_IO_DATA_TIMER* task = (PER_IO_DATA_TIMER*)_memoryPool.Allocate();
				task->id = id;
				task->interval = interval;
				task->startTime = std::chrono::steady_clock::now();
				task->dueTime = task->startTime + interval;
				task->maxExecutions = executions;
				if (executions) task->currentExecution = 1;
				else task->currentExecution = 0;
				task->callback = callback;
				task->mode = mode;
				task->cancelled = false;
				task->ioDataType = game::IOCP::IOCP_TYPE_TIMER;

				{
					std::lock_guard<std::mutex> lock(_mtx);
					_timers[id] = task;
					_queue.push(task);
				}

				_cv.notify_one();
				return id;
			}

			void TimerManager::CancelTimer(const uint64_t id)
			{
				std::lock_guard<std::mutex> lock(_mtx);
				auto it = _timers.find(id);
				if (it != _timers.end())
				{
					it->second->cancelled = true;
					_timers.erase(it);
				}
			}

			uint64_t TimerManager::GetSize()
			{
				std::lock_guard<std::mutex> lock(_mtx);
				return _timers.size();
			}

			void TimerManager::SetOnTimer(std::function<void(const PER_IO_DATA_TIMER& task)> OnTimer)
			{
				_OnTimer = OnTimer;
			}

			void TimerManager::_OnTimerDefault(const PER_IO_DATA_TIMER& task)
			{
				std::cout << "Default OnTimer function! Create your own with the signature of\n";
				std::cout << "void(const game::IOCP::Timer::TimerTask& task)\n";
				std::cout << "Task ID " << task.id << " fired!\n";
			}

			void TimerManager::_DeleteTimer(PER_IO_DATA_TIMER* timer)
			{
				if (timer != nullptr)
				{
					{
						std::unique_lock<std::mutex> lock(_mtx);
						_timers.erase(timer->id);
					}
					_memoryPool.Deallocate(timer);
				}
			}

			void TimerManager::_Loop()
			{
				std::unique_lock<std::mutex> lock(_mtx);
				while (!_stop)
				{
					if (_queue.empty())
					{
						_cv.wait(lock);
						continue;
					}
					PER_IO_DATA_TIMER* task = _queue.top();


					if (task->dueTime >= std::chrono::steady_clock::now())
					{
						_cv.wait_until(lock, task->dueTime);
						continue;
					}
					_queue.pop();

					PostQueuedCompletionStatus(_iocp, 0, task->id, (OVERLAPPED*)task);
				}
			}

			void TimerManager::_DoWork(const int32_t result, game::IOCP::PER_IO_DATA* ioDataIn)
			{
				// TODO: not checking result for errors
				PER_IO_DATA_TIMER* task = (PER_IO_DATA_TIMER*)ioDataIn;
				if ((task->currentExecution <= task->maxExecutions) && !task->cancelled)
				{
					const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
					
					if (task->callback != nullptr)
					{
						task->callback(*task);
					}
					else
					{
						_OnTimer(*task);
					}

					// Update timer for repeations
					if (task->mode == TimerMode::FixedRate)
					{
						task->startTime = now; // this will allow per fire timing, removing will give elapsed
						task->dueTime += task->interval;
					}
					else // FixedDelay, not realyl sure if needed
					{
						task->startTime = std::chrono::steady_clock::now();
						task->dueTime = task->startTime + task->interval;
					}

					// Increase task execution count if needed
					if (task->maxExecutions) task->currentExecution++;

					// Add the task back in to the queue
					if (task->currentExecution <= task->maxExecutions)
					{
						{
							std::unique_lock<std::mutex> lock(_mtx);
							_queue.push(task);
						}
						// and notify the timer work thread
						_cv.notify_one();
						return;
					}
					// at this point the timer has run out of executions, fall through to delete
				}

				_DeleteTimer(task);
			}
		}
	}
}