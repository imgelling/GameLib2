#include "GameIOCP.h"
#include <string.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

namespace game
{

	SerializeToU8::SerializeToU8()
	{
		// TODO : don't like max_packet, allow to make own size
		_data = new uint8_t[game::IOCP::Network::MAX_PACKET_SIZE];
		ClearData();
	}
	SerializeToU8::~SerializeToU8()
	{
		if (_data) delete[] _data;
		_data = nullptr;
		_dataWritePosition = nullptr;
		_dataReadPosition = nullptr;

	}
	SerializeToU8::SerializeToU8(const std::vector<uint8_t>& vec)
	{
		_data = new uint8_t[game::IOCP::Network::MAX_PACKET_SIZE];
		ClearData();
		if (vec.size() > game::IOCP::Network::MAX_PACKET_SIZE)
		{
			std::cout << "Vector too big, needs to be less than " << game::IOCP::Network::MAX_PACKET_SIZE << ".\n";
			return;
		}
		memcpy(_data, vec.data(), vec.size());
		_dataLength = (uint32_t)vec.size();
		_dataReadPosition = _data;
		_dataWritePosition = (_data + vec.size());
	}

	void SerializeToU8::GetVector(std::vector<uint8_t>& ret)
	{
		ret.assign(_data, _data + _dataLength);
	}

	const uint8_t* const SerializeToU8::GetData()
	{
		return _data;
	}

	size_t SerializeToU8::Size() const
	{
		return _dataLength;
	}

