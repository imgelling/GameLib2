#include "GameIOCP.h"

#define NETWORK_COMPLETION_TYPE pad[0]
#define NETWORK_CHANNEL pad[1]
#define NETWORK_SOCKET_TYPE pad[2]
namespace game
{
	namespace IOCP
	{
		namespace Network
		{

			static constexpr uint8_t NETWORK_SEND_COMPLETION_TYPE = 1;
			static constexpr uint8_t NETWORK_RECEIVE_COMPLETION_TYPE = 2;
			static constexpr uint8_t NETWORK_ACCEPT_COMPLETION_TYPE = 3;
			static constexpr uint8_t NETWORK_CONNECT_COMPLETION_TYPE = 4;
			static constexpr uint8_t NETWORK_SENDTO_COMPLETION_TYPE = 5;
			static constexpr uint8_t NETWORK_RECEIVEFROM_COMPLETION_TYPE = 6;
			static constexpr uint32_t sizeOfUInt = (uint32_t)sizeof(uint32_t);
			static constexpr uint32_t sizeOfHeader = sizeOfUInt + 1;

			NetworkManager::NetworkManager()
			{
				_stopping.store(0);
				_completionPort = INVALID_HANDLE_VALUE;
				_listenSocket = INVALID_SOCKET;
				_lpfnAcceptEx = nullptr;
				_lpfnConnectEx = nullptr;
			}

			void NetworkManager::Shrink()
			{
				_ioDataPool.Shrink();
			}

			bool NetworkManager::_ExtractHeaderForReceive(PER_IO_DATA_NETWORK* ioData, uint32_t& bytesReceived)
			{
				// If expectedTransferTotal is zero, then we need to extract a header
				if (!ioData->expectedTransferTotal)
				{
					uint8_t encodedHeader[5] = { 0 };
					uint32_t bytesFromConnection = 0;
					uint32_t sizeOfConnection = 0;
					if (ioData->NETWORK_SOCKET_TYPE == SOCK_STREAM)
					{
						std::lock_guard<std::mutex> lock(_connectionsMutex);
						sizeOfConnection = (uint32_t)_connections[ioData->socket].receiveData.size();
						bytesFromConnection = sizeOfConnection >= sizeOfUInt ? sizeOfUInt : sizeOfConnection;
						for (uint32_t byte = 0; byte < bytesFromConnection; byte++)
						{
							encodedHeader[byte] = _connections[ioData->socket].receiveData[byte];
						}
						if (sizeOfConnection >= sizeOfHeader)
						{
							ioData->NETWORK_CHANNEL = _connections[ioData->socket].receiveData[5];
							bytesFromConnection++;
						}
					}
					if (bytesFromConnection < sizeOfHeader)
					{
						uint32_t bytesFromNewData = (sizeOfHeader - bytesFromConnection);
						if (bytesReceived >= bytesFromNewData)
						{
							for (uint64_t byte = bytesFromConnection; byte < sizeOfHeader; byte++)
							{
								encodedHeader[byte] = ioData->buffer.buf[byte - bytesFromConnection];
							}
							// Remove the bytes we got from new data from the buffer
							GAME_ASSERT(ioData->buffer.buf + bytesFromNewData <= ioData->data + NETWORK_BUFFER_SIZE);
							ioData->buffer.buf += bytesFromNewData;
							GAME_ASSERT(ioData->buffer.len >= bytesFromNewData);
							ioData->buffer.len -= bytesFromNewData;
							// Remove the amount of bytes we just used from new data
							bytesReceived -= bytesFromNewData;
							// Remove the amount of bytes we just used from connection
							if (bytesFromConnection)
							{
								std::lock_guard<std::mutex> lock(_connectionsMutex);
								_connections[ioData->socket].receiveData.erase(_connections[ioData->socket].receiveData.begin(),
									_connections[ioData->socket].receiveData.begin() + bytesFromConnection);
							}
						}
						else
						{
							std::cout << "Not enough data to extract header.\n";
							if (ioData->NETWORK_SOCKET_TYPE == SOCK_STREAM)
							{
								std::lock_guard<std::mutex> lock(_connectionsMutex);
								_connections[ioData->socket].AddBytesReceivedBy(bytesReceived);
								_connections[ioData->socket].receiveData.insert(_connections[ioData->socket].receiveData.end(), ioData->buffer.buf, ioData->buffer.buf + bytesReceived);
							}
							ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
							ioData->buffer.buf = ioData->data;
							ioData->buffer.len = NETWORK_BUFFER_SIZE;
							ioData->bytesTransferred = bytesReceived;
							bytesReceived = 0;
							Receive(ioData->socket, ioData);
							return false;
						}
					}

					uint32_t encodedLengthInt = ((uint32_t)(encodedHeader[3]) << 24) | ((uint32_t)(encodedHeader[2]) << 16) | ((uint32_t)(encodedHeader[1]) << 8) | (uint32_t)(encodedHeader[0] * 255); //0;
					uint32_t decodedLength = ntohl(encodedLengthInt);
					GAME_ASSERT(decodedLength >= sizeOfHeader);
					ioData->expectedTransferTotal = decodedLength - sizeOfHeader;
					ioData->bytesTransferred = (uint32_t)(sizeOfConnection - bytesFromConnection);
					ioData->expectedTransferLeft = ioData->expectedTransferTotal - ioData->bytesTransferred;
					ioData->NETWORK_CHANNEL = encodedHeader[4];
					//std::cout << "Expected data length is : " << ioData->expectedTransferTotal << "\n";
					//std::cout << "Data transferred is : " << ioData->bytesTransferred << "\n";
					//std::cout << "Expected data left is : " << ioData->expectedTransferLeft << "\n";
					//std::cout << "Channel is : " << (uint32_t)ioData->channel << "\n";
				}
				return true;
			}

			uint32_t NetworkManager::_RemoveProcessedData(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived)
			{
				uint32_t bytesToRemove = 0;
				if (bytesReceived)
				{
					bytesToRemove = ioData->bytesTransferred % NETWORK_BUFFER_SIZE;
					GAME_ASSERT(ioData->buffer.buf + bytesToRemove <= ioData->data + NETWORK_BUFFER_SIZE);
					ioData->buffer.buf += bytesToRemove;
					GAME_ASSERT(ioData->buffer.len >= bytesToRemove);
					ioData->buffer.len -= bytesToRemove;
					ioData->expectedTransferTotal = 0;
				}
				ioData->expectedTransferLeft = 0;
				ioData->bytesTransferred = 0;
				if (ioData->NETWORK_SOCKET_TYPE == SOCK_STREAM)
				{
					std::lock_guard<std::mutex> lock(_connectionsMutex);
					_connections[ioData->socket].receiveData.clear();
				}
				GAME_ASSERT(bytesReceived - bytesToRemove >= 0);
				return bytesReceived - bytesToRemove;
			}

