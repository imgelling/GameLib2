#pragma once
#include <ws2tcpip.h>
#include <mswsock.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <string>
#include <atomic>
#include <codecvt>
#include <natupnp.h>

#pragma comment(lib, "ws2_32.lib") 

#include "GameThreadPool.h"
#include "Game_MemoryPool.h"
#include "GameIOCP_IOCPManager.h"
#include "GameIOCP_ErrorOutput.h"


namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			constexpr uint32_t MAX_PACKET_SIZE = 64 * 1024 - 1; // 64k - 1 
			//constexpr auto MIN_BUFFER_SIZE = (sizeof(SOCKADDR_IN) + 16) * 2;
			constexpr uint32_t NETWORK_BUFFER_SIZE = MAX_PACKET_SIZE + (uint32_t)sizeof(uint32_t) + 1; // 64k + 4 bytes of length + 1 byte of channel


			struct PER_IO_DATA_NETWORK : game::IOCP::PER_IO_DATA 
			{
				WSABUF buffer = { 0 };
				//uint8_t NETWORK_COMPLETION_TYPE pad[0]	defined in cpp
				//uint8_t NETWORK_CHANNEL = pad[1]			defined in cpp
				SOCKET socket = INVALID_SOCKET;
				uint32_t expectedTransferLeft = 0;
				uint32_t expectedTransferTotal = 0;
				uint32_t bytesTransferred = 0;
				sockaddr_in addr = { 0 };
				char data[NETWORK_BUFFER_SIZE] = { 0 };
				// uint8_t NETWORK_SOCKET_TYPE = pad[2]		defined in cpp
			};

			struct ConnectionInfo
			{
				SOCKET socket = INVALID_SOCKET;
				std::string remoteIpAddress;
				std::string localIpAddress;
				uint32_t remotePort = 0;
				uint32_t localPort = 0;
				int64_t bytesSentFrom = 0;		// Bytes sent from connection
				int64_t bytesReceivedBy = 0;	// Bytes received by connection
				int32_t ping = 0;
			};

			class Connection
			{
			public:
				SOCKET socket;		// Getters and setters for this stuff?	
				std::string remoteIpAddress;
				std::string localIpAddress;
				uint32_t remotePort;
				uint32_t localPort;
				uint32_t ping;
				const ConnectionInfo GetInfo() const noexcept
				{
					ConnectionInfo info;
					info.socket = socket;
					info.bytesReceivedBy = _bytesReceivedBy;
					info.bytesSentFrom = _bytesSentFrom;
					info.remoteIpAddress = remoteIpAddress;
					info.remotePort = remotePort;
					info.localIpAddress = localIpAddress;
					info.localPort = localPort;
					info.ping = 0;
					return info;
				}
				const uint64_t BytesSentFrom() const noexcept
				{
					return _bytesSentFrom;
				}
				const uint64_t BytesReceivedBy() const noexcept
				{
					return _bytesReceivedBy;
				}
				void AddBytesReceivedBy(const uint64_t count) noexcept// should minus the 4 bytes because length, maybe
				{
					_bytesReceivedBy += count;
				}
				void AddBytesSentFrom(const uint64_t count) noexcept
				{
					_bytesSentFrom += count;
				}
				std::vector<unsigned char> receiveData; // Full data received
				Connection()
				{
					socket = INVALID_SOCKET;
					_bytesSentFrom = 0;
					_bytesReceivedBy = 0;
					receiveData.reserve(NETWORK_BUFFER_SIZE);
					remotePort = 0;
					localPort = 0;
					ping = 0;
				}
			private:
				uint64_t _bytesSentFrom;		// Bytes sent from connection
				uint64_t _bytesReceivedBy;	// Bytes received by connection
			};

			struct NetworkError
			{
				std::string errorString;
				uint32_t errorNumber = 0;
				std::string errorName;
				std::string errorDetail;
			};

			struct NetworkAttributes
			{
				uint16_t port = NULL;
				bool verboseOutputDEBUG = false;
				bool tcpNoDelay = false;
				uint32_t numberOfAcceptors = 1;
				uint32_t initialIoDataPoolSize = 1;
				uint32_t initialVectorPoolSize = 1;
				uint32_t vectorPoolReserveSize = 0;
			};

			class NetworkInternalStats
			{
			public:
				enum statName
				{
					CONNECT_ALLOCATE_COUNT,
					CONNECT_DEALLOCATE_COUNT,
					RECEIVE_ALLOCATE_COUNT,
					RECEIVE_DEALLOCATE_COUNT,
					SEND_ALLOCATE_COUNT,
					SEND_DEALLOCATE_COUNT,
					ACCEPT_ALLOCATE_COUNT,
					ACCEPT_DEALLOCATE_COUNT,
					BYTES_SENT,
					BYTES_RECEIVED,
					NUMBER_OF_CONNECTIONS
				};

				bool verbose;
				NetworkInternalStats();
				void AddConnection();
				void AddBytesReceived(const uint64_t count);
				void AddBytesSent(const uint64_t count);
				void PrintStats();
				void MemoryAllocate(const uint32_t type);
				void MemoryDeallocate(const uint32_t type);
				uint64_t GetStat(const uint32_t stat) const;
			private:
				std::atomic_uint64_t _connectAllocateCount;
				std::atomic_uint64_t _connectDeallocateCount;
				std::atomic_uint64_t _receiveAllocateCount;
				std::atomic_uint64_t _receiveDeallocateCount;
				std::atomic_uint64_t _sendAllocateCount;
				std::atomic_uint64_t _sendDeallocateCount;
				std::atomic_uint64_t _acceptAllocateCount;
				std::atomic_uint64_t _acceptDeallocateCount;
				std::atomic_uint64_t _bytesSent;
				std::atomic_uint64_t _bytesReceived;
				std::atomic_uint64_t _numberOfConnections;
			};
		}
	}
}
