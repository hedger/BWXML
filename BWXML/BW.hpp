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

#include <cassert>

namespace BigWorld
{
	static const int PACKED_SECTION_MAGIC = 0x62a14e45;

	enum PackedDataType
	{
		BW_Section  = 0x0,
		BW_String   = 0x1,
		BW_Int      = 0x2,
		BW_Float    = 0x3,
		BW_Bool     = 0x4,
		BW_Blob     = 0x5,
		BW_Enc_blob = 0x6,
		BW_Reserved = 0x7
	};

#pragma pack(push, 1)
	// FIXME: use union?
	class DataDescriptor
	{
		unsigned int __raw;
	public:
		DataDescriptor() : __raw(0)	{};
		DataDescriptor(PackedDataType type, unsigned int offset)
		{
			assert(offset < 0x0FFFFFFF);
			__raw = ((static_cast<unsigned int>(type) << 28) | offset);
		}

		PackedDataType typeId() const {return static_cast<PackedDataType>(__raw >> 28); };
		unsigned int offset() const {return __raw & 0x0FFFFFFF; };
	};
	static_assert(sizeof(DataDescriptor) == 4, "Wrong sizeof DataDescriptor, check your compiler settings!");

	struct DataNode
	{
		unsigned short nameIdx;
		DataDescriptor data;
	};

	static_assert(sizeof(DataNode) == 6, "Wrong sizeof DataNode, check your compiler settings!");
#pragma pack(pop)
}