/****************************************************************************
*
* This is a part of theTOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kaspar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#ifndef _Totem_TTPFile_h_
#define _Totem_TTPFile_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"


namespace Totem {


/**
 * Reads TTP data file.
**/
class TTPFile : public DataFile
{
  public:
    TTPFile();
    virtual ~TTPFile();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);
    virtual void Close(); 
    
    virtual VFATFrameCollection* CreateCollection() const
      { return new SimpleVFATFrameCollection; }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
      { return (c->GetClassName().compare("SimpleVFATFrameCollection") == 0); }

    virtual unsigned char GetNextEvent(RawEvent *);
    virtual unsigned char GetEvent(unsigned long, RawEvent *);
    virtual void Rewind();

    virtual void StartIndexing();
    
    void StopIndexing()
      { indexStatus = isIndexed; }

    virtual unsigned long GetEventsCount() const
      { return positions.size(); }

    virtual unsigned long GetEventsCorrupted() const
      { return 0; }      

    virtual std::string GetClassName() const                                
      { return "TTPFile"; }
    
    virtual bool RandomAccessSupported() const
      { return true; }


  protected:
    /// the file object
    StorageFile *file;

    /// array of indices of event beginnings in the file 
    std::vector<int> positions;
    
    /// reads one event from the file
    unsigned char ReadOneEvent(RawEvent *);
   
    unsigned int Reopen();

    char *filename;
};

} // namespace

#endif
