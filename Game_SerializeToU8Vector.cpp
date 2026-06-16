#include <string.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include "Game_Assert.h"
//#include "GameIOCPNetwork.h"
#include "Game_SerializeToU8Vector.h"

namespace game
{
	
	SerializeToU8Vector::SerializeToU8Vector()
	{
		// TODO : don't like max_packet, allow to make own size
		//_data = new uint8_t[game::IOCP::Network::MAX_PACKET_SIZE];
		ClearData();
	}
	SerializeToU8Vector::~SerializeToU8Vector()
	{
		//if (_data) delete[] _data;
		//_data = nullptr;
		_dataWritePosition = 0;// nullptr;
		_dataReadPosition = 0;// nullptr;
		_dataLength = 0;

	}
	SerializeToU8Vector::SerializeToU8Vector(const std::vector<uint8_t>& vec)
	{
		//(void)vec;
////		_data = new uint8_t[game::IOCP::Network::MAX_PACKET_SIZE];
//		ClearData();
//		memcpy(_data, vec.data(), vec.size());
//		_dataLength = (uint32_t)vec.size();
//		_dataReadPosition = 0;// _data;
//		_dataWritePosition = vec.size();// (_data + vec.size());
		_data.assign(vec.begin(), vec.end());
		_dataLength = _data.size();
		_dataWritePosition = _dataLength;// nullptr;
		_dataReadPosition = 0;// nullptr;
	}

	void SerializeToU8Vector::GetVector(std::vector<uint8_t>& ret)
	{
		//ret.assign(_data, _data + _dataLength);
		ret.assign(_data.begin(), _data.end());
	}

	const uint8_t* const SerializeToU8Vector::GetData()
	{
		return _data.data();
	}

	size_t SerializeToU8Vector::Size() const
	{
		return _dataLength;
	}

	bool SerializeToU8Vector::CheckValidSize(size_t size) const
	{
		bool check = _GetPeekSizeCheck(size);
		GAME_ASSERT(check);
		return check;
	}

