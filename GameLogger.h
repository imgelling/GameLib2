#pragma once
#include <fstream>
#include <mutex>
#include <sstream>

#include "GameErrors.h"


namespace game
{
    class Logger {
    public:
        Logger(const std::string filename);
        ~Logger();
        void Header(const std::string name, const std::string version);
        void Error(const std::string logline);
        void Error(const GameError error);
        void Line();
        void Warning(const std::string logline);
        void Write(const std::string logline);
        void WriteQuiet(const std::string logline);
    private:
        char _buffer[256];
        enum class _LogType { NORMAL, WARNING, ERRORS, NONE };
        time_t _rawtime;
        std::ofstream _stream;
        std::mutex _streamlock;
        tm _timeinfo = { 0 };
        void _Write(const _LogType type, const std::string logline);
        inline void _GetTime();
    };

    inline void Logger::Line()
	{
		//#ifdef _DEBUG
		_streamlock.lock();
		std::cout << "--------------------------------------------------------------------------------" << std::endl;
		_streamlock.unlock();
		//#endif
		_streamlock.lock();
		_stream << "<HR>";
		_streamlock.unlock();
	}

	inline Logger::Logger(const std::string filename)
	{
		for (uint16_t buff = 0; buff < 256; buff++)
		{
			_buffer[buff] = 0;
		}
		_rawtime = 0;
		_streamlock.lock();
		_stream.open(filename.c_str(), std::ios::out);
#if defined(_WIN32_)
		SetLastError(0);
#elif defined(__linux__)
#endif
		_streamlock.unlock();
	}

	inline void Logger::Write(const std::string logline)
	{
		_Write(_LogType::NORMAL, logline);
	}

	inline void Logger::Error(const std::string logline)
	{
		_Write(_LogType::ERRORS, logline);
	}

	inline void Logger::Error(const GameError error)
	{
		std::stringstream str;
		str << error;
		_Write(_LogType::ERRORS, str.str());
	}

	inline void Logger::Warning(const std::string logline)
	{
		_Write(_LogType::WARNING, logline);
	}

	inline void Logger::_Write(const _LogType type, const std::string logline)
	{
		std::string color;
		std::string temp;
		std::string pre;
		std::string time;

		_GetTime();
		time = std::string(_buffer);
		time.pop_back();
		pre = "[" + time + "] ";
		color = "FFFFFF";

		if (type == _LogType::ERRORS)
		{
			pre += "ERROR : ";
			color = "FF0000";
		}
		else if (type == _LogType::WARNING)
		{
			pre += "WARNING : ";
			color = "FFFF00";
		}
		//#ifdef _DEBUG
		_streamlock.lock();
		std::cout << pre << logline << std::endl;
		_streamlock.unlock();
		//#endif
		temp += "<FONT style=\"background-color:#" + color + "\">";
		temp += pre;
		temp += logline + "</FONT><BR>";
		_streamlock.lock();
		_stream << temp << std::endl;
		_streamlock.unlock();
	}

	inline void Logger::WriteQuiet(const std::string logline)
	{
		std::string color;
		std::string temp;
		std::string pre;
		std::string time;

		_GetTime();
		time = std::string(_buffer);
		time.pop_back();
		pre = "[" + time + "] ";
		color = "FFFFFF";

		temp += "<FONT style=\"background-color:#" + color + "\">";
		temp += pre;
		temp += logline + "</FONT><BR>";
		_streamlock.lock();
		_stream << temp << std::endl;
		_streamlock.unlock();
	}

	inline void Logger::_GetTime()
	{
#ifdef __linux__

#else
		time(&_rawtime);
		localtime_s(&_timeinfo, &_rawtime);
		asctime_s(_buffer, &_timeinfo);
#endif
	}

	inline void Logger::Header(const std::string name, const std::string version)
	{
		Write(name + ", version " + version);
		Write("Built on " + std::string(__DATE__) + " at " + std::string(__TIME__) + ".");
		Line();
	}

	inline Logger::~Logger()
	{
		_streamlock.lock();
		_stream.close();
		_streamlock.unlock();
	}

}