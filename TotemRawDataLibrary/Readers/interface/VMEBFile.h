/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*    
****************************************************************************/

#ifndef _Totem_VMEBFile_h_
#define _Totem_VMEBFile_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/OptoRxVFATFrameCollection.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

#include <vector>

namespace Totem {

/**
 * Reads a file in VMEB format, without using the DAQA monitoring library.
 **/ 
class VMEBFile : public DataFile
{
public:
    /// standard equipment types
    enum { etOptoRxOld = 100, etLoneG = 110, etOptoRx = 120 };

    typedef unsigned long long word;

    VMEBFile();
    virtual ~VMEBFile();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);

    virtual void Close();
    virtual void Rewind();
    virtual unsigned char GetEvent(unsigned long, RawEvent*);
    virtual unsigned char GetNextEvent(RawEvent*);

    virtual VFATFrameCollection* CreateCollection() const
	{
        return new OptoRxVFATFrameCollection;
    }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
	{
        return (c->GetClassName().compare("OptoRxVFATFrameCollection") == 0);
    }

    virtual std::string GetClassName() const
	{
        return "VMEBFile";
    }

    virtual unsigned long GetEventsCount() const
    {
        return positions.size();
    }

    virtual unsigned long GetEventsCorrupted() const
    {
        return corruptedEventCounter;
    }

    virtual void StartIndexing()
    {
        positions.clear();
        indexStatus = isIndexing;
    }

    virtual void StopIndexing()
    {
        indexStatus = isIndexed;
    }

    virtual bool RandomAccessSupported() const
    {
        return true;
    }

    /// Processes a VMEB Event.
    /// returns the number of GOH blocks that failed consistency checks
    static unsigned int ProcessVMEBEvent(char *ptr, OptoRxVFATFrameCollection *, RawEvent *);

protected:
    static const unsigned int eventHeaderSize;

    /// Process one LDC event.
    /// returns the number of GOH blocks that failed consistency checks
    static unsigned int ProcessSubEvent(char *ptr, OptoRxVFATFrameCollection *, RawEvent *);



    /// reads 'bytesToRead' bytes from the file to buffer, starting at the given offset
    virtual unsigned char ReadToBuffer(unsigned int bytesToRead, unsigned int offset);
    
	/// processes a LoneG frame (trigger data)
    static unsigned int ProcessLoneGFrame(word *ptr, signed long size, RawEvent *);

    /// data pointer, to be allocated one time only
    char *dataPtr;
 
 	/// data buffer size
    unsigned int dataPtrSize;
 
 	/// stores event positions in file
    std::vector<std::streampos> positions;
 
 	/// input file pointer
 	StorageFile *infile;

	/// number of corrupted frames
    unsigned int corruptedEventCounter;
   
   	/// name of opened file
    std::string filename;
};

}
#endif
