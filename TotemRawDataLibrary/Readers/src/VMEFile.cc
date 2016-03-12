/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors:
*   Mate Csanad (Mate.Csanad@cern.ch)
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/VMEFile.h"
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"
#include "TotemRawDataLibrary/Readers/interface/LocalStorageFile.h"

using namespace std;

namespace Totem {

unsigned int VMEFile::maxFrames = 6;
unsigned int VMEFile::vfatsPerFrame = 32;

//----------------------------------------------------------------------------------------------------

VMEFile::VMEFile() : buf(NULL)
{
}


//----------------------------------------------------------------------------------------------------

VMEFile::~VMEFile()
{
  Close();
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VMEFile::Open(const std::string &filename)
{
  // check if file ends on .vme, if not => not VME file
  size_t dotPos = filename.rfind('.');
  string extension = (dotPos == string::npos) ? "" : filename.substr(dotPos);
  if (extension.compare(".vme") != 0)
    return osWrongFormat;

  StorageFile *storageFile = StorageFile::CreateInstance(filename);
  if(!storageFile) {
    return osCannotOpen;
  }

  // create buffer with size of 12000 words
  buf = new CircularBuffer<word>(storageFile, 12000);

  if (!buf->FileOpened())
    return osCannotOpen;

  // set maximum of data counters
  dataEventNumberMax = 0xffff;
  dataConfNumberMax = 0xfff;

  // reset counters
  indexStatus = isNotIndexed;
  corruptedEventCounter = 0;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VMEFile::Open(StorageFile *storageFile)
{
  std::string filename = storageFile->GetURLPath();

  // check if file ends on .vme, if not => not VME file
  size_t dotPos = filename.rfind('.');
  string extension = (dotPos == string::npos) ? "" : filename.substr(dotPos);
  if (extension.compare(".vme") != 0)
    return osWrongFormat;

  // create buffer with size of 12000 words
  buf = new CircularBuffer<word>(storageFile, 12000);

  if (!buf->FileOpened())
    return osCannotOpen;

  // set maximum of data counters
  dataEventNumberMax = 0xffff;
  dataConfNumberMax = 0xfff;

  // reset counters
  indexStatus = isNotIndexed;
  corruptedEventCounter = 0;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

void VMEFile::Close()
{
  if (buf) delete buf;
  buf = NULL;
}

//----------------------------------------------------------------------------------------------------

bool VMEFile::MatchingHeaderFooter(word head, word foot)
{
  if ((head == 0x50010000UL && foot == 0xa0010000UL) || (head == 0x50030002UL && foot == 0xa0030002UL) ||
     (head == 0x50050004UL && foot == 0xa0050004UL) || (head == 0x50070006UL && foot == 0xa0070006UL) ||
     (head == 0x50090008UL && foot == 0xa0090008UL) || (head == 0x500b000aUL && foot == 0xa00b000aUL))
    return true;
  else
    return false;
}

//----------------------------------------------------------------------------------------------------

unsigned char VMEFile::GetNextEvent(RawEvent *event)
{
  OptoRxVFATFrameCollection *fc = (OptoRxVFATFrameCollection *) event->frames;
  fc->Invalidate();
  OptoRxVFATFrameCollection::IteratorType block = fc->InsertOptoRxBlock(0);

  try {
    while (true) {
      // store current position for indexing
      CircularBuffer<word>::position_type currentPos = buf->CurrentFilePosition();

      // we expect BTAG
      word ch = buf->PopWord();
      if (ch >> 28 != 0xb) {
        // but BTAG is not there - probably rubbish - skip it 
      } else {
        // BTAG really found
        word btag = ch;

        // Fill data counters
        event->dataConfNumber = (btag >> 16) & 0xFFF; 
        event->dataEventNumber = btag & 0xFFFF;
        //printf("**\tfound event, b tag = %x, ec = %lu, cc = %lu\n", btag, event->dataEventNumber, event->dataConfNumber);

        // digits in binary account for one frametype (1 if found, 0 if not)
        unsigned int framesFlag = 0;

        while (true) {
          // Make the buffer at least 193 lines long
          buf->EnsureOffset(193);

          // Check for appropriate header and matching footer
          word head = buf->at(0);
          word foot = buf->at(193);
          int frameID = -1;
          if (MatchingHeaderFooter(head, foot)) {
            frameID = (head & 0xF) >> 1;

            // If first time here (and file isIndexing), add a new position
            if (framesFlag==0 && isIndexing)
              positions.push_back(currentPos);

            // If frame type already found: corrupted event; else set the appropriate flag;
            if (framesFlag & (1 << frameID)) {
              ERROR("VMEFile::GetNextEvent") << "Frame #" << frameID << " present twice in event #" << event->dataEventNumber
                << " (filledFlag: 0x" << hex << framesFlag << dec << ")." << c_endl;
              corruptedEventCounter++;
              buf->PopWord();
              return 0;
            }
            framesFlag |= (1 << frameID);

            // validate corresponding GOH blocks (there are two blocks per one frame)
            VFATFrame::word** dataPtrs[2] = 
              { fc->ValidateGOHBlock(block, 2*frameID+0), fc->ValidateGOHBlock(block, 2*frameID+1) };
            
            // deserialization
            for (unsigned int i = 0; i < 192; i++) {
              //printf("i = %u\n", i);
              int iword = 11 - i/16;  // number of current word (11...0)
              int ibit = 15 - i%16;   // number of current bit (15...0)
              word w = buf->at(i+1);

              // Fill the current bit of the current word of all VFAT frames
              for (unsigned int idx = 0; idx < vfatsPerFrame; idx++) {
                //printf("\tidx = %u\n", idx);
                if (w & (1 << idx))
                  dataPtrs[idx/16][idx%16][iword] |= (1 << ibit);
              }
            }

            // Tell the buffer to skip the block that has been just processed
            buf->FastShift(194);

            // Return if all subframes already found; i.e. if framesFlag is 111111 (in binary)
            if (framesFlag == 63)
              return 0;

          } else {
            if (head>>28 == 0xb) {
                // not all frames are present, BTAG found in place of HEAD
                WARN("VMEFile::GetNextEvent") << "Not all frames filled in event #" << event->dataEventNumber
                  << " (filledFlag: 0x" << hex << framesFlag << dec << ")." << c_endl;
                corruptedEventCounter++;
                return 0;
              } else {
                // there is rubbish after the last FOOT, skip it!
                buf->PopWord();
              }
          } // if HEAD/FOOT
        } // HEAD/FOOT while
      } // if BTAG
    } // BTAG while
  } // try 

  // catch buffer exceptions: EOF and real problems
  catch (CircularBuffer<word>::Exception ex) {
    if (ex.code != CircularBuffer<word>::Exception::exEOF)
      ERROR("VMEFile::GetNextEvent") << "CircularBuffer exception: " << ex.what() << c_endl;

    // if indexing, set indexed 
    if (indexStatus == isIndexing) indexStatus = isIndexed;
    return 1;
  }

  // catch other possible exceptions - this should not happen
  catch (...) {
    ERROR("VMEFile::GetNextEvent") << "Unknown exception caught - this should not happen." << c_endl;
    return 2;
  }

  // one should never get here, but it is necessary to satisfy the compiler
  return 3;
}

//----------------------------------------------------------------------------------------------------

unsigned char VMEFile::GetEvent(unsigned long n, RawEvent *event)
{
  if (n >= positions.size()) {
    ERROR("VMEFile::GetEvent") << "Requested event number (" << n << ") is larger than event count (" << positions.size() << ")." << c_endl;
    return 1;  
  }

  // seek and load the frame
  try {
    buf->SeekAndReset(positions[n]);
  }
  catch (CircularBuffer<word>::Exception ex) {
    ERROR("VMEFile::GetEvent") << "CircularBuffer exception caught." << c_endl;
    return 1;
  }
  int ret = GetNextEvent(event);
  return ret;
}

} // namespace

