/*
Copyright 2013-2014 hedger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "BW.hpp"
#include <string>
#include <cstdint>

namespace BWPack
{
	static const int BW_MATRIX_NROWS = 4;
	static const int BW_MATRIX_NCOLS = 3;
	static const int BW_MATRIX_SIZE = BW_MATRIX_NROWS * BW_MATRIX_NCOLS;

	struct rawDataBlock
	{
		BigWorld::PackedDataType type;
		std::string data;
		rawDataBlock(const BigWorld::PackedDataType _type, const std::string _data)
			: type(_type), data(_data) {};
	};

	struct dataBlock
	{
		uint16_t stringId;
		rawDataBlock data;
		dataBlock(const uint16_t _stringId, rawDataBlock _data) 
			: stringId(_stringId), data(_data) {};
	};

	rawDataBlock PackBuffer(const std::string& strVal);
}