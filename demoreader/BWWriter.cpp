#include "BWWriter.h"

#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>

using boost::property_tree::ptree;
using namespace BigWorld;

BWXMLWriter::BWXMLWriter(const std::string& fname)
{
	try
	{
		boost::property_tree::xml_parser::read_xml(fname, mTree, 
			boost::property_tree::xml_parser::trim_whitespace);
	}
	catch(...)
	{
		throw std::exception("XML parsing error");
	};
	if (mTree.size() != 1)
		throw std::exception("XML file must contain only 1 root level node");
	
	mTree.swap(mTree.begin()->second); // swapping the whole tree to its first node
}

void BWXMLWriter::treeWalker(const ptree& node)
{
	for (auto subNode=node.begin(); subNode != node.end(); ++subNode)
	{
		mStrings.push_back(subNode->first);
		treeWalker(subNode->second);
	}
}

void BWXMLWriter::collectStrings()
{
	mStrings.clear();
	treeWalker(mTree);
	std::sort(mStrings.begin(), mStrings.end());
	mStrings.erase(std::unique(mStrings.begin(), mStrings.end()),
		mStrings.end());
}

int BWXMLWriter::resolveString(const std::string& str)
{
	auto pos = std::find(mStrings.begin(), mStrings.end(), str);
	if (pos == mStrings.end())
		throw("String key not found!");
	return (pos - mStrings.begin());
}

BWXMLWriter::rawDataBlock BWXMLWriter::serializeNode(const boost::property_tree::ptree& node_value, bool simple)
{
	if (!simple && node_value.size()) // has sub-nodes
	{
		return rawDataBlock(BW_Section, serializeSection(node_value));
	}

	try
	{
		int val = node_value.get_value<int>();
		//std::cout << "int!" << val << std::endl;
		return rawDataBlock(BW_Int, serializeI(val));
	}
	catch (const boost::property_tree::ptree_bad_data& e)	{ }

	try
	{
		float val = node_value.get_value<float>();
		//std::cout << "float!" << val << std::endl;
		return rawDataBlock(BW_Float, serializeF(val));
	}
	catch (const boost::property_tree::ptree_bad_data& e)	{ }

	try
	{
		bool val = node_value.get_value<bool>();
		//std::cout << "bool!" << val << std::endl;
		return rawDataBlock(BW_Bool, serializeB(val));
	}
	catch (const boost::property_tree::ptree_bad_data& e)	{ }

	try
	{
		std::string val = node_value.get_value<std::string>();
		//std::cout << "string!" << val << std::endl;
		return rawDataBlock(BW_String, serializeS(val));
	}
	catch (const boost::property_tree::ptree_bad_data& e)	{ }

	throw std::exception("Couldn't find a conversion!");
}

BigWorld::DataDescriptor BWXMLWriter::BuildDescriptor(rawDataBlock block, int prevOffset)
{
	return DataDescriptor(block.type, prevOffset + block.data.length());
}

void BWXMLWriter::saveTo(const std::string& destname)
{
	collectStrings();
	
	std::stringstream outbuf;
	StreamBufWriter outstream(outbuf.rdbuf());
	outstream.put(BigWorld::PACKED_SECTION_MAGIC);
	outstream.put<char>(0);
	for (auto it = mStrings.begin(); it!= mStrings.end(); ++it)
		outstream.putString(*it);
	outstream.put<char>(0);

	outstream.putString(serializeSection(mTree), false);

	std::ofstream mFile;
	mFile.open(destname, std::ios::binary);
	if (!mFile.is_open())
		throw std::exception("Can't open the file");
	mFile << outbuf.rdbuf();
	mFile.close();
}

std::string BWXMLWriter::serializeSection(const ptree& node)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());

	rawDataBlock ownData = serializeNode(node, true); // getting own plain content
	dataArray childData;
	for (auto it=node.begin(); it!=node.end(); ++it)
	{
		//std::cout << "For " << it->first << " : ";
		childData.push_back(dataBlock(resolveString(it->first), serializeNode(it->second, false)));
	}

	DataDescriptor ownDescriptor = BuildDescriptor(ownData, 0);
	ret.put<short>(node.size());
	ret.put<DataDescriptor>(ownDescriptor);
	ret.putString(ownData.data, false);
	
	int currentOffset = ownDescriptor.offset();
	for (auto it=childData.begin(); it!=childData.end(); ++it)
	{
		//std::cout << "off=" << currentOffset << std::endl;
		DataNode bwNode;
		bwNode.nameIdx = it->stringId;
		bwNode.data = BuildDescriptor(it->data, currentOffset);
		ret.put<DataNode>(bwNode);
		currentOffset = bwNode.data.offset();
	}
	
	for (auto it=childData.begin(); it!=childData.end(); ++it)
	{
		ret.putString(it->data.data, false);
	}

	return _ret.str();
}

std::string BWXMLWriter::serializeF(float floatVal)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());
	ret.put(floatVal);
	return _ret.str();
}

std::string BWXMLWriter::serializeI(unsigned int intVal)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());

	if (intVal > 0xFFFF)
	{
		ret.put<unsigned int>(intVal);
	}
	else if (intVal > 0xFF)
	{
		ret.put<unsigned short>(static_cast<unsigned short>(intVal));
	}
	else if (intVal > 0)
	{
		ret.put<unsigned char>(static_cast<unsigned char>(intVal));
	}
	return _ret.str();
}

std::string BWXMLWriter::serializeB(bool boolVal)
{
	std::stringstream _ret;
	StreamBufWriter ret(_ret.rdbuf());
	if (boolVal)
		ret.put<unsigned char>(1);
	return _ret.str();
}

std::string BWXMLWriter::serializeS(const std::string& stringVal)
{
	return stringVal;
}
