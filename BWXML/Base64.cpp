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

#include "Base64.h"

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

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
		while ((src.length()) % 4 != 0) // fuck you, boost
			src.push_back(0);
		return std::string(binary_t(src.begin()), binary_t(src.end()));
	}

	bool Is(std::string src)
	{
		try
		{
			std::string tmp = Decode(src);
			bool ret = !Encode(tmp).compare(src);
			return (ret);
		}
		catch (...) // (const boost::archive::iterators::dataflow_exception&)
		{
			return false;
		}
	}
}