
namespace PackedSectionData
{
  typedef unsigned char VersionType;
  typedef unsigned short NumChildrenType;
  typedef int DataPosType;
  typedef short KeyPosType;
}

enum SectionType
{
  DATA_POS_MASK = 0x0fffffff,
  TYPE_MASK     = ~DATA_POS_MASK,

  TYPE_DATA_SECTION	= 0x00000000,
  TYPE_STRING			= 0x10000000,
  TYPE_INT			= 0x20000000,
  TYPE_FLOAT			= 0x30000000, // Used for Vector[234] and Matrix too.
  TYPE_BOOL			= 0x40000000,
  TYPE_BLOB			= 0x50000000,
  TYPE_ENCRYPTED_BLOB	= 0x60000000,

  // Not used.
  TYPE_UNUSED_2		= 0x70000000,
};

#pragma pack( push, 1 )
class ChildRecord
{
  PackedSectionData::DataPosType dataPos_;
  PackedSectionData::KeyPosType keyPos_;
public:
  PackedSectionData::KeyPosType keyPos() { return keyPos_; };
  PackedSectionData::DataPosType startPos() const	{ return dataPos_ & DATA_POS_MASK; }
  PackedSectionData::DataPosType endPos() const		{ return (this + 1)->startPos(); }
  SectionType type() const
  { return SectionType((this + 1)->dataPos_ & TYPE_MASK); }
  static ChildRecord Dummy(PackedSectionData::DataPosType fakeDataPos)
  {
    ChildRecord dummy;
    dummy.dataPos_ = fakeDataPos;
    dummy.keyPos_ = -1;
    return dummy;
  }
};
static_assert(sizeof(ChildRecord) == 6, "Wrong sizeof ChildRecord, check your compiler settings!");
#pragma pack( pop )