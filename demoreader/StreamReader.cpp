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
  ++len; // get() reads (length-1) chars
  std::vector<char> buf(len);
  input.get(reinterpret_cast<char*>(buf.data()), len, '\0');
  return std::string(buf.data());
}

size_t StreamReader::Pos()
{
  return int(input.tellg());
}

size_t StreamReader::Size()
{
  size_t ret, oldpos = input.tellg();
  input.seekg(0, std::ios_base::end);
  ret = input.tellg();
  input.seekg(oldpos);
  return ret;
}

void StreamReader::Advance(size_t offset)
{
  input.seekg(offset, std::ios_base::cur);
}