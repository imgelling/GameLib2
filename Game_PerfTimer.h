#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <mutex>

namespace game
{

    class PerfTimer
    {
    private:
        // Holds relavent data per function timed
        struct FunctionToTime
        {
            std::string name;
            double time = 0.0;
            uint32_t ticks = 0;
            double cumulative = 0.0;
            std::chrono::high_resolution_clock::time_point start;
        };
        std::mutex mutex;

        // Holds a FunctionToTime keyed to a string specified by user
        std::unordered_map <std::string, FunctionToTime> timedfuncs;
    public:
        ~PerfTimer()
        {
            timedfuncs.clear();
        }

        // Returns number of times the function was timed, debug breaks if not found
        unsigned int Ticks(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            uint32_t ret = 0;
            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].ticks;
            return ret;
        }

        // Returns the nanoseconds the function took on the last call, debug breaks if not found
        double LastRunNano(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            double ret = 0.0;

            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].time;
            return ret;
        }

        // Returns the microseconds the function took on the last call, debug breaks if not found
        double LastRunMicro(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            double ret = 0.0;

            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].time;
            return ret / 1000.0;
        }

        // Returns the milliseconds the function took on the last call, debug breaks if not found
        double LastRunMilli(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            double ret = 0.0;

            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].time;
            return ret / 1000.0 / 1000.0;
        }

        // Returns the seconds the function took on the last call, debug breaks if not found
        double LastRunSec(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            double ret = 0.0;

            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].time;
            return ret / 1000.0 / 1000.0 / 1000.0;
        }

        // Returns the total nanoseconds the function ran over all the calls, debug breaks if not found
        double Cumulative(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            double ret = 0.0;

            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].cumulative;

            return ret;
        }

        // Returns the average nanoseconds the function ran over all the calls, debug breaks if not found
        double Average(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());
            double ret = 0.0;

            const std::lock_guard<std::mutex> lock(mutex);
            ret = timedfuncs[name].cumulative / timedfuncs[name].ticks;
            return ret;
        }

        // Returns an unmodifiable container of timed functions
        const std::unordered_map <std::string, FunctionToTime>& TimedFunctions()
        {
            return timedfuncs;
        }

        // Start timing
        void Start(const std::string& name)
        {
            //GAME_ASSERT(timedfuncs.find(name) == timedfuncs.end());
            if (timedfuncs.find(name) == timedfuncs.end())
            {
                FunctionToTime temp;
                temp.name = name;
                temp.start = std::chrono::high_resolution_clock::now();
                temp.ticks = 1;
                const std::lock_guard<std::mutex> lock(mutex);
                timedfuncs.try_emplace(name, temp);
            }
            else
            {
                const std::lock_guard<std::mutex> lock(mutex);
                timedfuncs[name].start = std::chrono::high_resolution_clock::now();
                timedfuncs[name].ticks++;
            }
        };

        // Stop timing
        void Stop(const std::string& name)
        {
            GAME_ASSERT(timedfuncs.find(name) != timedfuncs.end());

            const std::lock_guard<std::mutex> lock(mutex);
            auto& ref = timedfuncs[name];
            auto now = std::chrono::high_resolution_clock::now();
            ref.time = (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(
                now - timedfuncs[name].start).count());
            ref.cumulative += ref.time;
        };

    };
}