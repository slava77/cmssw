/****************************************************************************
*
* This is a part of TOTEM testbeam/monitoring software.
* Authors:
*   Jan Kašpar (jan.kaspar@cern.ch)
*
****************************************************************************/


#ifndef _CircularBuffer_h_
#define _CircularBuffer_h_

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include <cstdio>
#include <cstring>
#include <exception>
#include <cerrno>
#include <string>
#include "TotemRawDataLibrary/Readers/interface/StorageFile.h"

namespace Totem {

/**
 * An implementation of circular buffer for faster file reading.
 **/
template <typename word>
class CircularBuffer
{
 protected:
  word *addr;                                             ///< pointer to the buffer
  StorageFile *file;                                      ///< the source file
  std::string filename;		                              ///< the name of the file opened
  unsigned int size;                                      ///< buffer size in words  
  unsigned int wsize;                                     ///< word size in bytes
  unsigned int begin;                                     ///< index of data beginning; within range [0, size)
  unsigned int end;                                       ///< first index after data end; within range [0, size)
  bool empty;                                             ///< buffer empty flag; can only happen if begin = end
  enum state_type {sCanReadFile, sFileEmpty} state;       ///< sFileEmpty: there is less than sizeof(word) bytes in the file

  void Refill();                                          ///< refill to maximum: to full buffer or empty file
                                                          ///< after Refill() there must be at least one word in the buffer

 public:
  typedef long position_type;
  CircularBuffer(StorageFile *file, unsigned int _size);
  ~CircularBuffer();

    bool FileOpened()                                       ///< returns true if file was opened correctly
    { return file->IsOpened(); }

    unsigned int GetSize()                                ///< returns the size (in words) of the buffer
    { return size; }

    unsigned int GetWordSize()                            ///< returns the word size (in bytes)
    { return wsize; }

    unsigned int GetSizeInBytes()                         ///< returns the size (in bytes) of the buffer
    { return size * wsize; }

  word& PopWord();                                        ///< pops a word from the begging of the buffer and returs the reference to it
  void EnsureOffset(unsigned int offset);                 ///< ensures that offsets up to 'offset' can be read and are valid
                                                          ///< throws an exception if this cannot be ensured

  inline word& operator[] (unsigned int offset) const     ///< returns reference to a word at given offset from current beginning
    { return *(addr + ((begin + offset) % size)); }

  inline word& at(unsigned int offset) const              ///< returns reference to a word at given offset from current beginning
    { return *(addr + ((begin + offset) % size)); }

  inline void FastShift(unsigned int offset)              ///< shifts the beginning with no checks, to be used with EnsureOffset
    { begin = (begin + offset) % size; 
      if (begin == end) empty = true; }

  void Rewind()                                           ///< rewind file
    {
        file->Seek(0);
    }

  void Reset()                                            ///< resets the buffer; useful e.g. when rewinding the input file
    { empty = true; state = sCanReadFile; Refill(); }

  void SeekAndReset(position_type position)               ///< moves to the given position in the file and fills the buffer
    {
        file->Seek(position);
        Reset();
    }

  int WordsInBuffer()                                     ///< returns number of words left in the buffer
  {
    int wordsinbuffer = 0;
    if(!empty) {
      if(end>begin) wordsinbuffer = end-begin;
      else wordsinbuffer = end-begin+size; 
    }
    return wordsinbuffer;
  }

  position_type CurrentFilePosition()                     ///< returns current file position for indexing
    {
      long int ft = file->CurrentPosition();
      return (position_type)ft-(position_type)(WordsInBuffer()*wsize);
    }