			void NetworkManager::_HandleNotEnoughDataReceived(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived)
			{
				if (ioData->NETWORK_SOCKET_TYPE == SOCK_STREAM)
				{
					std::lock_guard<std::mutex> lock(_connectionsMutex);
					_connections[ioData->socket].AddBytesReceivedBy(bytesReceived);
					_connections[ioData->socket].receiveData.insert(_connections[ioData->socket].receiveData.end(), ioData->buffer.buf, ioData->buffer.buf + bytesReceived);
				}
				ioData->buffer.buf = ioData->data;
				ioData->buffer.len = (uint32_t)(NETWORK_BUFFER_SIZE);
				ioData->expectedTransferLeft -= bytesReceived;

				Receive(ioData->socket, ioData);
			}

			bool NetworkManager::_DoOnReceive(PER_IO_DATA_NETWORK* ioData)
			{
				NetworkError error;
				std::vector<unsigned char> temp = _vectorPool.Allocate();
				if (ioData->NETWORK_SOCKET_TYPE == SOCK_STREAM)
				{
					std::lock_guard<std::mutex> lock(_connectionsMutex);
					_connections[ioData->socket].AddBytesReceivedBy(ioData->expectedTransferLeft);
					if (_connections[ioData->socket].receiveData.size())
					{
						temp.assign(_connections[ioData->socket].receiveData.begin(), _connections[ioData->socket].receiveData.end());
						_connections[ioData->socket].receiveData.clear();
					}
				}
				if (temp.size())
				{
					temp.insert(temp.end(), ioData->buffer.buf, ioData->buffer.buf + ioData->expectedTransferLeft);
				}
				else
					temp.assign(ioData->buffer.buf, ioData->buffer.buf + ioData->expectedTransferLeft);

				temp.emplace_back('\0');

				_OnReceive(ioData->socket, temp.data(), temp.size() - 1, ioData->NETWORK_CHANNEL, error);
				_vectorPool.Deallocate(temp);
				ioData->bytesTransferred = ioData->expectedTransferLeft;
				return true;
			}

			void NetworkManager::_HandleReceive(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived)
			{
				_stats.AddBytesReceived(bytesReceived);

				//std::cout << "NEW Data received, " << bytesReceived << " bytes.\n";
				// -------- comment below to web serve
				uint32_t totalReceivedData = bytesReceived;
				do
				{
					if (_ExtractHeaderForReceive(ioData, totalReceivedData))
					{
						//std::cout << "passed extract length\n";
						if (ioData->expectedTransferLeft > totalReceivedData)
						{
							//std::cout << "!!!! NotEnoughData\n";
							_HandleNotEnoughDataReceived(ioData, totalReceivedData);
							//std::cout << "Data size is " << totalReceivedData << "\n";
							//std::cout << "Expected is  " << ioData->expectedTransferTotal << "\n";
							//std::cout << "Expected left is " << ioData->expectedTransferLeft << "\n";
							totalReceivedData = 0;
							return;
						}
					}
					else
					{
						return;
					}
					_DoOnReceive(ioData);
					totalReceivedData = _RemoveProcessedData(ioData, totalReceivedData);
					//std::cout << "New Data size is " << totalReceivedData << "\n";
				} while (totalReceivedData > 0);
				//std::cout << "Data done processing!!!!!\n";
				if (ioData->expectedTransferLeft)
					std::cout << "Left over data to get!\n";
				// comment above to webserve
				//ioData->expectedTransferLeft = bytesReceived; // used for web serve
				//_DoOnReceive(ioData); // used for web serve

				Receive(ioData->socket);
				_DeleteIoData(ioData);
			}

