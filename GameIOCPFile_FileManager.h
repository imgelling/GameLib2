#pragma once


#define FILE_ONWRITE_SIGNATURE const int32_t result, uint64_t id, const DWORD bytesTransferred, const DWORD bytesToTransfer, const uint8_t* data
#define FILE_ONWRITE_PARAMETERS result, id, bytesTransferred, bytesToTransfer, data
#define FILE_ONREAD_SIGNATURE const int32_t result, uint64_t id, const DWORD bytesTransferred, const DWORD bytesToTransfer, const uint8_t* data
#define FILE_ONREAD_PARAMETERS result, id, bytesTransferred, bytesToTransfer, data

namespace game
{
	namespace IOCP
	{
		namespace File
		{
			struct PER_IO_DATA_FILE : game::IOCP::PER_IO_DATA
			{
				WSABUF buffer = { 0 };
				char* data = nullptr;
				uint64_t bytesToTransfer = 0;
				uint64_t bytesTransferred = 0;
				uint64_t id = 0;
				HANDLE hFile = INVALID_HANDLE_VALUE;
				std::string filename;  // string hash?
				//FILE_IO_TYPE = pad[0] = type of completion
			};

			class FileManager
			{
			public:
				FileManager();
				~FileManager();

				bool Initialize(game::IOCP::IOCPManager& iocpManager);
				void ShutDown();
				// TODO: add defaults to explaing signatures and prevent crashes
				
				// Will open a file, creating if it doesn't exist, and read the whole file in
				bool Read(const std::string& filename, uint64_t * id = nullptr ,PER_IO_DATA_FILE* ioDataIn = nullptr);

				void SetOnRead(std::function<void(FILE_ONREAD_SIGNATURE)>);

				// read write exists methods
				bool Append(const std::string& filename, const char* data, const uint64_t size, uint64_t* id = nullptr);

				uint64_t GetNextID()
				{
					return fileID;
				}

				void SetOnWrite(std::function<void(FILE_ONWRITE_SIGNATURE)>);
				// Will create the file, if it does not exsist.  Will truncate a file if it does exsist. Copies the data,
				bool Write(const std::string& filename, const char* data, const uint64_t size, uint64_t* id = nullptr, PER_IO_DATA_FILE* ioDataIn = nullptr, const bool append = false);
				//void SetOnWrite
			private:
				HANDLE _OpenFile(const std::string& filename, DWORD access, DWORD share, DWORD creation);

				// Delete data function
				void _DeleteIoData(PER_IO_DATA_FILE* ioData);

				void _HandleWrite(PER_IO_DATA_FILE* ioData, const DWORD bytesTransferred);
				void _HandleRead(PER_IO_DATA_FILE* ioData, const DWORD bytesTransferred);

				void _DoWork(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, const game::IOCP::PER_IO_DATA* ioDataIn);

				HANDLE _iocpHandle;
				std::function<void(FILE_ONREAD_SIGNATURE)> _onRead;
				std::function<void(FILE_ONWRITE_SIGNATURE)> _onWrite;
				std::atomic_uint64_t fileID;
				//std::atomic_uint64_t writeID;
			};
		}
	}
}