 public:
  class Exception : public std::exception
  {
   public:
    enum ex_code {exOffsetLargerThanSize, exEOF, exCannotEnsureOffset, exOops} code;
    Exception(ex_code _code) : code(_code) {}
    virtual const char* what() const throw()
    {
      switch (code) {
        case exOffsetLargerThanSize: return "Offset in EnsureOffset is larger that buffer size.";
        case exEOF: return "End of file.";
        case exCannotEnsureOffset: return "Cannot ensure offset. File probably corrupted at the end.";
        case exOops: return "Oops, this should not happen.";
        default: return "unknown exception";
      }
    }

  };
};

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

template <typename word>
CircularBuffer<word>::CircularBuffer(StorageFile *file, unsigned int _size) :
    file(file), size(_size), wsize(sizeof(word)), begin(0), end(0), empty(true), state(sCanReadFile)
{
#ifdef DEBUG
  printf(">> CircularBuffer::CircularBuffer\n");
#endif

    file->OpenFile();

    if(!file->IsOpened()) {
        ERROR("CircularBuffer::CircularBuffer") << "Cannot open file '" << filename << "'. "<< c_endl;
        file->PrintError("Error message");
    }

  // allocate
  addr = new word[size];

  // full read
  if(file->IsOpened()) Refill();
}

//----------------------------------------------------------------------------------------------------

template <typename word>
CircularBuffer<word>::~CircularBuffer()
{
#ifdef DEBUG
  printf(">> CircularBuffer::~CircularBuffer\n");
#endif

    if(file && file->IsOpened()) {
        file->CloseFile();
        delete file;
    }
    if (addr)
        delete [] addr;
}

//----------------------------------------------------------------------------------------------------

template <typename word>
word& CircularBuffer<word>::PopWord()
{
  if (empty)
    Refill();
  word *ret = addr + begin;
  begin = (begin + 1) % size;
  if (begin == end)
    empty = true;
  return *ret;
}

//----------------------------------------------------------------------------------------------------

template <typename word>
void CircularBuffer<word>::EnsureOffset(unsigned int offset)
{
#ifdef DEBUG
  printf(">> CircularBuffer::EnsureOffset(%u), begin = %i, end = %i, empty = %i, state = %u\n", offset, begin, end, empty, state);
#endif

  if (offset >= size)
    throw Exception(Exception::exOffsetLargerThanSize);

  unsigned int max = end - begin + size;
  if (max > size) max -= size;
#ifdef DEBUG
  printf("\tmax = %i, empty = %i\n", max, empty);
#endif
  if (empty || offset >= max) {
    if (state == sCanReadFile)
      Refill();
    max = end - begin + size;
    if (max > size) max -= size;
#ifdef DEBUG
    printf("\tmax2 = %i\n", max);
#endif
    if (offset >= max)
      throw Exception(Exception::exCannotEnsureOffset);
  }
}

//----------------------------------------------------------------------------------------------------

template <typename word>
void CircularBuffer<word>::Refill()
{
#ifdef DEBUG
  printf(">> CircularBuffer::Refill\n\tbefore: begin = %i, end = %i, empty = %i, state = %u\n", begin, end, empty, state);
#endif
  if (state != sCanReadFile)
    throw Exception(Exception::exEOF);

  if (empty) {
#ifdef DEBUG
    printf("\tFullRefill\n");
#endif
    begin = 0;
    end = file->ReadData(addr, sizeof(char), wsize*size)/wsize;

    if (end == 0)
      throw Exception(Exception::exEOF);
    if (end < size)
      state = sFileEmpty;
    empty = false;
    end = end % size;

#ifdef DEBUG
    printf("\tafter : begin = %i, end = %i, empty = %i, state = %u\n", begin, end, empty, state);
#endif
    return;
  }
  
  if (end < begin) {
    end += file->ReadData(addr+end, sizeof(char), wsize*(begin-end))/wsize;
    if (end < begin)
      state = sFileEmpty;

#ifdef DEBUG
    printf("\tafter : begin = %i, end = %i, empty = %i, state = %u\n", begin, end, empty, state);
#endif
    return;
  }

  if (begin < end) {
    end += file->ReadData(addr+end, sizeof(char), wsize*(size-end))/wsize;

    if (end < size)
      state = sFileEmpty;
    else {
      end = file->ReadData(addr, sizeof(char), wsize*begin)/wsize;
      if (end < begin)
        state = sFileEmpty;
    }

#ifdef DEBUG
    printf("\tafter : begin = %i, end = %i, empty = %i, state = %u\n", begin, end, empty, state);
#endif
    return;
  }
  
#ifdef DEBUG
  printf(">> don't know how to refill\n");
#endif
  throw Exception(Exception::exOops);
}

} // namespace

#endif
