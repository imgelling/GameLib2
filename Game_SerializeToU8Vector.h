#pragma once
#include <vector>
#include <string>
#include <string.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace game
{
	class SerializeToU8Vector
	{
	public:
		SerializeToU8Vector();
		~SerializeToU8Vector();
		SerializeToU8Vector(const std::vector<uint8_t>&);
		void GetVector(std::vector<uint8_t>&);
		size_t Size() const;

		void Assign(const uint8_t* data, const int64_t size)
		{
			_data.assign(data, data+size);
			_IncreaseWritePos(size);
		}

		// uint8_t*
		void Add(const uint8_t* data, const int64_t size)
		{
			_data.insert(_data.begin() + _dataWritePosition, data, data + size);
			//memcpy(_dataWritePosition, data, size);
			_IncreaseWritePos(size);
		}

		// std::string	-- Done
		void Add(const std::string&);
		bool Get(std::string &);
		bool Peek(std::string&);

		// uint8_t -- Done
		void Add(const uint8_t);
		bool Get(uint8_t&);
		bool Peek(uint8_t&);

		// int8_t  -- Done
		void Add(const int8_t);
		bool Get(int8_t&);
		bool Peek(int8_t&);

		// uint16_t  -- Done
		void Add(const uint16_t);
		bool Get(uint16_t&);
		bool Peek(uint16_t&);

		// int16_t  -- Done
		void Add(const int16_t);
		bool Get(int16_t&);
		bool Peek(int16_t&);

		// uint32_t  -- Done
		void Add(const uint32_t);
		bool Peek(uint32_t&);
		bool Get(uint32_t&);

		// int32_t  -- Done
		void Add(const int32_t);
		bool Peek(int32_t&);
		bool Get(int32_t&);

		// uint64_t  -- Done
		void Add(const uint64_t);
		bool Peek(uint64_t&);
		bool Get(uint64_t&);

		// int64_t  -- Done
		void Add(const int64_t);
		bool Peek(int64_t&);
		bool Get(int64_t&);

		// float_t  -- Done
		void Add(const float_t);
		bool Peek(float_t&);
		bool Get(float_t&);

		// double_t  -- Done
		void Add(const double_t);
		bool Peek(double_t&);
		bool Get(double_t&);

		// bool  -- Done
		void Add(const bool);
		bool Peek(bool&);
		bool Get(bool&);

		void ClearData();

		// Prints out data as uint32_t
		void PrintAsU32();
		// Prints out data as uint8_t
		void PrintAsU8();
		const uint8_t* const GetData();
	private:
		std::vector<uint8_t> _data;
		int64_t _dataWritePosition;
		int64_t _dataReadPosition;
		uint64_t _dataLength;
		bool CheckValidSize(size_t size) const;
		bool _GetPeekSizeCheck(const size_t size) const;
		void _IncreaseReadPos(const uint64_t amount);
		void _IncreaseWritePos(const uint64_t amount);

#define IS_LITTLE_ENDIAN (1 == *(unsigned char *)&(const int){1})
		inline uint16_t _htons(const uint16_t c)
		{
#ifdef IS_LITTLE_ENDIAN
			return _byteswap_ushort(c);
#else
			return c;
#endif
		}
		inline uint16_t _ntohs(const uint16_t c)
		{
#ifdef IS_LITTLE_ENDIAN
			return _byteswap_ushort(c);
#else
			return c;
#endif
		}
		inline uint32_t _htonl(const uint32_t c)
		{
#ifdef IS_LITTLE_ENDIAN
			return _byteswap_ulong(c);
#else
			return c;
#endif
		}
		inline uint32_t _ntohl(const uint32_t c)
		{
#ifdef IS_LITTLE_ENDIAN
			return _byteswap_ulong(c);
#else
			return c;
#endif
		}
		inline uint64_t _hton64(const uint64_t c)
		{
#ifdef IS_LITTLE_ENDIAN
			return _byteswap_uint64(c);
#else
			return c;
#endif
		}
		inline uint64_t _ntoh64(const uint64_t c)
		{
#ifdef IS_LITTLE_ENDIAN
			return _byteswap_uint64(c);
#else
			return c;
#endif
		}
#undef IS_LITTLE_ENDIAN
	};
}