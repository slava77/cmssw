/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors:
*  Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef _Totem_SRSFile_h_
#define _Totem_SRSFile_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

#include <vector>

namespace Totem {

/**
 * Reads a file in SRS format.
 **/ 
class SRSFile : public DataFile
{
public:
    /// standard equipment types
    enum { etOptoRxVME = 120, etOptoRxSRS = 22 };

    /// VFAT transmission modes
    enum { vmCluster = 0x80, vmRaw = 0x90 };

    /// 1 word = 64 bits = 8 bytes
    /// NOTE: it would be better to use uint64_t, but this requires C++11
    typedef unsigned long long word;

    SRSFile();
    virtual ~SRSFile();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);

    virtual void Close();
    virtual void Rewind();
    virtual unsigned char GetEvent(unsigned long, RawEvent*);
    virtual unsigned char GetNextEvent(RawEvent*);

    virtual VFATFrameCollection* CreateCollection() const
	{
        return new SimpleVFATFrameCollection;
    }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
	{
        return (c->GetClassName().compare("SimpleVFATFrameCollection") == 0);
    }

    virtual std::string GetClassName() const
	{
        return "SRSFile";
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

    std::streampos GetLastEventPosition() const
    {
      if (positions.empty())
        return 0;

      return positions.back();
    }

    int Seek(long position)
    {
      return infile->Seek(position);
    }

protected:
    static const unsigned int eventHeaderSize;

    /// Processes one DATE super-event (GDC).
    /// returns the number of GOH blocks that failed consistency checks
    unsigned int ProcessDATESuperEvent(char *ptr, SimpleVFATFrameCollection *, RawEvent *);

    /// Processes one DATE event (LDC).
    /// returns the number of GOH blocks that failed consistency checks
    unsigned int ProcessDATEEvent(char *ptr, SimpleVFATFrameCollection *, RawEvent *);

    /// Processes data from one OptoRx.
    /// TODO: update
    /// Extract VFAT data from an OptoRx frame and saves them to the collection.
    /// BufferType can be any type that has [] operator (array, CircularBuffer, ...)
    /// returns the number of cells that failed consistency checks
    /// \param buf is the OptoRx frame buffer, including the header and footer
    /// \param frameSize is the size (in words) of the OptoRx frame including the header and footer

    // TODO: describe
    unsigned int ProcessOptoRxFrame(word *buf, unsigned int frameSize, SimpleVFATFrameCollection *, RawEvent *);

    // TODO: describe
    // "old" format
    unsigned int ProcessOptoRxFrameSerial(word *buf, unsigned int frameSize, SimpleVFATFrameCollection *);

    // TODO: describe
    // "new" format
    unsigned int ProcessOptoRxFrameParallel(word *buf, unsigned int frameSize, SimpleVFATFrameCollection *);

    // TODO: describe
    unsigned int ProcessVFATDataParallel(unsigned short *buf, unsigned int OptoRxId, SimpleVFATFrameCollection *fc);

	/// processes a LoneG frame (trigger data) encoded in an OptoRxFrame
    unsigned int ProcessLoneGFrame(word *ptr, signed long size, RawEvent *);

    /// reads 'bytesToRead' bytes from the file to buffer, starting at the given offset
    virtual unsigned char ReadToBuffer(unsigned int bytesToRead, unsigned int offset);

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
