/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/


#include "TotemRawDataLibrary/DataFormats/interface/OptoRxVFATFrameCollection.h"

#include <cstdio>

using namespace std;

//#define DEBUG

namespace Totem {

unsigned int OptoRxVFATFrameCollection::GOHBlock::size = 16;
unsigned int OptoRxVFATFrameCollection::OptoRxBlock::size = 12;

//----------------------------------------------------------------------------------------------------

OptoRxVFATFrameCollection::GOHBlock::GOHBlock() : valid(false), buffer(new VFATFrame[size]),
  dataPtrs(new VFATFrame::word* [size])
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::GOHBlock::GOHBlock\n");
#endif

  for (unsigned int i = 0; i < size; i++)
    dataPtrs[i] = (VFATFrame::word*) buffer[i].getData();
}

//----------------------------------------------------------------------------------------------------

OptoRxVFATFrameCollection::OptoRxVFATFrameCollection() 
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::OptoRxVFATFrameCollection, this = %p\n", (void *)this);
#endif
}

//----------------------------------------------------------------------------------------------------

OptoRxVFATFrameCollection::~OptoRxVFATFrameCollection()
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::~OptoRxVFATFrameCollection, this = %p\n", (void *)this);
#endif

  for (IteratorType it = data.begin(); it != data.end(); it++) {
    delete it->second;
  }

  data.clear();
}

//----------------------------------------------------------------------------------------------------

const VFATFrame* OptoRxVFATFrameCollection::GetFrameByID(unsigned int /*ID*/) const
{
  printf(">> ERROR: OptoRxVFATFrameCollection::GetFrameByID not implemented.\n");
  return NULL;
}

//----------------------------------------------------------------------------------------------------

const VFATFrame* OptoRxVFATFrameCollection::GetFrameByIndex(FramePosition index) const
{
  const unsigned int &FullOptoRxId = index.GetFullOptoRxId();
  const unsigned int &goh = index.GetGOHId();
  const unsigned int &fiber = index.GetIdxInFiber();

  MapType::const_iterator it = data.find(FullOptoRxId);
  return (it == data.end() || !it->second->buffer[goh].valid) ? NULL : &it->second->buffer[goh].buffer[fiber];
}

//----------------------------------------------------------------------------------------------------

unsigned int OptoRxVFATFrameCollection::Size() const
{
  unsigned int n = 0;
  
  for (MapType::const_iterator it = data.begin(); it != data.end(); ++it)
    for (unsigned int i = 0; i < OptoRxBlock::size; i++)
      if (it->second->buffer[i].valid)
        n++;

  return n * GOHBlock::size;
}

//----------------------------------------------------------------------------------------------------

void OptoRxVFATFrameCollection::Invalidate()
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::Invalidate, this = %p\n", this);
#endif

  for (MapType::iterator it = data.begin(); it != data.end(); ++it)
    for (unsigned int i = 0; i < OptoRxBlock::size; i++) {
      it->second->buffer[i].valid = false;
      for (unsigned int j = 0; j < GOHBlock::size; j++) {
        it->second->buffer[i].buffer[j] = VFATFrame();
      }
    }
}

//----------------------------------------------------------------------------------------------------

OptoRxVFATFrameCollection::IteratorType OptoRxVFATFrameCollection::InsertOptoRxBlock(unsigned int OptoRxId)
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::InsertOptoRxBlock(%u), this = %p\n", OptoRxId, this);
#endif

  IteratorType it = data.find(OptoRxId);
  if (it == data.end()) {
    MapType::value_type temp(OptoRxId, new OptoRxBlock());
    it = data.insert(temp).first;
  }
  
  return it;
}

//----------------------------------------------------------------------------------------------------

VFATFrame::word** OptoRxVFATFrameCollection::ValidateGOHBlock(const OptoRxVFATFrameCollection::IteratorType &it, unsigned int GOHId)
{ 
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::ValidateGOHBlock(%u, %u)\n", it->first, GOHId);
#endif

  it->second->buffer[GOHId].valid = true;
  return it->second->buffer[GOHId].dataPtrs;
}

//----------------------------------------------------------------------------------------------------

VFATFrameCollection::value_type OptoRxVFATFrameCollection::BeginIterator() const
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::BeginIterator, this = %p\n", this);
  //printf("\tbefore: %u, %u, %u\n", optoRxIterator->first, gohIterator, fiberIterator);
#endif

  MapType::const_iterator optoRxIterator = data.begin();
  unsigned int gohIterator = 0;
  unsigned int fiberIterator = 0;
  while (optoRxIterator != data.end() && !optoRxIterator->second->buffer[gohIterator].valid) {
    gohIterator++;
    if (gohIterator >= OptoRxBlock::size) {
      gohIterator = 0;
      optoRxIterator++;
    }
  }

  return (optoRxIterator == data.end()) ? value_type(FramePosition(), NULL) :
    value_type(FramePosition(0, 0, optoRxIterator->first, gohIterator, fiberIterator), optoRxIterator->second->buffer[gohIterator].buffer);
}

//----------------------------------------------------------------------------------------------------

VFATFrameCollection::value_type OptoRxVFATFrameCollection::NextIterator(const value_type &value) const
{
#ifdef DEBUG
  printf(">> OptoRxVFATFrameCollection::NextIterator, this = %p\n", this);
  //printf("\tbefore: %u, %u, %u\n", optoRxIterator->first, gohIterator, fiberIterator);
#endif
  
  if (!value.second)
    return value;

  MapType::const_iterator optoRxIterator = data.find(value.first.GetFullOptoRxId());
  unsigned int gohIterator = value.first.GetGOHId();
  unsigned int fiberIterator = value.first.GetIdxInFiber();

  fiberIterator++;
  if (fiberIterator >= GOHBlock::size) {
    fiberIterator = 0;
    do {
      gohIterator++;
      if (gohIterator >= OptoRxBlock::size) {
        gohIterator = 0;
        optoRxIterator++;
      }
    } while (optoRxIterator != data.end() && !optoRxIterator->second->buffer[gohIterator].valid);
  }

  return (optoRxIterator == data.end()) ? value_type(FramePosition(), NULL) :
    value_type(FramePosition(0, 0, optoRxIterator->first, gohIterator, fiberIterator), 
        &optoRxIterator->second->buffer[gohIterator].buffer[fiberIterator]);
}

//----------------------------------------------------------------------------------------------------

bool OptoRxVFATFrameCollection::IsEndIterator(const value_type &value) const
{
  return (value.second == NULL);
}

} // namespace
