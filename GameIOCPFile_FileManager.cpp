#include "GameIOCP.h"

namespace game
{
	namespace IOCP
	{
		namespace File
		{
			static constexpr uint8_t FILE_READ_COMPLETION_TYPE = 1;
			static constexpr uint8_t FILE_WRITE_COMPLETION_TYPE = 2;
			static constexpr uint32_t MAX_IO_SIZE = 64 * 1024;

#define FILE_IO_TYPE pad[0]
#define FILE_PERCENT_DONE pad[1]

			FileManager::FileManager()
			{
				_iocpHandle = INVALID_HANDLE_VALUE;
			}
			FileManager::~FileManager()
			{

			}

			bool FileManager::Initialize(game::IOCP::IOCPManager& iocpManager)
			{
				_iocpHandle = iocpManager.GetCompeltionPort();
				if (_iocpHandle == INVALID_HANDLE_VALUE)
					return false;
				auto callback = [&](int32_t result, DWORD bytesTransferred, ULONG_PTR completionKey, game::IOCP::PER_IO_DATA* ioData) // Hides this pointer and allows for dynamic arguments
					{
						_DoWork(result, bytesTransferred, completionKey, ioData);
					};
				iocpManager.SetFileFunction(callback);
				return true;
			}
			void FileManager::ShutDown()
			{

			}

			bool FileManager::Read(const std::string& filename, PER_IO_DATA_FILE* ioDataIn)
			{
				PER_IO_DATA_FILE* ioData = ioDataIn;

				// Setup the ioData if it was not passed in
				if (ioData == nullptr)
				{
					ioData = new PER_IO_DATA_FILE();
					ioData->ioDataType = game::IOCP::IOCP_TYPE_FILE;
					ioData->hFile = INVALID_HANDLE_VALUE;
					ioData->FILE_IO_TYPE = FILE_READ_COMPLETION_TYPE;


					// ioData has not been passed in so the file needs to be opened
					ioData->hFile = _OpenFile(filename, GENERIC_READ, 0, OPEN_ALWAYS);
					if (ioData->hFile == INVALID_HANDLE_VALUE)
					{
						game::IOCP::ErrorOutput("_OpenFile", __LINE__);
						_DeleteIoData(ioData);
						return false;
					}

					// Get the file size
					LARGE_INTEGER fileSize = { 0 };
					if (!GetFileSizeEx(ioData->hFile, &fileSize))
					{
						game::IOCP::ErrorOutput("GetFileSizeEX", __LINE__);
						_DeleteIoData(ioData);
						return false;
					}
					ioData->bytesToTransfer = fileSize.QuadPart;
					try
					{
						ioData->data = new char[ioData->bytesToTransfer]; // new could fail if LARGE amount of RAM requested
					}
					catch (const std::bad_alloc& e)
					{
						std::cout << "Exception : " << e.what() << ". Could not allocate memory : " << ioData->bytesToTransfer << " bytes\n";
						_DeleteIoData(ioData);
						return false;
					}
					ioData->buffer.buf = ioData->data;
					ioData->buffer.len = ioData->bytesToTransfer > MAX_IO_SIZE ? MAX_IO_SIZE : (uint32_t)ioData->bytesToTransfer;

					std::cout << "Bytes to read : " << ioData->bytesToTransfer << "\n";

					// Error checks
					if (WSAGetLastError() == ERROR_ALREADY_EXISTS) // not really an error with OPEN_ALWAYS, truncate will not have
					{
						std::cout << "File exists!\n";
					}

					// Associate the file handle with IOCP
					if (!CreateIoCompletionPort(ioData->hFile, _iocpHandle, 0, 0))
					{
						game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__);
						_DeleteIoData(ioData);
						return false;
					}
				}

				BOOL res = ReadFile(ioData->hFile, ioData->buffer.buf, (DWORD)ioData->buffer.len, nullptr, (LPOVERLAPPED)ioData);
				if (!res) // will be false because async
				{
					if (game::IOCP::ErrorOutput("ReadFile", __LINE__))
					{
						CloseHandle(ioData->hFile);
						delete ioData;
						return false;
					}
				}

