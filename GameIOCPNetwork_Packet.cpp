#include "GameIOCP.h"

namespace game
{
	namespace IOCP
	{
		namespace Network
		{
			Packet::Packet()
			{
				_data = new uint8_t[MAX_PACKET_SIZE];
				ClearData();
			}
			Packet::~Packet()
			{
				if (_data) delete[] _data;
				_data = nullptr;
				_dataWritePosition = nullptr;
				_dataReadPosition = nullptr;

			}
			Packet::Packet(const std::vector<uint8_t>& vec)
			{
				_data = new uint8_t[MAX_PACKET_SIZE];
				ClearData();
				if (vec.size() > MAX_PACKET_SIZE)
				{
					std::cout << "Vector too big, needs to be less than " << MAX_PACKET_SIZE << ".\n";
					return;
				}
				memcpy(_data, vec.data(), vec.size());
				_dataLength = (uint32_t)vec.size();
				_dataReadPosition = _data;
				_dataWritePosition = (_data + vec.size());
			}

			void Packet::GetVector(std::vector<uint8_t>& ret)
			{
				ret.assign(_data, _data + _dataLength);
			}

			const size_t Packet::Size() const
			{
				return _dataLength;
			}

			// std::string
			void Packet::Add(const std::string& str)
			{
				const char* s = str.c_str();
				uint16_t size = (uint16_t)str.length();
				GAME_ASSERT(_AddSizeCheck(sizeof(size)));
				Add(size);
				GAME_ASSERT(_AddSizeCheck(size));
				memcpy(_dataWritePosition, str.c_str(), str.length());
				_IncreaseWritePos((uint32_t)str.length());

				return;
			}
			void Packet::Peek(std::string& str)
			{
				str.clear();
				uint16_t size;
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
				Peek(size);
				GAME_ASSERT(_GetPeekSizeCheck(size + sizeof(uint16_t)));

				for (uint16_t i = sizeof(uint16_t); i < size + sizeof(uint16_t); i++)
				{
					str += *(_dataReadPosition + i);
				}
			}
			void Packet::Get(std::string& str)
			{
				str.clear();

				uint16_t size = 0;
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
				Get(size);
				GAME_ASSERT(_GetPeekSizeCheck(size));

				str = std::string(size, '\0');
				memcpy((uint8_t*)(&str[0]), _dataReadPosition, size);
				_IncreaseReadPos(size);
			}
			// uint8_t
			void Packet::Add(const uint8_t c) // ccd
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*_dataWritePosition = c;
				_IncreaseWritePos(sizeof(uint8_t));
			}
			void Packet::Peek(uint8_t& c) // ccd
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
				c = *(_dataReadPosition);
			}
			void Packet::Get(uint8_t& c) // ccd
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
				c = *(_dataReadPosition);
				_IncreaseReadPos(sizeof(uint8_t));
			}
			// int8_t
			void Packet::Add(const int8_t c) // ccd
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*_dataWritePosition = (int8_t)c;
				_IncreaseWritePos(sizeof(int8_t));
			}
			void Packet::Peek(int8_t& c) // ccd
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int8_t)));
				c = (int8_t) * (_dataReadPosition);
			}
			void Packet::Get(int8_t& c) // ccd
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int8_t)));
				c = (int8_t) * (_dataReadPosition);
				_IncreaseReadPos(sizeof(int8_t));
			}
			// uint16_t
			void Packet::Add(const uint16_t c) // ccd
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*(uint16_t*)(_dataWritePosition) = _htons(c);
				_IncreaseWritePos(sizeof(uint16_t));
			}
			void Packet::Peek(uint16_t& c) // ccd
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
				c = *(uint16_t*)(_dataReadPosition);
				c = _ntohs(c);
			}
			void Packet::Get(uint16_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
				c = *(uint16_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(uint16_t));
				c = _ntohs(c);
			}
			// int16_t
			void Packet::Add(const int16_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*(int16_t*)(_dataWritePosition) = _htons(c);
				_IncreaseWritePos(sizeof(int16_t));
			}
			void Packet::Peek(int16_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
				c = *(int16_t*)(_dataReadPosition);
				c = _ntohs(c);
			}
			void Packet::Get(int16_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
				c = *(int16_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(int16_t));
				c = _ntohs(c);
			}
			// uint32_t
			void Packet::Add(const uint32_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*(uint32_t*)(_dataWritePosition) = _htonl(c);
				_IncreaseWritePos(sizeof(uint32_t));
			}
			void Packet::Peek(uint32_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
				c = *(uint32_t*)(_dataReadPosition);
				c = _ntohl(c);
			}
			void Packet::Get(uint32_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
				c = *(uint32_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(uint32_t));
				c = _ntohl(c);
			}
			// int32_t
			void Packet::Add(const int32_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*(int32_t*)(_dataWritePosition) = _htonl(c);
				_IncreaseWritePos(sizeof(int32_t));
			}
			void Packet::Peek(int32_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
				c = *(int32_t*)(_dataReadPosition);
				c = _ntohl(c);
			}
			void Packet::Get(int32_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
				c = *(int32_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(int32_t));
				c = _ntohl(c);
			}
			// uint64_t
			void Packet::Add(const uint64_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*(uint64_t*)(_dataWritePosition) = _hton64(c);
				_IncreaseWritePos(sizeof(uint64_t));
			}
			void Packet::Peek(uint64_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
				c = *(uint64_t*)(_dataReadPosition);
				c = _ntoh64(c);
			}
			void Packet::Get(uint64_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
				c = *(uint64_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(uint64_t));
				c = _ntoh64(c);
			}
			// int64_t 
			void Packet::Add(const int64_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(c)));
				*(int64_t*)(_dataWritePosition) = _hton64(c);
				_IncreaseWritePos(sizeof(int64_t));
			}
			void Packet::Peek(int64_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
				c = *(int64_t*)(_dataReadPosition);
				c = _ntoh64(c);
			}
			void Packet::Get(int64_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
				c = *(int64_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(int64_t));
				c = _ntoh64(c);
			}
			// float_t
			void Packet::Add(const float_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(float_t)));
				uint32_t temp = 0;
				memcpy(&temp, &c, sizeof(float_t));
				*(uint32_t*)(_dataWritePosition) = _htonl(temp);
				_IncreaseWritePos(sizeof(float_t));
			}
			void Packet::Peek(float_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
				uint32_t temp = *(uint32_t*)(_dataReadPosition);
				temp = _ntohl(temp);
				memcpy(&c, &temp, sizeof(float_t));
			}
			void Packet::Get(float_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
				uint32_t temp = *(uint32_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(float_t));
				temp = _ntohl(temp);
				memcpy(&c, &temp, sizeof(float_t));
			}
			// double_t
			void Packet::Add(const double_t c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(double_t)));
				uint64_t temp = 0;
				memcpy(&temp, &c, sizeof(double_t));
				*(uint64_t*)(_dataWritePosition) = _hton64(temp);
				_IncreaseWritePos(sizeof(double_t));
			}
			void Packet::Peek(double_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
				uint64_t temp = *(uint64_t*)(_dataReadPosition);
				temp = _ntoh64(temp);
				memcpy(&c, &temp, sizeof(double_t));
			}
			void Packet::Get(double_t& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
				uint64_t temp = *(uint64_t*)(_dataReadPosition);
				_IncreaseReadPos(sizeof(double_t));
				temp = _ntoh64(temp);
				memcpy(&c, &temp, sizeof(double_t));
			}
			// bool
			void Packet::Add(const bool c)
			{
				GAME_ASSERT(_AddSizeCheck(sizeof(uint8_t)));
				uint8_t a = c ? 1 : 0;
				Add(a);
			}
			void Packet::Peek(bool& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
				uint8_t a = 0;
				Peek(a);
				c = a ? true : false;
			}
			void Packet::Get(bool& c)
			{
				GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
				uint8_t a = 0;
				Get(a);
				c = a ? true : false;
			}

			void Packet::ClearData()
			{
				if (_data)
				{
					memset(_data, 0, MAX_PACKET_SIZE);
				}
				_dataLength = 0;
				_dataWritePosition = _data;
				_dataReadPosition = _data;
			}

			void Packet::PrintAsU32()
			{
				for (size_t i = 0; i < _dataLength; i++)
				{
					std::cout << (uint32_t) * (_dataReadPosition + i);
				}
				std::cout << std::endl;

			}
			void Packet::PrintAsU8()
			{
				for (size_t i = 0; i < _dataLength; i++)
				{
					std::cout << *(_dataReadPosition + i);
				}
				std::cout << std::endl;
			}

			const bool Packet::_GetPeekSizeCheck(const size_t size) const
			{
				return size <= _dataLength ? true : false;
			}
			const bool Packet::_AddSizeCheck(const size_t size) const
			{
				return (_dataLength + size) <= MAX_PACKET_SIZE ? true : false;
			}
			//#else
			//		const bool Packet::_GetPeekSizeCheck(const size_t size) const
			//		{
			//			return size > _dataLength ? false : true;
			//		}
			//		const bool Packet::_AddSizeCheck(const size_t size) const
			//		{
			//			return (_dataLength + size) > MAX_PACKET_SIZE ? false : true;
			//		}

			void Packet::_IncreaseReadPos(const uint32_t amount)
			{
				_dataReadPosition += amount;
				_dataLength -= amount;
			}
			void Packet::_IncreaseWritePos(uint32_t amount)
			{
				_dataWritePosition += amount;
				_dataLength += amount;
			}
		}
	}
}