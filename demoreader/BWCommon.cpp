#include "BWCommon.h"

#include "DataStream.h"
#include "Base64.h"

#include <vector>
#include <sstream>
#include <algorithm>

using namespace BigWorld;

static std::string serializeF(std::vector<float> floatVals)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());
	std::for_each(floatVals.begin(), floatVals.end(), [&](float v){ ret.put<float>(v); });
	return _ret.str();
}

static std::string serializeI(int intVal)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());

	unsigned int absVal = abs(intVal);
	if (absVal > 0x7FFF)
		ret.put<int>(intVal);
	else if (absVal > 0x7F)
		ret.put<short>(static_cast<short>(intVal));
	else if (absVal != 0)
		ret.put<char>(static_cast<char>(intVal));

	return _ret.str();
}

static std::string serializeB(bool boolVal)
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
		int i;
		ss >> i;
		if (!ss.fail() && ss.eof())
			return rawDataBlock(BW_Int, serializeI(i));
	}

	{
		std::stringstream ss;
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
}