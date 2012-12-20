// demoreader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "BWReader.h"
#include <boost/property_tree/xml_parser.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
  BWXMLReader reader("engine_config.xml");
  boost::property_tree::write_xml("out.xml", reader.toPtree());

  return 0;
}