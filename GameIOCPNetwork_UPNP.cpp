#include "GameIOCP.h"

namespace game
{
	namespace IOCP
	{
		namespace Network
		{

			bool UPNPPortForward::Close(const uint16_t externalPort, const std::string& protocol)
			{
				BSTR wideProtocol = SysAllocString(_converter.from_bytes(protocol).c_str());
				long tempExternalPort = 0;
				BSTR tempProtocol = NULL;
				HRESULT hr = 0;

				for (std::vector<IStaticPortMapping*>::iterator it = _openPorts.begin(); it != _openPorts.end(); )
				{
					(*it)->get_ExternalPort(&tempExternalPort);
					if (!tempExternalPort)
					{
						std::cout << "Could not get external port.\n";
						it++;
						continue;
					}
					(*it)->get_Protocol(&tempProtocol);
					if (!tempProtocol)
					{
						std::cout << "Could not get protocol.\n";
						it++;
						continue;
					}

					if ((tempExternalPort == externalPort) && (!std::wcscmp(tempProtocol, wideProtocol)))
					{
						hr = _mappingCollection->Remove(externalPort, wideProtocol);
						if (SUCCEEDED(hr))
						{
							std::cout << "Removed port ";
						}
						else
						{
							std::cout << "Could not remove port ";
						}
						std::cout << "forwarding from external " << externalPort << " using " << protocol << ".\n";
						(*it)->Release();
						it = _openPorts.erase(it);
						SysFreeString(tempProtocol);
						SysFreeString(wideProtocol);
						return true;
					}
					else
					{
						it++;
					}
					SysFreeString(tempProtocol);
				}
				SysFreeString(wideProtocol);

				return false;
			}

			bool UPNPPortForward::Open(const uint16_t externalPort, const uint16_t internalPort, const std::string& internalIp, const std::string& protocol, const std::string& indescription)
			{
				HRESULT hr = 0;
				if (!_nat)
				{
					hr = CoInitialize(NULL);
					if (FAILED(hr))
					{
						std::cerr << "COM initialization failed.\n";
						return 1;
					}


					hr = CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUPnPNAT), (void**)&_nat);
					if (FAILED(hr) || !_nat)
					{
						std::cerr << "Failed to create UPnPNAT instance.\n";
						CoUninitialize();
						return false;
					}
				}

				if (_mappingCollection)
				{
					_mappingCollection->Release();
					_mappingCollection = nullptr;
				}

				hr = _nat->get_StaticPortMappingCollection(&_mappingCollection); // this causes output for some reason
				if (FAILED(hr) || !_mappingCollection)
				{
					std::cerr << "Failed to get port mapping collection.\n";
					_nat->Release();
					CoUninitialize();
					return false;
				}

				BSTR internalClient = SysAllocString(_converter.from_bytes(internalIp).c_str());
				BSTR description = SysAllocString(_converter.from_bytes(indescription).c_str());
				BSTR wideProtocol = SysAllocString(_converter.from_bytes(protocol).c_str());
				IStaticPortMapping* pMapping = nullptr;
				hr = _mappingCollection->Add(externalPort, wideProtocol, internalPort, internalClient, VARIANT_TRUE, description, &pMapping);
				if (SUCCEEDED(hr))
				{
					std::cout << "Added ";
					_openPorts.emplace_back(pMapping);
				}
				else
				{
					std::cout << "Failed to add ";
				}
				std::cout << "port forwarding from external " << externalPort << " port to internal " << internalPort << " port";
				std::cout << " with local ip of " << internalIp << " using " << protocol << ".\n";

				// Needs to be its own functions that can work with all functions of pMapping
				// Query ports
				IUnknown* pEnum = nullptr;
				hr = _mappingCollection->get__NewEnum(&pEnum);
				if (SUCCEEDED(hr) && pEnum)
				{
					IEnumVARIANT* pEnumVariant = nullptr;
					hr = pEnum->QueryInterface(IID_IEnumVARIANT, (void**)&pEnumVariant);
					if (SUCCEEDED(hr) && pEnumVariant)
					{
						VARIANT var;
						VariantInit(&var);
						while (pEnumVariant->Next(1, &var, nullptr) == S_OK)
						{
							IStaticPortMapping* pMapping = nullptr;
							if (var.vt == VT_DISPATCH)
							{
								hr = var.pdispVal->QueryInterface(IID_IStaticPortMapping, (void**)&pMapping);
								if (SUCCEEDED(hr))
								{
									long externalPort;
									BSTR protocol;
									BSTR description;
									pMapping->get_ExternalPort(&externalPort);
									pMapping->get_Protocol(&protocol);
									pMapping->get_Description(&description);

									std::wcout << L"Port: " << externalPort << L", Protocol: " << protocol << L", Description : " << description << std::endl;
									SysFreeString(protocol);
									SysFreeString(description);
									pMapping->Release();
								}
							}
							VariantClear(&var);
						}
						pEnumVariant->Release();
					}
					pEnum->Release();
				}

				// Clean up
				SysFreeString(wideProtocol);
				SysFreeString(description);
				SysFreeString(internalClient);

				return true;
			}

			void UPNPPortForward::CleanUp()
			{
				long port = 0;
				BSTR protocol = nullptr;
				std::string byteProtocol;
				for (auto i : _openPorts)
				{
					i->get_ExternalPort(&port);
					i->get_Protocol(&protocol);
					if (port && (protocol != nullptr))
					{
						byteProtocol = _converter.to_bytes(protocol);
						Close((uint16_t)port, byteProtocol);
						SysFreeString(protocol);
					}
					protocol = nullptr;
					port = 0;
					byteProtocol = "";
				}
				_openPorts.clear();
				CoUninitialize();
			}

			UPNPPortForward::~UPNPPortForward()
			{
				CleanUp();
				if (_mappingCollection) _mappingCollection->Release();
				if (_nat) _nat->Release();
			}

			UPNPPortForward::UPNPPortForward()
			{
				_nat = nullptr;
				_mappingCollection = nullptr;
			}
		}
	}
}