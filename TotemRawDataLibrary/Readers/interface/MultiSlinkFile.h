/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*    
****************************************************************************/

#ifndef _Totem_MultiSlinkFile_h_
#define _Totem_MultiSlinkFile_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

#include <vector>
#include <stdio.h>

// TODO: make those static const data members ??
#define NUM_OF_SLINKFRAMES 3
#define NUM_OF_VFATS_PER_FRAME 64
#define VFAT_FRAME_BIT_SIZE 192
#define WORD_BIT_SIZE 16
#define NUM_OF_WORDS VFAT_FRAME_BIT_SIZE/WORD_BIT_SIZE 

namespace Totem {

/**
 * TotemRawDataLibrary
 * Reads a Slink data file.
**/
class MultiSlinkFile : public DataFile
{
  public:
    MultiSlinkFile();
    ~MultiSlinkFile();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);
    virtual void Close(); 
    
    virtual VFATFrameCollection* CreateCollection() const
      { return new SimpleVFATFrameCollection; }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
      { return (c->GetClassName().compare("SimpleVFATFrameCollection") == 0); }
    
    virtual void Rewind();
    virtual unsigned char GetNextEvent(RawEvent*);
    virtual unsigned char GetEvent(unsigned long, RawEvent*);

    virtual unsigned long GetEventsCount() const
      {  return eventCounter; }

    virtual unsigned long GetEventsCorrupted() const
      { return corrNum; }

    virtual void StartIndexing();
    
    void StopIndexing()
      { indexStatus = isIndexed; }

    virtual std::string GetClassName() const                                
      { return "MultiSlinkFile"; }

    virtual bool RandomAccessSupported() const
      { return true; }

  protected:
    /// structure holding a value and a mask, supports comparison to another value (when mask is applied)
    struct sequence
    {
      unsigned long long value, mask;
      sequence(unsigned long long v = 0, unsigned long long m = 0xffffffffffffffffULL) : value(v & m), mask(m) {}

      /// returns true only if v is equal to value when mask is applied
      bool compare(unsigned long long v) const
      {
        if ((v & mask) == value) return true;
        return false;
      }
    };

    /// sequences Begin-Of-Frame, End-Of-Frame
    std::vector <sequence> seqBOF, seqEOF;  
    
    /// TODO: describe if needed
    char Read(unsigned long long&);

    /// Compares the number to numbers in the vector.
    /// returns the index of matched number or -1 if not found
    signed int CheckSequence(unsigned long long, const std::vector<sequence> &);

    /// Searches for the next BOF sequence (of type frametype).
    /// returns 0 if found, non-zero in case of errors
    char SeekNextFrame();
    
    /// loads an iframe-th data frame to an event
    char LoadFrame(int iframe, SimpleVFATFrameCollection*);
    
    /// loads all frames to an event (via LoadFrame)
    std::vector<int> LoadFrames(SimpleVFATFrameCollection*);
    
    /// Stores the frame type (0,1,2)
    int activeFrame;

    /// Stores whether active frame is the beginning of an event
    bool isStartFrame;

    /// Stores loaded event number
    unsigned long eventCounter;

    /// file handle
    StorageFile *file;

    /// positions of frame beginnings (with type) in the file, needs to be indexed
    std::vector<int> positions;

    /// number of corrupted frame
    unsigned int corrNum;

    /// if feof is reached, prints the message and return non-zero, otherwise 0 returned
    unsigned char LoadFrameCheckFEOF(const char *);

    char* filename;
};

//----------------------------------------------------------------------------------------------------

inline void MultiSlinkFile::Rewind()
{
  file->Seek(0);
  positions.clear();
  corrNum = 0;
  eventCounter = 0;
  activeFrame = -1;
  isStartFrame = false;
}

//----------------------------------------------------------------------------------------------------

inline void MultiSlinkFile::StartIndexing()
{
  indexStatus = isIndexing;
  positions.clear();
}

} // namespace
#endif