			void NetworkManager::_HandleReceiveFrom(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesReceived)
			{
				_stats.AddBytesReceived(bytesReceived);

				uint32_t totalReceivedData = bytesReceived;
				do
				{
					if (_ExtractHeaderForReceive(ioData, totalReceivedData))
					{
						//std::cout << "passed extract length\n";
						if (ioData->expectedTransferLeft > totalReceivedData)
						{
							//std::cout << "!!!! NotEnoughData\n";
							_HandleNotEnoughDataReceived(ioData, totalReceivedData);
							//std::cout << "Data size is " << totalReceivedData << "\n";
							//std::cout << "Expected is  " << ioData->expectedTransferTotal << "\n";
							//std::cout << "Expected left is " << ioData->expectedTransferLeft << "\n";
							totalReceivedData = 0;
							return;
						}
					}
					else
					{
						return;
					}
					_DoOnReceive(ioData);
					totalReceivedData = _RemoveProcessedData(ioData, totalReceivedData);
					//std::cout << "New Data size is " << totalReceivedData << "\n";
				} while (totalReceivedData > 0);
				std::cout << "Data done processing!!!!!\n";
				if (ioData->expectedTransferLeft)
					std::cout << "Left over data to get!\n";

				ReceiveFrom(ioData->socket);
				_DeleteIoData(ioData);
			}
			void NetworkManager::_HandleSendTo(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesSent)
			{
				_stats.AddBytesSent(bytesSent);
				ioData->bytesTransferred += bytesSent;
				ioData->expectedTransferLeft = ioData->expectedTransferTotal - ioData->bytesTransferred;

				// Only sent partial data, send the rest of data
				// Never tested
				if (ioData->expectedTransferLeft > 0)
				{
					//std::cout << "!!!!!!!PARTIAL SEND Expected left : " << ioData->expectedTransferLeft << "\n";
					//std::cout << "Partial send of " << bytesSent << " bytes with " << ioData->expectedTransferLeft << " bytes left.\n";
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
					ioData->buffer.buf = ioData->data + ioData->bytesTransferred;
					ioData->buffer.len = (uint32_t)(sizeof(ioData->data) - ioData->bytesTransferred);
					SendTo(ioData->socket, ioData->addr, NULL, 0, ioData->NETWORK_CHANNEL, ioData);
					return;
				}

				NetworkError error;
				_OnSend(ioData->socket, (uint64_t)bytesSent - sizeOfHeader, error);
				_DeleteIoData(ioData);
			}
			void NetworkManager::_HandleSend(PER_IO_DATA_NETWORK* ioData, const uint32_t bytesSent)
			{
				_stats.AddBytesSent(bytesSent);
				ioData->bytesTransferred += bytesSent;
				ioData->expectedTransferLeft = ioData->expectedTransferTotal - ioData->bytesTransferred;

				// Only sent partial data, send the rest of data
				// Never tested
				if (ioData->expectedTransferLeft > 0)
				{
					//std::cout << "!!!!!!!PARTIAL SEND Expected left : " << ioData->expectedTransferLeft << "\n";
					//std::cout << "Partial send of " << bytesSent << " bytes with " << ioData->expectedTransferLeft << " bytes left.\n";
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
					ioData->buffer.buf = ioData->data + ioData->bytesTransferred;
					ioData->buffer.len = (uint32_t)(sizeof(ioData->data) - ioData->bytesTransferred);
					Send(ioData->socket, NULL, 0, ioData->NETWORK_CHANNEL, ioData);
					return;
				}

				// All data was sent
				{
					std::lock_guard<std::mutex> lock(_connectionsMutex);
					_connections[ioData->socket].AddBytesSentFrom(bytesSent);
				}
				NetworkError error;
				_OnSend(ioData->socket, (uint64_t)bytesSent - sizeOfUInt, error);
				_DeleteIoData(ioData);
			}
			void NetworkManager::_HandleAccept(PER_IO_DATA_NETWORK* ioData)
			{
				//std::cout << "Socket Connected\n";
				// Associate accepted socket with completion port
				if (!CreateIoCompletionPort((HANDLE)ioData->socket, _completionPort, (ULONG_PTR)ioData->socket, 0))
				{
					game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__);
					_CloseConnection(ioData, __LINE__);
				}
				else
				{
					// Update the connected socket properties to show that it is connected (for getsock/setsock functions)
					if (setsockopt(ioData->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&_listenSocket, sizeof(_listenSocket)) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("setsockopt", __LINE__);
					}

					// Save local and remote ip info in connection
					SOCKADDR_IN* localAddr = (SOCKADDR_IN*)ioData->data;
					int32_t localAddrLen = sizeof(SOCKADDR_IN) + 16;
					int32_t remoteAddrLen = sizeof(SOCKADDR_IN) + 16;
					SOCKADDR_IN* remoteAddr = (SOCKADDR_IN*)(ioData->data + localAddrLen);


					char localAddrStr[INET6_ADDRSTRLEN] = { 0 };
					char remoteAddrStr[INET6_ADDRSTRLEN] = { 0 };

					getnameinfo((SOCKADDR*)localAddr, localAddrLen, localAddrStr, sizeof(localAddrStr), nullptr, 0, NI_NUMERICHOST);
					getnameinfo((SOCKADDR*)remoteAddr, remoteAddrLen, remoteAddrStr, sizeof(remoteAddrStr), nullptr, 0, NI_NUMERICHOST);

					uint16_t localPort = ntohs(localAddr->sin_port);
					uint16_t remotePort = ntohs(remoteAddr->sin_port);

					_AddConnection(ioData);
					_stats.AddConnection();

					{
						std::lock_guard<std::mutex> lock(_connectionsMutex);
						_connections[ioData->socket].socket = ioData->socket;
						_connections[ioData->socket].remoteIpAddress = remoteAddrStr;
						_connections[ioData->socket].remotePort = remotePort;
						_connections[ioData->socket].localIpAddress = localAddrStr;
						_connections[ioData->socket].localPort = localPort;
					}

					NetworkError error;
					_OnAccept(ioData->socket, error);

					Receive(ioData->socket);
				}

				// Clean up
				_DeleteIoData(ioData);

