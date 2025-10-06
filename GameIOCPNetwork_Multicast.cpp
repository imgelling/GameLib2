#include "GameIOCP.h"

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			MulticastSender::MulticastSender()
			{
				_multicastTTL = 1;
				_multicastMessage = "Sample multicast message";
				_multicastSendSocket = INVALID_SOCKET;
				_multicastIP = "239.255.255.250";
				_multicastPort = 4445;
				_network = nullptr;
				_address = { 0 };
			}

			void MulticastSender::SetTTL(const uint8_t ttl)
			{
				_multicastTTL = ttl;

				// Set TTL
				setsockopt(_multicastSendSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&_multicastTTL, sizeof(_multicastTTL));
				if (game::IOCP::ErrorOutput("setsockopt", __LINE__))
				{
					return;
				}
			}

			void MulticastSender::SetMessage(const std::string& message)
			{
				_multicastMessage = message;
			}

			void MulticastSender::SetAddressAndPort(const std::string& address, const uint32_t port)
			{
				_multicastIP = address;
				_multicastPort = port;

				_address = { 0 };
				_address.sin_family = AF_INET;
				_address.sin_port = htons((uint16_t)_multicastPort);
				inet_pton(_address.sin_family, _multicastIP.c_str(), &_address.sin_addr.s_addr);
			}

			bool MulticastSender::Initilize(game::IOCP::Network::NetworkManager& network)
			{
				_multicastSendSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
				if (game::IOCP::ErrorOutput("socket", __LINE__))
				{
					return false;
				}

				BOOL reuse = TRUE;
				setsockopt(_multicastSendSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
				if (game::IOCP::ErrorOutput("socket", __LINE__))
				{
					closesocket(_multicastSendSocket);
					return false;
				}

				// non blocking seems to fix random getline errors
				// TODO error fix
				u_long mode = 1;
				if (ioctlsocket(_multicastSendSocket, FIONBIO, &mode) != 0) {
					printf("Failed to set non-blocking mode with error: %d\n", WSAGetLastError());
					closesocket(_multicastSendSocket);
					WSACleanup();
					return 1;
				}

				// Set TTL
				setsockopt(_multicastSendSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&_multicastTTL, sizeof(_multicastTTL));
				if (game::IOCP::ErrorOutput("setsockopt", __LINE__))
				{
					return false;
				}

				CreateIoCompletionPort((HANDLE)_multicastSendSocket, network.GetCompletionPort(), 0, 0);
				if (game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__))
				{
					closesocket(_multicastSendSocket);
					return false;
				}

				_address = { 0 };
				_address.sin_family = AF_INET;
				_address.sin_port = htons((uint16_t)_multicastPort);
				inet_pton(_address.sin_family, _multicastIP.c_str(), &_address.sin_addr.s_addr);

				_network = &network;
				return true;
			}

			void MulticastSender::SendMulticast()
			{
				if (!_network) return;
				if (_multicastSendSocket == INVALID_SOCKET)
				{
					return;
				}
				// Send data to the multicast group
				_network->SendTo(_multicastSendSocket, _address, (const unsigned char*)_multicastMessage.c_str(), _multicastMessage.size());
			}

			void MulticastSender::ShutDown()
			{
				if (_multicastSendSocket != INVALID_SOCKET)
				{
					closesocket(_multicastSendSocket);
					game::IOCP::ErrorOutput("closesocket", __LINE__);

					_multicastSendSocket = INVALID_SOCKET;
				}
			}

			MulticastListener::MulticastListener()
			{
				_multicastIP = "239.255.255.250";
				_multicastPort = 4445;
				_multicastReceiveSocket = INVALID_SOCKET;
				_isMember = false;
			}

			bool MulticastListener::AddMembership()
			{
				if (_multicastReceiveSocket == INVALID_SOCKET)
				{
					return false;
				}
				if (_isMember)
				{
					return false;
				}

				ip_mreq mreq = { 0 };
				inet_pton(AF_INET, _multicastIP.c_str(), &mreq.imr_multiaddr);
				mreq.imr_interface.s_addr = INADDR_ANY;
				setsockopt(_multicastReceiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
				if (game::IOCP::ErrorOutput("setsockopt", __LINE__))
				{
					return false;
				}
				_isMember = true;
				return true;
			}

			bool MulticastListener::DropMembership()
			{
				if (_multicastReceiveSocket == INVALID_SOCKET)
				{
					return false;
				}
				if (!_isMember)
				{
					return false;
				}
				ip_mreq mreq = { 0 };
				inet_pton(AF_INET, _multicastIP.c_str(), &mreq.imr_multiaddr);
				mreq.imr_interface.s_addr = INADDR_ANY;
				setsockopt(_multicastReceiveSocket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
				if (game::IOCP::ErrorOutput("setsockopt", __LINE__))
				{
					return false;
				}
				_isMember = false;
				return true;
			}

			bool MulticastListener::Initilize(game::IOCP::Network::NetworkManager& network)
			{
				if (_multicastReceiveSocket != INVALID_SOCKET)
				{
					return false;
				}

				_multicastReceiveSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);
				if (game::IOCP::ErrorOutput("socket", __LINE__))
				{
					return false;
				}

				BOOL reuse = TRUE;
				setsockopt(_multicastReceiveSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
				if (game::IOCP::ErrorOutput("setsockopt", __LINE__))
				{
					closesocket(_multicastReceiveSocket);
					game::IOCP::ErrorOutput("closesocket", __LINE__);
					return false;
				}

				// non blocking seems to fix random getline errors
				u_long mode = 1;
				ioctlsocket(_multicastReceiveSocket, FIONBIO, &mode);
				if (game::IOCP::ErrorOutput("ioctlsocket", __LINE__))
				{
					closesocket(_multicastReceiveSocket);
					game::IOCP::ErrorOutput("closesocket", __LINE__);
					return false;
				}

				sockaddr_in localAddr = { 0 };
				localAddr.sin_family = AF_INET;
				localAddr.sin_port = htons((uint16_t)_multicastPort);
				localAddr.sin_addr.s_addr = INADDR_ANY;
				bind(_multicastReceiveSocket, (sockaddr*)&localAddr, sizeof(localAddr));
				if (game::IOCP::ErrorOutput("bind", __LINE__))
				{
					closesocket(_multicastReceiveSocket);
					game::IOCP::ErrorOutput("closesocket", __LINE__);
					return false;
				}

				AddMembership();

				CreateIoCompletionPort((HANDLE)_multicastReceiveSocket, network.GetCompletionPort(), 0, 0);
				if (game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__))
				{
					closesocket(_multicastReceiveSocket);
					game::IOCP::ErrorOutput("closesocket", __LINE__);
					return false;
				}


				network.ReceiveFrom(_multicastReceiveSocket);
				return true;
			}

			void MulticastListener::ShutDown()
			{
				if (_multicastReceiveSocket != INVALID_SOCKET)
				{
					DropMembership();
					closesocket(_multicastReceiveSocket);
					game::IOCP::ErrorOutput("closesocket", __LINE__);

					_multicastReceiveSocket = INVALID_SOCKET;
				}
			}

			void MulticastListener::SetAddressAndPort(const std::string& address, const uint32_t port)
			{
				_multicastIP = address;
				_multicastPort = port;
			}
		}
	}
}