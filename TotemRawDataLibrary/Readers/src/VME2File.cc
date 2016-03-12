/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kaspar (jan.kaspar@gmail.com)
*   Mate Csanad
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/VME2File.h"

//#define DEBUG 1

using namespace std;

namespace Totem {

//----------------------------------------------------------------------------------------------------

VME2File::VME2File() : buf(NULL)
{
}

//----------------------------------------------------------------------------------------------------

VME2File::~VME2File()
{
  Close();
  if (buf) delete buf;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VME2File::Open(const std::string &filename)
{
  // check if file ends on .vme2, if not => not a VME2 file
  size_t dotPos = filename.rfind('.');
  string extension = (dotPos == string::npos) ? "" : filename.substr(dotPos);
  if (extension.compare(".vme2") != 0)
    return osWrongFormat;

  StorageFile *storageFile = StorageFile::CreateInstance(filename);
  if(!storageFile) {
    return osCannotOpen;
  }

  // create buffer with size of 12000 words
  buf = new CircularBuffer<word>(storageFile, 12000);
  if(!buf->FileOpened())
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

DataFile::OpenStatus VME2File::Open(StorageFile *storageFile)
{
  std::string filename = storageFile->GetURLPath();

  // check if file ends on .vme2, if not => not a VME2 file
  size_t dotPos = filename.rfind('.');
  string extension = (dotPos == string::npos) ? "" : filename.substr(dotPos);
  if (extension.compare(".vme2") != 0)
    return osWrongFormat;

  storageFile->OpenFile();

  // create buffer with size of 12000 words
  buf = new CircularBuffer<word>(storageFile, 12000);
  if(!buf->FileOpened())
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

void VME2File::Close()
{
}

//----------------------------------------------------------------------------------------------------

unsigned char VME2File::GetNextEvent(RawEvent *event)
{
  // "empty" the collection first
  OptoRxVFATFrameCollection *fc = (OptoRxVFATFrameCollection *) event->frames;
  fc->Invalidate();

  unsigned int bufferSize = buf->GetSizeInBytes();

  try {
    while (true) {
      // search for b-tag
	  CircularBuffer<word>::position_type currentPos = buf->CurrentFilePosition();
      word ch = buf->PopWord();
      //printf("= %016llx\n", ch);

      if (((ch >> 60) & 0xf) == 0xb) {
		word btag = ch;
        unsigned long eventSize = ch & 0xFFFFFFFF;

        // check sanity of event size - only thing sure is that it is even
        if (eventSize % 2 != 0 || eventSize > bufferSize || eventSize<192*buf->GetWordSize()) {
          ERROR("VME2File::GetNextEvent") << "A (possible) B-tag claims an invalid event size " << eventSize << ". Skipping." << c_endl;
          continue;
        }

        // calculate number of words
        unsigned long eventWords = eventSize / buf->GetWordSize();
        buf->EnsureOffset(eventWords - 1);

        // check first line
        ch = (*buf)[0];
        if (((ch >> 60) & 0xf) != 0x5) {
          ERROR("VME2File::GetNextEvent") << "First word " << hex << ch << dec << " is not a BOE sequence." << c_endl;
          continue;
        }

        // check last line
        ch = (*buf)[eventWords - 1];
        if (((ch >> 60) & 0xf) != 0xa) {
          ERROR("VME2File::GetNextEvent") << "Last word " << hex << ch << dec << " is not a EOE sequence." << c_endl;
          continue;
        }

		// event accepted - fill data counters (not implemented yet)
		event->dataConfNumber = (btag >> 48) & 0xFFF; 
		event->dataEventNumber = (btag >> 32) & 0xFFFF;
		
        // save event position
        if (isIndexing)
          positions.push_back(currentPos);

        //printf("----------------------------------------------------------------------------------------------------\n");
        //printf("> event found: conf = %lu, evCounter = %lu, event size %lu\n", event->dataConfNumber, event->dataEventNumber, eventSize);

        // extract all OptoRx frames
        unsigned long wordsRead = 0;
        while (wordsRead < eventWords) {
          unsigned long long head = buf->at(0);
          if (((head >> 60) & 0xF) != 0x5) {
            ERROR("VME2File::GetNextEvent") << "OptoRx BOF sequence not found as expected. Skipping the entire rest of the event." << c_endl;
            break;
          }

          // try all possible frame sizes (1 to 3 sub-frames)
          bool frameFound = false;
          for(unsigned int frameSize = 196; frameSize <= 584 && !frameFound; frameSize += 194) {
            unsigned long long foot = buf->at(frameSize-1);
            if (((foot >> 60) & 0xF) == 0xA) {
              ProcessOptoRxFrame(*buf, frameSize, fc, event);
              frameFound = true;
              wordsRead += frameSize;
              buf->FastShift(frameSize);
              break;
            }
          }

          if (!frameFound) {
            ERROR("VME2File::GetNextEvent") << "OptoRx EOF sequence not found at any of allowed positions. Skipping the entire rest of the event." << c_endl;
            break;
          }
        }

        if(wordsRead != eventWords)
          ERROR("VME2File::GetNextEvent") << "VME2File::GetNextEvent,Not all words of the event have been read (" << wordsRead <<
            " of " << eventWords << ")." << c_endl;
        //printf("::: event reading finished :::\n");
        return 0;
      }
    }
  } 

  // catch buffer exceptions: EOF and real problems
  catch (CircularBuffer<word>::Exception ex) {
    if (ex.code != CircularBuffer<word>::Exception::exEOF)
      ERROR("VME2File::GetNextEvent") << "CircularBuffer exception: " << ex.what() << c_endl;

    // if indexing, set indexed 
    if (indexStatus == isIndexing) indexStatus = isIndexed;
    return 1;
  }

  // catch other exceptions
  catch (...) {
    ERROR("VME2File::GetNextEvent") << "An unknown exception caught." << c_endl;
    return 2;
  }

  // one should never get here, but it is necessary to satisfy the compiler
  return 3;
}

//----------------------------------------------------------------------------------------------------

unsigned char VME2File::GetEvent(unsigned long n, RawEvent *event)
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
    ERROR("VME2File::GetEvent") << "CircularBuffer exception caught." << c_endl;
    return 1;
  }

  return GetNextEvent(event);
}

} // namespace