	// std::string
	void SerializeToU8::Add(const std::string& str)
	{
		//const char* s = str.c_str();
		uint16_t size = (uint16_t)str.length();
		GAME_ASSERT(_AddSizeCheck(sizeof(size)));
		Add(size);
		GAME_ASSERT(_AddSizeCheck(size));
		memcpy(_dataWritePosition, str.c_str(), str.length());
		_IncreaseWritePos((uint32_t)str.length());

		return;
	}
	void SerializeToU8::Peek(std::string& str)
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
	void SerializeToU8::Get(std::string& str)
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
	void SerializeToU8::Add(const uint8_t c) // ccd
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*_dataWritePosition = c;
		_IncreaseWritePos(sizeof(uint8_t));
	}
	void SerializeToU8::Peek(uint8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		c = *(_dataReadPosition);
	}
	void SerializeToU8::Get(uint8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		c = *(_dataReadPosition);
		_IncreaseReadPos(sizeof(uint8_t));
	}
	// int8_t
	void SerializeToU8::Add(const int8_t c) // ccd
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*_dataWritePosition = (int8_t)c;
		_IncreaseWritePos(sizeof(int8_t));
	}
	void SerializeToU8::Peek(int8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int8_t)));
		c = (int8_t) * (_dataReadPosition);
	}
	void SerializeToU8::Get(int8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int8_t)));
		c = (int8_t) * (_dataReadPosition);
		_IncreaseReadPos(sizeof(int8_t));
	}
	// uint16_t
	void SerializeToU8::Add(const uint16_t c) // ccd
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*(uint16_t*)(_dataWritePosition) = _htons(c);
		_IncreaseWritePos(sizeof(uint16_t));
	}
	void SerializeToU8::Peek(uint16_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
		c = *(uint16_t*)(_dataReadPosition);
		c = _ntohs(c);
	}
	void SerializeToU8::Get(uint16_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
		c = *(uint16_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(uint16_t));
		c = _ntohs(c);
	}
	// int16_t
	void SerializeToU8::Add(const int16_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*(int16_t*)(_dataWritePosition) = _htons(c);
		_IncreaseWritePos(sizeof(int16_t));
	}
	void SerializeToU8::Peek(int16_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
		c = *(int16_t*)(_dataReadPosition);
		c = _ntohs(c);
	}
	void SerializeToU8::Get(int16_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
		c = *(int16_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(int16_t));
		c = _ntohs(c);
	}
	// uint32_t
	void SerializeToU8::Add(const uint32_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*(uint32_t*)(_dataWritePosition) = _htonl(c);
		_IncreaseWritePos(sizeof(uint32_t));
	}
	void SerializeToU8::Peek(uint32_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
		c = *(uint32_t*)(_dataReadPosition);
		c = _ntohl(c);
	}
	void SerializeToU8::Get(uint32_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
		c = *(uint32_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(uint32_t));
		c = _ntohl(c);
	}
	// int32_t
	void SerializeToU8::Add(const int32_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*(int32_t*)(_dataWritePosition) = _htonl(c);
		_IncreaseWritePos(sizeof(int32_t));
	}
	void SerializeToU8::Peek(int32_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
		c = *(int32_t*)(_dataReadPosition);
		c = _ntohl(c);
	}
	void SerializeToU8::Get(int32_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
		c = *(int32_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(int32_t));
		c = _ntohl(c);
	}
	// uint64_t
	void SerializeToU8::Add(const uint64_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*(uint64_t*)(_dataWritePosition) = _hton64(c);
		_IncreaseWritePos(sizeof(uint64_t));
	}
	void SerializeToU8::Peek(uint64_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		c = *(uint64_t*)(_dataReadPosition);
		c = _ntoh64(c);
	}
	void SerializeToU8::Get(uint64_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		c = *(uint64_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(uint64_t));
		c = _ntoh64(c);
	}
	// int64_t 
	void SerializeToU8::Add(const int64_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		*(int64_t*)(_dataWritePosition) = _hton64(c);
		_IncreaseWritePos(sizeof(int64_t));
	}
	void SerializeToU8::Peek(int64_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
		c = *(int64_t*)(_dataReadPosition);
		c = _ntoh64(c);
	}
	void SerializeToU8::Get(int64_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
		c = *(int64_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(int64_t));
		c = _ntoh64(c);
	}
	// float_t
	void SerializeToU8::Add(const float_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(float_t)));
		uint32_t temp = 0;
		memcpy(&temp, &c, sizeof(float_t));
		*(uint32_t*)(_dataWritePosition) = _htonl(temp);
		_IncreaseWritePos(sizeof(float_t));
	}
	void SerializeToU8::Peek(float_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		uint32_t temp = *(uint32_t*)(_dataReadPosition);
		temp = _ntohl(temp);
		memcpy(&c, &temp, sizeof(float_t));
	}
	void SerializeToU8::Get(float_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		uint32_t temp = *(uint32_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(float_t));
		temp = _ntohl(temp);
		memcpy(&c, &temp, sizeof(float_t));
	}
	// double_t
	void SerializeToU8::Add(const double_t c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(double_t)));
		uint64_t temp = 0;
		memcpy(&temp, &c, sizeof(double_t));
		*(uint64_t*)(_dataWritePosition) = _hton64(temp);
		_IncreaseWritePos(sizeof(double_t));
	}
	void SerializeToU8::Peek(double_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
		uint64_t temp = *(uint64_t*)(_dataReadPosition);
		temp = _ntoh64(temp);
		memcpy(&c, &temp, sizeof(double_t));
	}
	void SerializeToU8::Get(double_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
		uint64_t temp = *(uint64_t*)(_dataReadPosition);
		_IncreaseReadPos(sizeof(double_t));
		temp = _ntoh64(temp);
		memcpy(&c, &temp, sizeof(double_t));
	}
	// bool
	void SerializeToU8::Add(const bool c)
	{
		GAME_ASSERT(_AddSizeCheck(sizeof(uint8_t)));
		uint8_t a = c ? 1 : 0;
		Add(a);
	}
	void SerializeToU8::Peek(bool& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		uint8_t a = 0;
		Peek(a);
		c = a ? true : false;
	}
	void SerializeToU8::Get(bool& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		uint8_t a = 0;
		Get(a);
		c = a ? true : false;
	}

	void SerializeToU8::ClearData()
	{
		if (_data)
		{
			memset(_data, 0, game::IOCP::Network::MAX_PACKET_SIZE);
		}
		_dataLength = 0;
		_dataWritePosition = _data;
		_dataReadPosition = _data;
	}

	void SerializeToU8::PrintAsU32()
	{
		for (size_t i = 0; i < _dataLength; i++)
		{
			std::cout << (uint32_t) * (_dataReadPosition + i);
		}
		std::cout << std::endl;

	}
	void SerializeToU8::PrintAsU8()
	{
		for (size_t i = 0; i < _dataLength; i++)
		{
			std::cout << *(_dataReadPosition + i);
		}
		std::cout << std::endl;
	}

	bool SerializeToU8::_GetPeekSizeCheck(const size_t size) const
	{
		return size <= _dataLength ? true : false;
	}

	bool SerializeToU8::_AddSizeCheck(const size_t size) const
	{
		return (_dataLength + size) <= game::IOCP::Network::MAX_PACKET_SIZE ? true : false;
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

	void SerializeToU8::_IncreaseReadPos(const uint32_t amount)
	{
		_dataReadPosition += amount;
		_dataLength -= amount;
	}
	void SerializeToU8::_IncreaseWritePos(uint32_t amount)
	{
		_dataWritePosition += amount;
		_dataLength += amount;
	}
}