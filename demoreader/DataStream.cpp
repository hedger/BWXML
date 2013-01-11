#include "DataStream.h"

#include <vector>
#include <iterator>

StreamReader::StreamReader(const std::string& fname)
{
  mInput.open(fname, std::ios::binary);
  if (!mInput.is_open())
    throw std::exception("File not found");
}

StreamReader::~StreamReader()
{
	mInput.close();
}

std::string StreamReader::getString(int len)
{
  std::string ret;
	if (len)
	{
    ret.reserve(len);
    std::vector<char> buf(len);
    mInput.read(reinterpret_cast<char*>(buf.data()), len);
    std::copy(buf.begin(), buf.end(), std::back_inserter(ret));
	}
  return ret;
}

std::string StreamReader::getNullTerminatedString()
{
  char buf[255] = { 0 };
  mInput.getline(buf, 255, '\0');
  return std::string(buf);
}

void StreamReader::Advance(size_t offset)
{
  mInput.seekg(offset, std::ios_base::cur);
}

//////////////////////////////////////////////////////////////////////////

void StreamBufWriter::putString(const std::string& str, bool nullTerminate/* =true */)
{
	mOutput << str;
	if (nullTerminate)
		mOutput << '\0';
}
