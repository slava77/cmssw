/****************************************************************************
*
* This is a part of theTOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kaspar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/TTPFile.h"

#include <cstdlib>
#include <cstring>

namespace Totem {

TTPFile::TTPFile()
{
  indexStatus = isNotIndexed;
}

//----------------------------------------------------------------------------------------------------

TTPFile::~TTPFile()
{
  Close();
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus TTPFile::Open(const std::string &fn)
{
  file = StorageFile::CreateInstance(fn);
  if(!file) {
    return osCannotOpen;
  }

  // try to open
  file->OpenFile();

  if (!file->IsOpened())
    return osCannotOpen;

  // check header
  char header[7]; header[6] = 0;
  file->ReadData(header, sizeof(char), 6);

  if (strcmp(header, "TTP001"))
    return osWrongFormat;
  
  this->filename = (char*)malloc(sizeof(char)*fn.size()+1);
  strcpy(this->filename, fn.c_str());

  return osOK;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus TTPFile::Open(StorageFile *storageFile)
{
  std::string fn = storageFile->GetURLPath();

  file = storageFile;

  // try to open
  file->OpenFile();

  if (!file->IsOpened())
    return osCannotOpen;

  // check header
  char header[7]; header[6] = 0;
  file->ReadData(header, sizeof(char), 6);

  if (strcmp(header, "TTP001"))
    return osWrongFormat;

  this->filename = (char*)malloc(sizeof(char)*fn.size()+1);
  strcpy(this->filename, fn.c_str());

  return osOK;
}

//----------------------------------------------------------------------------------------------------

void TTPFile::Close() {
  if(file && file->IsOpened()) {
    file->CloseFile();
    delete file;
  }
}

//----------------------------------------------------------------------------------------------------

unsigned char TTPFile::GetNextEvent(RawEvent *event)
{
  // remember file position
  int pos = (int) file->CurrentPosition();

  unsigned char r = ReadOneEvent(event);
  if (r > 0) {
    if (indexStatus == isIndexing)
      indexStatus = isIndexed;

    if (r == 2)
      WARN("TTPFile::GetNextEvent") << "File not terminated correctly." << c_endl;

    return 1;
  }

  // add to list of indices
  if (indexStatus == isIndexing)
    positions.push_back(pos);

  return 0;
}

//----------------------------------------------------------------------------------------------------

/**
Reads one event from current position of the stream. Return values are
  - 0 => OK
  - 1 => EOF at correct place
  - 2 => EOF at wrong place
  - 3 => Forbidden number of events
**/
unsigned char TTPFile::ReadOneEvent(RawEvent *event)
{
  // read frame count
  unsigned short nFrames;
  file->ReadData(&nFrames, sizeof(char), sizeof(nFrames));

  int flagEof = file->CheckEOF();
  int flagError = file->CheckError();


  if (flagEof || flagError)
    return 1;

  /*
  if (nFrames > 4) {
    fprintf(stderr, ERROR(TTPFile::ReadOneEvent,Number of frames to be read %i. It cannot be greater that 4. Exiting.), nFrames);
    return 3;
  }
  */

  SimpleVFATFrameCollection *sc = (SimpleVFATFrameCollection *) event->frames;
  sc->Clear();

  // read frames to event
  for (unsigned short i = 0; i < nFrames; i++)
  {
    VFATFrame *fp = sc->InsertEmptyFrame(i);
    file->ReadData(const_cast<short unsigned int *>(fp->getData()), sizeof(char), sizeof(VFATFrame));
  }

  flagEof = file->CheckEOF();
  flagError = file->CheckError();

  return (flagEof || flagError) ? 2 : 0;
}

//----------------------------------------------------------------------------------------------------

unsigned char TTPFile::GetEvent(unsigned long idx, RawEvent *event)
{
  if (indexStatus != isIndexed || idx >= positions.size())
    return 1;

  // this clears error flags, necessary to to make any operations with the file

  file->Seek(positions[idx]);
  ReadOneEvent(event);
  return 0;
}

//----------------------------------------------------------------------------------------------------

void TTPFile::Rewind()
{
  // clear file's error flags and seek to the beginning
  file->Seek(6);

  // reset status
  indexStatus = isNotIndexed;
  positions.clear();
}

//----------------------------------------------------------------------------------------------------

void TTPFile::StartIndexing()
{
  indexStatus = isIndexing;
  positions.clear();
}

//----------------------------------------------------------------------------------------------------

unsigned int TTPFile::Reopen()
{
  file->CloseFile();
  file->OpenFile();
  if (!file->IsOpened()) {
      ERROR("TPPFile::Reopen") << "Could not reopen file `" << filename << "'." << c_endl;
    return 1;
  }

  return 0;
}

} // namespace

