#include "GameIOCP_IOCPManager.h"

namespace game
{
	namespace IOCP
	{
		IOCPManager::IOCPManager()
		{
			_completionPort = INVALID_HANDLE_VALUE;
			_numberCompletionThreads = 1;
			_numberWorkerThreads = 1;
			_networkWork = nullptr;
			_timerWork = nullptr;
		}

		IOCPManager::~IOCPManager()
		{
			Shutdown();
		}

		void IOCPManager::Shutdown()
		{
			if (_completionPort != INVALID_HANDLE_VALUE)
			{
				// Send the kill switch to worker threads
				for (uint32_t i = 0; i < _numberWorkerThreads; i++)
				{
					long key = IOCP_TYPE_INVALID;
					PostQueuedCompletionStatus(_completionPort, 0, (ULONG_PTR)key, nullptr);
					game::IOCP::ErrorOutput("PostQueuedCompletionStatus", __LINE__);
				}

				// Close worker threads
				std::cout << "Stopping worker threads.\n";
				_threadPool.Stop();

				if (!CloseHandle(_completionPort))
				{
					game::IOCP::ErrorOutput("CloseHandle", __LINE__);
				}
				_completionPort = INVALID_HANDLE_VALUE;
			}
		}

		bool IOCPManager::Initialize(const uint32_t numberCompletionThreads, const uint32_t numberWorkerThreads)
		{
			_numberCompletionThreads = numberCompletionThreads;
			_numberCompletionThreads = max(_numberCompletionThreads, 1);

			_numberWorkerThreads = numberWorkerThreads;
			_numberWorkerThreads = max(_numberWorkerThreads, 1);

			_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numberCompletionThreads);
			if (game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__))
			{
				_completionPort = INVALID_HANDLE_VALUE;
				return false;
			}

			_threadPool.Start(_numberWorkerThreads);
			for (uint32_t i = 0; i < _numberWorkerThreads; i++)
			{
				_threadPool.Queue(std::bind(&IOCPManager::_WorkerThread, this));
			}
			return true;
		}

		HANDLE IOCPManager::GetCompeltionPort() const
		{
			return (_completionPort);
		}

		void IOCPManager::SetNetworkFunction(std::function<void(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, PER_IO_DATA* ioDataIn)> function)
		{
			_networkWork = function;
		}

		void IOCPManager::SetTimerFunction(std::function<void(const int32_t result, PER_IO_DATA* ioDataIn)> function)
		{
			_timerWork = function;
		}

		void IOCPManager::_WorkerThread()
		{
			DWORD bytesTransferred = 0;
			ULONG_PTR completionKey = 0;
			PER_IO_DATA* ioData = nullptr;
			int32_t result = 0;

			WSASetLastError(0); // All threads seem to come with ERROR_SEM_NOT_FOUND error in GetLastError/WSAGetLastError
			// for some reason. tested in standalone file with just std::thread
			while (true)
			{
				result = GetQueuedCompletionStatus(_completionPort, &bytesTransferred, &completionKey, (LPOVERLAPPED*)&ioData, INFINITE);
				// Loop kill switch
				if (ioData == nullptr && completionKey == IOCP_TYPE_INVALID)
				{
					return;
				}

				switch (ioData->ioDataType)
				{
				case IOCP_TYPE_NETWORK: if (_networkWork) _networkWork(result, bytesTransferred, completionKey, ioData); continue;
				case IOCP_TYPE_TIMER: if (_timerWork) _timerWork(result, ioData); continue;
				case IOCP_TYPE_FILE: std::cout << "Bytes written : " << bytesTransferred << "\n"; delete ioData; continue;
				default:
					std::cout << "Invalid ioDataType\n";
				}
			}
		}

	}
}