				// Start another accept
				Accept();
			}
			void NetworkManager::_HandleConnect(PER_IO_DATA_NETWORK* ioData)
			{
				// Update the connected socket properties to show that it is connected (for getsock/setsock functions)
				if (setsockopt(ioData->socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0) == SOCKET_ERROR)
				{
					game::IOCP::ErrorOutput("setsockopt", __LINE__);
					_CloseConnection(ioData, __LINE__);
					return;
				}

				if (_attributes.tcpNoDelay)
				{
					int32_t flag = 1;
					if (setsockopt(ioData->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("setsockopt", __LINE__);
						_CloseConnection(ioData, __LINE__);
						return;
					}
				}

				if (_attributes.port == NULL)
				{
					_AddConnection(ioData);
					_stats.AddConnection();

					sockaddr_in address = { 0 };
					sockaddr_in addressr = { 0 };
					int32_t addrLen = sizeof(address);
					int32_t addrLenr = sizeof(addressr);
					char localAddrStr[INET6_ADDRSTRLEN] = { 0 };
					char remoteAddrStr[INET6_ADDRSTRLEN] = { 0 };
					uint16_t localPort = 0;
					uint16_t remotePort = 0;

					// Get local socket information
					if (getsockname(ioData->socket, (sockaddr*)&address, &addrLen) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("getsockname", __LINE__);
						_CloseConnection(ioData, __LINE__);
					}
					else
					{
						inet_ntop(AF_INET, &address.sin_addr, localAddrStr, INET_ADDRSTRLEN);
						localPort = ntohs(address.sin_port);
					}

					// Get remote socket information
					if (getpeername(ioData->socket, (sockaddr*)&addressr, &addrLenr) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("getpeername", __LINE__);
						_CloseConnection(ioData, __LINE__);
					}
					else
					{
						inet_ntop(AF_INET, &addressr.sin_addr, remoteAddrStr, INET_ADDRSTRLEN);
						remotePort = ntohs(addressr.sin_port);
					}

					{
						std::lock_guard<std::mutex> lock(_connectionsMutex);
						_connections[ioData->socket].remoteIpAddress = remoteAddrStr;
						_connections[ioData->socket].remotePort = remotePort;
						_connections[ioData->socket].localIpAddress = localAddrStr;
						_connections[ioData->socket].localPort = localPort;
					}
				}
				NetworkError err;
				_OnConnect(ioData->socket, err);

				Receive(ioData->socket);
				_DeleteIoData(ioData);
			}

			void NetworkManager::_OnReceiveDefault(const SOCKET socket, const unsigned char* data, const uint64_t bytesReceived, const uint32_t channel, const NetworkError& error)
			{
				std::cout << "Default OnReceive function! Create your own with the signature of\n";
				std::cout << "void(const SOCKET, const std::vector<unsigned char>&, const uint64_t, const game::Network::NetworkError&)\n";
				std::cout << bytesReceived << " bytes received : \n";
				std::cout << data << '\n';
				std::cout << "From socket : " << socket << "\n";
				std::cout << "On channel  : " << channel << "\n";
			}
			void NetworkManager::_OnSendDefault(const SOCKET socket, const uint64_t bytesSent, const NetworkError& error)
			{
				std::cout << "Default OnSend function! Create your own with the signature of\n";
				std::cout << "void(const SOCKET, const uint64_t, const game::Network::NetworkError&)\n";
				std::cout << "Sent " << bytesSent << " bytes to socket : " << socket << "\n";
			}
			void NetworkManager::_OnConnectDefault(const SOCKET socket, const NetworkError& error)
			{
				std::cout << "Default OnConnect function! Create your own with the signature of\n";
				std::cout << "void(const SOCKET socket, const game::Network::NetworkError& error)\n";
				std::cout << "Connected to Remote!\n";
				std::cout << "Connected with socket : " << socket << "\n";
			}
			void NetworkManager::_OnAcceptDefault(const SOCKET socket, const NetworkError& error)
			{
				std::cout << "Default OnAccept function! Create your own with the signature of\n";
				std::cout << "void(const SOCKET socket, const game::Network::NetworkError& error)\n";
				std::cout << "Incoming Connection Connected!\n";
				std::cout << "Connected with socket : " << socket << "\n";
			}
			void NetworkManager::_OnDisconnectDefault(const SOCKET socket, const NetworkError& error)
			{
				std::cout << "Default OnDisconnect function! Create your own with the signature of\n";
				std::cout << "void(const SOCKET socket, const game::Network::NetworkError& error)\n";
				std::cout << "Socket disconnected.\n";
				std::cout << "Socket : " << socket << "\n";
			}

			void NetworkManager::SetOnReceive(std::function<void(const SOCKET socket, const unsigned char*, const uint64_t, const uint32_t channel, const NetworkError&)> func)
			{
				_OnReceive = func;
			}
			void NetworkManager::SetOnConnect(std::function<void(const SOCKET socket, const NetworkError& error)> func)
			{
				_OnConnect = func;
			}
			void NetworkManager::SetOnAccept(std::function<void(const SOCKET socket, const NetworkError& error)> func)
			{
				_OnAccept = func;
			}
			void NetworkManager::SetOnSend(std::function<void(const SOCKET socket, const uint64_t, const NetworkError&)> func)
			{
				_OnSend = func;
			}
			void NetworkManager::SetOnDisconnect(std::function<void(const SOCKET socket, const NetworkError& error)> func)
			{
				_OnDisconnect = func;
			}

			void NetworkManager::Receive(const SOCKET socket, PER_IO_DATA_NETWORK* ioData_in)
			{
				PER_IO_DATA_NETWORK* ioData = ioData_in;

				if (ioData == nullptr)
				{
					ioData = (PER_IO_DATA_NETWORK*)_ioDataPool.Allocate();
					ioData->ioDataType = game::IOCP::IOCP_TYPE_NETWORK;
					ioData->NETWORK_COMPLETION_TYPE = NETWORK_RECEIVE_COMPLETION_TYPE;
					_stats.MemoryAllocate(ioData->NETWORK_COMPLETION_TYPE);
					ioData->socket = socket;
					ioData->NETWORK_SOCKET_TYPE = (uint8_t)SOCK_STREAM;
					ioData->buffer.buf = ioData->data;
					ioData->buffer.len = (u_long)sizeof(ioData->data);
					ioData->expectedTransferLeft = 0;
					ioData->expectedTransferTotal = ioData->expectedTransferLeft;
					{
						std::lock_guard<std::mutex> lock(_connectionsMutex);
						_connections[ioData->socket].receiveData.clear();
					}
				}
				else
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));


