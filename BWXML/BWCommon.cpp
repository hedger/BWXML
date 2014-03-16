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

#include "BWCommon.h"

#include "DataStream.h"
#include "Base64.h"

#include <vector>
#include <sstream>
#include <algorithm>

namespace BWPack
{
	using namespace BigWorld;
	using namespace IO;

	static std::string serializeF(const std::vector<double>& floatVals)
	{
		std::stringstream _ret;
		StreamBufWriter ret(_ret.rdbuf());
		std::for_each(floatVals.begin(), floatVals.end(), 
			[&](float v){ ret.put<float>(static_cast<float>(v)); });
		return _ret.str();
	}

	static std::string serializeI(const int64_t& intVal)
	{
		std::stringstream _ret;
		StreamBufWriter ret(_ret.rdbuf());

		uint64_t absVal = abs(intVal);
		if (absVal > std::numeric_limits<int32_t>::max())
			ret.put<int64_t>(intVal);
		else if (absVal > std::numeric_limits<int16_t>::max())
			ret.put<int32_t>(static_cast<int>(intVal));
		else if (absVal > std::numeric_limits<int8_t>::max())
			ret.put<int16_t>(static_cast<int16_t>(intVal));
		else if (absVal != 0)
			ret.put<int8_t>(static_cast<int8_t>(intVal));

		return _ret.str();
	}

	static std::string serializeB(const bool boolVal)
	{
		std::stringstream _ret;
		StreamBufWriter ret(_ret.rdbuf());
		if (boolVal)
			ret.put<uint8_t>(1);
		return _ret.str();
	}


	rawDataBlock PackBuffer(const std::string& strVal)
	{
		if (strVal.empty())
		{
			return rawDataBlock(BW_String, "");
		}

		// contains a dot, maybe that's a float/floats?
		// but values with 'f' in the end are strings!
		if ((strVal.find('.') != std::string::npos)  
			&& (strVal.find('f') == std::string::npos))
		{
			std::vector<double> values;
			double tmp;
			std::stringstream ss;

			ss << strVal;
			ss >> tmp;
			if (!ss.fail()) // that WAS a float
			{
				values.push_back(tmp);
				while (!ss.eof() && !ss.fail())
				{
					ss >> tmp;
					if (!ss.fail())
						values.push_back(tmp);
				}
				return rawDataBlock(BW_Float, serializeF(values));
			}
		}

		{
			std::stringstream ss;
			ss << strVal;
			uint64_t i;
			ss >> i;
			if (!ss.fail() && ss.eof())
				return rawDataBlock(BW_Int, serializeI(i));
		}

		{
			if (!strVal.compare("true"))
				return rawDataBlock(BW_Bool, serializeB(true));
			else if (!strVal.compare("false"))
				return rawDataBlock(BW_Bool, serializeB(false));
		}

		//check if we can B64 this
		if (B64::Is(strVal))
		{
			return rawDataBlock(BW_Blob, B64::Decode(strVal));
		}

		// fallback
		return rawDataBlock(BW_String, strVal);
	}}