				return true;
			}
			bool FileManager::Append(const std::string& filename, const char* data, const uint64_t size)
			{
				return Write(filename, data, size, nullptr, true);
			}
			bool FileManager::Write(const std::string& filename, const char* data, const uint64_t size, PER_IO_DATA_FILE* ioDataIn, const bool append)
			{
				PER_IO_DATA_FILE* ioData = ioDataIn;

				// Setup the ioData if it was not passed in
				if (ioData == nullptr)
				{
					ioData = new PER_IO_DATA_FILE();
					ioData->ioDataType = game::IOCP::IOCP_TYPE_FILE;
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
					ioData->hFile = INVALID_HANDLE_VALUE;
					ioData->FILE_IO_TYPE = FILE_WRITE_COMPLETION_TYPE;

					ioData->bytesToTransfer = size;
					std::cout << "Bytes to write = " << ioData->bytesToTransfer << "\n"; // not needed	

					try
					{
						ioData->data = new char[ioData->bytesToTransfer]; // new could fail if LARGE amount of RAM requested
					}
					catch (const std::bad_alloc& e)
					{
						std::cout << "Exception : " << e.what() << ". Could not allocate memory : " << ioData->bytesToTransfer << " bytes\n";
						_DeleteIoData(ioData);
						return false;
					}
					memcpy(ioData->data, data, ioData->bytesToTransfer);

					ioData->buffer.buf = ioData->data;  // buffer will be used to walk the data if no full write happens
					ioData->buffer.len = ioData->bytesToTransfer > MAX_IO_SIZE ? MAX_IO_SIZE : (uint32_t)ioData->bytesToTransfer;


					// ioData has not been passed in so the file needs to be opened
					DWORD openMethod = CREATE_ALWAYS;
					if (append)
					{
						openMethod = OPEN_ALWAYS;
						LARGE_INTEGER writeOffset = { 0 };
						writeOffset.QuadPart = ioData->bytesTransferred;
						ioData->overlapped.Offset = 0xFFFFFFFF;// writeOffset.LowPart;
						ioData->overlapped.OffsetHigh = 0xFFFFFFFF;// writeOffset.HighPart;
					}
					ioData->hFile = _OpenFile(filename, GENERIC_WRITE, 0, openMethod);
					if (ioData->hFile == INVALID_HANDLE_VALUE)
					{
						game::IOCP::ErrorOutput("_OpenFile", __LINE__);
						_DeleteIoData(ioData);
						return false;
					}

					// Error checks
					if (WSAGetLastError() == ERROR_ALREADY_EXISTS) // not really an error with OPEN_ALWAYS, truncate will not have
					{
						std::cout << "File exsists and will be over written!\n";
						//ioData->wasOverwritten = true;
					}

					// Associate the file handle with IOCP
					if (!CreateIoCompletionPort(ioData->hFile, _iocpHandle, 0, 0))
					{
						game::IOCP::ErrorOutput("CreateIoCompletionPort", __LINE__);
						_DeleteIoData(ioData);
						return false;
					}
				}

				WriteFile(ioData->hFile, ioData->buffer.buf, (DWORD)ioData->buffer.len, NULL, (LPOVERLAPPED)ioData);
				if (game::IOCP::ErrorOutput("WriteFile", __LINE__))
				{
					_DeleteIoData(ioData);
					return false;
				}

				return true;
			}

