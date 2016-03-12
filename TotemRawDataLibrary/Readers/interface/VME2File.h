/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef _Totem_VME2File_h_
#define _Totem_VME2File_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/Readers/interface/CircularBuffer.h"
#include "TotemRawDataLibrary/DataFormats/interface/OptoRxVFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"

#include <vector>
#include <stdio.h>

//#define DEBUG

namespace Totem {

/**
 * Reads the "new" VME file format.
**/

class VME2File : public DataFile
{
  public:
    /// 1 word = 64 bits
    typedef unsigned long long word;

    VME2File();
    ~VME2File();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);
    virtual void Close();
    
    virtual VFATFrameCollection* CreateCollection() const
      { return new OptoRxVFATFrameCollection; }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
      { return (c->GetClassName().compare("OptoRxVFATFrameCollection") == 0); }

    void Rewind();
    unsigned char GetNextEvent(RawEvent*);
    unsigned char GetEvent(unsigned long, RawEvent*);

    unsigned long GetEventsCount() const
      { return positions.size(); }

    unsigned long GetEventsCorrupted() const
      { return corruptedEventCounter; }

    void StartIndexing(); 
    
    void StopIndexing()
      { indexStatus = isIndexed; }

    virtual std::string GetClassName() const                                
      { return "VME2File"; }
    
    virtual bool RandomAccessSupported() const
      { return true; }

    /// Extract VFAT data from an OptoRx frame and saves them to the collection.
    /// BufferType can be any type that has [] operator (array, CircularBuffer, ...)
    /// returns the number of cells that failed consistency checks
    /// \param buf is the OptoRx frame buffer, including the header and footer
    /// \param frameSize is the size (in words) of the OptoRx frame including the header and footer
    template <class BufferType>
    static unsigned int ProcessOptoRxFrame(const BufferType& buf, unsigned int frameSize,
      OptoRxVFATFrameCollection *, RawEvent *);

  protected:
    /// circular buffer of words
    CircularBuffer<word> *buf;
    
    /// Stores event positions in file.
    /// status of indexing of event positions
    std::vector<CircularBuffer<word>::position_type> positions;
                                                               
    /// the number of corrupted events
    unsigned int corruptedEventCounter;
};

//----------------------------------------------------------------------------------------------------

inline void VME2File::Rewind()
{
  if (buf) {
    buf->Rewind();
    buf->Reset();
  }

  corruptedEventCounter = 0;
  positions.clear();
  indexStatus = isNotIndexed;
}

//----------------------------------------------------------------------------------------------------

inline void VME2File::StartIndexing()
{
  positions.clear();
  indexStatus = isIndexing;
}

//----------------------------------------------------------------------------------------------------

template <class BufferType>
unsigned int VME2File::ProcessOptoRxFrame(const BufferType& buf, unsigned int frameSize,
  OptoRxVFATFrameCollection *fc, RawEvent *event)
{
  using namespace std;

  // get (full) OptoRxId
  unsigned long long head = buf[0];
  unsigned int OptoRxId = (head >> 8) & 0xFFF;
  unsigned long BX = (head >> 20) & 0xFFF;
  unsigned long LV1 = (head >> 32) & 0xFFFFFF;

  // get OptoRx metadata
  if (event) {
    OptoRxMetaData &md = event->optoRxMetaData[OptoRxId];
    md.BX = BX;
    md.LV1 = LV1;
  }

  OptoRxVFATFrameCollection::IteratorType block = fc->InsertOptoRxBlock(OptoRxId);
  unsigned int subFrames = (frameSize - 2) / 194;

#ifdef DEBUG
  printf(">> VME2File::ProcessOptoRxFrame > OptoRxId = %u, BX = %lu, LV1 = %lu, frameSize = %u, subFrames = %u)\n",
    OptoRxId, BX, LV1, frameSize, subFrames);
#endif

  unsigned int errorCounter = 0; 

  // process all sub-frames
  for (unsigned int r = 0; r < subFrames; ++r) {
    for (unsigned int c = 0; c < 4; ++c) {
      unsigned int head = (buf[  1 + 194*r] >> (16*c)) & 0xFFFF;
      unsigned int foot = (buf[194 + 194*r] >> (16*c)) & 0xFFFF;

#ifdef DEBUG
      printf(">>>> r = %i, c = %i: S = %i, BOF = %i, EOF = %i, ID = %i, ID' = %i\n", r, c, head & 0x1, head >> 12, foot >> 12, (head >> 8) & 0xF, (foot >> 8) & 0xF);
#endif      

      // stop if this GOH is NOT active
      if ((head & 0x1) == 0)
        continue;

#ifdef DEBUG
      printf("\tHeader active (%04x -> %x).\n", head, head & 0x1);
#endif      

      // check structure
      if (head >> 12 != 0x4 || foot >> 12 != 0xB ||((head >> 8) & 0xF) != ((foot >> 8) & 0xF)) {
        //stringstream ss;
	char ss[200];
        if (head >> 12 != 0x4) sprintf(ss, "\n\tHeader is not 0x4 as expected (%x).", head);
	//ss << "\n\tHeader is not 0x4 as expected (" << hex << head << dec << ").";
        if (foot >> 12 != 0xB) sprintf(ss, "\n\tFooter is not 0xB as expected (%x).", foot);
	//ss << "\n\tFooter is not 0xB as expected (" << hex << foot << dec << ").";
        if (((head >> 8) & 0xF) != ((foot >> 8) & 0xF)) 
	  sprintf(ss, "\n\tIncompatible GOH IDs in header (%x) and footer (%x).", ((head >> 8) & 0xF), ((foot >> 8) & 0xF));
	//ss << "\n\tIncompatible GOH IDs in header ("
	//<< hex << ((head >> 8) & 0xF) << ") and footer ("
	//<< ((foot >> 8) & 0xF) << dec << ").";

        ERROR("VME2File::ProcessOptoRxFrame") << "Wrong payload structure (in GOH block row " << r << " and column " << c
          << ") in OptoRx frame ID " << OptoRxId << ". GOH block omitted." << ss << c_endl;

        errorCounter++;
        continue;
      }

      unsigned int goh = (head >> 8) & 0xF;
      VFATFrame::word** dataPtrs = fc->ValidateGOHBlock(block, goh);

#ifdef DEBUG
      printf(">>>> transposing GOH block at prefix: %i, dataPtrs = %p\n", OptoRxId*192 + goh*16, dataPtrs);
#endif      

      // deserialization
      for (int i = 0; i < 192; i++) {
        int iword = 11 - i/16;  // number of current word (11...0)
        int ibit = 15 - i%16;   // number of current bit (15...0)
        unsigned int w = ( buf[i + 2 + 194*r] >> (16*c) ) & 0xFFFF;

        // Fill the current bit of the current word of all VFAT frames
        for (int idx = 0; idx < 16; idx++)
          if (w & (1 << idx))
            dataPtrs[idx][iword] |= (1 << ibit);
      }
    }
  }

  return errorCounter;
}

} // namespace
#endif
