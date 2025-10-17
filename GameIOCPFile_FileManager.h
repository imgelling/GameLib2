#pragma once


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
				HANDLE hFile = INVALID_HANDLE_VALUE;
				//FILE_IO_TYPE = pad[0] = type of completion
				//FILE_PERCENT_DONE = pad[1] = percent done with file operation
			};

			class FileManager
			{
			public:
				FileManager();
				~FileManager();

				bool Initialize(game::IOCP::IOCPManager& iocpManager);
				void ShutDown();

				// Will open a file, creating if it doesn't exist, and read the whole file in
				bool Read(const std::string& filename, PER_IO_DATA_FILE* ioDataIn = nullptr);

				bool Append(const std::string& filename, const char* data, const uint64_t size);

				// Will create the file, if it does not exsist.  Will truncate a file if it does exsist. Copies the data,
				bool Write(const std::string& filename, const char* data, const uint64_t size, PER_IO_DATA_FILE* ioDataIn = nullptr, const bool append = false);
			private:
				HANDLE _OpenFile(const std::string& filename, DWORD access, DWORD share, DWORD creation);

				// Delete data function
				void _DeleteIoData(PER_IO_DATA_FILE* ioData);

				void _HandleWrite(PER_IO_DATA_FILE* ioData, const DWORD bytesTransferred);

				void _HandleRead(PER_IO_DATA_FILE* ioData, const DWORD bytesTransferred);

				void _DoWork(const int32_t result, const DWORD bytesTransferred, const ULONG_PTR completionKey, const game::IOCP::PER_IO_DATA* ioDataIn);

				HANDLE _iocpHandle;
			};
		}
	}
}