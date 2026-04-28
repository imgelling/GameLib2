#pragma once
#include <chrono>

namespace game
{
	class Timer
	{
	public:
		Timer();
		double Elapsed() const;
		void Reset();
	private:
		std::chrono::high_resolution_clock::time_point _startTime;
	};

	inline Timer::Timer()
	{
		_startTime = std::chrono::high_resolution_clock::now();
	}

	inline void Timer::Reset()
	{
		_startTime = std::chrono::high_resolution_clock::now();
	}

	inline double Timer::Elapsed() const
	{
		std::chrono::microseconds _ticks = std::chrono::duration_cast<std::chrono::microseconds>((std::chrono::high_resolution_clock::now() - _startTime));
		double diff = (float)(_ticks.count()) / 1000.0;
		return diff;
	}
}