	// std::string
	void SerializeToU8Vector::Add(const std::string& str)
	{
		uint64_t size = str.length();
		Add(size);
		_data.insert(_data.begin() + _dataWritePosition, str.begin(), str.end());
		_IncreaseWritePos((uint32_t)str.length());
		return;
	}
	bool SerializeToU8Vector::Peek(std::string& str)
	{
		//str.clear();
		//uint64_t size = 0;
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		//Peek(size);

		//for (uint16_t i = sizeof(uint64_t); i < size + sizeof(uint64_t); i++)
		//{
		//	str += _data[_dataReadPosition + i];
		//}
		uint64_t size = 0;
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		if (CheckValidSize(sizeof(uint64_t)))
		{
			Peek(size);
			if (!size) return true; // a empty string was serialized
			//GAME_ASSERT(_GetPeekSizeCheck(size));
			if (CheckValidSize(size))
			{
				str = std::string(size, '\0');
				memcpy((uint8_t*)(&str[0]), &_data[_dataReadPosition+sizeof(uint64_t)], size);
				//_IncreaseReadPos(size);
				return true;
			}
		}
		return false;
	}
	bool game::SerializeToU8Vector::Get(std::string &str)
	{
		uint64_t size = 0;

		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		if (CheckValidSize(sizeof(uint64_t)))
		{
			Get(size);
			if (!size) return true; // a empty string was serialized
			//GAME_ASSERT(_GetPeekSizeCheck(size));
			if (CheckValidSize(size))
			{
				str = std::string(size, '\0');
				memcpy((uint8_t*)(&str[0]), &_data[_dataReadPosition], size);
				_IncreaseReadPos(size);
				return true;
			}
		}
		return false;
	}
	// uint8_t
	void SerializeToU8Vector::Add(const uint8_t c) 
	{
		_data.push_back(c);
		_IncreaseWritePos(sizeof(uint8_t));
	}
	bool SerializeToU8Vector::Peek(uint8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		c = _data[_dataReadPosition];
		return true;
	}
	bool SerializeToU8Vector::Get(uint8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		c = _data[_dataReadPosition];
		_IncreaseReadPos(sizeof(uint8_t));
		return true;
	}
	// int8_t
	void SerializeToU8Vector::Add(const int8_t c) // ccd
	{
		_data.push_back((uint8_t)c);
		_IncreaseWritePos(sizeof(int8_t));
	}
	bool SerializeToU8Vector::Peek(int8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int8_t)));
		c = (int8_t)_data[_dataReadPosition];
		return true;
	}
	bool SerializeToU8Vector::Get(int8_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int8_t)));
		c = (int8_t)_data[_dataReadPosition];
		_IncreaseReadPos(sizeof(int8_t));
		return true;
	}
	// uint16_t
	void SerializeToU8Vector::Add(const uint16_t c) // ccd
	{
		uint16_t t = _htons(c);
		uint8_t temp[sizeof(uint16_t)] = {};
		memcpy(temp, &t, sizeof(uint16_t));
		_data.insert(_data.end(), temp, temp + sizeof(uint16_t));
		_IncreaseWritePos(sizeof(uint16_t));
	}
	bool SerializeToU8Vector::Peek(uint16_t& c) // ccd
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(uint16_t));
		c = _ntohs(c);
		return true;
	}
	bool SerializeToU8Vector::Get(uint16_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint16_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(uint16_t));
		c = _ntohs(c);
		_IncreaseReadPos(sizeof(uint16_t));
		return true;
	}
	// int16_t
	void SerializeToU8Vector::Add(const int16_t c)
	{
		//GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		//*(int16_t*)(_dataWritePosition) = _htons(c);
		//_IncreaseWritePos(sizeof(int16_t));
		int16_t t = _htons(c);
		uint8_t temp[sizeof(int16_t)] = {};
		memcpy(temp, &t, sizeof(int16_t));
		_data.insert(_data.end(), temp, temp + sizeof(int16_t));
		_IncreaseWritePos(sizeof(int16_t));
	}
	bool SerializeToU8Vector::Peek(int16_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
		//c = *(int16_t*)(_dataReadPosition);
		//c = _ntohs(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(int16_t));
		c = _ntohs(c);
		return true;
	}
	bool SerializeToU8Vector::Get(int16_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
		//c = *(int16_t*)(_dataReadPosition);
		//_IncreaseReadPos(sizeof(int16_t));
		//c = _ntohs(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int16_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(int16_t));
		c = _ntohs(c);
		_IncreaseReadPos(sizeof(int16_t));
		return true;
	}
	// uint32_t
	void SerializeToU8Vector::Add(const uint32_t c)
	{
		//GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		//*(uint32_t*)(_dataWritePosition) = _htonl(c);
		//_IncreaseWritePos(sizeof(uint32_t));
		uint32_t t = _htonl(c);
		uint8_t temp[sizeof(uint32_t)] = {};
		memcpy(temp, &t, sizeof(uint32_t));
		_data.insert(_data.end(), temp, temp + sizeof(uint32_t));
		_IncreaseWritePos(sizeof(uint32_t));
	}
	bool SerializeToU8Vector::Peek(uint32_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
		//c = *(uint32_t*)(_dataReadPosition);
		//c = _ntohl(c);
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(uint32_t));
		c = _ntohl(c);
		return true;
	}
	bool SerializeToU8Vector::Get(uint32_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
		//c = *(uint32_t*)(_dataReadPosition);
		//_IncreaseReadPos(sizeof(uint32_t));
		//c = _ntohl(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint32_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(uint32_t));
		c = _ntohl(c);
		_IncreaseReadPos(sizeof(uint32_t));
		return true;
	}
	// int32_t
	void SerializeToU8Vector::Add(const int32_t c)
	{
		//GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		//*(int32_t*)(_dataWritePosition) = _htonl(c);
		//_IncreaseWritePos(sizeof(int32_t));
		int32_t t = _htonl(c);
		uint8_t temp[sizeof(int32_t)] = {};
		memcpy(temp, &t, sizeof(int32_t));
		_data.insert(_data.end(), temp, temp + sizeof(int32_t));
		_IncreaseWritePos(sizeof(int32_t));
	}
	bool SerializeToU8Vector::Peek(int32_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
		//c = *(int32_t*)(_dataReadPosition);
		//c = _ntohl(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(int32_t));
		c = _ntohl(c);
		return true;
	}
	bool SerializeToU8Vector::Get(int32_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
		//c = *(int32_t*)(_dataReadPosition);
		//_IncreaseReadPos(sizeof(int32_t));
		//c = _ntohl(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int32_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(int32_t));
		c = _ntohl(c);
		_IncreaseReadPos(sizeof(int32_t));
		return true;
	}
	// uint64_t
	void SerializeToU8Vector::Add(const uint64_t c)
	{
		//GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		//*(uint64_t*)(_dataWritePosition) = _hton64(c);
		uint64_t t = _hton64(c);
		uint8_t temp[sizeof(uint64_t)] = {};// = (uint8_t*)t;// _hton64(c);
		memcpy(temp, &t, sizeof(uint64_t));
		_data.insert(_data.end(), temp, temp + sizeof(uint64_t));
		_IncreaseWritePos(sizeof(uint64_t));
	}
	bool SerializeToU8Vector::Peek(uint64_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		//uint64_t t = 0;
		//uint8_t bytes[sizeof(uint64_t)] = {};
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(uint64_t));
		//c = *(uint64_t*)(_dataReadPosition);
		c = _ntoh64(c);
		return true;
	}
	bool SerializeToU8Vector::Get(uint64_t& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint64_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(uint64_t));
		//c = *(uint64_t*)(_dataReadPosition);
		c = _ntoh64(c);
		_IncreaseReadPos(sizeof(uint64_t));
		return true;
	}
	// int64_t 
	void SerializeToU8Vector::Add(const int64_t c)
	{
		////GAME_ASSERT(_AddSizeCheck(sizeof(c)));
		//*(int64_t*)(_dataWritePosition) = _hton64(c);
		//_IncreaseWritePos(sizeof(int64_t));
		int64_t t = _hton64(c);
		uint8_t temp[sizeof(int64_t)] = {};// = (uint8_t*)t;// _hton64(c);
		memcpy(temp, &t, sizeof(uint64_t));
		_data.insert(_data.end(), temp, temp + sizeof(int64_t));
		_IncreaseWritePos(sizeof(int64_t));
	}
	bool SerializeToU8Vector::Peek(int64_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
		//c = *(int64_t*)(_dataReadPosition);
		//c = _ntoh64(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
		//uint64_t t = 0;
		//uint8_t bytes[sizeof(uint64_t)] = {};
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(int64_t));
		//c = *(uint64_t*)(_dataReadPosition);
		c = _ntoh64(c);
		return true;
	}
	bool SerializeToU8Vector::Get(int64_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
		//c = *(int64_t*)(_dataReadPosition);
		//_IncreaseReadPos(sizeof(int64_t));
		//c = _ntoh64(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(int64_t)));
		memcpy(&c, _data.data() + _dataReadPosition, sizeof(int64_t));
		//c = *(uint64_t*)(_dataReadPosition);
		c = _ntoh64(c);
		_IncreaseReadPos(sizeof(int64_t));
		return true;
	}
	// float_t
	void SerializeToU8Vector::Add(const float_t c)
	{
		////GAME_ASSERT(_AddSizeCheck(sizeof(float_t)));
		//uint32_t temp = 0;
		//memcpy(&temp, &c, sizeof(float_t));
		//*(uint32_t*)(_dataWritePosition) = _htonl(temp);
		//_IncreaseWritePos(sizeof(float_t));
		uint32_t t = 0;
		memcpy(&t, &c, sizeof(float_t));
		t = _htonl(t);
		uint8_t temp[sizeof(float_t)] = {};// = (uint8_t*)t;// _hton64(c);
		memcpy(temp, &t, sizeof(float_t));
		_data.insert(_data.end(), temp, temp + sizeof(float_t));
		_IncreaseWritePos(sizeof(float_t));
	}
	bool SerializeToU8Vector::Peek(float_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		//uint32_t temp = *(uint32_t*)(_dataReadPosition);
		//temp = _ntohl(temp);
		//memcpy(&c, &temp, sizeof(float_t));
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		uint32_t temp = 0;
		memcpy(&temp, _data.data() + _dataReadPosition, sizeof(float_t));
		temp = _ntohl(temp);
		memcpy(&c, &temp, sizeof(float_t));
		//c = _ntohl(c);
		return true;
	}
	bool SerializeToU8Vector::Get(float_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		//uint32_t temp = *(uint32_t*)(_dataReadPosition);
		//_IncreaseReadPos(sizeof(float_t));
		//temp = _ntohl(temp);
		//memcpy(&c, &temp, sizeof(float_t));
		////GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		////memcpy(&c, _data.data() + _dataReadPosition, sizeof(float_t));
		//////c = *(uint64_t*)(_dataReadPosition);
		////c = _ntohl(c);
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(float_t)));
		uint32_t temp = 0;
		memcpy(&temp, _data.data() + _dataReadPosition, sizeof(float_t));
		temp = _ntohl(temp);
		memcpy(&c, &temp, sizeof(float_t));
		_IncreaseReadPos(sizeof(float_t));
		return true;
	}
	// double_t
	void SerializeToU8Vector::Add(const double_t c)
	{
		//GAME_ASSERT(_AddSizeCheck(sizeof(double_t)));
		//uint64_t temp = 0;
		//memcpy(&temp, &c, sizeof(double_t));
		//*(uint64_t*)(_dataWritePosition) = _hton64(temp);
		//_IncreaseWritePos(sizeof(double_t));
		uint64_t t = 0;
		memcpy(&t, &c, sizeof(double_t));
		t = _hton64(t);
		uint8_t temp[sizeof(double_t)] = {};// = (uint8_t*)t;// _hton64(c);
		memcpy(temp, &t, sizeof(double_t));
		_data.insert(_data.end(), temp, temp + sizeof(double_t));
		_IncreaseWritePos(sizeof(double_t));
	}
	bool SerializeToU8Vector::Peek(double_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
		//uint64_t temp = *(uint64_t*)(_dataReadPosition);
		//temp = _ntoh64(temp);
		//memcpy(&c, &temp, sizeof(double_t));
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
		uint64_t temp = 0;
		memcpy(&temp, _data.data() + _dataReadPosition, sizeof(double_t));
		temp = _ntoh64(temp);
		memcpy(&c, &temp, sizeof(double_t));
		return true;
	}
	bool SerializeToU8Vector::Get(double_t& c)
	{
		//GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
		//uint64_t temp = *(uint64_t*)(_dataReadPosition);
		//_IncreaseReadPos(sizeof(double_t));
		//temp = _ntoh64(temp);
		//memcpy(&c, &temp, sizeof(double_t));
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(double_t)));
		uint64_t temp = 0;
		memcpy(&temp, _data.data() + _dataReadPosition, sizeof(double_t));
		temp = _ntoh64(temp);
		memcpy(&c, &temp, sizeof(double_t));
		_IncreaseReadPos(sizeof(double_t));
		return true;
	}
	// bool
	void SerializeToU8Vector::Add(const bool c)
	{
		//GAME_ASSERT(_AddSizeCheck(sizeof(uint8_t)));
		uint8_t a = c ? 1 : 0;
		Add(a);
	}
	bool SerializeToU8Vector::Peek(bool& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		uint8_t a = 0;
		Peek(a);
		c = a ? true : false;
		return true;
	}
	bool SerializeToU8Vector::Get(bool& c)
	{
		GAME_ASSERT(_GetPeekSizeCheck(sizeof(uint8_t)));
		uint8_t a = 0;
		Get(a);
		c = a ? true : false;
		return true;
	}

	void SerializeToU8Vector::ClearData()
	{
		_data.clear();
		_dataLength = 0;
		_dataWritePosition = 0;// _data;
		_dataReadPosition = 0;// _data;
	}

	void SerializeToU8Vector::PrintAsU32()
	{
		for (size_t i = 0; i < _dataLength; i++)
		{
			//std::cout << (uint32_t) * (_dataReadPosition + i);
			std::cout << (uint32_t)_data[_dataReadPosition + i];
		}
		std::cout << std::endl;

	}
	void SerializeToU8Vector::PrintAsU8()
	{
		for (size_t i = 0; i < _dataLength; i++)
		{
			//std::cout << *(_dataReadPosition + i);
			std::cout << _data[_dataReadPosition + i];
		}
		std::cout << std::endl;
	}

	bool SerializeToU8Vector::_GetPeekSizeCheck(const size_t size) const
	{
		return size <= _dataLength ? true : false;
	}

	//bool SerializeToU8Vector::_AddSizeCheck(const size_t size) const
	//{
	//	//return (_dataLength + size) <= game::IOCP::Network::MAX_PACKET_SIZE ? true : false;
	//	return true;
	//}
	//#else
	//		const bool Packet::_GetPeekSizeCheck(const size_t size) const
	//		{
	//			return size > _dataLength ? false : true;
	//		}
	//		const bool Packet::_AddSizeCheck(const size_t size) const
	//		{
	//			return (_dataLength + size) > MAX_PACKET_SIZE ? false : true;
	//		}

	void SerializeToU8Vector::_IncreaseReadPos(const uint64_t amount)
	{
		_dataReadPosition += amount;
		_dataLength -= amount;
	}
	void SerializeToU8Vector::_IncreaseWritePos(uint64_t amount)
	{
		_dataWritePosition += amount;
		_dataLength += amount;
	}
}