#pragma once
#include "GameIOCPNetwork_NetworkManager.h"

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			class UPNPPortForward
			{
			public:
				bool Close(const uint16_t externalPort, const std::string& protocol);
				bool Open(const uint16_t externalPort, const uint16_t internalPort, const std::string& internalIp, const std::string& protocol, const std::string& indescription);
				void CleanUp();

				UPNPPortForward();
				~UPNPPortForward();
			private:
				IUPnPNAT* _nat;
				IStaticPortMappingCollection* _mappingCollection;
				std::vector<IStaticPortMapping*> _openPorts;
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> _converter;
			};
		}
	}
}