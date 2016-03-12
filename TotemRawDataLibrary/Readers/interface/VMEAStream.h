/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef _Totem_VMEAStream_h_
#define _Totem_VMEAStream_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/OptoRxVFATFrameCollection.h"

#include <vector>
#include <stdio.h>

namespace Totem {

/**
 * Reads a data stream in VMEA format, using the DAQA monitoring library.
**/
class VMEAStream : public DataFile
{
  public:
    typedef unsigned long long word;

    VMEAStream();
    ~VMEAStream();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);
    virtual void Close();
    
    virtual VFATFrameCollection* CreateCollection() const
      { return new OptoRxVFATFrameCollection; }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
      { return (c->GetClassName().compare("OptoRxVFATFrameCollection") == 0); }

    /// random event access not supported by this class
    void Rewind();

    unsigned char GetNextEvent(RawEvent*);
    
    /// random event access not supported by this class
    unsigned char GetEvent(unsigned long, RawEvent*);

    unsigned long GetEventsCount() const
      { return eventCounter; }

    unsigned long GetEventsCorrupted() const
      { return corruptedEventCounter; }

    void StartIndexing() {}
    
    void StopIndexing() {}
    
    /// random event access not supported by this class
    bool RandomAccessSupported() const
      { return false; }

    virtual std::string GetClassName() const                                
      { return "VMEAStream"; }
    
  private:
    /// number of events read
    unsigned int eventCounter;

    /// number of corrupted events
    unsigned int corruptedEventCounter;
};


}
#endif
