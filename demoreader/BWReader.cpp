#include "BWReader.h"
#include "Base64.h"
#include "BWCommon.h"

#include <sstream>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;
using namespace BigWorld;

BWXMLReader::BWXMLReader(const std::string& fname) : mStream(fname)
{
	int magic = mStream.get<int>();
	if (magic != PACKED_SECTION_MAGIC)
		throw std::exception("Wrong header magic");

	unsigned char version = mStream.get<char>();
	if (version != 0)
		throw std::exception("Unsupported file version");
  ReadStringTable();

  mTree.put_child("root", ReadSection());
};

void BWXMLReader::saveTo(const std::string& destname)
{
  static auto settings = boost::property_tree::xml_writer_make_settings('\t', 1);
  boost::property_tree::write_xml(destname, mTree, std::locale(), settings);
}

void BWXMLReader::ReadStringTable()
{
	for (std::string tmp = mStream.getNullTerminatedString(); 
    !tmp.empty(); 
    tmp = mStream.getNullTerminatedString())
		  mStrings.push_back(tmp);
	std::cout << "Collected " << mStrings.size() << " strings." << std::endl;
};

void BWXMLReader::readData(DataDescriptor descr, ptree& current_node, int prev_offset)
{
	current_node.clear();
	int startPos = prev_offset, endPos = descr.offset();
	//if (!endPos)
	//	return;
	int var_size = endPos - startPos;
	assert(var_size >= 0);

	std::stringstream contentBuffer;
	switch(descr.typeId())
	{
	case BW_Section:
    //std::cerr << "BW_Section\n";
		current_node.swap(ReadSection()); //yay recursion!
		//current_node.put("<xmlcomment>", "BW_Section");
		break;
	case BW_String:
    //std::cerr << "BW_String\n";
		contentBuffer << mStream.getString(var_size);
		if (PackBuffer(contentBuffer.str()).type != BW_String)
			current_node.put("<xmlcomment>", "BW_String");
		current_node.put_value(contentBuffer.str());
		break;
	case BW_Int:
    //std::cerr << "BW_Int\n";
		int tmp;
		switch (var_size)
		{
		case 4:
			tmp = mStream.get<int>();
			break;
		case 2:
			tmp = mStream.get<short>();
			break;
		case 1:
			tmp = mStream.get<char>();
			break;
		case 0:
			tmp = 0;
			break;
		default:
			throw std::exception("Unsupported int size!");
		}
		current_node.put_value(tmp);
		//current_node.put("<xmlcomment>", "BW_Int");
		break;
	case BW_Float:
    //std::cerr << "BW_Float\n";
		assert(var_size % sizeof(float) == 0);
		
		for (size_t i=0; i<(var_size / sizeof(float)); ++i)
		{
			if (!contentBuffer.str().empty())
				contentBuffer << " ";
			contentBuffer << mStream.get<float>();
		}
		current_node.put_value(contentBuffer.str());
		//current_node.put("<xmlcomment>", "BW_Float");
		break;
	case BW_Bool:
    //std::cerr << "BW_Bool\n";
		// false is encoded as 0, that is, no bytes at all
		current_node.put_value((var_size != 0));
		//current_node.put("<xmlcomment>", "BW_Bool");
    mStream.getString(var_size);
		break;
	case BW_Blob:
    //std::cerr << "BW_Blob\n";
		current_node.put_value(B64::Encode(mStream.getString(var_size)));
		//current_node.put("<xmlcomment>", "BW_Blob");
		break;
	case BW_Enc_blob:
    //std::cerr << "BW_Enc_blob\n";
    mStream.getString(var_size); // TBD?
		//mStream.getBuffer(var_size);
		current_node.put_value("TYPE_ENCRYPTED_BLOB is (yet) unsupported!");
		//current_node.put("<xmlcomment>", "BW_Enc_blob");
		std::cerr <<"unsupported section TYPE_ENCRYPTED_BLOB!" << std::endl;
		break;
	default:
		throw std::exception("Unknown section!");
	}
}

ptree BWXMLReader::ReadSection()
{
	ptree current_node;
	int nChildren = mStream.get<short>();

	DataDescriptor ownData = mStream.get<DataDescriptor>();

	std::vector<DataNode> children;
	children.reserve(nChildren);
	for (int i=0; i<nChildren; ++i)
	{
		children.push_back(mStream.get<DataNode>());
	}

	readData(ownData, current_node, 0);

	int prev_offset = ownData.offset();
	for (auto it = children.begin(); it != children.end(); ++it)
	{
		//keys may contain dots, ptree gets confused
		auto path = ptree::path_type(mStrings[it->nameIdx], '\0'); // so we make a custom path
    //std::cerr << ">> " << mStrings[it->nameIdx] << " is ";
		readData(it->data, current_node.add(path, ""), prev_offset); 
		prev_offset = it->data.offset();
	}
	return current_node;
}