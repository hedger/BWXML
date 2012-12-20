#include <iostream>
#include <fstream>

#include <string>

class StreamReader
{
  std::ifstream input;

public:
  StreamReader(const std::string& fname);

  template<typename T>
  T get()
  {
    T buf;
    input.read(reinterpret_cast<char*>(&buf), sizeof(buf));
    return buf;
  }

  std::string get();
  std::string getFixedString(int len);
  size_t Pos();
  size_t Size();
  void Advance(size_t offset);
};