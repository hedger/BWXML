#pragma once

#include <vector>
#include <tuple>
#include <boost/property_tree/ptree.hpp>

#include "BW.hpp"
#include "BWCommon.h"
#include "DataStream.h"

namespace BWPack
{
	class BWXMLWriter
	{
	public:
		BWXMLWriter(const std::string& fname);
		void saveTo(const std::string& destname);

	protected:
		std::vector<std::string> mStrings;
		boost::property_tree::ptree mTree;

		typedef std::vector<BWPack::dataBlock> dataArray;

		void collectStrings();
		int  resolveString(const std::string& str) const;
		void treeWalker(const boost::property_tree::ptree& node);

		BigWorld::DataDescriptor BuildDescriptor(BWPack::rawDataBlock block, int prevOffset) const;
		BWPack::rawDataBlock serializeNode(const boost::property_tree::ptree& nodeVal, bool simple) const;
		std::string serializeSection(const boost::property_tree::ptree& section) const;
	};
}