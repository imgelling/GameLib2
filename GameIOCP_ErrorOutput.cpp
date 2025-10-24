#include "GameIOCP.h"

namespace game
{
	namespace IOCP
	{
		static void GetErrorDetail(const int32_t error, std::string& errorDetailString)
		{
			char* messageBuffer = nullptr;

			// Use FormatMessage to get the error message string
			FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&messageBuffer,
				0,
				nullptr
			);

			messageBuffer ? errorDetailString.append(messageBuffer) : errorDetailString.append("Unknown error");

			// Free the allocated buffer
			LocalFree(messageBuffer);
		}

		static const int32_t ErrorToString(std::string& errorString)
		{
			const uint32_t error = WSAGetLastError();
			switch (error)
			{
			case 0: errorString = "NONE"; return error;
			case ERROR_SEM_TIMEOUT: errorString = "ERROR_SEM_TIMEOUT"; break;
			case ERROR_CONNECTION_REFUSED: errorString = "ERROR_CONNECTION_REFUSED"; break;
			case ERROR_SEM_NOT_FOUND: errorString = "ERROR_SEM_NOT_FOUND"; break;
			case ERROR_NETNAME_DELETED: errorString = "ERROR_NETNAME_DELETED"; break;
			case WAIT_TIMEOUT: errorString = "WAIT_TIMEOUT"; break;
			case WSA_INVALID_HANDLE: errorString = "WSA_INVALID_HANDLE"; break;
			case WSA_NOT_ENOUGH_MEMORY: errorString = "WSA_NOT_ENOUGH_MEMORY"; break;
			case WSA_INVALID_PARAMETER: errorString = "WSA_INVALID_PARAMETER"; break;
			case WSA_OPERATION_ABORTED: errorString = "WSA_OPERATION_ABORTED"; break;
			case WSA_IO_INCOMPLETE: errorString = "WSA_IO_INCOMPLETE"; break;
			case WSA_IO_PENDING: errorString = "WSA_IO_PENDING"; break;
			case WSAEINTR: errorString = "WSAEINTR"; break;
			case WSAEBADF: errorString = "WSAEBADF"; break;
			case WSAEACCES: errorString = "WSAEACCES"; break;
			case WSAEFAULT: errorString = "WSAEFAULT"; break;
			case WSAEINVAL: errorString = "WSAEINVAL"; break;
			case WSAEMFILE: errorString = "WSAEMFILE"; break;
			case WSAEWOULDBLOCK: errorString = "WSAEWOULDBLOCK"; break;
			case WSAEINPROGRESS: errorString = "WSAEINPROGRESS"; break;
			case WSAEALREADY: errorString = "WSAEALREADY"; break;
			case WSAENOTSOCK: errorString = "WSAENOTSOCK"; break;
			case WSAEDESTADDRREQ: errorString = "WSAEDESTADDRREQ"; break;
			case WSAEMSGSIZE: errorString = "WSAEMSGSIZE"; break;
			case WSAEPROTOTYPE: errorString = "WSAEPROTOTYPE"; break;
			case WSAENOPROTOOPT: errorString = "WSAENOPROTOOPT"; break;
			case WSAEPROTONOSUPPORT: errorString = "WSAEPROTONOSUPPORT"; break;
			case WSAESOCKTNOSUPPORT: errorString = "WSAESOCKTNOSUPPORT"; break;
			case WSAEOPNOTSUPP: errorString = "WSAEOPNOTSUPP"; break;
			case WSAEPFNOSUPPORT: errorString = "WSAEPFNOSUPPORT"; break;
			case WSAEAFNOSUPPORT: errorString = "WSAEAFNOSUPPORT"; break;
			case WSAEADDRINUSE: errorString = "WSAEADDRINUSE"; break;
			case WSAEADDRNOTAVAIL: errorString = "WSAEADDRNOTAVAIL"; break;
			case WSAENETDOWN: errorString = "WSAENETDOWN"; break;
			case WSAENETUNREACH: errorString = "WSAENETUNREACH"; break;
			case WSAENETRESET: errorString = "WSAENETRESET"; break;
			case WSAECONNABORTED: errorString = "WSAECONNABORTED"; break;
			case WSAECONNRESET: errorString = "WSAECONNRESET"; break;
			case WSAENOBUFS: errorString = "WSAENOBUFS"; break;
			case WSAEISCONN: errorString = "WSAEISCONN"; break;
			case WSAENOTCONN: errorString = "WSAENOTCONN"; break;
			case WSAESHUTDOWN: errorString = "WSAESHUTDOWN"; break;
			case WSAETOOMANYREFS: errorString = "WSAETOOMANYREFS"; break;
			case WSAETIMEDOUT: errorString = "WSAETIMEDOUT"; break;
			case WSAECONNREFUSED: errorString = "WSAECONNREFUSED"; break;
			case WSAELOOP: errorString = "WSAELOOP"; break;
			case WSAENAMETOOLONG: errorString = "WSAENAMETOOLONG"; break;
			case WSAEHOSTDOWN: errorString = "WSAEHOSTDOWN"; break;
			case WSAEHOSTUNREACH: errorString = "WSAEHOSTUNREACH"; break;
			case WSAENOTEMPTY: errorString = "WSAENOTEMPTY"; break;
			case WSAEPROCLIM: errorString = "WSAEPROCLIM"; break;
			case WSAEUSERS: errorString = "WSAEUSERS"; break;
			case WSAEDQUOT: errorString = "WSAEDQUOT"; break;
			case WSAESTALE: errorString = "WSAESTALE"; break;
			case WSAEREMOTE: errorString = "WSAEREMOTE"; break;
			case WSASYSNOTREADY: errorString = "WSASYSNOTREADY"; break;
			case WSAVERNOTSUPPORTED: errorString = "WSAEPROTONOSUPPORT"; break;
			case WSANOTINITIALISED: errorString = "WSAESOCKTNOSUPPORT"; break;
			case WSAEDISCON: errorString = "WSAEDISCON"; break;
			case WSAENOMORE: errorString = "WSAENOMORE"; break;
			case WSAECANCELLED: errorString = "WSAECANCELLED"; break;
			case WSAEINVALIDPROCTABLE: errorString = "WSAEINVALIDPROCTABLE"; break;
			case WSAEINVALIDPROVIDER: errorString = "WSAEINVALIDPROVIDER"; break;
			case WSAEPROVIDERFAILEDINIT: errorString = "WSAEPROVIDERFAILEDINIT"; break;
			case WSASYSCALLFAILURE: errorString = "WSASYSCALLFAILURE"; break;
			case WSASERVICE_NOT_FOUND: errorString = "WSAENETRESET"; break;
			case WSATYPE_NOT_FOUND: errorString = "WSATYPE_NOT_FOUND"; break;
			case WSA_E_NO_MORE: errorString = "WSA_E_NO_MORE"; break;
			case WSA_E_CANCELLED: errorString = "WSA_E_CANCELLED"; break;
			case WSAEREFUSED: errorString = "WSAEREFUSED"; break;
			case WSAHOST_NOT_FOUND: errorString = "WSAHOST_NOT_FOUND"; break;
			case WSATRY_AGAIN: errorString = "WSATRY_AGAIN"; break;
			case WSANO_RECOVERY: errorString = "WSANO_RECOVERY"; break;
			case WSANO_DATA: errorString = "WSANO_DATA"; break;
			case ERROR_CONNECTION_ABORTED: errorString = "ERROR_CONNECTION_ABORTED"; break;
			case ERROR_ALREADY_EXISTS: errorString = "ERROR_ALREADY_EXISTS"; break;
			default: errorString = "UNKNOWN"; break;
			}
			errorString += ' ';
			GetErrorDetail(error, errorString);

			return error;
		}

		int32_t ErrorOutput(const std::string& command, const uint32_t line, const bool ignoreIOPending)
		{
			std::string errorString;
			const int32_t error = ErrorToString(errorString);
			if (!error) return 0;
			if ((error == WSA_IO_PENDING) && ignoreIOPending) return 0;
			std::cout << "Error with " << command << " : ";

			std::cout << errorString << " (" << error << ") on line " << std::to_string(line) << std::endl;
#if defined(GAME_BREAK_ON_ERROR)
			__debugbreak();
#endif
			return error;
		}

	}
}
