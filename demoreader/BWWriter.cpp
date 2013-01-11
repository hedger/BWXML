#include "BWWriter.h"

#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
//#include <forward_list>

using boost::property_tree::ptree;
using namespace BigWorld;

static void concatBuffers(StreamWriter::DataBuffer& a, const StreamWriter::DataBuffer& b)
{
	a.reserve(a.size() + b.size());
	a.insert(a.end(), b.cbegin(), b.cend());
}

template<typename T>
static void writeToBuffer(StreamWriter::DataBuffer& a, const T& val)
{
	size_t insertPos = a.size();
	a.resize(a.size()+sizeof(T));
	memcpy(&(a.data()[insertPos]), &val, sizeof(T));
}

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
	return DataDescriptor(std::get<0>(block), prevOffset + std::get<1>(block).size());
}

void BWXMLWriter::saveTo(const std::string& destname)
{
	collectStrings();
	
	StreamWriter outstream(destname);
	outstream.put(BigWorld::PACKED_SECTION_MAGIC);
	outstream.put<char>(0);
	for (auto it = mStrings.begin(); it!= mStrings.end(); ++it)
		outstream.putString(*it);
	outstream.put<char>(0);

	outstream.putBuffer(serializeSection(mTree));
}

StreamWriter::DataBuffer BWXMLWriter::serializeSection(const ptree& node)
{
	StreamWriter::DataBuffer ret;
	rawDataBlock ownData = serializeNode(node, true); // getting own plain content
	dataArray childData;
	for (auto it=node.begin(); it!=node.end(); ++it)
	{
		//std::cout << "For " << it->first << " : ";
		childData.push_back(dataBlock(resolveString(it->first), serializeNode(it->second, false)));
	}

	DataDescriptor ownDescriptor = BuildDescriptor(ownData, 0);
	writeToBuffer<short>(ret, node.size());
	writeToBuffer<DataDescriptor>(ret, ownDescriptor);
	concatBuffers(ret, std::get<1>(ownData));
	
	int currentOffset = ownDescriptor.offset();
	for (auto it=childData.begin(); it!=childData.end(); ++it)
	{
		//std::cout << "off=" << currentOffset << std::endl;
		DataNode bwNode;
		bwNode.nameIdx = std::get<0>(*it);
		bwNode.data = BuildDescriptor(std::get<1>(*it), currentOffset);
		writeToBuffer<DataNode>(ret, bwNode);
		currentOffset = bwNode.data.offset();
	}
	
	for (auto it=childData.begin(); it!=childData.end(); ++it)
	{
		concatBuffers(ret, std::get<1>(std::get<1>(*it)));
	}

	return ret;
}

StreamWriter::DataBuffer BWXMLWriter::serializeF(float floatVal)
{
	StreamWriter::DataBuffer ret;
	ret.resize(sizeof(float));
	memcpy(ret.data(), &floatVal, sizeof(float));
	return ret;
}

StreamWriter::DataBuffer BWXMLWriter::serializeI(unsigned int intVal)
{
	StreamWriter::DataBuffer ret;
	if (intVal > 0xFFFF)
	{
		ret.resize(sizeof(unsigned int));
		memcpy(ret.data(), &intVal, sizeof(int));
	}
	else if (intVal > 0xFF)
	{
		ret.resize(sizeof(unsigned short));
		unsigned short tmp = static_cast<unsigned short>(intVal);
		memcpy(ret.data(), &tmp, sizeof(unsigned short));
	}
	else if (intVal > 0)
	{
		ret.resize(sizeof(unsigned char));
		unsigned char tmp = static_cast<unsigned char>(intVal);
		memcpy(ret.data(), &tmp, sizeof(unsigned char));
	}
	//std::cout << "Compressed 0x" << std::hex << intVal << " to " << std::dec << ret.size() << " b." << std::endl;
	return ret;
}

StreamWriter::DataBuffer BWXMLWriter::serializeB(bool boolVal)
{
	StreamWriter::DataBuffer ret;
	if (boolVal)
		ret.push_back(1);
	return ret;
}

StreamWriter::DataBuffer BWXMLWriter::serializeS(const std::string& stringVal)
{
	StreamWriter::DataBuffer ret;
	std::copy(stringVal.cbegin(), stringVal.cend(), std::back_inserter(ret));
	return ret;
}
