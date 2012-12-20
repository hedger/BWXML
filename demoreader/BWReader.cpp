#include "BWReader.h"

using boost::property_tree::ptree;

BWXMLReader::BWXMLReader(const std::string& fname) : fstr(fname)
{
  int magic = fstr.get<int>();
  if (magic != PACKED_SECTION_MAGIC)
    throw std::exception("Wrong header magic");

  unsigned char version = fstr.get<char>();
  if (version != 0)
    throw std::exception("Unsupported file version");
  ReadStringTable();

  tree.put_child("root", ReadSection());
};

void BWXMLReader::ReadStringTable()
{
  for (std::string tmp = fstr.get(); !tmp.empty(); tmp = fstr.get())
    strings.push_back(tmp);
  std::cout << "Collected " << strings.size() << " strings." << std::endl;
};

void BWXMLReader::dumppos(const std::string comment)
{
  std::cout << comment << " (@ 0x"  << std::hex << fstr.Pos() << ") " << std::dec;
}

ptree BWXMLReader::ReadSection()
{
  std::cout << "Parsing section @ 0x" << std::hex << fstr.Pos() << std::endl;
  //dumppos();
  ptree current_node;
  int nChildren = fstr.get<PackedSectionData::NumChildrenType>();
  std::cout << "Got " << nChildren << " to go!" << std::endl;
  std::vector<ChildRecord> children;
  for (int i=0; i<nChildren; ++i)
  {
    children.push_back(fstr.get<ChildRecord>());
  }
  children.push_back(ChildRecord::Dummy(fstr.get<PackedSectionData::DataPosType>()));

  size_t dataRawStart = fstr.Pos();
  for (auto it = children.begin(); it != children.end() - 1; ++it)
  {
    std::string& name = strings[it->keyPos()];
    size_t startPos = it->startPos(), endPos = it->endPos();
    size_t var_size = endPos - startPos;

    std::cout << "Section " << name
      << " is @ 0x" << std::hex << dataRawStart + startPos
      << " - @ 0x" << std::hex << dataRawStart + endPos << std::endl;

    auto pos = fstr.Pos(), desiredPos = dataRawStart + startPos;
    if ( pos != desiredPos)
      std::cout << "FAILED STRIDE! " << std::hex << pos << " != " << desiredPos << std::endl;
    switch(it->type())
    {
    case TYPE_DATA_SECTION:
      current_node.add_child(name, ReadSection()); //yay recursion!
      break;
    case TYPE_STRING:
      current_node.add(name, fstr.getFixedString(var_size));
      break;
    case TYPE_INT:
      int tmp;
      switch (var_size)
      {
      case 4:
        tmp = fstr.get<int>();
        break;
      case 2:
        tmp = fstr.get<short>();
        break;
      case 1:
        tmp = fstr.get<char>();
        break;
      case 0:
        tmp = 0;
        break;
      default:
        throw std::exception("Unsupported int size!");
      }
      current_node.put(name, tmp);
      break;
    case TYPE_FLOAT:
      current_node.put(name, fstr.get<float>());
      break;
    case TYPE_BOOL:
      // false is encoded as 0, that is, no bytes at all
      current_node.put(name, (var_size != 0));
      fstr.Advance(var_size);
      break;
    case TYPE_BLOB:
      fstr.Advance(var_size); //TBD
      std::cout << "Unknown section TYPE_BLOB!" << std::endl;
      break;
    case TYPE_ENCRYPTED_BLOB:
      fstr.Advance(var_size); //TBD
      std::cout << "Unknown section TYPE_ENCRYPTED_BLOB!" << std::endl;
      break;
    default:
      throw std::exception("Unknown section!");
    }
  }
  return current_node;
}