#pragma once
#include "GameIOCPNetwork.h"

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			class MulticastSender
			{
			public:
				MulticastSender();
				void SetTTL(const uint8_t ttl);
				void SetMessage(const std::string& message);
				void SetAddressAndPort(const std::string& address, const uint32_t port);
				bool Initilize(game::IOCP::Network::NetworkManager& network);
				void SendMulticast();
				void ShutDown();

			private:
				uint8_t _multicastTTL;
				std::string _multicastMessage;
				std::string _multicastIP;
				uint32_t _multicastPort;
				SOCKET _multicastSendSocket;
				game::IOCP::Network::NetworkManager* _network;
				sockaddr_in _address;
			};

			class MulticastListener
			{
			public:
				MulticastListener();
				bool AddMembership();
				bool DropMembership();
				bool Initilize(game::IOCP::Network::NetworkManager& network);
				void ShutDown();
				void SetAddressAndPort(const std::string& address, const uint32_t port);
			private:
				std::string _multicastIP;
				uint32_t _multicastPort;
				//ip_mreq _multicastRequest;
				SOCKET _multicastReceiveSocket;
				bool _isMember;
			};
		}
	}
}