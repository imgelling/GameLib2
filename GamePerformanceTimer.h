#pragma once
#include <chrono>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace game
{
	class PerformanceTimer
	{
	private:
		// Holds relavent data per function timed
		struct _FunctionToTime
		{
			std::string _name;
			double_t _time = 0.0;
			uint32_t _ticks = 0;
			double_t _cumulative = 0.0;
			std::chrono::high_resolution_clock::time_point _start;
		};
		// Think I need this if using threads
		std::shared_mutex _mutex;
		// Holds a FunctionToTime keyed to a string specified by user
		std::unordered_map <std::string, _FunctionToTime> _timedFuncs;
	public:
		~PerformanceTimer()
		{
			_timedFuncs.clear();
		}
		// Returns number of times the function was timed, -1 if not found
		uint32_t Ticks(const std::string name);

		// Returns the nanoseconds the function took on the last call, -1 if not found
		double_t LastRun(const std::string name);

		// Returns the total nanoseconds the function ran over all the calls, -1 if not found
		double_t Cumulative(const std::string name);

		// Returns the average nanoseconds the function ran over all the calls, -1 if not found
		double_t Average(const std::string name);

		// Returns an unmodifiable container of timed functions
		const std::unordered_map <std::string, _FunctionToTime>& TimedFunctions()
		{
			return _timedFuncs;
		}

		// Start timing
		void Start(const std::string name);

		// Stop timing
		void Stop(const std::string name);

	};

	inline uint32_t PerformanceTimer::Ticks(const std::string name)
	{
		_mutex.lock();
		unsigned int ret = 0;
		try
		{
			ret = _timedFuncs.at(name)._ticks;
		}
		catch (...)
		{
			ret = -1;
		}
		_mutex.unlock();
		return ret;
	}

	inline double_t PerformanceTimer::LastRun(const std::string name)
	{
		_mutex.lock();
		double_t ret = 0.0;
		try
		{
			ret = _timedFuncs.at(name)._time;
		}
		catch (...)
		{
			ret = -1.0;
		}
		_mutex.unlock();
		return ret;
	}

	inline double_t PerformanceTimer::Cumulative(const std::string name)
	{
		_mutex.lock();
		double_t ret = 0.0;
		try
		{
			ret = _timedFuncs.at(name)._cumulative;
		}
		catch (...)
		{
			ret = -1.0;
		}
		_mutex.unlock();
		return ret;
	}

	inline double_t PerformanceTimer::Average(const std::string name)
	{
		_mutex.lock();
		double_t ret = 0.0;
		try
		{
			ret = _timedFuncs.at(name)._cumulative / _timedFuncs.at(name)._ticks;
		}
		catch (...)
		{
			ret = -1.0;
		}
		_mutex.unlock();
		return ret;
	}

	inline void PerformanceTimer::Start(const std::string name)
	{
		_FunctionToTime t;
		_mutex.lock();
		_timedFuncs.try_emplace(name, t);
		_timedFuncs[name]._name = name;
		_timedFuncs[name]._start = std::chrono::high_resolution_clock::now();
		_timedFuncs[name]._ticks++;
		_mutex.unlock();
	};

	inline void PerformanceTimer::Stop(const std::string name)
	{
#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()
		_mutex.lock();
		try
		{
			_timedFuncs.at(name)._time = (double)(duration(timeNow() - _timedFuncs.at(name)._start));
			_timedFuncs.at(name)._cumulative += _timedFuncs[name]._time;
		}
		catch (...)
		{
			// Output to a log or something
			//std::cout << __func__ << ": \"" << name << "\" is not found." << "\n";
		}
		_mutex.unlock();

#undef duration
#undef timeNow
	};

}