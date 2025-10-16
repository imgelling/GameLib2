#pragma once
#include "GameThreadPool.h"
#include "GameIOCP_ErrorOutput.h"
#include <ws2tcpip.h>

namespace game
{
	namespace IOCP
	{
		constexpr uint8_t IOCP_TYPE_NETWORK = 1;
		constexpr uint8_t IOCP_TYPE_TIMER = 2;
		constexpr uint8_t IOCP_TYPE_FILE = 3;
		constexpr uint8_t IOCP_TYPE_INVALID = 255;

		// Has 7 uint8_t array "pad" for some storage
		struct PER_IO_DATA
		{
			OVERLAPPED overlapped = { 0 };
			uint8_t ioDataType = IOCP_TYPE_INVALID;  // or a union?
			uint8_t pad[7] = { 0 };
		};

		class IOCPManager
		{
		public:
			IOCPManager();
			~IOCPManager();

			void Shutdown();
			bool Initialize(const uint32_t numberCompletionThreads, const uint32_t numberWorkerThreads);

			HANDLE GetCompeltionPort() const;

			void SetNetworkFunction(std::function<void(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, PER_IO_DATA* ioDataIn)> function);
			void SetTimerFunction(std::function<void(const int32_t result, PER_IO_DATA* ioDataIn)> function);
			void SetFileFunction(std::function<void(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, PER_IO_DATA* ioDataIn)> function);

		private:
			HANDLE _completionPort;
			uint32_t _numberCompletionThreads;
			uint32_t _numberWorkerThreads;
			ThreadPool _threadPool;
			std::function<void(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, PER_IO_DATA* ioDataIn)> _networkWork;
			std::function<void(const int32_t result, PER_IO_DATA* ioDataIn)> _timerWork;
			std::function<void(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, PER_IO_DATA* ioDataIn)> _fileWork;
			void _WorkerThread();
		};
	}
}
