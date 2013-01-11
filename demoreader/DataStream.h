#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

class StreamReader
{
	std::ifstream mInput;

public:
	typedef std::vector<char> DataBuffer;
	StreamReader(const std::string& fname);
	~StreamReader();

	template<typename T>
	T get()
	{
		T buf;
		mInput.read(reinterpret_cast<char*>(&buf), sizeof(buf));
		return buf;
	}

	std::string getString(int len=0);
	DataBuffer getBuffer(size_t len);
	void Advance(size_t offset);
};

class StreamWriter
{
	std::ofstream mOutput;

public:
	typedef std::vector<char> DataBuffer;
	StreamWriter(const std::string& fname);
	~StreamWriter();

	template<typename T>
	void put(const T& buf)
	{
		mOutput.write(reinterpret_cast<const char*>(&buf), sizeof(buf));
	}

	void putString(const std::string& str, bool nullTerminate=true);
	void putBuffer(DataBuffer buf);
};

