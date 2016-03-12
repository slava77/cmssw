/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/SlinkFile.h"

#include <cstring>
#include <cstdlib>

using namespace std;

namespace Totem {

//----------------------------------------------------------------------------------------------------

  SlinkFile::SlinkFile() : filename(NULL)
{
  file = NULL;

  // default values for BOF, EOF
  seqBOF.clear(); seqEOF.clear();
  seqBOF.push_back(sequence(0x50c800c700c600c5ULL)); seqEOF.push_back(sequence(0xa0c800c700c600c5ULL));
  seqBOF.push_back(sequence(0x5000000000000b13ULL)); seqEOF.push_back(sequence(0xa000000000000b13ULL));
  seqBOF.push_back(sequence(0x5000000000000b14ULL)); seqEOF.push_back(sequence(0xa000000000000b14ULL));
  seqBOF.push_back(sequence(0x5000000000000000ULL)); seqEOF.push_back(sequence(0xa000000000000000ULL, 0xffffffff00000000ULL));
}

//----------------------------------------------------------------------------------------------------

SlinkFile::~SlinkFile()
{
  if (file) Close();
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus SlinkFile::Open(const std::string &filename)
{
  file = StorageFile::CreateInstance(filename);
  if(!file) {
    return osCannotOpen;
  }

  file->OpenFile();

  if (!file->IsOpened())
    return osCannotOpen;

  // reset counters
  indexStatus = isNotIndexed;
  corrNum = 0;
  
  this->filename = (char*)malloc(sizeof(char)*filename.size()+1);
  strcpy(this->filename, filename.c_str());

  return osOK;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus SlinkFile::Open(StorageFile *storageFile)
{
  std::string filename = storageFile->GetURLPath();

  file = storageFile;
  file->OpenFile();

  if (!file->IsOpened())
    return osCannotOpen;

  // reset counters
  indexStatus = isNotIndexed;
  corrNum = 0;

  this->filename = (char*)malloc(sizeof(char)*filename.size()+1);
  strcpy(this->filename, filename.c_str());

  return osOK;
}

//----------------------------------------------------------------------------------------------------

void SlinkFile::Close()
{
  if(file && file->IsOpened()) {
    file->CloseFile();
    delete file;
  }

  if (filename) {
    delete filename;
    filename = NULL;
  }
}

//----------------------------------------------------------------------------------------------------

char SlinkFile::Read(unsigned long long &n)
{
  int ret = (int) file->ReadData(&n, 1, 8);

  if (ret != 8) {
    int eofFlag = file->CheckEOF();
    if (!eofFlag)
      ERROR("SlinkFile::Read") << "Problems with reading binary file. " << ret << " bytes read instead of 8." << c_endl;
    return 2;
  }

  return 0;  
}

//----------------------------------------------------------------------------------------------------

signed int SlinkFile::CheckSequence(unsigned long long n, const std::vector<sequence> &seqs)
{
  for (unsigned int i = 0; i < seqs.size(); i++)
    if (seqs[i].compare(n)) return i;
    
  return -1;
}

//----------------------------------------------------------------------------------------------------

char SlinkFile::SeekNextFrame()
{
  // check if file is open
  if (!file->IsOpened())
    return 1;

  bool found = false;
  unsigned long long n = 0;  

  while(!file->CheckEOF()) {
    Read(n);
    if (CheckSequence(n, seqBOF) >= 0) {
      found = true;
      break;
    }
  }

  // remember position if indexing
  if (indexStatus == isIndexing && found) positions.push_back((int const &) file->CurrentPosition());
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

unsigned char SlinkFile::LoadFrameCheckFEOF(const char *msg)
{
  // check for EOF
  int eofFlag = file->CheckEOF();
  if (eofFlag) {
    // print message
    ERROR("SlinkFile::LoadFrame") << "Corrupted Slink frame (event " << positions.size() << "). " << msg << c_endl;

    // if indexing, set index and remove the last position - it's not complete
    if (indexStatus == isIndexing) {
      indexStatus = isIndexed;
      positions.pop_back();
    }

    return 1;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------

char SlinkFile::LoadFrame()
{
  if (LoadFrameCheckFEOF("File ends after BOF sequence. Frame ignored.")) return 10;

  unsigned long long n;

  // header
  // the line with flags (f7f7) whether fiber is connected
  unsigned long long mask = 0;
  Read(n);
  if ((n & 0x000000000000FFFFULL) == 0x000000000000F7F7ULL) mask |= 0x000000000000FFFFULL;
  if ((n & 0x00000000FFFF0000ULL) == 0x00000000F7F70000ULL) mask |= 0x00000000FFFF0000ULL;
  if ((n & 0x0000FFFF00000000ULL) == 0x0000F7F700000000ULL) mask |= 0x0000FFFF00000000ULL;
  if ((n & 0xFFFF000000000000ULL) == 0xF7F7000000000000ULL) mask |= 0xFFFF000000000000ULL;
  if (LoadFrameCheckFEOF("File ends after 1st line of header. Frame ignored.")) return 11;
  if (!mask)
    WARN("SlinkFile::LoadFrame") << "Frame mask is zero. No data will be read from this Slink frame. (row = 0x" << hex << n << dec << ")." << c_endl;

  // the line with tag
  Read(n);
  frame.SetTag(n);
  if (LoadFrameCheckFEOF("File ends after 2nd line of header. Frame ignored.")) return 12;
  if (n) WARN("SlinkFile::LoadFrame") << "Frame tag is " << hex << n << dec << " instead of 0 as it should be for this version." << c_endl;

  // data block
  unsigned long long* addr = (unsigned long long*) frame.GetSlinkFrame();
  for (int i = 0; i < VFAT_FRAME_BIT_SIZE; i++) {  
    // read data and remember previous position
    if (LoadFrameCheckFEOF("File ends while reading data block. Frame ignored.")) return 13;

    long int prevPos = file->CurrentPosition();
    Read(n);

    if (CheckSequence(n, seqBOF) >= 0) {
      ERROR("SlinkFile::LoadFrame") << "Corrupted Slink frame (event " << positions.size() << "). A BOF sequence found in data block (at position " 
        << i << "). Frame dropped." << c_endl;
      file->Seek(prevPos);  // go one record back so as SeekNextFrame can find it
      if (indexStatus == isIndexing)
        positions.pop_back();
      return 1;
    }

    if (CheckSequence(n, seqEOF) >= 0) {
      ERROR("SlinkFile::LoadFrame") << "Corrupted Slink frame (event " << positions.size() << "). An EOF sequence found in data block (at position "
        << i << "). Frame dropped.)" << c_endl;
      if (indexStatus == isIndexing)
        positions.pop_back();
      return 1;
    }

    // save value
    addr[i] = n & mask;
  }

  // footer
  long int prevPos = file->CurrentPosition();
  Read(n);
  if (CheckSequence(n, seqBOF) >= 0) {
    ERROR("SlinkFile::LoadFrame") << "Corrupted Slink frame (event " << positions.size()
      << "). A BOF sequence found in place of EOF sequence. Frame dropped.)" << c_endl;
    file->Seek(prevPos);  // go one record back so as SeekNextFrame can find it
    if (indexStatus == isIndexing)
      positions.pop_back();
    return 1;
  }

  if (CheckSequence(n, seqEOF) < 0) {
    ERROR("SlinkFile::LoadFrame") << "Corrupted Slink frame (event " << positions.size() << "). EOF sequence not found after data block. Frame dropped.)" << c_endl;
    if (indexStatus == isIndexing)
      positions.pop_back();
    return 1;
  }

  // everything OK
  return 0;  
}

//----------------------------------------------------------------------------------------------------

unsigned char SlinkFile::GetNextEvent(RawEvent *event)
{
  while (true) {
    // try to move to the next frame beggining
     if (SeekNextFrame()) return 1;    // eof without finding BOF or file not open

    // read data block to frame
     int resLoadFrame = LoadFrame();
    if (resLoadFrame == 1) corrNum++;  // corrupted frame
      if (resLoadFrame == 0) {       // success
        LoadEvent(event);
      return 0;  
    }
      if (resLoadFrame > 1)  return 2;     // eof
    
    }
}

//----------------------------------------------------------------------------------------------------

unsigned char SlinkFile::GetEvent(unsigned long n,RawEvent *event)
{
  // check if indexed and n <= number of frames
  if (indexStatus != 2) return 1;
  if (n >= positions.size()) return 2;

  // load the frame
  file->Seek(positions[n]);
  int resLoadFrame = LoadFrame();
  LoadEvent(event);
  return (unsigned char) resLoadFrame;
}

unsigned int SlinkFile::Reopen()
{
  file->CloseFile();
  file->OpenFile();
  if (!file->IsOpened()) {
      ERROR("SlinkFile::Reopen") << "Could not reopen file `" << filename << "'." << c_endl;
    return 1;
  }

  return 0;
}

} // namespace
