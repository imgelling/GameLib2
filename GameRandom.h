#if !defined(GAMERANDOM_H)
#define GAMERANDOM_H

#include <random>
#include <chrono>

namespace game
{
	class Random
	{
	public:
		Random();
		void SetSeed(const uint32_t seed);
		void NewSeed();
		uint32_t GetSeed() const noexcept;
		uint32_t Randui() noexcept;
		float_t Randf() noexcept;
		uint32_t RndRange(const uint32_t min, const uint32_t max) noexcept;
		~Random();
	private:
		uint32_t _seed;
		std::mt19937 _mt19937Generator;

	};

	Random::Random()
	{
		std::random_device rd;
		_seed = rd() ^
			(
				(std::mt19937::result_type)
				std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() +
				(std::mt19937::result_type)
				std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
				);
		_mt19937Generator.seed(_seed);
	}

	inline void Random::NewSeed()
	{
		std::random_device rd;
		_seed = rd() ^
			(
				(std::mt19937::result_type)
				std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() +
				(std::mt19937::result_type)
				std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()
				);
		_mt19937Generator.seed(_seed);
	}

	inline void Random::SetSeed(const uint32_t seed)
	{
		this->_seed = seed;
		_mt19937Generator.seed(seed);
	}

	inline uint32_t Random::GetSeed() const noexcept
	{
		return _seed;
	}

	// Returns a random unsigned int
	inline uint32_t Random::Randui() noexcept
	{
		return _mt19937Generator();
	}

	// Returns a float between 0.0f and 1.0f
	inline float_t Random::Randf() noexcept
	{
		return RndRange(0,INT_MAX) / (float_t)INT_MAX;
	}

	inline uint32_t Random::RndRange(const uint32_t min, const uint32_t max) noexcept
	{
		std::uniform_int_distribution<unsigned> distrib(min, max);
		return distrib(_mt19937Generator);
	}

	Random::~Random()
	{
	}
}

#endif