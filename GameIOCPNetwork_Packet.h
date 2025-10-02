#pragma once
#include <vector>
#include <string>

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			class Packet
			{
			public:
				Packet();
				~Packet();
				Packet(const std::vector<uint8_t>&);
				void GetVector(std::vector<uint8_t>&);
				const size_t Size() const;

				// std::string	-- Done
				void Add(const std::string&);
				void Get(std::string&);
				void Peek(std::string&);

				// uint8_t -- Done
				void Add(const uint8_t);
				void Get(uint8_t&);
				void Peek(uint8_t&);

				// int8_t  -- Done
				void Add(const int8_t);
				void Get(int8_t&);
				void Peek(int8_t&);

				// uint16_t  -- Done
				void Add(const uint16_t);
				void Get(uint16_t&);
				void Peek(uint16_t&);

				// int16_t  -- Done
				void Add(const int16_t);
				void Get(int16_t&);
				void Peek(int16_t&);

				// uint32_t  -- Done
				void Add(const uint32_t);
				void Peek(uint32_t&);
				void Get(uint32_t&);

				// int32_t  -- Done
				void Add(const int32_t);
				void Peek(int32_t&);
				void Get(int32_t&);

				// uint64_t  -- Done
				void Add(const uint64_t);
				void Peek(uint64_t&);
				void Get(uint64_t&);

				// int64_t  -- Done
				void Add(const int64_t);
				void Peek(int64_t&);
				void Get(int64_t&);

				// float_t  -- Done
				void Add(const float_t);
				void Peek(float_t&);
				void Get(float_t&);

				// double_t  -- Done
				void Add(const double_t);
				void Peek(double_t&);
				void Get(double_t&);

				// bool  -- Done
				void Add(const bool);
				void Peek(bool&);
				void Get(bool&);

				void ClearData();

				// Prints out data as uint32_t
				void PrintAsU32();
				// Prints out data as uint8_t
				void PrintAsU8();
			private:
				uint8_t* _data;
				uint8_t* _dataWritePosition;
				uint8_t* _dataReadPosition;
				uint32_t _dataLength;

				const bool _GetPeekSizeCheck(const size_t size) const;
				const bool _AddSizeCheck(const size_t size) const;
				void _IncreaseReadPos(const uint32_t amount);
				void _IncreaseWritePos(const uint32_t amount);

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
	}
}