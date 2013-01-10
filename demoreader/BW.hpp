#pragma once

enum BW_DataType
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
struct DataDescriptor
{
	//BW_DataType typeId:8;
	//int offset:24;
	int __raw;
	BW_DataType typeId() {return static_cast<BW_DataType>(__raw >> 28); };
	int offset() {return __raw & 0x0FFFFFFF; };
};
static_assert(sizeof(DataDescriptor) == 4, "Wrong sizeof DataDescriptor, check your compiler settings!");

struct DataNode
{
	short nameIdx;
	DataDescriptor data;
};

static_assert(sizeof(DataNode) == 6, "Wrong sizeof DataNode, check your compiler settings!");
#pragma pack(pop)