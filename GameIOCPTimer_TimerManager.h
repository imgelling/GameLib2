#pragma once
#include "GameIOCP_IOCPManager.h"
#include "Game_MemoryPool.h"
#include <queue>

namespace game
{
	namespace IOCP
	{
		namespace Timer
		{
			enum class TimerMode
			{
				// Waits after callback before next fire
				FixedDelay,
				// Fires every interval regardless of callback duration
				FixedRate
			};

			struct PER_IO_DATA_TIMER : game::IOCP::PER_IO_DATA
			{
				uint64_t id = 0;
				std::chrono::steady_clock::time_point dueTime;
				std::chrono::steady_clock::time_point startTime;
				std::chrono::milliseconds interval = std::chrono::milliseconds(0);
				TimerMode mode = TimerMode::FixedRate;
				// Setting this to 0 is infinite repeating, 1 is a one shot, 2+ will repeat that many times
				uint32_t maxExecutions = 1;
				uint32_t currentExecution = 0;
				std::atomic<bool> cancelled = false;
				std::function<void(const PER_IO_DATA_TIMER& task)> callback = nullptr;
				bool operator()(const PER_IO_DATA_TIMER* t1, const PER_IO_DATA_TIMER* t2) {
					return t1->dueTime > t2->dueTime;
				}
			};

			class TimerManager
			{
			public:
				TimerManager();
				~TimerManager();

				bool Initialize(game::IOCP::IOCPManager& iocpManager);
				void Shutdown();

				uint64_t AddTimer(const std::chrono::milliseconds interval, const uint32_t executions = 1, const std::function<void(const PER_IO_DATA_TIMER& task)> callback = nullptr, const TimerMode mode = TimerMode::FixedRate);
				void CancelTimer(const uint64_t id);

				const uint64_t GetSize();

				void SetOnTimer(std::function<void(const PER_IO_DATA_TIMER &task)> OnTimer);

			private:
				HANDLE _iocp;
				std::atomic<uint64_t> _nextId;
				std::unordered_map<uint64_t, PER_IO_DATA_TIMER*> _timers;
				std::priority_queue <PER_IO_DATA_TIMER*, std::vector<PER_IO_DATA_TIMER*>, PER_IO_DATA_TIMER> _queue;
				std::mutex _mtx;
				std::condition_variable _cv;
				std::atomic<bool> _stop;
				std::thread _worker;
				game::MemoryPool _memoryPool;

				std::function<void(const PER_IO_DATA_TIMER& task)> _OnTimer;
				void _OnTimerDefault(const PER_IO_DATA_TIMER& task);
				void _DeleteTimer(PER_IO_DATA_TIMER* timer);
				void _Loop();
				void _DoWork(const int32_t result, game::IOCP::PER_IO_DATA* ioDataIn);
			};
		}
	}
}