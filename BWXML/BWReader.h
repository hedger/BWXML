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
		void readData(BigWorld::DataDescriptor descr, boost::property_tree::ptree& current_node, uint32_t prev_offset);
	};
}
