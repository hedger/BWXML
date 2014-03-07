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

#include "BWReader.h"
#include "Base64.h"
#include "BWCommon.h"

#include <sstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

namespace BWPack
{
	using boost::property_tree::ptree;
	using namespace BigWorld;

	BWXMLReader::BWXMLReader(const std::string& fname) : mStream(fname)
	{
		int magic = mStream.get<int>();
		if (magic != PACKED_SECTION_MAGIC)
			throw std::runtime_error("Wrong header magic");

		unsigned char version = mStream.get<unsigned char>();
		if (version != 0)
			throw std::runtime_error("Unsupported file version");
		ReadStringTable();

		mTree.put_child("root", ReadSection());
	};

	void BWXMLReader::saveTo(const std::string& destname) const
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
		//std::cout << "Collected " << mStrings.size() << " strings." << std::endl;
	};

	void BWXMLReader::readData(DataDescriptor descr, ptree& current_node, int prev_offset)
	{
		current_node.clear();
		int startPos = prev_offset, endPos = descr.offset();
		int var_size = endPos - startPos;
		assert(var_size >= 0);

		std::stringstream contentBuffer;
		switch(descr.typeId())
		{
		case BW_Section:
			current_node = ReadSection(); //yay recursion!
			break;

		case BW_String:
			contentBuffer << mStream.getString(var_size);
			if (PackBuffer(contentBuffer.str()).type != BW_String)
				current_node.put("<xmlcomment>", "BW_String");
			current_node.put_value(contentBuffer.str());
			break;

		case BW_Int:
			switch (var_size)
			{
			case 8:
				current_node.put_value(mStream.get<long long>());
				break;
			case 4:
				current_node.put_value(mStream.get<int>());
				break;
			case 2:
				current_node.put_value(mStream.get<short>());
				break;
			case 1:
				// static_cast'ing to force ptree interprete our 1-byte value as a number, not a character
				current_node.put_value(static_cast<int>(mStream.get<char>()));
				break;
			case 0:
				current_node.put_value(0);
				break;
			default:
				throw std::runtime_error("Unsupported int size!");
			}
			break;

		case BW_Float:
			assert(var_size % sizeof(float) == 0);
			contentBuffer << std::fixed << std::setfill('\t');
			if (var_size / sizeof(float) == BW_MATRIX_SIZE) // we've got a matrix!
			{
				for (int i=0; i<BW_MATRIX_NROWS; ++i)
				{
					for (int j=0; j<BW_MATRIX_NCOLS; ++j)
					{
						if (!contentBuffer.str().empty())
							contentBuffer << " ";
						contentBuffer << mStream.get<float>();
					}
					current_node.put("row" + boost::lexical_cast<std::string>(i), contentBuffer.str());
					contentBuffer.str(""); // clearing our buffer
				}
				break;
			}
			// not a matrix, building a plain string
			for (size_t i=0; i<(var_size / sizeof(float)); ++i)
			{
				if (!contentBuffer.str().empty())
					contentBuffer << " ";
				contentBuffer << mStream.get<float>();
			}
			current_node.put_value(contentBuffer.str());
			break;

		case BW_Bool:
			current_node.put_value((var_size != 0));
			mStream.getString(var_size);
			break;

		case BW_Blob:
			current_node.put_value(B64::Encode(mStream.getString(var_size)));
			break;

		case BW_Enc_blob:
			mStream.getString(var_size); // TBD?
			current_node.put_value("TYPE_ENCRYPTED_BLOB is (yet) unsupported!");
			std::cerr <<"unsupported section TYPE_ENCRYPTED_BLOB!" << std::endl;
			break;

		default:
			throw std::runtime_error("Unsupported section type!");
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
			assert(it->nameIdx < mStrings.size());
			//keys may contain dots, they confuse the ptree
			auto path = ptree::path_type(mStrings[it->nameIdx], '\0'); // so we make a custom path
			readData(it->data, current_node.add(path, ""), prev_offset); 
			prev_offset = it->data.offset();
		}
		return current_node;
	}
}
