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
	if (len)
	{
		auto buf = getBuffer(len);
		buf.push_back('\0');
		return std::string(buf.data());
	}
	else
	{
		char buf[255] = { 0 };
		mInput.getline(buf, 255, '\0');
		return std::string(buf);
	}
}

StreamReader::DataBuffer StreamReader::getBuffer(size_t len)
{
	DataBuffer buf(len);
	if (len)
		mInput.read(reinterpret_cast<char*>(buf.data()), len);
	return buf;
}

void StreamReader::Advance(size_t offset)
{
  mInput.seekg(offset, std::ios_base::cur);
}

//////////////////////////////////////////////////////////////////////////

StreamWriter::StreamWriter(const std::string& fname)
{
	mOutput.open(fname, std::ios::binary);
	if (!mOutput.is_open())
		throw std::exception("Can't open the file");
}

void StreamWriter::putString(const std::string& str, bool nullTerminate/* =true */)
{
	DataBuffer strBuf;
	std::copy(str.begin(), str.end(), std::back_inserter(strBuf));
	if (nullTerminate)
		strBuf.push_back('\0');
	putBuffer(strBuf);
}

void StreamWriter::putBuffer(DataBuffer buf)
{
	mOutput.write(buf.data(), buf.size());
}

StreamWriter::~StreamWriter()
{
	mOutput.close();
};

