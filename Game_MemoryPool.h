#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace game
{
    enum class PoolGrowthType
    {
        // Will grow by initial pool size
        Constant,
        // Will double in size
        Linear,
        // Will be multiplied by its self
        Exponential
    };

    // Memory manager
    // Size will grow depending on GrowthType (Linear is default) when it empties
    // Does not clean up any blocks that are not given back by Deallocate
    class MemoryPool
    {
    public:


        MemoryPool(const uint64_t blockSize, const uint64_t poolSize, const PoolGrowthType growthType = PoolGrowthType::Linear);

        MemoryPool();

        ~MemoryPool();


        uint64_t GetSize() const;
        void PrintStats(const std::string& name);


        void Initialize(const uint64_t blockSize, const uint64_t poolSize, const PoolGrowthType growthType = PoolGrowthType::Linear);


        void* Allocate();

        void Deallocate(void* block);

    private:
        void _AllocatePool();


        uint64_t _blockSize;
        std::atomic<uint64_t> _poolSize;
        uint64_t _grownPoolSize;
        uint64_t _initialPoolSize;
        std::vector<void*> _freeBlocks;
        std::mutex _mutex;
        PoolGrowthType _growthType;
        std::atomic<uint64_t> _allocations;
        std::atomic<uint64_t> _deallocations;
    };

    template <typename T>
    class VectorPool
    {
    public:

        VectorPool(const uint64_t poolSize, const uint64_t reserveSize = 0, const PoolGrowthType growthType = PoolGrowthType::Linear)
        {
            _poolSize = poolSize;
            _initialPoolSize = poolSize;
            _grownPoolSize = poolSize;
            _growthType = _growthType;
            _reserveSize = reserveSize;
            _allocations = 0;
            _deallocations = 0;
            std::lock_guard<std::mutex> lock(_mutex);
            _AllocatePool();
        }
        VectorPool()
        {
            _poolSize = 0;
            _grownPoolSize = 0;
            _initialPoolSize = 0;
            _growthType = PoolGrowthType::Linear;
            _reserveSize = 0;
            _allocations = 0;
            _deallocations = 0;
        }
        ~VectorPool()
        {
        }

        const uint64_t GetSize() const
        {
            return _poolSize;
        }

        void Initialize(const uint64_t poolSize, const uint64_t reserveSize = 0, const PoolGrowthType growthType = PoolGrowthType::Linear)
        {
            _poolSize = poolSize;
            _initialPoolSize = _poolSize;
            _grownPoolSize = poolSize;
            _growthType = growthType;
            _reserveSize = reserveSize;
            _allocations = 0;
            _deallocations = 0;
            std::lock_guard<std::mutex> lock(_mutex);
            _AllocatePool();
        }

        std::vector<T> Allocate()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            // If pool is empty, grow it according to GrowthType
            if (_freeVectors.empty())
            {
                _AllocatePool();
            }
            _allocations.fetch_add(1);
            std::vector<T> block = _freeVectors.back();
            _freeVectors.pop_back();
            return block;
        }

        void Deallocate(std::vector<T> block)
        {
            _deallocations.fetch_add(1);
            std::lock_guard<std::mutex> lock(_mutex);
            block.clear();
            _freeVectors.push_back(block);
        }

        void PrintStats(std::string name)
        {
            std::cout << name << "\n";
            //uint64_t capacity = 0;
            //{
            //    std::lock_guard<std::mutex> lock(_mutex);
            //    for (auto& i : _freeVectors)
            //    {
            //        capacity += i.capacity();
            //    }
            //}
            //std::cout << name << " count         : " << _poolSize << "\n";
            //std::cout << name << " capacity      : " << formatDataSize(capacity) << "\n";
            //std::cout << name << " allocations   : " << _allocations.load() << "\n";
            //std::cout << name << " deallocations : " << _deallocations.load() << "\n";
        }

    private:
        void _AllocatePool()
        {
            _poolSize = _grownPoolSize;
            for (uint64_t i = 0; i < _poolSize; ++i)
            {
                std::vector<T> temp;
                _freeVectors.push_back(temp);
                if (_reserveSize)
                    _freeVectors[i].reserve(_reserveSize);
            }

            switch (_growthType)
            {
            case PoolGrowthType::Constant: _grownPoolSize += _initialPoolSize; break;
            case PoolGrowthType::Exponential: _grownPoolSize *= _grownPoolSize; break;
            case PoolGrowthType::Linear:
            default: _grownPoolSize += _grownPoolSize;
            }
        }
        uint64_t _poolSize;
        uint64_t _grownPoolSize;
        uint64_t _reserveSize;
        uint64_t _initialPoolSize;
        std::vector<std::vector<T>> _freeVectors;
        std::mutex _mutex;
        PoolGrowthType _growthType;
        std::atomic<uint64_t> _allocations;
        std::atomic<uint64_t> _deallocations;

    };
}
