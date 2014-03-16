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
		uint16_t resolveString(const std::string& str) const;
		void treeWalker(const boost::property_tree::ptree& node);

		BigWorld::DataDescriptor BuildDescriptor(BWPack::rawDataBlock block, uint32_t prevOffset) const;
		BWPack::rawDataBlock serializeNode(const boost::property_tree::ptree& nodeVal, bool simple) const;
		std::string serializeSection(const boost::property_tree::ptree& section) const;
	};
}