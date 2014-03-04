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

	static std::string serializeF(const std::vector<float>& floatVals)
	{
		std::stringstream _ret;
		StreamBufWriter ret(_ret.rdbuf());
		std::for_each(floatVals.begin(), floatVals.end(), [&](float v){ ret.put<float>(v); });
		return _ret.str();
	}

	static std::string serializeI(const long long& intVal)
	{
		std::stringstream _ret;
		StreamBufWriter ret(_ret.rdbuf());

		unsigned long long absVal = abs(intVal);
		if (absVal > std::numeric_limits<long>::max())
			ret.put<long long>(intVal);
		else if (absVal > std::numeric_limits<short>::max())
			ret.put<int>(static_cast<int>(intVal));
		else if (absVal > std::numeric_limits<char>::max())
			ret.put<short>(static_cast<short>(intVal));
		else if (absVal != 0)
			ret.put<char>(static_cast<char>(intVal));

		return _ret.str();
	}

	static std::string serializeB(const bool boolVal)
	{
		std::stringstream _ret;
		StreamBufWriter ret(_ret.rdbuf());
		if (boolVal)
			ret.put<unsigned char>(1);
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
			std::vector<float> values;
			float tmp;
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
			long long i;
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

		return rawDataBlock(BW_String, strVal);
	}}
