/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Mate Csanad (mate.csanad@cern.ch) 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*    
****************************************************************************/


#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"

//#define DEBUG 1

using namespace std;

namespace Totem {

SimpleVFATFrameCollection::SimpleVFATFrameCollection() :
  data()
{
#ifdef DEBUG
  printf(">> SimpleVFATFrameCollection, this = %p\n", this);
  printf("\tdata = %p, data.size = %u\n", &data, data.size());
#endif
}

//----------------------------------------------------------------------------------------------------

SimpleVFATFrameCollection::~SimpleVFATFrameCollection()
{
#ifdef DEBUG
  printf(">> ~SimpleVFATFrameCollection, this = %p\n", this);
#endif
  data.clear();
}

//----------------------------------------------------------------------------------------------------

const VFATFrame* SimpleVFATFrameCollection::GetFrameByID(unsigned int ID) const
{
  // first convert ID to 12bit form
  ID = ID & 0xFFF;

  for (MapType::const_iterator it = data.begin(); it != data.end(); ++it)
    if (it->second.getChipID() == ID)
      if (it->second.checkFootprint() && it->second.checkCRC())
        return &(it->second);

  return NULL;
}

//----------------------------------------------------------------------------------------------------

const VFATFrame* SimpleVFATFrameCollection::GetFrameByIndex(FramePosition index) const
{
  MapType::const_iterator it = data.find(index);
  if (it != data.end())
    return &(it->second);
  else
    return NULL;
}

//----------------------------------------------------------------------------------------------------

VFATFrameCollection::value_type SimpleVFATFrameCollection::BeginIterator() const
{
  MapType::const_iterator it = data.begin();
  return (it == data.end()) ? value_type(FramePosition(), NULL) : value_type(it->first, &it->second);
}

//----------------------------------------------------------------------------------------------------

VFATFrameCollection::value_type SimpleVFATFrameCollection::NextIterator(const value_type &value) const
{
  if (!value.second)
    return value;

  MapType::const_iterator it = data.find(value.first);
  it++;

  return (it == data.end()) ? value_type(FramePosition(), NULL) : value_type(it->first, &it->second);
}

//----------------------------------------------------------------------------------------------------

bool SimpleVFATFrameCollection::IsEndIterator(const value_type &value) const
{
  return (value.second == NULL);
}

} // namespace
