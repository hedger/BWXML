#pragma once

#include "BW.hpp"
#include <string>

struct rawDataBlock
{
	BigWorld::PackedDataType type;
	std::string data;
	rawDataBlock(BigWorld::PackedDataType _type,  std::string _data) : type(_type), data(_data) {};
};

struct dataBlock
{
	short stringId;
	rawDataBlock data;
	dataBlock(short _stringId, rawDataBlock& _data) : stringId(_stringId), data(_data) {};
};

rawDataBlock PackBuffer(const std::string& strVal);