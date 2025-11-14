#pragma once
#include "GameIOCPNetwork.h"

// TODO: add ONCONNECT
#define ONACCEPT_SIGNATURE const SOCKET socket, const game::IOCP::Network::NetworkError& error
#define ONACCEPT_PARAMETERS socket,error
#define ONDISCONNECT_SIGNATURE const SOCKET socket, const game::IOCP::Network::NetworkError& error
#define ONDISCONNECT_PARAMETERS socket,error
#define ONRECEIVE_SIGNATURE const SOCKET socket, const unsigned char* data, const uint64_t bytesReceived, const uint32_t channel, const game::IOCP::Network::NetworkError& error
#define ONRECEIVE_PARAMETERS socket, data, bytesReceived, channel, error
#define ONSEND_SIGNATURE const SOCKET socket, const uint64_t bytesSent, const uint32_t channel, const game::IOCP::Network::NetworkError& error
#define ONSEND_PARAMETERS socket, bytesSent, channel, error

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			class NetworkManager
			{
				HANDLE _completionPort;
				SOCKET _listenSocket;
				LPFN_ACCEPTEX _lpfnAcceptEx;
				LPFN_CONNECTEX _lpfnConnectEx;
				MemoryPool _ioDataPool;
				VectorPool<unsigned char> _vectorPool;
				NetworkInternalStats _stats;
				NetworkAttributes _attributes;
				std::atomic<uint32_t> _stopping;


				/// <summary>
				/// Handles full and partial receives.
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

				void _CloseConnection(PER_IO_DATA_NETWORK* ioData, const int32_t line = 0, const bool alreadyClosed = false);
				void _AddConnection(const PER_IO_DATA_NETWORK* ioData);

				void _DeleteIoData(PER_IO_DATA_NETWORK* ioData);

				// Operation complete functions
				std::function<void(ONRECEIVE_SIGNATURE)> _OnReceive;
				void _OnReceiveDefault(ONRECEIVE_SIGNATURE);

				std::function<void(const SOCKET socket, const NetworkError& error)> _OnConnect;
				void _OnConnectDefault(const SOCKET socket, const NetworkError& error);

				std::function<void(ONACCEPT_SIGNATURE)> _OnAccept;
				void _OnAcceptDefault(ONACCEPT_SIGNATURE);

				std::function<void(ONSEND_SIGNATURE)> _OnSend;
				void _OnSendDefault(ONSEND_SIGNATURE);

				std::function<void(ONDISCONNECT_SIGNATURE)> _OnDisconnect;
				void _OnDisconnectDefault(ONDISCONNECT_SIGNATURE);

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

				HANDLE GetCompletionPort() const;
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

				void SetOnReceive(std::function<void(ONRECEIVE_SIGNATURE)> func);
				void SetOnConnect(std::function<void(const SOCKET socket, const NetworkError& error)> func);
				void SetOnAccept(std::function<void(ONACCEPT_SIGNATURE)> func);
				void SetOnSend(std::function<void(ONSEND_SIGNATURE)> func);
				void SetOnDisconnect(std::function<void(ONDISCONNECT_SIGNATURE)> func);

				void PrintStats();
				uint64_t GetStat(StatName name) const;

				void ShrinkMemoryPool();
			};
		}
	}
}