#include "Base64.h"

#include "boost/archive/iterators/base64_from_binary.hpp"
#include "boost/archive/iterators/binary_from_base64.hpp"
#include <boost/archive/iterators/transform_width.hpp>

//#include <sstream>
#include <algorithm>
#include <iterator>

namespace B64
{
  using namespace boost::archive::iterators;

  typedef
    base64_from_binary<
    transform_width<std::string::const_iterator, 6, 8>
    > base64_t;

  typedef
    transform_width<
    binary_from_base64<std::string::const_iterator>, 8, 6
    > binary_t;


  std::string Encode(std::string src)
  {
    for (int bytesToPad = src.length() % 3; bytesToPad != 0; --bytesToPad)
      src.push_back('='); // fuck you, boost!

    return std::string(base64_t(src.begin()), base64_t(src.end()));
  }

  std::string Decode(std::string src)
  {
    src.push_back(0); // fuck you, boost

    return std::string(binary_t(src.begin()), binary_t(src.end()));
  }

  bool Is(std::string src)
  {
    try
    {
      std::string tmp = Decode(src);
      return (!Encode(tmp).compare(src));
    }
    catch (...)
    {
    	return false;
    }
  }
}