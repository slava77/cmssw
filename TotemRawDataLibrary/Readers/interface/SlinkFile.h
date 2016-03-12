/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#ifndef _Totem_SlinkFile_h_
#define _Totem_SlinkFile_h_

#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/SlinkFrame.h"
#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

#include <vector>

#undef min
#undef log

namespace Totem {

/**
 * Reads an Slink data file.
**/
class SlinkFile : public DataFile
{
  public:
    SlinkFile();
    ~SlinkFile();

    virtual OpenStatus Open(const std::string &);
    virtual OpenStatus Open(StorageFile* storageFile);
    virtual void Close();
    
    virtual VFATFrameCollection* CreateCollection() const
      { return new SimpleVFATFrameCollection; }

    virtual bool IsCollectionCompatible(VFATFrameCollection *c) const
      { return (c->GetClassName().compare("SimpleVFATFrameCollection") == 0); }

    void Rewind();                                      ///< rewinds back to the begging of the file
    char Read(unsigned long long&);                     ///< reads a number from data file

    unsigned char GetNextEvent(RawEvent *);             ///< retrieves the next event, returns 0 if successful
    unsigned char GetEvent(unsigned long, RawEvent *);  ///< retrieves the event of given number (numbered from 0), must be indexed before, 
                                                        ///< returns 0 is successful

    unsigned long GetEventsCount() const
      {  return (indexStatus == 2) ? positions.size() : 0 ; }

    unsigned long GetEventsCorrupted() const
      { return corrNum; }

    void StartIndexing();
    
    void StopIndexing()
      { indexStatus = isIndexed; }

    virtual std::string GetClassName() const                                
      { return "SlinkFile"; }
    
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

    /// Compares the number to numbers in the vector.
    /// returns the index of matched number or -1 if not found
    signed int CheckSequence(unsigned long long, const std::vector<sequence> &);
    
    /// loads data block at current position to memory
    char LoadFrame();

    /// Searches for the next BOF sequence.
    /// returns 0 if found, non-zero in case of errors
    char SeekNextFrame();

    /// loads the actual frame, non-zero return value if fails
    unsigned char LoadEvent(RawEvent *);

    unsigned int Reopen();

  protected:
    StorageFile *file;                  ///< file handle
    SlinkFrame frame;                   ///< Slink frame object
    std::vector<int> positions;         ///< positions of frame beginnings in the file, needs to be indexed
    
    unsigned int corrNum;               ///< number of corrupted frame
    unsigned char indexStatus;          ///< 0=not indexed, 1=indexing just now, 2=indexed

    char *filename;

    /// if feof is reached, prints the message and return non-zero, otherwise 0 returned
    unsigned char LoadFrameCheckFEOF(const char *);
};

//----------------------------------------------------------------------------------------------------

inline void SlinkFile::Rewind()
{
  file->Seek(0);
  positions.clear();
  corrNum = 0;
}

//----------------------------------------------------------------------------------------------------

inline unsigned char SlinkFile::LoadEvent(RawEvent *event)
{
  frame.DecodeVFATFrames((SimpleVFATFrameCollection *) event->frames);
  return 0;
}

//----------------------------------------------------------------------------------------------------

inline void SlinkFile::StartIndexing()
{
  indexStatus = 1;
  positions.clear();
}

} // namespace
#endif
