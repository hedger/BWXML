#pragma once

#include <vector>

#include "StreamReader.h"
#include "BW.hpp"
#include <boost/property_tree/ptree.hpp>

class BWXMLReader
{
public:
  BWXMLReader(const std::string& fname);

  boost::property_tree::ptree toPtree()
  {
    return tree;
  };

  static const int PACKED_SECTION_MAGIC = 0x62a14e45;

protected:
  StreamReader mStream;
  std::vector<std::string> strings;
  boost::property_tree::ptree tree;

  void dumppos(const std::string comment = "");
  void ReadStringTable();
  boost::property_tree::ptree ReadSection();
	void readData(DataDescriptor descr, boost::property_tree::ptree& current_node, int prev_offset);
};