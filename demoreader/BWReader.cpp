#include "BWReader.h"

#include <sstream>
using boost::property_tree::ptree;

static std::string byteArrayToBase64(StreamReader::DataBuffer a)
{
	static char intToBase64[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };
	int aLen = a.size();
	int numFullGroups = aLen / 3;
	int numBytesInPartialGroup = aLen - 3 * numFullGroups;
	int resultLen = 4 * ((aLen + 2) / 3);
	std::string result;
	result.reserve(resultLen);

	int inCursor = 0;
	for (int i = 0; i < numFullGroups; i++)
	{
		int byte0 = a[inCursor++] & 0xff;
		int byte1 = a[inCursor++] & 0xff;
		int byte2 = a[inCursor++] & 0xff;
		result.push_back(intToBase64[byte0 >> 2]);
		result.push_back(intToBase64[(byte0 << 4) & 0x3f | (byte1 >> 4)]);
		result.push_back(intToBase64[(byte1 << 2) & 0x3f | (byte2 >> 6)]);
		result.push_back(intToBase64[byte2 & 0x3f]);
	}

	if (numBytesInPartialGroup != 0)
	{
		int byte0 = a[inCursor++] & 0xff;
		result.push_back(intToBase64[byte0 >> 2]);
		if (numBytesInPartialGroup == 1)
		{
			result.push_back(intToBase64[(byte0 << 4) & 0x3f]);
			result.append("==");
		}
		else
		{
			int byte1 = a[inCursor++] & 0xff;
			result.push_back(intToBase64[(byte0 << 4) & 0x3f | (byte1 >> 4)]);
			result.push_back(intToBase64[(byte1 << 2) & 0x3f]);
			result.push_back('=');
		}
	}

	return result;
}

BWXMLReader::BWXMLReader(const std::string& fname) : mStream(fname)
{
	int magic = mStream.get<int>();
	if (magic != PACKED_SECTION_MAGIC)
		throw std::exception("Wrong header magic");

	unsigned char version = mStream.get<char>();
	if (version != 0)
		throw std::exception("Unsupported file version");
	ReadStringTable();

	tree.put_child("root", ReadSection());
};

void BWXMLReader::ReadStringTable()
{
	for (std::string tmp = mStream.get(); !tmp.empty(); tmp = mStream.get())
		strings.push_back(tmp);
	std::cout << "Collected " << strings.size() << " strings." << std::endl;
};

void BWXMLReader::dumppos(const std::string comment)
{
	std::cout << comment << " (@ 0x"  << std::hex << mStream.Pos() << ") " << std::dec;
}

void BWXMLReader::readData(DataDescriptor descr, ptree& current_node, int prev_offset)
{
	current_node.clear();
	int startPos = prev_offset, endPos = descr.offset();
	if (!endPos)
		return;
	int var_size = endPos - startPos;
	assert(var_size >= 0);

	std::stringstream contentBuffer;
	switch(descr.typeId())
	{
	case BW_Section:
		current_node.swap(ReadSection()); //yay recursion!
		break;
	case BW_String:
		current_node.put_value(mStream.getFixedString(var_size));
		break;
	case BW_Int:
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
		break;
	case BW_Float:
		assert(var_size % sizeof(float) == 0);
		
		for (size_t i=0; i<(var_size / sizeof(float)); ++i)
		{
			if (!contentBuffer.str().empty())
				contentBuffer << " ";
			contentBuffer << mStream.get<float>();
		}
		current_node.put_value(contentBuffer.str());
		break;
	case BW_Bool:
		// false is encoded as 0, that is, no bytes at all
		current_node.put_value((var_size != 0));
		mStream.Advance(var_size);
		break;
	case BW_Blob:
		current_node.put_value(byteArrayToBase64(mStream.getBuffer(var_size)));
		break;
	case BW_Enc_blob:
		mStream.Advance(var_size); //TBD
		//mStream.getBuffer(var_size);
		current_node.put_value("TYPE_ENCRYPTED_BLOB is (yet) unsupported!");
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
		auto path = ptree::path_type(strings[it->nameIdx], '\0'); // so we make a custom path
		readData(it->data, current_node.add(path, ""), prev_offset); 
		prev_offset = it->data.offset();
	}
	return current_node;
}