				// Receive data
				DWORD flags = 0;
				WSARecv(socket, &ioData->buffer, 1, NULL, &flags, &ioData->overlapped, NULL);
				if (game::IOCP::ErrorOutput("WSARecv", __LINE__))
				{
					_CloseConnection(ioData, __LINE__);
				}
			}
			// UDP ONLY
			void NetworkManager::ReceiveFrom(const SOCKET socket, PER_IO_DATA_NETWORK* ioData_in)
			{
				PER_IO_DATA_NETWORK* ioData = ioData_in;

				if (ioData == nullptr)
				{
					ioData = (PER_IO_DATA_NETWORK*)_ioDataPool.Allocate();
					ioData->ioDataType = game::IOCP::IOCP_TYPE_NETWORK;
					ioData->NETWORK_COMPLETION_TYPE = NETWORK_RECEIVEFROM_COMPLETION_TYPE;
					_stats.MemoryAllocate(ioData->NETWORK_COMPLETION_TYPE);
					ioData->socket = socket;
					ioData->buffer.buf = ioData->data;
					ioData->buffer.len = (u_long)sizeof(ioData->data);
					ioData->expectedTransferLeft = 0;
					ioData->expectedTransferTotal = ioData->expectedTransferLeft;
					ioData->addr = { 0 };
					ioData->NETWORK_SOCKET_TYPE = (uint8_t)SOCK_DGRAM;
					//if (ioData->NETWORK_SOCKET_TYPE == SOCK_STREAM)
					//{
					//	std::lock_guard<std::mutex> lock(_connectionsMutex);
					//	_connections[ioData->socket].receiveData.clear();
					//}
				}
				else
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));


				// Receive data
				DWORD flags = 0;
				int senderAddrSize = sizeof(ioData->addr);
				WSARecvFrom(ioData->socket, &ioData->buffer, 1, NULL, &flags, (sockaddr*)&ioData->addr, &senderAddrSize, &ioData->overlapped, nullptr);
				if (game::IOCP::ErrorOutput("WSARecvFrom", __LINE__))
				{
					_CloseConnection(ioData, __LINE__);
				}
			}
			void NetworkManager::Send(const SOCKET socket, const unsigned char* data, const uint64_t length, const uint8_t channel, PER_IO_DATA_NETWORK* ioData_in)
			{
				PER_IO_DATA_NETWORK* ioData = ioData_in;

				if (length > ((uint64_t)NETWORK_BUFFER_SIZE - sizeOfHeader))
				{
					std::cout << "Send length too long, needs to be less than " << (NETWORK_BUFFER_SIZE - sizeOfHeader) << "\n";
					return;
				}

				if (ioData == nullptr)
				{
					ioData = (PER_IO_DATA_NETWORK*)_ioDataPool.Allocate(); //new PER_IO_DATA;
					ioData->ioDataType = game::IOCP::IOCP_TYPE_NETWORK;
					ioData->NETWORK_COMPLETION_TYPE = NETWORK_SEND_COMPLETION_TYPE;
					_stats.MemoryAllocate(ioData->NETWORK_COMPLETION_TYPE);
					ioData->socket = socket;
					ioData->NETWORK_CHANNEL = channel;
					ioData->NETWORK_SOCKET_TYPE = (uint8_t)SOCK_STREAM;
					// --- comment below to web serve
					// Make length network byte order
					uint32_t pre = (uint32_t)(length + sizeOfHeader);
					uint32_t networkdata = htonl(pre);
					// Copy length to front of buffer
					memcpy(ioData->data, &networkdata, sizeOfUInt);
					// Copy channel into data
					ioData->data[4] = channel;
					// Copy data into buffer offset by header size from header encoding
					memcpy(ioData->data + sizeOfHeader, data, length);
					// --- comment above to webserve
					//memcpy(ioData->data, data, length); // to webserve
					ioData->buffer.buf = ioData->data;
					ioData->buffer.len = (uint32_t)length + sizeOfHeader; // remove + sizeofHeader for webserve
					ioData->expectedTransferTotal = ioData->buffer.len;
					ioData->expectedTransferLeft = ioData->buffer.len;
				}

				WSASend(socket, &ioData->buffer, 1, NULL, 0, &ioData->overlapped, NULL);
				if (game::IOCP::ErrorOutput("WSASend", __LINE__))
				{
					_CloseConnection(ioData, __LINE__);
				}
			}
			// UDP ONLY
			void NetworkManager::SendTo(const SOCKET socket, const sockaddr_in& addr, const unsigned char* data, const uint64_t length, uint8_t channel, PER_IO_DATA_NETWORK* ioData_in)
			{
				PER_IO_DATA_NETWORK* ioData = ioData_in;
				if (length > ((uint64_t)NETWORK_BUFFER_SIZE - sizeOfHeader))
				{
					std::cout << "SendTo length too long, needs to be less than " << (NETWORK_BUFFER_SIZE - sizeOfHeader) << "\n";
					return;
				}
				if (ioData == nullptr)
				{
					ioData = (PER_IO_DATA_NETWORK*)_ioDataPool.Allocate(); //new PER_IO_DATA;
					ioData->ioDataType = game::IOCP::IOCP_TYPE_NETWORK;
					ioData->NETWORK_COMPLETION_TYPE = NETWORK_SENDTO_COMPLETION_TYPE;
					ioData->NETWORK_SOCKET_TYPE = (uint8_t)SOCK_DGRAM;
					_stats.MemoryAllocate(ioData->NETWORK_COMPLETION_TYPE);
					ioData->socket = socket;
					ioData->NETWORK_CHANNEL = channel;
					ioData->addr = addr;
					// Make length network byte order
					uint32_t pre = (uint32_t)(length + sizeOfHeader);
					uint32_t networkdata = htonl(pre);
					// Copy length to front of buffer
					memcpy(ioData->data, &networkdata, sizeOfUInt);
					// Copy channel into data
					ioData->data[4] = channel;
					// Copy data into buffer offset by uint32_t size for length encoding
					memcpy(ioData->data + sizeOfHeader, data, length);

					ioData->buffer.buf = ioData->data;
					ioData->buffer.len = (uint32_t)length + sizeOfHeader;
					ioData->expectedTransferTotal = ioData->buffer.len;
					ioData->expectedTransferLeft = ioData->buffer.len;
				}

				WSASendTo(socket, &ioData->buffer, 1, NULL, 0, (sockaddr*)&addr, sizeof(addr), &ioData->overlapped, NULL);
				if (game::IOCP::ErrorOutput("WSASendTo", __LINE__))
				{
					_CloseConnection(ioData, __LINE__);
				}
			}
			void NetworkManager::Accept()
			{
				// Allocate I/O data structure
				PER_IO_DATA_NETWORK* ioData = (PER_IO_DATA_NETWORK*)_ioDataPool.Allocate(); //new PER_IO_DATA();
				ioData->ioDataType = game::IOCP::IOCP_TYPE_NETWORK;
				ioData->NETWORK_COMPLETION_TYPE = NETWORK_ACCEPT_COMPLETION_TYPE;
				_stats.MemoryAllocate(ioData->NETWORK_COMPLETION_TYPE);

				ioData->socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
				if (ioData->socket == INVALID_SOCKET)
				{
					game::IOCP::ErrorOutput("WSASocket", __LINE__);
					_DeleteIoData(ioData);
					return;
				}

				if (_attributes.tcpNoDelay)
				{
					int32_t flag = 1; 
					if (setsockopt(ioData->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) == SOCKET_ERROR) 
					{
						game::IOCP::ErrorOutput("setsockopt", __LINE__);
						_CloseConnection(ioData, __LINE__);
						return;
					}
				}

				DWORD bytesReceived = 0;
				_lpfnAcceptEx(_listenSocket, ioData->socket, (void*)&ioData->data, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, &ioData->overlapped);
				if (game::IOCP::ErrorOutput("lpfnAcceptEx", __LINE__))
				{
					_CloseConnection(ioData, __LINE__);
				}
			}
			void NetworkManager::Connect(const std::string& ipAddress, const uint32_t port)
			{
				// Create a socket
				SOCKET Socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
				if (Socket == INVALID_SOCKET)
				{
					game::IOCP::ErrorOutput("WSASocket", __LINE__);
					return;
				}

				// Retrieve the ConnectEx function pointer if we haven't yet
				if (!_lpfnConnectEx)
				{
					GUID guidConnectEx = WSAID_CONNECTEX;
					DWORD bytesReturned = 0;
					if (WSAIoctl(Socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx),
						&_lpfnConnectEx, sizeof(_lpfnConnectEx), &bytesReturned, nullptr, nullptr) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("WSAIoctl", __LINE__);
						closesocket(Socket);
						return;
					}
				}

				SOCKADDR_IN localAddr = { 0 };
				localAddr.sin_family = AF_INET;
				localAddr.sin_addr.s_addr = INADDR_ANY;
				localAddr.sin_port = 0; // Let the system choose a port

				if (bind(Socket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
				{
					game::IOCP::ErrorOutput("bind", __LINE__);
					closesocket(Socket);
					return;
				}

				// Associate with Completion port
				if (!CreateIoCompletionPort((HANDLE)Socket, _completionPort, (ULONG_PTR)Socket, 0))
				{
					game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__);
					closesocket(Socket);
					return;
				}

				PER_IO_DATA_NETWORK* ioData = (PER_IO_DATA_NETWORK*)_ioDataPool.Allocate();
				ioData->ioDataType = game::IOCP::IOCP_TYPE_NETWORK;
				ioData->NETWORK_COMPLETION_TYPE = NETWORK_CONNECT_COMPLETION_TYPE;
				_stats.MemoryAllocate(ioData->NETWORK_COMPLETION_TYPE);
				ioData->socket = Socket;
				ioData->buffer.buf = ioData->data;
				ioData->buffer.len = (u_long)sizeof(ioData->data);


				// Fill out address info to connect to
				addrinfo hints = { 0 };
				addrinfo* addressInfoResults = nullptr;

				// Set up hints
				hints.ai_family = AF_INET;       // IPv4 (AF_INET) or IPv6 (AF_INET6)
				hints.ai_socktype = SOCK_STREAM;  // TCP
				hints.ai_protocol = IPPROTO_TCP;  // TCP protocol

				// Resolve the server address and port
				getaddrinfo(ipAddress.c_str(), nullptr, &hints, &addressInfoResults);
				if (game::IOCP::ErrorOutput("getaddrinfo", __LINE__))
				{
					closesocket(Socket);
					_DeleteIoData(ioData);
					return;
				}

				// Loop through all results
				char ipstr[INET6_ADDRSTRLEN] = { 0 };
				for (addrinfo* iterator = addressInfoResults; iterator != nullptr; iterator = iterator->ai_next)
				{
					void* address = nullptr;
					const char* ipVersion = nullptr;

					// Get pointer to the address (IPv4 or IPv6)
					if (iterator->ai_family == AF_INET)
					{ // IPv4
						sockaddr_in* ipv4 = (sockaddr_in*)iterator->ai_addr;
						address = &(ipv4->sin_addr);
						ipVersion = "IPv4";
					}
					else
					{ // IPv6
						sockaddr_in6* ipv6 = (sockaddr_in6*)iterator->ai_addr;
						address = &(ipv6->sin6_addr);
						ipVersion = "IPv6";
					}

					// Convert the IP to a string
					inet_ntop(iterator->ai_family, address, ipstr, sizeof(ipstr));
					std::cout << ipVersion << ": " << ipstr << std::endl;
				}


				SOCKADDR_IN remoteAddress = {};
				remoteAddress.sin_family = AF_INET;
				inet_pton(remoteAddress.sin_family, ipstr, &remoteAddress.sin_addr.s_addr);
				remoteAddress.sin_port = htons((uint16_t)port);

				DWORD bytesSent = 0;
				if (!_lpfnConnectEx(Socket, (sockaddr*)&remoteAddress, sizeof(remoteAddress), nullptr, 0, &bytesSent, &ioData->overlapped))
				{
					if (game::IOCP::ErrorOutput("lpfnConnectEx", __LINE__))
					{
						closesocket(Socket);
						return;
					}
				}
			}
			void NetworkManager::BroadCast(const unsigned char* data, const uint64_t length, const uint8_t channel, const SOCKET except)
			{
				std::lock_guard<std::mutex> lock(_connectionsMutex);
				for (auto& a : _connections)
				{
					if (a.first != except)
					{
						Send((SOCKET)a.first, data, length, channel);
					}
				}
			}

			void NetworkManager::_DoWork(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, game::IOCP::PER_IO_DATA* ioDataIn)
			{
				PER_IO_DATA_NETWORK* ioData = (PER_IO_DATA_NETWORK*)ioDataIn;
				if (!result) // GetQueuedCompletionStatus failed
				{
					int32_t error = WSAGetLastError();
					switch (error)
					{
					case WSA_OPERATION_ABORTED:
					case ERROR_CONNECTION_ABORTED: _CloseConnection(ioData, __LINE__, true); return;
					case WSA_INVALID_HANDLE:
					case ERROR_ABANDONED_WAIT_0: if (_stopping.load()) { return; }
											   else game::IOCP::ErrorOutput("GetQueuedCompletionStatus", __LINE__); break;
					case ERROR_NETNAME_DELETED: /*std::cout << "---- Socket disconnected!\n";*/ break;
					case ERROR_CONNECTION_REFUSED: ioData->socket = INVALID_SOCKET; std::cout << "--- Connection Refused\n"; break;
					default: game::IOCP::ErrorOutput("GetQueuedCompletionStatus fail", __LINE__);
					}
					_CloseConnection(ioData, __LINE__);
					return;
				} // end GetQueuedCompletionStatus failed

				// If bytes transferred == 0, then the socket a send or receive was used on disconnected 
				if ((bytesTransferred == 0) && (ioData->NETWORK_COMPLETION_TYPE != NETWORK_ACCEPT_COMPLETION_TYPE) && (ioData->NETWORK_COMPLETION_TYPE != NETWORK_CONNECT_COMPLETION_TYPE))
				{
					_CloseConnection(ioData, __LINE__);
					return;
				}

				// Handle the data depending on data type
				switch (ioData->NETWORK_COMPLETION_TYPE)
				{
				case NETWORK_SEND_COMPLETION_TYPE: _HandleSend(ioData, bytesTransferred); return;
				case NETWORK_SENDTO_COMPLETION_TYPE: _HandleSendTo(ioData, bytesTransferred); return;
				case NETWORK_RECEIVE_COMPLETION_TYPE: _HandleReceive(ioData, bytesTransferred); return;
				case NETWORK_RECEIVEFROM_COMPLETION_TYPE: _HandleReceiveFrom(ioData, bytesTransferred); return;
				case NETWORK_ACCEPT_COMPLETION_TYPE: _HandleAccept(ioData); return;
				case NETWORK_CONNECT_COMPLETION_TYPE: _HandleConnect(ioData); return;
				default: "GetQueuedCompletionStatus received an invalid PER_IO_DATA type.\n"; break;
				}

				// Only called if bad data type was given
				_DeleteIoData(ioData);
			}

			void NetworkManager::_AddConnection(const PER_IO_DATA_NETWORK* ioData)
			{
				Connection newConnection;
				newConnection.socket = ioData->socket;
				std::lock_guard<std::mutex> lock(_connectionsMutex);
				auto t = _connections.try_emplace(newConnection.socket, newConnection);
				if (!t.second)
					std::cout << "Connection is already connected!\n";
			}
			void NetworkManager::_CloseConnection(PER_IO_DATA_NETWORK* ioData, const uint32_t line, const bool alreadyClosed)
			{
				if (ioData)
				{
					int64_t connectionConnected = 0;
					{
						std::lock_guard<std::mutex> lock(_connectionsMutex);
						connectionConnected = _connections.erase(ioData->socket);
					}
					if (ioData->socket != INVALID_SOCKET)
					{
						if (!alreadyClosed)
						{
							if (shutdown(ioData->socket, SD_BOTH) == SOCKET_ERROR)
							{
								game::IOCP::ErrorOutput("shutdown", line);
							}
							if (closesocket(ioData->socket) == SOCKET_ERROR)
							{
								game::IOCP::ErrorOutput("closesocket", line);
							}
						}
						// Dont want to report open accept sockets to _OnDisconnect
						if (connectionConnected)
						{
							NetworkError error;
							_OnDisconnect(ioData->socket, error);
						}
						ioData->socket = INVALID_SOCKET;
					}
					_DeleteIoData(ioData);
				}
			}
			void NetworkManager::GetConnections(std::vector<SOCKET>& connections)
			{
				std::lock_guard<std::mutex> lock(_connectionsMutex);
				for (const auto& pair : _connections) {
					connections.push_back(pair.first);
				}
			}
			void NetworkManager::GetConnectionStats(const SOCKET socket, uint64_t& receivedFrom, uint64_t& sentTo)
			{
				std::lock_guard<std::mutex> lock(_connectionsMutex);
				auto a = _connections.find(socket);
				if (a != _connections.end())
				{
					receivedFrom = _connections[socket].BytesReceivedBy();
					sentTo = _connections[socket].BytesSentFrom();
				}
				else
				{
					receivedFrom = 0;
					sentTo = 0;
				}
			}
			const ConnectionInfo NetworkManager::GetConnectionInfo(const SOCKET socket)
			{
				std::lock_guard<std::mutex> lock(_connectionsMutex);
				auto a = _connections.find(socket);
				if (a != _connections.end())
				{
					return a->second.GetInfo();
				}
				else
				{
					ConnectionInfo info;
					return info;
				}
			}

			const HANDLE NetworkManager::GetCompletionPort() const
			{
				return _completionPort;
			}

			void NetworkManager::_DeleteIoData(PER_IO_DATA_NETWORK* ioData)
			{
				_stats.MemoryDeallocate(ioData->NETWORK_COMPLETION_TYPE);
				_ioDataPool.Deallocate(ioData);
			}

			bool NetworkManager::Initialize(const NetworkAttributes& attributes, game::IOCP::IOCPManager& ioHandler)
			{
				_stats.verbose = attributes.verboseOutputDEBUG;
				_attributes = attributes;

				// If port is set, then it will need atleast 1 accept
				if (_attributes.port)
				{
					_attributes.numberOfAcceptors = max(1, _attributes.numberOfAcceptors);
				}
				else
				{
					_attributes.numberOfAcceptors = max(0, _attributes.numberOfAcceptors);
				}

				_ioDataPool.Initialize(sizeof(PER_IO_DATA_NETWORK), attributes.initialIoDataPoolSize);

				_vectorPool.Initialize(attributes.initialVectorPoolSize, attributes.vectorPoolReserveSize);

				_OnReceive = std::bind(&NetworkManager::_OnReceiveDefault, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
				_OnConnect = std::bind(&NetworkManager::_OnConnectDefault, this, std::placeholders::_1, std::placeholders::_2);
				_OnAccept = std::bind(&NetworkManager::_OnAcceptDefault, this, std::placeholders::_1, std::placeholders::_2);
				_OnSend = std::bind(&NetworkManager::_OnSendDefault, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
				_OnDisconnect = std::bind(&NetworkManager::_OnDisconnectDefault, this, std::placeholders::_1, std::placeholders::_2);

				// Start WinSock 2.2
				WSADATA WSAData;
				if (!WSAStartup(MAKEWORD(2, 2), &WSAData))
				{
					game::IOCP::ErrorOutput("WSAStartup", __LINE__);
				}
				std::cout << "WinSock started.\n";

				// Create the main completion port
				if (ioHandler.GetCompeltionPort() == INVALID_HANDLE_VALUE)
				{
					return false;
				}
				_completionPort = ioHandler.GetCompeltionPort();
				auto callback = [&](int32_t result, DWORD bytesTransferred, ULONG_PTR completionKey, game::IOCP::PER_IO_DATA* ioData) // Hides this pointer and allows for dynamic arguments
					{
						_DoWork(result, bytesTransferred, completionKey, ioData);
					};
				ioHandler.SetNetworkFunction(callback);

				if (_attributes.port != NULL)
				{
					// Create the listen socket
					_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
					if (_listenSocket == INVALID_SOCKET)
					{
						game::IOCP::ErrorOutput("WSASocket", __LINE__);
						return false;
					}

					// Assign address and port to listen socket
					SOCKADDR_IN serverAddr = { 0 };
					serverAddr.sin_family = AF_INET;
					serverAddr.sin_port = htons(_attributes.port);
					serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
					if (bind(_listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("bind", __LINE__);
						WSACleanup();
						return false;
					}

					// Start listening
					if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("listen", __LINE__);
						closesocket(_listenSocket);
						WSACleanup();
						return false;
					}

					// Associate the listening port to the completion port
					if (!CreateIoCompletionPort((HANDLE)_listenSocket, _completionPort, (ULONG_PTR)_listenSocket, 0))
					{
						game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__);
						closesocket(_listenSocket);
						WSACleanup();
						return false;
					}

					// Get a pointer to WSAAcceptEx
					GUID GuidAcceptEx = WSAID_ACCEPTEX;
					DWORD dwBytes = 0;
					if (WSAIoctl(_listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &_lpfnAcceptEx, sizeof(_lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("WSAIoctl", __LINE__);
						closesocket(_listenSocket);
						_listenSocket = INVALID_SOCKET;
						WSACleanup();
						return false;
					}

					// Start the accepting
					for (uint32_t i = 0; i < _attributes.numberOfAcceptors; i++)
						Accept();
				}
				return true;
			}
			void NetworkManager::Shutdown()
			{
				std::cout << "Stopping Socket operations.\n";

				// Set flag we are quitting
				_stopping.store(1);

				// Close the listening socket
				if (_listenSocket != INVALID_SOCKET)
				{
					if (closesocket(_listenSocket) == SOCKET_ERROR)
					{
						game::IOCP::ErrorOutput("closesocket", __LINE__);
					}
					_listenSocket = INVALID_SOCKET;
				}

				// Close any open connections gracefully
				{
					std::lock_guard<std::mutex> lock(_connectionsMutex); // Is this a problem? maybe move inside?
					for (auto& i : _connections)
					{
						if (shutdown(i.second.socket, SD_BOTH) == SOCKET_ERROR)
						{
							game::IOCP::ErrorOutput("shutdown", __LINE__);
						}
						if (closesocket(i.second.socket) == SOCKET_ERROR)
						{
							game::IOCP::ErrorOutput("closesocket", __LINE__);
						}
						i.second.socket = INVALID_SOCKET;
					}
				}

				// Wait a second to let threads clean up
				Sleep(1000);

				// Clear out the connections
				{
					std::lock_guard<std::mutex> lock(_connectionsMutex);
					_connections.clear();
				}
				// Clean up winsock
				std::cout << "Cleaning up winsock.\n";
				if (WSACleanup() == SOCKET_ERROR)
				{
					game::IOCP::ErrorOutput("WSACleanup", __LINE__);
				}
			}

			uint64_t NetworkManager::GetStat(StatName name) const
			{
				//NetworkInternalStats::statName::bytesReceived
				uint64_t ret = _stats.GetStat(name);
				if (ret == UINT64_MAX)
				{
					ret = _ioDataPool.GetStat(name);
				}
				return ret;
			}

			void NetworkManager::PrintStats()
			{
				std::cout << "<--------------------------------------->\n";
				_ioDataPool.PrintStats("IoData Pool");
				_vectorPool.PrintStats("Vector Pool");
				_stats.PrintStats();
				std::cout << "<--------------------------------------->\n";
			}

			NetworkInternalStats::NetworkInternalStats()
			{
				_receiveAllocateCount.store(0);
				_receiveDeallocateCount.store(0);
				_sendAllocateCount.store(0);
				_sendDeallocateCount.store(0);
				_acceptAllocateCount.store(0);
				_acceptDeallocateCount.store(0);
				_bytesSent.store(0);
				_bytesReceived.store(0);
				verbose = true;
				_numberOfConnections = 0;
			}

			uint64_t NetworkInternalStats::GetStat(const uint32_t stat) const
			{
				switch (stat)
				{
				case StatName::BYTES_RECEIVED: return _bytesReceived;
				case StatName::BYTES_SENT: return _bytesSent;
				case StatName::ACCEPT_ALLOCATE_COUNT: return _acceptAllocateCount;
				case StatName::ACCEPT_DEALLOCATE_COUNT: return _acceptDeallocateCount;
				case StatName::CONNECT_ALLOCATE_COUNT: return _connectAllocateCount;
				case StatName::CONNECT_DEALLOCATE_COUNT: return _connectDeallocateCount;
				case StatName::RECEIVE_ALLOCATE_COUNT: return _receiveAllocateCount;
				case StatName::RECEIVE_DEALLOCATE_COUNT: return _receiveDeallocateCount;
				case StatName::SEND_ALLOCATE_COUNT: return _sendAllocateCount;
				case StatName::SEND_DEALLOCATE_COUNT: return _sendDeallocateCount;
				default: return UINT64_MAX;
				}
			}

			void NetworkInternalStats::AddConnection()
			{
				_numberOfConnections++;
			}
			void NetworkInternalStats::AddBytesReceived(const uint64_t count)
			{
				_bytesReceived.fetch_add(count);
			}
			void NetworkInternalStats::AddBytesSent(const uint64_t count)
			{
				_bytesSent.fetch_add(count);
			}
			void NetworkInternalStats::PrintStats()
			{
#if defined(_DEBUG)
				std::cout << "Connect Allocs/Deallocs  : " << _connectAllocateCount.load() << "/" << _connectDeallocateCount.load() << "\n";
				std::cout << "Receive Allocs/Deallocs  : " << _receiveAllocateCount.load() << "/" << _receiveDeallocateCount.load() << "\n";
				std::cout << "Send Allocs/Deallocs     : " << _sendAllocateCount.load() << "/" << _sendDeallocateCount.load() << "\n";
				std::cout << "Accept Allocs/Deallocs   : " << _acceptAllocateCount.load() << "/" << _acceptDeallocateCount.load() << "\n";
#endif
				std::cout << "Total Data Sent     : " << formatDataSize(_bytesSent.load()) << "\n";
				std::cout << "Total Data Received : " << formatDataSize(_bytesReceived.load()) << "\n";
				std::cout << "Total # of connections : " << _numberOfConnections.load() << "\n";
			}
			void NetworkInternalStats::MemoryAllocate(const uint32_t type)
			{
#if defined(_DEBUG)
				if (type == NETWORK_RECEIVE_COMPLETION_TYPE || type == NETWORK_RECEIVEFROM_COMPLETION_TYPE)
				{
					_receiveAllocateCount++;
					if (verbose) std::cout << "Receive type allocate" << _receiveAllocateCount << "\n";
				}
				else if (type == NETWORK_SEND_COMPLETION_TYPE || type == NETWORK_SENDTO_COMPLETION_TYPE)
				{
					if (verbose) std::cout << "Send type allocate\n";
					_sendAllocateCount++;
				}
				else if (type == NETWORK_ACCEPT_COMPLETION_TYPE)
				{
					if (verbose) std::cout << "Accept type allocate\n";
					_acceptAllocateCount++;
				}
				else if (type == NETWORK_CONNECT_COMPLETION_TYPE)
				{
					if (verbose) std::cout << "Connect type allocate\n";
					_connectAllocateCount++;
				}
#endif
			}
			void NetworkInternalStats::MemoryDeallocate(const uint32_t type)
			{
#if defined(_DEBUG)
				if (type == NETWORK_RECEIVE_COMPLETION_TYPE || type == NETWORK_RECEIVEFROM_COMPLETION_TYPE)
				{
					_receiveDeallocateCount++;
					if (verbose) std::cout << "---------------Receive type deallocate" << _receiveDeallocateCount << "\n";
				}
				else if (type == NETWORK_SEND_COMPLETION_TYPE || type == NETWORK_SENDTO_COMPLETION_TYPE)
				{
					if (verbose) std::cout << "Send type deallocate\n";
					_sendDeallocateCount++;
				}
				else if (type == NETWORK_ACCEPT_COMPLETION_TYPE)
				{
					if (verbose) std::cout << "Accept type deallocate\n";
					_acceptDeallocateCount++;
				}
				else if (type == NETWORK_CONNECT_COMPLETION_TYPE)
				{
					if (verbose) std::cout << "Connect type deallocate\n";
					_connectDeallocateCount++;
				}
#endif
			}

		}
	}
}

#undef NETWORK_COMPLETION_TYPE
#undef NETWORK_CHANNEL
#undef NETWORK_SOCKET_TYPE