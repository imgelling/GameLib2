#include "Game_MemoryPool.h"
// TODO : remove after adding helpers.h or whatever
#include "GameIOCP.h"

namespace game
{
    MemoryPool::MemoryPool(const uint64_t blockSize, const uint64_t poolSize, const PoolGrowthType growthType)
    {
        _blockSize = blockSize;
        _poolSize = 0;
        _initialPoolSize = poolSize;
        _grownPoolSize = poolSize;
        _growthType = growthType;
        _allocations = 0;
        _deallocations = 0;
        std::lock_guard<std::mutex> lock(_mutex);
        _AllocatePool();
    }

    MemoryPool::MemoryPool()
    {
        _blockSize = 0;
        _poolSize = 0;
        _grownPoolSize = 0;
        _initialPoolSize = 0;
        _growthType = PoolGrowthType::Linear;
        _allocations = 0;
        _deallocations = 0;
    }

    MemoryPool::~MemoryPool()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (void* block : _freeBlocks)
        {
            operator delete(block);
        }
    }

    void MemoryPool::Initialize(const uint64_t blockSize, const uint64_t poolSize, const PoolGrowthType growthType)
    {
        _blockSize = blockSize;
        _poolSize = 0;
        _initialPoolSize = poolSize;
        _grownPoolSize = poolSize;
        _growthType = growthType;
        std::lock_guard<std::mutex> lock(_mutex);
        _AllocatePool();
    }

    void* MemoryPool::Allocate()
    {
        void* block = nullptr;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            // If pool is empty, grow it according to GrothType
            if (_freeBlocks.empty())
            {
                _AllocatePool();
            }
            block = _freeBlocks.back();
            _freeBlocks.pop_back();
        }
        _allocations.fetch_add(1);
        memset(block, 0, _blockSize);
        return block;
    }

    void MemoryPool::Deallocate(void* block)
    {
        _deallocations.fetch_add(1);
        std::lock_guard<std::mutex> lock(_mutex);
        _freeBlocks.push_back(block);
        //block = nullptr;
    }

    void MemoryPool::_AllocatePool()
    {
        for (uint64_t i = 0; i < _grownPoolSize; ++i)
        {
            void* block = operator new(_blockSize);
            _freeBlocks.push_back(block);
            _poolSize++;
        }
        switch (_growthType)
        {
        case PoolGrowthType::Constant: break;
        case PoolGrowthType::Exponential: _grownPoolSize = _grownPoolSize * _grownPoolSize; break;
        case PoolGrowthType::Linear:
        default: _grownPoolSize += _grownPoolSize;
        }
    }

    uint64_t MemoryPool::GetSize() const
    {
        return _poolSize.load();  
    }

    void MemoryPool::PrintStats(const std::string &name)
    {
        // TODO:: calculate % reuse
        std::lock_guard<std::mutex> lock(_mutex);
        std::cout << name << " count         : " << _poolSize.load()  << "\n";
        std::cout << name << " size          : " << formatDataSize(_poolSize * _blockSize) << "\n";
        std::cout << name << " allocations   : " << formatDataSize(_allocations.load() * _blockSize) << "\n";
        std::cout << name << " deallocations : " << formatDataSize(_deallocations.load() * _blockSize) << "\n";
    }
}