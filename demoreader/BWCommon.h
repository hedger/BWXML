#pragma once

#include "BW.hpp"
#include <string>

namespace BWPack
{
	struct rawDataBlock
	{
		BigWorld::PackedDataType type;
		std::string data;
		rawDataBlock(const BigWorld::PackedDataType _type,  const std::string& _data)
			: type(_type), data(_data) {};
	};

	struct dataBlock
	{
		short stringId;
		rawDataBlock data;
		dataBlock(const short _stringId, rawDataBlock& _data) : stringId(_stringId), data(_data) {};
	};

	rawDataBlock PackBuffer(const std::string& strVal);
}