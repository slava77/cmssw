/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#ifndef _Totem_DataFile_h_
#define _Totem_DataFile_h_

#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

#include <string>


namespace Totem {

class VFATFrameCollection;

/**
 * Generic (and abstract) class for reading a data file/stream of certain type.
**/
class DataFile 
{
  public:
    /// maximum of event number in datafile (0 if event numbering not available in data file)
    unsigned long dataEventNumberMax;

    /// maximum of configuration number in datafile
    unsigned long dataConfNumberMax;

    DataFile() : dataEventNumberMax(0), dataConfNumberMax(0), indexStatus(isNotIndexed) {}

    virtual ~DataFile() {}

    /// to open a file
    enum OpenStatus { osOK, osCannotOpen, osWrongFormat };
    virtual OpenStatus Open(const std::string&) = 0;
    virtual OpenStatus Open(StorageFile *file) = 0;

    /// to close a file
    virtual void Close() = 0;
    
    /// creates (instantiates) an adequate VFAT frame collection
    virtual VFATFrameCollection* CreateCollection() const = 0;

    /// creates (instantiates) event with an adequate VFAT frame collection
    virtual RawEvent* CreateEvent() const
      { return new RawEvent(CreateCollection()); }

    /// returns whether the collection instance passed as parameter is compatible with the file reader
    virtual bool IsCollectionCompatible(VFATFrameCollection *) const = 0;
    
    /// retrieves the next event, returns zero if successful 
    virtual unsigned char GetNextEvent(RawEvent *) = 0;

    /// retrieves the event at given position (numbered from 0), must be indexed before
    virtual unsigned char GetEvent(unsigned long, RawEvent *) = 0;

    /// rewinds back to the beginning of the file
    virtual void Rewind();
    
    /// start indexing via successive calls of GetNextEvent() up to the very end of the file
    virtual void StartIndexing() = 0;

    /// sets the state to indexed
    virtual void StopIndexing() = 0;
    
    /// returns the number events (total or read so far)
    virtual unsigned long GetEventsCount() const = 0;

    /// returns the number of corrupted events (total or read so far)
    virtual unsigned long GetEventsCorrupted() const = 0;

    /// returns class name
    virtual std::string GetClassName() const                                
      { return "DataFile"; }
    
    /// returns true if random event can be accessed (false for streams for example)
    virtual bool RandomAccessSupported() const = 0;

    /**
	 * Tries to open the file/stream with an appropriate reader.
	 * The match between input and reader class is tested one by one, in the following
	 * order and using the given criteria:
	 *   - VMEAStream: tries to open a dedicated data stream (DAQA library call)
	 *   - VMEBFile: file name ends with .vmeb
	 *   - VMEAFile: file name ends with .vmea
	 *   - VME2File: file name ends with .vme2
	 *   - VMEFile: file name ends with .vme
	 *   - MultiSlinkFile: checks first 8 bytes for a signature
	 *   - TTPFile: checks first 6 bytes for a signature
	 *   - SlinkFile: no data-format compatibility check available, accepts everything
	 *         (should be tested last)
     **/
    static DataFile* OpenStandard(const std::string&);

  protected:
     enum IndexStatus {isNotIndexed, isIndexing, isIndexed} indexStatus;
};

} // namespace

#endif
