#pragma once
#include "GameIOCPNetwork.h"

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			class NetworkManager
			{
				std::atomic<uint32_t> _stopping;
				HANDLE _completionPort;
				SOCKET _listenSocket;
				LPFN_ACCEPTEX _lpfnAcceptEx;
				LPFN_CONNECTEX _lpfnConnectEx;
				MemoryPool _ioDataPool;
				VectorPool<unsigned char> _vectorPool;
				NetworkInternalStats _stats;
				NetworkAttributes _attributes;


				/// <summary>
				/// Handles full and partial receives. Also handles receiving multiple sends at once. 
				/// </summary>
				/// <param name="ioData">: Per IO data being processed</param>
				/// <param name="bytesReceived">: Number of bytes received</param>
				void _HandleReceive(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived);
				void _HandleReceiveFrom(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived);
				/// <summary>
				/// Handles full and partial sends.
				/// </summary>
				/// <param name="ioData">: Per IO data being processed</param>
				/// <param name="bytesSent">: Number of bytes sent</param>
				void _HandleSend(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesSent);
				void _HandleSendTo(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesSent);
				void _HandleAccept(PER_IO_DATA_NETWORK* ioData);
				void _HandleConnect(PER_IO_DATA_NETWORK* ioData);
				void _DoWork(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, game::IOCP::PER_IO_DATA* ioDataIn);

				void _CloseConnection(PER_IO_DATA_NETWORK* ioData, const uint32_t line = 0, const bool alreadyClosed = false);
				void _AddConnection(const PER_IO_DATA_NETWORK* ioData);

				void _DeleteIoData(PER_IO_DATA_NETWORK* ioData);

				// Operation complete functions
				std::function<void(const SOCKET socket, const unsigned char*, const uint64_t, const uint32_t, const NetworkError&)> _OnReceive;
				void _OnReceiveDefault(const SOCKET socket, const unsigned char* data, const uint64_t bytesReceived, const uint32_t channel, const NetworkError& error);

				std::function<void(const SOCKET socket, const NetworkError& error)> _OnConnect;
				void _OnConnectDefault(const SOCKET socket, const NetworkError& error);

				std::function<void(const SOCKET socket, const NetworkError& error)> _OnAccept;
				void _OnAcceptDefault(const SOCKET socket, const NetworkError& error);

				std::function<void(const SOCKET socket, const uint64_t, const uint32_t channel, const NetworkError&)> _OnSend;
				void _OnSendDefault(const SOCKET socket, const uint64_t bytesSent, const uint32_t channel, const NetworkError& error);

				std::function<void(const SOCKET socket, const NetworkError& error)> _OnDisconnect;
				void _OnDisconnectDefault(const SOCKET socket, const NetworkError& error);

				std::mutex _connectionsMutex;
				std::unordered_map<SOCKET, Connection> _connections;



				// Returns true if it could extract header of size and channel
				// Returns false if it could not extract header
				bool _ExtractHeaderForReceive(PER_IO_DATA_NETWORK* ioData, uint32_t& bytesReceived);
				bool _DoOnReceive(PER_IO_DATA_NETWORK* ioData);
				uint32_t _RemoveProcessedData(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived);
				void _HandleNotEnoughDataReceived(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived);

			public:
				NetworkManager();

				const HANDLE GetCompletionPort() const;
				void GetConnections(std::vector<SOCKET>& connections);
				void GetConnectionStats(const SOCKET socket, uint64_t& receivedFrom, uint64_t& sentTo);
				const ConnectionInfo GetConnectionInfo(const SOCKET socket);

				void Receive(const SOCKET socket, PER_IO_DATA_NETWORK* ioData_in = nullptr);
				void ReceiveFrom(const SOCKET socket, PER_IO_DATA_NETWORK* ioData_in = nullptr);
				void Send(const SOCKET socket, const unsigned char* data, const uint64_t length, const uint8_t channel = 0, PER_IO_DATA_NETWORK* ioData_in = nullptr);
				void SendTo(const SOCKET socket, const sockaddr_in& addr, const unsigned char* data, const uint64_t length, uint8_t channel = 0, PER_IO_DATA_NETWORK* ioData_in = nullptr);
				void BroadCast(const unsigned char* data, const uint64_t length, const uint8_t channel, const SOCKET except = INVALID_SOCKET);
				void Accept();
				void Connect(const std::string& ipAddress, const uint32_t port);


				bool Initialize(const NetworkAttributes& attributes, game::IOCP::IOCPManager& ioHandler);
				void Shutdown();

				void SetOnReceive(std::function<void(const SOCKET socket, const unsigned char*, const uint64_t, const uint32_t, const NetworkError&)> func);
				void SetOnConnect(std::function<void(const SOCKET socket, const NetworkError& error)> func);
				void SetOnAccept(std::function<void(const SOCKET socket, const NetworkError& error)> func);
				void SetOnSend(std::function<void(const SOCKET socket, const uint64_t, const uint32_t, const NetworkError&)> func);
				void SetOnDisconnect(std::function<void(const SOCKET socket, const NetworkError& error)> func);

				void PrintStats();
				uint64_t GetStat(StatName name) const;

				void Shrink();
			};
		}
	}
}