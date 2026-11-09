#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fstream>
#include <string>
#include <cstdint>
#include <filesystem>
namespace game
{
	class SimpleFileManager
	{
	public:

		bool Exists(const std::string& filename)
		{
			return std::filesystem::exists(filename);
		}
		bool Write(const std::string& filename, const std::string& data, int64_t* bytesWritten = nullptr)
		{
			// std::ios::app to append
			std::ofstream file(filename, std::ios::out | std::ios::trunc | std::ios::binary);
			if (GetLastError() == 183 /*ERROR_ALREADY_EXISTS*/)
			{
				SetLastError(0);
			}
			if (file.is_open())
			{
				file << data;
				if (bytesWritten) *bytesWritten = file.tellp();
				file.close();
				return true;
			}
			else
				return false;
		}
		int64_t GetFileSize(const std::string& filename)
		{
			std::streampos fsize = 0;
			std::ifstream file(filename, std::ios::binary);

			fsize = file.tellg();
			file.seekg(0, std::ios::end);
			fsize = file.tellg() - fsize;
			file.close();

			return fsize;
		}
		bool Read(const std::string& fileName, std::string& data, uint64_t* bytesRead = nullptr)
		{
			data = "";
			std::ifstream file(fileName, std::ios::in | std::ios::binary);
			if (bytesRead) *bytesRead = 0;

			if (file.is_open())
			{
				std::string in;
				while (std::getline(file, in))
				{
					data += in + "\n";
					//if (bytesRead) *bytesRead = file.tellg(); // slow
				}
				file.close();

				if (bytesRead) *bytesRead = std::filesystem::file_size(fileName);
				return true;
			}
			else
				return false;
		}
	};
}