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