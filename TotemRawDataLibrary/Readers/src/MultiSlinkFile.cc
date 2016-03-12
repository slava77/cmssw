/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/MultiSlinkFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrame.h"

#include <cmath>
#include <map>
#include <algorithm>
#include <cstring>

using namespace std;

namespace Totem {

  MultiSlinkFile::MultiSlinkFile():filename(NULL)
{
  file = NULL;
  activeFrame = -1;
  isStartFrame = false;
  eventCounter = 0;
  
  // default values for BOF, EOF
  seqBOF.clear();
  seqBOF.push_back(sequence(0x5003000200010000ULL));
  seqBOF.push_back(sequence(0x5007000600050004ULL));
  seqBOF.push_back(sequence(0x5011001000090008ULL));
  seqEOF.clear();
  seqEOF.push_back(sequence(0xa000000000000000ULL));
}

//----------------------------------------------------------------------------------------------------

MultiSlinkFile::~MultiSlinkFile()
{
  if (file) Close();
  //frames.clear();
  positions.clear();
  seqBOF.clear();
  seqEOF.clear();
  delete file;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus MultiSlinkFile::Open(const string &filename)
{
  file = StorageFile::CreateInstance(filename);
  if(!file) {
    return osCannotOpen;
  }

  // open the file
  file->OpenFile();

  if (!file->IsOpened()) {
    //    printf("multi cannot open\n");
    return osCannotOpen;
  }

  // check if MultiSlinkFile (read 8 bytes and then return to the beginning)
  unsigned long long n = 0;
  Read(n);
  int iframe = CheckSequence(n, seqBOF);
  if (iframe < 0)
    return osWrongFormat; // TODO: lol file?

  // copying the filename
  this->filename = (char*)malloc(sizeof(char)*filename.size()+1);
  strcpy(this->filename, filename.c_str());

  Rewind();

  // reset counters
  indexStatus = isNotIndexed;
  corrNum = 0;
  eventCounter = 0;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus MultiSlinkFile::Open(StorageFile *storageFile)
{
  std::string filename = storageFile->GetURLPath();

  file = storageFile;

  // open the file
  file->OpenFile();

  if (!file->IsOpened()) {
    //    printf("multi cannot open\n");
    return osCannotOpen;
  }

  // check if MultiSlinkFile (read 8 bytes and then return to the beginning)
  unsigned long long n = 0;
  Read(n);
  int iframe = CheckSequence(n, seqBOF);
  if (iframe < 0)
    return osWrongFormat; // TODO: lol file?

  // copying the filename
  this->filename = (char*)malloc(sizeof(char)*filename.size()+1);
  strcpy(this->filename, filename.c_str());

  Rewind();

  // reset counters
  indexStatus = isNotIndexed;
  corrNum = 0;
  eventCounter = 0;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

void MultiSlinkFile::Close()
{
  file->CloseFile();

  file = NULL;
  //frames.clear();
  positions.clear();
}

//----------------------------------------------------------------------------------------------------

char MultiSlinkFile::Read(unsigned long long &n)
{
  // if binary file, do binary read
  //printf("0x");
  int ret = file->ReadData(&n, 1, 8);

  if (ret != 8) {
    int eofFlag = file->CheckEOF();
    if (!eofFlag)
      ERROR("SlinkFile::Read") << "Problems with reading binary file. " << ret << "bytes read instead of 8." << c_endl;
    return 2;
  }
  //printf("%016llx\n",n);

  return 0;
}

//----------------------------------------------------------------------------------------------------

signed int MultiSlinkFile::CheckSequence(unsigned long long n, const std::vector<sequence> &seqs)
{
  for (unsigned int i = 0; i < seqs.size(); i++)
    if (seqs[i].compare(n)) return i;
    
  return -1;
}

//----------------------------------------------------------------------------------------------------

char MultiSlinkFile::SeekNextFrame()
{
  // check if file is open
  activeFrame = -1;
  if (!file) return 1;
  bool found = false;
  unsigned long long n = 0;

  while (!file->CheckEOF()) {
    Read(n);
    activeFrame = CheckSequence(n, seqBOF);
    // if frametype = -1, return on any found frame
    // or return if frame of type frametype is found
    if(activeFrame>=0) {
      found = true;
      break;
    }
  }
  
    int eofFlag = file->CheckEOF();
  // if eof is found and was indexing, set indexed
  if (indexStatus == isIndexing && eofFlag)
    indexStatus = isIndexed;

  // return code
  if (!found)
    return 2;
  else
    return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned char MultiSlinkFile::LoadFrameCheckFEOF(const char *msg)
{
  // check for EOF
    int eofFlag = file->CheckEOF();
  if (eofFlag) {
    // print message
    ERROR("MultiSLinkFile::LoadFrame") << "FEOF inside frame after event #" << eventCounter << ": " << msg << "." << c_endl;

    // if indexing, set indexed 
    if (indexStatus == isIndexing)
      indexStatus = isIndexed;

    return 1;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------

char MultiSlinkFile::LoadFrame(int iframe, SimpleVFATFrameCollection *fc)
{
  if(iframe < 0 || iframe >= NUM_OF_SLINKFRAMES) {
    ERROR("MultiSlinkFile::LoadFrame") << "Wrong frame-type (" << iframe << ") found in (would be) event #" << eventCounter << "." << c_endl;
    return 1;
  }
  if (LoadFrameCheckFEOF("File ends after BOF sequence. Reading stopped.")) return 10;

  unsigned long long n=0;
  // Add 64 VFATFrames to SimpleVFATFrameCollection, starting from position iframe
  std::vector<short unsigned int*> datavector;
  unsigned int jstart = iframe*NUM_OF_VFATS_PER_FRAME;
  unsigned int jend = (iframe+1)*NUM_OF_VFATS_PER_FRAME;
  for (unsigned int j = jstart; j < jend; j++) {
    datavector.push_back((short unsigned int*) fc->InsertEmptyFrame(j)->getData());
  }
  // Calculate powers of 2 needed later on
  std::vector<unsigned long long>pow2ull;
  for (int j = 0; j < NUM_OF_VFATS_PER_FRAME; j++) {
    pow2ull.push_back((unsigned long long)pow(2., j));
  }

  // read VFAT_FRAME_BIT_SIZE blocks: data block
  for (int i = 0; i < VFAT_FRAME_BIT_SIZE; i++) {  
    if (LoadFrameCheckFEOF("File ends while reading data block. Reading stopped.")) return 11;
    long int prevPos = file->CurrentPosition();
    Read(n);

    if (CheckSequence(n, seqBOF) >= 0) { // BOF found inside data
      file->Seek(prevPos); // go one record back so as SeekNextFrame can find it
      return 1;
    }
    if (CheckSequence(n, seqEOF) >= 0) return 1; // EOF found inside data

    // save unsigned long long into VFATFrames
    int iword = NUM_OF_WORDS-i/WORD_BIT_SIZE-1;
    int ibit = WORD_BIT_SIZE-i%WORD_BIT_SIZE-1;
    for (int j = 0; j < NUM_OF_VFATS_PER_FRAME; j++) {
      // update the i-th bit of the data of the j-th frame (0<=j<64)
      short unsigned int * thisword = &datavector.at(j)[iword];
      unsigned long long divider = pow2ull.at(j);
      if((n/divider)%2 == 1) (*thisword) += (short unsigned int)pow(2., ibit);
    }
  }

  // read one block: footer
    long int prevPos = file->CurrentPosition();
  Read(n);
  if (CheckSequence(n, seqBOF) >= 0) { // BOF found instead of EOF
    file->Seek(prevPos);  // go one record back so as SeekNextFrame can find it
    return 1;
  }
  if (CheckSequence(n, seqEOF) < 0) return 1; // no EOF found after data

  // everything OK
  return 0;  
}

//----------------------------------------------------------------------------------------------------

std::vector<int> MultiSlinkFile::LoadFrames(SimpleVFATFrameCollection *fc)
{
  std::vector<int> load;
  std::vector<int> pos;
  std::vector<int> iframe;

  isStartFrame = true;
  for(int i=0;i<NUM_OF_SLINKFRAMES;i++) {
    pos.push_back(file->CurrentPosition());
    if (SeekNextFrame()) {
      load.resize(NUM_OF_SLINKFRAMES,10);
      break;
    }
    // check if this frame is already present in this event
    if(find(iframe.begin(), iframe.end(), activeFrame) != iframe.end()) {
      // go back to previous position
      file->Seek(pos[i]);
      // set the return value of other frames to zero
      for(int j=0;j<NUM_OF_SLINKFRAMES;j++) {
        if( find(iframe.begin(), iframe.end(), j) == iframe.end()) {
          load.push_back(0);
          iframe.push_back(j);
        }
      }
      break;
    }
    // load frame if everything all right
    load.push_back(LoadFrame(activeFrame, fc));
    iframe.push_back(activeFrame);
    if(isStartFrame) isStartFrame = false;
  }
  return load;
}

//----------------------------------------------------------------------------------------------------

unsigned char MultiSlinkFile::GetNextEvent(RawEvent *event)
{
  SimpleVFATFrameCollection *sc = (SimpleVFATFrameCollection *) event->frames;
  sc->Clear();
  int pos = file->CurrentPosition();
  while (true) {
    std::vector<int> load = LoadFrames(sc);
      if (load[0] == 0 && load[1] == 0 && load[2] == 0) {
      if (indexStatus == isIndexing)
        positions.push_back(pos);
      eventCounter++;
      return 0;    // success
    }
    if (load[0] > 1  || load[1] > 1  || load[2] > 1)
      return 2;     // eof
    corrNum++;  // corrupted frame, continue while loop
  }
}

//----------------------------------------------------------------------------------------------------

unsigned char MultiSlinkFile::GetEvent(unsigned long n, RawEvent *event)
{
  // check if indexed and n <= number of frames
  if (indexStatus != isIndexed)
    return 1;
  if (n >= positions.size())
    return 2;

  // empty the collection first
  SimpleVFATFrameCollection *sc = (SimpleVFATFrameCollection *) event->frames;
  sc->Clear();

  // load the frame
  file->Seek(positions[n]);
  std::vector<int> load = LoadFrames(sc);
  if (load[0] == 0 && load[1] == 0 && load[2] == 0)
    return 0;    // success
  
  if (load[0] > 1  || load[1] > 1  || load[2] > 1)
    return 2;     // eof

  return 1;
}

} // namespace

