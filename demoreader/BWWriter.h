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

	typedef std::tuple<BigWorld::PackedDataType, StreamWriter::DataBuffer> rawDataBlock;
	typedef std::tuple<short, rawDataBlock> dataBlock;
	typedef std::vector<dataBlock> dataArray;
	//struct dataBlock
	//{
	//	short keyId;
	//	rawDataBlock data;
	//};

	void collectStrings();
	int  resolveString(const std::string& str);
	void treeWalker(const boost::property_tree::ptree& node);

	BigWorld::DataDescriptor BuildDescriptor(rawDataBlock block, int prevOffset);
	rawDataBlock serializeNode(const boost::property_tree::ptree& nodeVal, bool simple);

	StreamWriter::DataBuffer serializeF(float floatVal);
	StreamWriter::DataBuffer serializeI(unsigned int intVal);
	StreamWriter::DataBuffer serializeB(bool boolVal);
	StreamWriter::DataBuffer serializeS(const std::string& stringVal);
	StreamWriter::DataBuffer serializeSection(const boost::property_tree::ptree& section);
	//void ReadStringTable();
	//boost::property_tree::ptree ReadSection();
	//void readData(DataDescriptor descr, boost::property_tree::ptree& current_node, int prev_offset);
};
