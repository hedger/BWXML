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

#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <sstream>

namespace BWPack
{
	namespace IO
	{
		class StreamReader
		{
			static const int MAX_STRING_LEN = 255;
			std::ifstream mInput;

		public:
			StreamReader(const std::string& fname);
			~StreamReader();

			template<typename T>
			T get()
			{
				T buf;
				mInput.read(reinterpret_cast<char*>(&buf), sizeof(buf));
				return buf;
			}

			std::string getString(size_t len);
			std::string getNullTerminatedString();
		};

		class StreamBufWriter
		{
			std::ostream mOutput;

		public:
			StreamBufWriter(std::stringbuf* buf) : mOutput(buf) {};

			template<typename T>
			void put(const T& buf)
			{
				mOutput.write(reinterpret_cast<const char*>(&buf), sizeof(buf));
			}

			void putString(const std::string& str, bool nullTerminate=true);
		};
	}
}
