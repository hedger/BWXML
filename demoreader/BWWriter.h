#pragma once

#include <vector>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

#include "BW.hpp"
#include "DataStream.h"

class BWXMLWriter
{
public:
	BWXMLWriter(const std::string& fname);
	void saveTo(const std::string& destname);

protected:
	std::vector<std::string> mStrings;
	boost::property_tree::ptree mTree;

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

	typedef std::vector<dataBlock> dataArray;

	void collectStrings();
	int  resolveString(const std::string& str);
	void treeWalker(const boost::property_tree::ptree& node);

	BigWorld::DataDescriptor BuildDescriptor(rawDataBlock block, int prevOffset);
	rawDataBlock serializeNode(const boost::property_tree::ptree& nodeVal, bool simple);

	std::string serializeF(float floatVal);
	std::string serializeI(unsigned int intVal);
	std::string serializeB(bool boolVal);
	std::string serializeS(const std::string& stringVal);
	std::string serializeSection(const boost::property_tree::ptree& section);
};
