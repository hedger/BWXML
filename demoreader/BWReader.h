#pragma once

#include <vector>

#include "DataStream.h"
#include "BW.hpp"
#include <boost/property_tree/ptree.hpp>

namespace BWPack
{
	class BWXMLReader
	{
	public:
		BWXMLReader(const std::string& fname);
		void saveTo(const std::string& destname) const;

	protected:
		IO::StreamReader mStream;
		std::vector<std::string> mStrings;
		boost::property_tree::ptree mTree;

		void ReadStringTable();
		boost::property_tree::ptree ReadSection();
		void readData(BigWorld::DataDescriptor descr, boost::property_tree::ptree& current_node, int prev_offset);
	};
}