			HANDLE FileManager::_OpenFile(const std::string& filename, DWORD access, DWORD share, DWORD creation)
			{
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> _stringConverter;
				HANDLE handle = CreateFile(
					_stringConverter.from_bytes(filename).c_str(),
					access, //GENERIC_READ | GENERIC_WRITE,
					share,  //FILE_SHARE_READ | FILE_SHARE_WRITE,
					nullptr,
					// use CREATE_ALWAYS if you want to truncate a file that may or may not exist,
					// TRUNCATE_EXISTING will only work if the file exists
					// OPEN_ALWAYS will not truncate
					creation,
					FILE_FLAG_OVERLAPPED,
					nullptr
				);
				return handle;
			}
			void FileManager::_DeleteIoData(PER_IO_DATA_FILE* ioData)
			{
				if (ioData != nullptr)
				{
					if (ioData->hFile != INVALID_HANDLE_VALUE)
					{
						CloseHandle(ioData->hFile);
					}
					if (ioData->data != nullptr)
					{
						delete ioData->data;
						ioData->data = nullptr;
					}
					delete ioData;
					ioData = nullptr;
				}
			}
			void FileManager::_HandleWrite(PER_IO_DATA_FILE* ioData, const DWORD bytesTransferred)
			{
				//std::cout << "Bytes written : " << bytesTransferred << "\n";
				ioData->bytesTransferred += bytesTransferred;
				ioData->FILE_PERCENT_DONE = (uint8_t)((ioData->bytesTransferred * 100) / ioData->bytesToTransfer);
				std::cout << "Percent done  : " << (uint32_t)ioData->FILE_PERCENT_DONE << "%\n";
				if (ioData->bytesToTransfer > ioData->bytesTransferred)
				{
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
					// Move file pointer to end of file (maybe a mistake for an offset write)
					ioData->overlapped.Offset = 0xFFFFFFFF;
					ioData->overlapped.OffsetHigh = 0xFFFFFFFF;
					// Increment the buffer
					ioData->buffer.buf = ioData->data + ioData->bytesTransferred;
					ioData->buffer.len = (ioData->bytesToTransfer - ioData->bytesTransferred) > MAX_IO_SIZE ? MAX_IO_SIZE : (uint32_t)(ioData->bytesToTransfer - ioData->bytesTransferred);


					Write("", nullptr, 0, ioData);
					return;
				}
				std::cout << "Write done!\n";
				_DeleteIoData(ioData);
			}
			void FileManager::_HandleRead(PER_IO_DATA_FILE* ioData, const DWORD bytesTransferred)
			{
				//std::cout << "Bytes read   : " << bytesTransferred << "\n";
				ioData->bytesTransferred += bytesTransferred;
				//std::cout << "Bytes left : " << ioData->bytesToTransfer - ioData->bytesTransferred << "\n";
				ioData->FILE_PERCENT_DONE = (uint8_t)((ioData->bytesTransferred * 100) / ioData->bytesToTransfer);
				std::cout << "Percent done : " << (uint32_t)ioData->FILE_PERCENT_DONE << "%\n";
				if ((ioData->bytesTransferred * 100) / ioData->bytesToTransfer == 100)
				{
					for (uint32_t i = 0; i < ioData->bytesTransferred; i++)
					{
						std::cout << ioData->data[i];
					}
					std::cout << "\n";
				}

				// Check if all the bytes requested were read
				if (ioData->bytesToTransfer > ioData->bytesTransferred)
				{
					// Set the offset to read from
					ZeroMemory(&ioData->overlapped, sizeof(OVERLAPPED));
					// Move file pointer bytesTransferred mount
					LARGE_INTEGER readOffset = { 0 };
					readOffset.QuadPart = ioData->bytesTransferred;
					ioData->overlapped.Offset = readOffset.LowPart;
					ioData->overlapped.OffsetHigh = readOffset.HighPart;
					// Increment the buffer 
					ioData->buffer.buf += bytesTransferred;
					ioData->buffer.len = (ioData->bytesToTransfer - ioData->bytesTransferred) > MAX_IO_SIZE ? MAX_IO_SIZE : (uint32_t)(ioData->bytesToTransfer - ioData->bytesTransferred);

					// Read more
					Read("", ioData);
					return;
				}
				// Read done, delete and close file
				std::cout << "Read done!\n";
				_DeleteIoData(ioData);
			}
			void FileManager::_DoWork(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, const game::IOCP::PER_IO_DATA* ioDataIn)
			{
				PER_IO_DATA_FILE* ioData = (PER_IO_DATA_FILE*)ioDataIn;
				// Handle errors
				if (!result)
				{
					game::IOCP::ErrorOutput("GetQueuedCompletionStatus", __LINE__);
					_DeleteIoData(ioData);
					return;
				}

				switch (ioData->FILE_IO_TYPE)
				{
				case FILE_WRITE_COMPLETION_TYPE:
					_HandleWrite(ioData, bytesTransferred);
					return;
				case FILE_READ_COMPLETION_TYPE:
					_HandleRead(ioData, bytesTransferred);
					return;
				default:
					break;
				}

				delete ioData;
			}

		}
	}
}