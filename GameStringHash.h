#pragma once
#include <string>
#include <cstdint>

//// 32-bit hash
//#define FNV_OFFSET_BASIS_32 2166136261u
//#define FNV_PRIME_32 16777619u
//
//uint32_t fnv1a_hash_32(const char* str) {
//	uint32_t hash = FNV_OFFSET_BASIS_32;
//	while (*str) {
//		hash ^= (uint8_t)(*str);
//		hash *= FNV_PRIME_32;
//		str++;
//	}
//	return hash;
//}

namespace game
{

	class StringHash
	{
	public:
		std::string string;
		uint64_t hash = 0;
		StringHash()
		{
			string = "";
			hash = 0;
		}
		StringHash(const std::string str)
		{
			hash = _ComputeHash(str.c_str());
			this->string = str;
		}
		// Overloaded assignment operator
		StringHash& operator=(const std::string& other)
		{
			string = other;
			hash = _ComputeHash(other.c_str());
			return *this;
		}
		// Overloaded equality operator
		bool operator==(const StringHash& other) const
		{
			return (this->hash == other.hash);
		}
		// Overloaded inequality operator
		bool operator!=(const StringHash& other) const
		{
			return (this->hash != other.hash);
		}

	private:
		// Does a fnv1a_hash_64
		uint64_t _ComputeHash(const char* str)
		{
			uint64_t computingHash = 14695981039346656037ull; // FNV_OFFSET_BASIS_64;
			while (*str)
			{
				computingHash ^= (uint8_t)(*str);
				computingHash *= 1099511628211ull; // FNV_PRIME_64;
				str++;
			}
			return computingHash;
		}
	};
}