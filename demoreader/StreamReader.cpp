#include "StreamReader.h"

#include <vector>

StreamReader::StreamReader(const std::string& fname)
{
  input.open(fname, std::ios::binary );
  if (!input.is_open())
    throw std::exception("File not found");
}

std::string StreamReader::get()
{
  char buf[255] = { 0 };
  input.getline(buf, 255, '\0');
  return std::string(buf);
}

std::string StreamReader::getFixedString(int len)
{
	auto buf = getBuffer(len);
	buf.push_back('\0');
  return std::string(buf.data());
}

StreamReader::DataBuffer StreamReader::getBuffer(size_t len)
{
	DataBuffer buf(len);
	if (len)
		input.read(reinterpret_cast<char*>(buf.data()), len);
	return buf;
}

std::streamoff StreamReader::Pos()
{
  return input.tellg();
}

std::streamoff StreamReader::Size()
{
  std::streamoff ret, oldpos = input.tellg();
  input.seekg(0, std::ios_base::end);
  ret = input.tellg();
  input.seekg(oldpos);
  return ret;
}

void StreamReader::Advance(size_t offset)
{
  input.seekg(offset, std::ios_base::cur);
}