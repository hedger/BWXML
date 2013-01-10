#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

class StreamReader
{
  std::ifstream input;

public:
	typedef std::vector<char> DataBuffer;
  StreamReader(const std::string& fname);

  template<typename T>
  T get()
  {
    T buf;
    input.read(reinterpret_cast<char*>(&buf), sizeof(buf));
    return buf;
  }

  std::string get();
	DataBuffer getBuffer(size_t len);
  std::string getFixedString(int len);
  std::streamoff Pos();
  std::streamoff Size();
  void Advance(size_t offset);
};