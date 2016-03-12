/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors:
*   Mate Csanad (Mate.Csanad@cern.ch)
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef _Totem_VMEFile_h_
#define _Totem_VMEFile_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/Readers/interface/CircularBuffer.h"
#include "TotemRawDataLibrary/DataFormats/interface/OptoRxVFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

class StorageFile;

#include <vector>
#include <stdio.h>

namespace Totem {

/**
 * Reads a VME file.
**/
class VMEFile : public DataFile
{
  public:
    static unsigned int maxFrames;
    static unsigned int vfatsPerFrame;

    typedef unsigned int word;

    VMEFile();
    ~VMEFile();

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
      { return "VMEFile"; }
    
    virtual bool RandomAccessSupported() const
      { return true; }

  protected:
    /// circular buffer of words
    CircularBuffer<word> *buf;
    
    /// Stores event positions in file.
    /// status of indexing of event positions
    std::vector<CircularBuffer<word>::position_type> positions;
                                      
    /// number of corrupted frames
    unsigned int corruptedEventCounter;
    
    /// check if matching header and footer
    bool MatchingHeaderFooter(word head, word foot);
};

//----------------------------------------------------------------------------------------------------

inline void VMEFile::Rewind()
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

inline void VMEFile::StartIndexing()
{
  positions.clear();
  indexStatus = isIndexing;
}

} // namespace
#endif
