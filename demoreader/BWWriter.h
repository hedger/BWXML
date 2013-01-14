#pragma once

#include <vector>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

#include "BW.hpp"
#include "BWCommon.h"
#include "DataStream.h"

class BWXMLWriter
{
public:
	BWXMLWriter(const std::string& fname);
	void saveTo(const std::string& destname);

protected:
	std::vector<std::string> mStrings;
	boost::property_tree::ptree mTree;

	typedef std::vector<dataBlock> dataArray;

	void collectStrings();
	int  resolveString(const std::string& str);
	void treeWalker(const boost::property_tree::ptree& node);

	BigWorld::DataDescriptor BuildDescriptor(rawDataBlock block, int prevOffset);
	rawDataBlock serializeNode(const boost::property_tree::ptree& nodeVal, bool simple);
	std::string serializeSection(const boost::property_tree::ptree& section);
};
