/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors:
*  Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/SRSFile.h"

// the RunII version
#include "TotemRawDataLibrary/DAQA/interface/event_3_14.h"

//#define DEBUG 0

using namespace std;


namespace Totem {

const unsigned int SRSFile::eventHeaderSize = sizeof(eventHeaderStruct);

//----------------------------------------------------------------------------------------------------

SRSFile::SRSFile() : dataPtr(NULL), dataPtrSize(0), infile(NULL)
{
#ifdef DEBUG
  printf(">> SRSFile::SRSFile \n");
#endif
}

//----------------------------------------------------------------------------------------------------

SRSFile::~SRSFile()
{
#ifdef DEBUG
  printf(">> SRSFile::~SRSFile, this = %p, dataPtr = %p\n", (void *) this, dataPtr);
#endif

  Close();

  if (dataPtr != NULL)
    delete [] dataPtr;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus SRSFile::Open(const std::string &fn)
{
  // check if the source is a SRS File
  size_t dotPos = fn.rfind('.');
  string extension = (dotPos == string::npos) ? "" : fn.substr(dotPos);
  if (extension.compare(".srs") != 0)
    return osWrongFormat;

  infile = StorageFile::CreateInstance(fn);
  infile->OpenFile();

  if (!infile->IsOpened()) {
    infile->PrintError("Error while opening file in SRSFile::Open");
    return osCannotOpen;
  }

  // set maximum of data counters
  dataEventNumberMax = 0xFFFFFFFF;
  dataConfNumberMax = 0;

  // reset counters
  indexStatus = isNotIndexed;
  corruptedEventCounter = 0;

  filename =  fn;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus SRSFile::Open(StorageFile *storageFile)
{
  std::string fn = storageFile->GetURLPath();

  // check if the source is a SRS File
  size_t dotPos = fn.rfind('.');
  string extension = (dotPos == string::npos) ? "" : fn.substr(dotPos);
  if (extension.compare(".srs") != 0)
    return osWrongFormat;

  infile = storageFile;
  infile->OpenFile();

  if (!infile->IsOpened())
  {
    infile->PrintError("Error while opening file in SRSFile::Open");
    return osCannotOpen;
  }

  // set maximum of data counters
  dataEventNumberMax = 0xFFFFFFFF;
  dataConfNumberMax = 0;

  // reset counters
  indexStatus = isNotIndexed;
  corruptedEventCounter = 0;

  filename =  fn;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

void SRSFile::Close()
{
#ifdef DEBUG
  printf(">> SRSFile::Close, this = %p", (void*) this);
#endif
  
  if (infile && infile->CloseFile() == EOF)
  {
    ERROR("SRSFile::Close") << "Cannot close the file." << c_endl;
    delete infile;
  } else {
	  infile = NULL;
  }
}

//----------------------------------------------------------------------------------------------------

unsigned char SRSFile::ReadToBuffer(unsigned int bytesToRead, unsigned int offset)
{
#ifdef DEBUG
  printf(">> SRSFile::ReadToBuffer(%u, %u), this = %p, dataPtr = %p, dataPtrSize = %u\n",
      bytesToRead, offset, (void*) this, dataPtr, dataPtrSize);
#endif

  // allocate new memory block if current one is too small
  if (dataPtrSize < bytesToRead+offset)
  {
    char *newPtr = NULL;
    try {
      newPtr = new char[bytesToRead+offset];
    }
    catch (bad_alloc& ba) {
      ERROR("SRSFile::ReadToBuffer") << "Cannot allocate buffer large enough." << c_endl;
      return 2;
    }

    if (dataPtr)
    {
      if (offset > 0)
        memcpy(newPtr, dataPtr, offset);
      delete [] dataPtr;
    }

    dataPtr = newPtr;
    dataPtrSize = bytesToRead+offset;
  }

  // read data at given offset
  unsigned int bytesRead;
  
  int eofFlag;
  bytesRead = infile->ReadData(dataPtr + offset, sizeof(char), bytesToRead);
  eofFlag = infile->CheckEOF();

  if (bytesRead != bytesToRead && !(bytesRead == 0 && eofFlag)) 
  {
    ERROR("SRSFile::ReadToBuffer") << "Reading from file to buffer failed. Only " << bytesRead
  			      << " B read from " << bytesToRead << " B." << c_endl;
    return 1;
  }

  return 0;
}
//----------------------------------------------------------------------------------------------------

unsigned char SRSFile::GetNextEvent(RawEvent* event)
{
#ifdef DEBUG
  printf(">> SRSFile::GetNextEvent, this = %p\n", (void*)this);
  printf("\teventHeaderSize = %u\n", eventHeaderSize);
#endif

  eventHeaderStruct *eventHeader = NULL;

  long int currentPos = infile->CurrentPosition();
  while (!infile->CheckEOF())
  {
    // read next header
    if (ReadToBuffer(eventHeaderSize, 0) != 0)
      return 10;

    eventHeader = (eventHeaderStruct *) dataPtr;

    // check the sanity of header data
    if (eventHeader->eventMagic != EVENT_MAGIC_NUMBER)
    {
      ERROR("SRSFile::GetNextEvent") << "Event magic check failed (" << hex << eventHeader->eventMagic << "!=" << EVENT_MAGIC_NUMBER << dec << "). Exiting." << c_endl;
      return 1;
    }

    unsigned int N = eventHeader->eventSize;
    if (N < eventHeaderSize)
    {
      ERROR("SRSFile::GetNextEvent") << "Event size (" << N << ") smaller than header size (" << eventHeaderSize << "). Exiting." << c_endl;
      return 1;
    }

    // get next event from the file (the header has already been read)
    if (ReadToBuffer(N-eventHeaderSize, eventHeaderSize) != 0)
      return 10;

    // because dataPtr could move, we have to be sure, that eventHeader still points the same as dataPtr
    eventHeader = (eventHeaderStruct *) dataPtr;

    // skip non physics events
    if (eventHeader->eventType != PHYSICS_EVENT)
      continue;

    break;
  }

  // check if the end of the file has been reached
  int eofFlag = infile->CheckEOF();

  if (eofFlag)
  {
    // if indexing, set indexed
    if (indexStatus == isIndexing)
      indexStatus = isIndexed;
    return 1;
  }

  // process the buffer
  SimpleVFATFrameCollection *oc = (SimpleVFATFrameCollection *) event->frames;
  oc->Clear();
  unsigned int errorCounter = ProcessDATESuperEvent(dataPtr, oc, event);

#ifdef DEBUG
  printf("* %u, %u, %u\n",
    EVENT_ID_GET_NB_IN_RUN( eventHeader->eventId ),
    EVENT_ID_GET_BURST_NB( eventHeader->eventId ),
    EVENT_ID_GET_NB_IN_BURST( eventHeader->eventId )
    );
#endif

  if (errorCounter > 0)
  {
    ERROR("SRSFile::GetNextEvent") << errorCounter << " GOH blocks have failed consistency checks in event "
      << event->dataEventNumber << "." << c_endl;
    corruptedEventCounter++;
  }

  // save event position
  if (indexStatus == isIndexing)
    positions.push_back(currentPos);

  return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessDATESuperEvent(char *ptr, SimpleVFATFrameCollection *oc, RawEvent *event)
{
  eventHeaderStruct *eventHeader = (eventHeaderStruct *) ptr;
  bool superEvent = TEST_ANY_ATTRIBUTE(eventHeader->eventTypeAttribute, ATTR_SUPER_EVENT);

#ifdef DEBUG
  printf(">> SRSFile::ProcessVMEBEvent\n");

  printf("\teventSize = %i\n", eventHeader->eventSize);
  printf("\teventMagic = %i\n", eventHeader->eventMagic);
  printf("\teventHeadSize = %i\n", eventHeader->eventHeadSize);
  printf("\t* eventHeadSize (extra) = %i\n", eventHeader->eventHeadSize - EVENT_HEAD_BASE_SIZE);
  printf("\t* eventPayloadSize = %i\n", eventHeader->eventSize - eventHeader->eventHeadSize);
  printf("\teventVersion = %i\n", eventHeader->eventVersion);
  printf("\teventType = %i\n", eventHeader->eventType);
  printf("\teventRunNb = %i\n", eventHeader->eventRunNb);
  printf("\teventId = %p\n", (void*)eventHeader->eventId);
  printf("\teventTriggerPattern = %p\n", (void*)eventHeader->eventTriggerPattern);
  printf("\teventDetectorPattern = %p\n",(void*) eventHeader->eventDetectorPattern);
  printf("\teventTypeAttribute = %p\n", (void*)eventHeader->eventTypeAttribute);
  printf("\t* super event = %i\n", superEvent);
  printf("\teventLdcId = %i\n", eventHeader->eventLdcId);
  printf("\teventGdcId = %i\n", eventHeader->eventGdcId);
  printf("\teventTimestamp = %i\n", eventHeader->eventTimestamp);
#endif

  // TODO: remove
  //printf("::::::::::: eventId = %i\n", EVENT_ID_GET_NB_IN_RUN(eventHeader->eventId));
  //printf("::::::::::: timestamp = %i\n", eventHeader->eventTimestamp);
  
  // store important GDC data
  event->dataEventNumber = EVENT_ID_GET_NB_IN_RUN(eventHeader->eventId) - 1;
  event->timestamp = eventHeader->eventTimestamp;

  eventSizeType eventSize = eventHeader->eventSize;
  eventHeadSizeType headSize = eventHeader->eventHeadSize;

  // process all sub-events (LDC frames)
  unsigned int errorCounter = 0;
  if (superEvent)
  {
    unsigned int offset = headSize;
    while (offset < eventSize)
    {
#ifdef DEBUG 
      printf("\t> offset before %i\n", offset);
#endif
      eventStruct *subEvPtr = (eventStruct *) (ptr + offset); 
      eventSizeType subEvSize = subEvPtr->eventHeader.eventSize;

      errorCounter += ProcessDATEEvent(ptr + offset, oc, event);

      offset += subEvSize;
#ifdef DEBUG 
      printf("\t> offset after %i\n", offset);
#endif
    }
  } else
    errorCounter += ProcessDATEEvent(ptr, oc, event);

  return errorCounter;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessDATEEvent(char *ptr, SimpleVFATFrameCollection *oc, RawEvent *event)
{
  eventHeaderStruct *eventHeader = (eventHeaderStruct *) ptr;

#ifdef DEBUG 
  printf("\t\t>> ProcessDATEEvent\n");

  printf("\t\teventSize = %u\n", eventHeader->eventSize);
  printf("\t\teventMagic = %u\n", eventHeader->eventMagic);
  printf("\t\teventHeadSize = %u\n", eventHeader->eventHeadSize);
  printf("\t\t* eventHeadSize (extra) = %u\n", eventHeader->eventHeadSize - EVENT_HEAD_BASE_SIZE);
  printf("\t\t* eventPayloadSize = %u\n", eventHeader->eventSize - eventHeader->eventHeadSize);
  printf("\t\teventVersion = %u\n", eventHeader->eventVersion);
  printf("\t\teventType = %u\n", eventHeader->eventType);
  printf("\t\teventRunNb = %u\n", eventHeader->eventRunNb);
  printf("\t\teventId = %p\n", (void*) eventHeader->eventId);
  printf("\t\teventTriggerPattern = %p\n", (void*) eventHeader->eventTriggerPattern);
  printf("\t\teventDetectorPattern = %p\n", (void*) eventHeader->eventDetectorPattern);
  printf("\t\teventTypeAttribute = %p\n", (void*) eventHeader->eventTypeAttribute);
  printf("\t\teventLdcId = %u\n", eventHeader->eventLdcId);
  printf("\t\teventGdcId = %u\n", eventHeader->eventGdcId);
  printf("\t\teventTimestamp = %u\n", eventHeader->eventTimestamp);
#endif

  // store important LDC data
  event->ldcTimeStamps[eventHeader->eventLdcId] = eventHeader->eventTimestamp;  

  unsigned long subEvSize = eventHeader->eventSize;
  unsigned long offset = eventHeader->eventHeadSize;

  // process all equipments (OptoRx frames)
  unsigned int errorCounter = 0;
  while (offset < subEvSize)
  {
#ifdef DEBUG 
    printf("\t\toffset (before) %lu\n", offset);
#endif
    
    equipmentHeaderStruct *eq = (equipmentHeaderStruct *) (ptr + offset);
    equipmentSizeType equipmentHeaderStructSize = sizeof(equipmentHeaderStruct);
    signed long payloadSize = eq->equipmentSize - equipmentHeaderStructSize;

    // check for presence of the "0xFAFAFAFA" word (32 bits)
    unsigned long long *payloadPtr = (unsigned long long *)(ptr + offset + equipmentHeaderStructSize);
    if ((*payloadPtr & 0xFFFFFFFF) == 0xFAFAFAFA)
      payloadPtr = (unsigned long long *)(ptr + offset + equipmentHeaderStructSize + 4);

#ifdef DEBUG 
    printf("\t\t\tequipmentSize = %u\n", eq->equipmentSize);
    printf("\t\t\tequipmentType = %u\n", eq->equipmentType);
    printf("\t\t\tequipmentId = %u\n", eq->equipmentId);
    printf("\t\t\tequipmentTypeAttribute = %p\n", (void*) eq->equipmentTypeAttribute);
    printf("\t\t\tequipmentBasicElementSize = %u\n", eq->equipmentBasicElementSize);
    printf("\t\t\t\t\tpayload size = %li\n", payloadSize);
    printf("\t\t\t\t\tpayload ptr = %p\n", (void*) payloadPtr);
#endif

    if (payloadSize > 0)
    {
      payloadSize /= 8; // bytes -> words

      switch (eq->equipmentType)
      {
        case etOptoRxVME:
        case etOptoRxSRS:
          errorCounter += SRSFile::ProcessOptoRxFrame(payloadPtr, payloadSize, oc, event);
          break;

        default:
          ERROR("SRSFile::ProcessDATEEvent") << "Unknown equipment type: " << eq->equipmentType << ". Skipping." << c_endl;
      }
    } 

    offset += eq->equipmentSize;

#ifdef DEBUG 
    printf("\t\toffset (after) %lu\n", offset);
#endif
  }

  return errorCounter;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessOptoRxFrame(SRSFile::word *buf, unsigned int frameSize,
  SimpleVFATFrameCollection *fc, RawEvent *event)
{
    // get OptoRx metadata
    unsigned long long head = buf[0];
    unsigned long long foot = buf[frameSize-1];

    unsigned int BOE = (head >> 60) & 0xF;
    unsigned int H0 = (head >> 0) & 0xF;

    unsigned long LV1 = (head >> 32) & 0xFFFFFF;
    unsigned long BX = (head >> 20) & 0xFFF;
    unsigned int OptoRxId = (head >> 8) & 0xFFF;
    unsigned int FOV = (head >> 4) & 0xF;

    unsigned int EOE = (foot >> 60) & 0xF;
    unsigned int F0 = (foot >> 0) & 0xF;
    unsigned int FSize = (foot >> 32) & 0x3FF;

    // check header and footer structure
    if (BOE != 5 || H0 != 0 || EOE != 10 || F0 != 0 || FSize != frameSize)
    {
      ERROR("SRSFile::ProcessOptoRxFrame") << "Wrong structure of OptoRx header/footer: "
        << "BOE=" << BOE << ", H0=" << H0 << ", EOE=" << EOE << ", F0=" << F0
        << ", size (OptoRx)=" << FSize << ", size (DATE)=" << frameSize
        << ". OptoRxID=" << OptoRxId << ". Skipping frame." << c_endl;
      return 0;
    }


    #ifdef DEBUG
      printf(">> SRSFile::ProcessOptoRxFrame > OptoRxId = %u, BX = %lu, LV1 = %lu, frameSize = %u, subFrames = %u)\n",
        OptoRxId, BX, LV1, frameSize, subFrames);
    #endif

    // save metadata to event
    if (event)
    {
        OptoRxMetaData &md = event->optoRxMetaData[OptoRxId];
        md.BX = BX;
        md.LV1 = LV1;
    }

    // TODO: remove
    //printf(">> ProcessOptoRxFrame > id = %i, FOV = %i\n", OptoRxId, FOV);

    // is it OptoRx transmitting LoneG data?
    if (OptoRxId == 0x29c)
    {
        return ProcessLoneGFrame(buf + 2, frameSize - 4, event);
    }

    // parallel or serial transmission?
    if (FOV == 1)
      return ProcessOptoRxFrameSerial(buf, frameSize, fc);

    if (FOV == 2)
      return ProcessOptoRxFrameParallel(buf, frameSize, fc);

    ERROR("SRSFile::ProcessOptoRxFrame") << "Unknown FOV = " << FOV << c_endl;
    return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessOptoRxFrameSerial(SRSFile::word *buf, unsigned int frameSize,
  SimpleVFATFrameCollection *fc)
{
  // TODO: remove
  //printf(">> SRSFile::ProcessOptoRxFrameSerial\n");

  // get OptoRx metadata
  unsigned int OptoRxId = (buf[0] >> 8) & 0xFFF;

  // get number of subframes
  unsigned int subFrames = (frameSize - 2) / 194;

  // process all sub-frames
  unsigned int errorCounter = 0;
  for (unsigned int r = 0; r < subFrames; ++r)
  {
    for (unsigned int c = 0; c < 4; ++c)
    {
      unsigned int head = (buf[1 + 194 * r] >> (16 * c)) & 0xFFFF;
      unsigned int foot = (buf[194 + 194 * r] >> (16 * c)) & 0xFFFF;

      #ifdef DEBUG
        printf(">>>> r = %i, c = %i: S = %i, BOF = %i, EOF = %i, ID = %i, ID' = %i\n", r, c, head & 0x1, head >> 12, foot >> 12, (head >> 8) & 0xF, (foot >> 8) & 0xF);
      #endif

      // stop if this GOH is NOT active
      if ((head & 0x1) == 0)
        continue;

      #ifdef DEBUG
        printf("\tHeader active (%04x -> %x).\n", head, head & 0x1);
      #endif

      // check structure
      if (head >> 12 != 0x4 || foot >> 12 != 0xB || ((head >> 8) & 0xF) != ((foot >> 8) & 0xF))
      {
        char ss[500];
        if (head >> 12 != 0x4)
          sprintf(ss, "\n\tHeader is not 0x4 as expected (%x).", head);
        if (foot >> 12 != 0xB)
          sprintf(ss, "\n\tFooter is not 0xB as expected (%x).", foot);
        if (((head >> 8) & 0xF) != ((foot >> 8) & 0xF))
          sprintf(ss, "\n\tIncompatible GOH IDs in header (%x) and footer (%x).", ((head >> 8) & 0xF),
            ((foot >> 8) & 0xF));

        ERROR("SRSFile::ProcessOptoRxFrame") << "Wrong payload structure (in GOH block row " << r <<
          " and column " << c
          << ") in OptoRx frame ID " << OptoRxId << ". GOH block omitted." << ss << c_endl;

        errorCounter++;
        continue;
      }

      // allocate memory for VFAT frames
      unsigned int goh = (head >> 8) & 0xF;
      vector<VFATFrame::word*> dataPtrs;
      for (unsigned int fi = 0; fi < 16; fi++)
      {
        FramePosition fp(0, 0, OptoRxId, goh, fi);
        dataPtrs.push_back( fc->InsertEmptyFrame(fp)->getData() );
      }

      #ifdef DEBUG
        printf(">>>> transposing GOH block at prefix: %i, dataPtrs = %p\n", OptoRxId*192 + goh*16, dataPtrs);
      #endif

      // deserialization
      for (int i = 0; i < 192; i++)
      {
        int iword = 11 - i / 16;  // number of current word (11...0)
        int ibit = 15 - i % 16;   // number of current bit (15...0)
        unsigned int w = (buf[i + 2 + 194 * r] >> (16 * c)) & 0xFFFF;

        // Fill the current bit of the current word of all VFAT frames
        for (int idx = 0; idx < 16; idx++)
        {
          if (w & (1 << idx))
            dataPtrs[idx][iword] |= (1 << ibit);
        }
      }
    }
  }

  return errorCounter;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessOptoRxFrameParallel(SRSFile::word *buf, unsigned int frameSize,
  SimpleVFATFrameCollection *fc)
{
  // TODO: remove
  //printf(">> SRSFile::ProcessOptoRxFrameParallel\n");

  // get OptoRx metadata
  unsigned long long head = buf[0];
  unsigned int OptoRxId = (head >> 8) & 0xFFF;

  // TODO: remove
  /*
  unsigned long BX = (head >> 20) & 0xFFF;
  unsigned long LV1 = (head >> 32) & 0xFFFFFF;
  printf("========================================\n");
  printf("OptoRxId = %u (0x%x), BX = %u, LV1 = %u\n", OptoRxId, OptoRxId, BX, LV1);
  */

  // recast data as buffer or 16bit words
  // NOTE: it would be better to use uint16_t instead of unsigned short, but this requires C++11
  unsigned short *payload = (unsigned short *) (buf + 1); // skip header
  payload += 2;                                           // skip orbit counter block
  unsigned int nWords = (frameSize-2) * 4 - 2;            // strip header, footer and orbit counter block

  // process all VFAT data
  for (unsigned int offset = 0; offset < nWords;)
  {
    unsigned int wordsProcessed = ProcessVFATDataParallel(payload + offset, OptoRxId, fc);
    offset += wordsProcessed;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessVFATDataParallel(unsigned short *buf, unsigned int OptoRxId, SimpleVFATFrameCollection *fc)
{
  // start counting processed words
  unsigned int wordsProcessed = 1;

  // padding word? skip it
  if (buf[0] == 0xFFFF)
    return wordsProcessed;

  // check header flag
  unsigned int hFlag = (buf[0] >> 8) & 0xFF;
  if (hFlag != vmCluster && hFlag != vmRaw)
  {
    ERROR("SRSFile::ProcessVFATDataParallel") << "Unknown header flag " << hFlag << ". Skipping this word." << c_endl;
    return wordsProcessed;
  }

  // compile frame position
  // NOTE: DAQ group uses terms GOH and fiber in the other way
  unsigned int gohIdx = (buf[0] >> 4) & 0xF;
  unsigned int fiberIdx = (buf[0] >> 0) & 0xF;
  FramePosition fp(0, 0, OptoRxId, gohIdx, fiberIdx);

  // TODO: remove
  //printf("\t%i, %i, %i\n", hFlag, fiberIdx, gohIdx);

  // prepare temporary VFAT frame
  VFATFrame f;
  VFATFrame::word *fd = f.getData();

  // copy footprint, BC, EC, Flags, ID, if they exist
  f.presenceFlags = 0;

  if (((buf[wordsProcessed] >> 12) & 0xF) == 0xA)  // BC
  {
    f.presenceFlags |= 0x1;
    fd[11] = buf[wordsProcessed];
    wordsProcessed++;
  }

  if (((buf[wordsProcessed] >> 12) & 0xF) == 0xC)  // EC, flags
  {
    f.presenceFlags |= 0x2;
    fd[10] = buf[wordsProcessed];
    wordsProcessed++;
  }

  if (((buf[wordsProcessed] >> 12) & 0xF) == 0xE)  // ID
  {
    f.presenceFlags |= 0x4;
    fd[9] = buf[wordsProcessed];
    wordsProcessed++;
  }

  // save offset where channel data start
  unsigned int dataOffset = wordsProcessed;

  // find trailer
  if (hFlag == vmCluster)
  {
    unsigned int nCl = 0;
    while ( (buf[wordsProcessed + nCl] >> 12) != 0xF )
      nCl++;

    wordsProcessed += nCl;
  }

  if (hFlag == vmRaw)
    wordsProcessed += 9;

  // process trailer
  unsigned int tSig = buf[wordsProcessed] >> 12;
  unsigned int tErrFlags = (buf[wordsProcessed] >> 8) & 0xF;
  unsigned int tSize = buf[wordsProcessed] & 0xFF;

  f.daqErrorFlags = tErrFlags;

  bool skipFrame = false;
  bool suppressChannelErrors = false;

  if (tSig != 0xF)
  {
    ERROR("SRSFile::ProcessVFATDataParallel") << "Wrong trailer signature (" << tSig << ") at "
      << fp << ". This frame will be skipped." << c_endl;
    skipFrame = true;
  }

  if (tErrFlags != 0)
  {
    ERROR("SRSFile::ProcessVFATDataParallel") << "Error flags not zero (" << tErrFlags << ") at "
      << fp << ". Channel errors will be suppressed." << c_endl;
    suppressChannelErrors = true;
  }

  wordsProcessed++;

  if (tSize != wordsProcessed)
  {
    ERROR("SRSFile::ProcessVFATDataParallel") << "Trailer size (" << tSize << ") does not match with words processed ("
      << wordsProcessed << ") at " << fp << ". This frame will be skipped." << c_endl;
    skipFrame = true;
  }

  if (skipFrame)
    return wordsProcessed;

  // TODO: remove
  //printf("\tBC=%u, EC=%u, ID=%u, footprint OK=%u\n", f->getBC(), f->getEC(), f->getChipID(), f->checkFootprint());

  // get channel data - cluster mode
  if (hFlag == vmCluster)
  {
    unsigned int nCl = 0;
    while ( (buf[dataOffset + nCl] >> 12) != 0xF )
    {
      unsigned short &w = buf[dataOffset + nCl];
      unsigned int clSize = (w >> 8) & 0x7F;
      unsigned int clPos = (w >> 0) & 0xFF;

      // special case: size 0 means chip full
      if (clSize == 0)
        clSize = 128;

      // TODO: remove
      //printf("\t\t %2u --> %04hX (size %i, pos %i)\n", dataOffset+nCl, w, clSize, clPos);

      nCl++;

      // activate channels
      //  convention - range <pos, pos-size+1>
      signed int chMax = clPos;
      signed int chMin = clPos - clSize + 1;
      if (chMax < 0 || chMax > 127 || chMin < 0 || chMin > 127 || chMin > chMax)
      {
        if (!suppressChannelErrors)
          ERROR("SRSFile::ProcessVFATDataParallel") << "Invalid cluster (pos=" << clPos
            << ", size=" << clSize << ", min=" << chMin << ", max=" << chMax << ") at " << fp
            <<". Skipping this cluster." << c_endl;

        continue;
      }

      for (signed int ch = chMin; ch <= chMax; ch++)
      {
        unsigned int wi = ch / 16;
        unsigned int bi = ch % 16;
        fd[wi + 1] |= (1 << bi);
      }
    }
  }

  // get channel data and CRC - raw mode
  if (hFlag == vmRaw)
  {
    // TODO: remove
    //for (unsigned int i = 0; i < 14; i++)
    //.  printf("\t\t %2u --> %04hX\n", i, *(buf+i));

    for (unsigned int i = 0; i < 8; i++)
      fd[8 - i] = buf[dataOffset + i];

    // copy CRC
    f.presenceFlags |= 0x8;
    fd[0] = buf[dataOffset + 8];
  }

  // TODO: remove
  //cout << fp << " > ";
  //f.Print();

  // TODO: remove
  /*
  vector<unsigned char> ch = f->getActiveChannels();
  for (unsigned int i = 0; i < ch.size(); i++)
    printf("%i, ", ch[i]);
  printf("\n");
  */

  // TODO save frame to output
  fc->Insert(fp, f);

  return wordsProcessed;
}

//----------------------------------------------------------------------------------------------------

unsigned int SRSFile::ProcessLoneGFrame(SRSFile::word *oBuf, signed long size, RawEvent *ev)
{
  if (size != 20)
  {
    ERROR("SRSFile::ProcessLoneGFrame") << "Wrong LoneG frame size: " << size << " (shall be 20)." << c_endl;
    return 1;
  }

  if (!ev)
    return 0;

  // buffer mapping: OptoRx buffer --> LoneG buffer
  SRSFile::word buf[5];
  for (unsigned int i = 0; i < 5; i++)
    buf[i] = 0;

  for (unsigned int i = 0; i < 20; i++)
  {
      int row = i / 4;
      int col = i % 4;
      buf[row] |= (oBuf[i] & 0xFFFF) << (col * 16);
  }

  ev->triggerData.type = (buf[0] >> 56) & 0xF;
  ev->triggerData.event_num = (buf[0] >> 32) & 0xFFFFFF;
  ev->triggerData.bunch_num = (buf[0] >> 20) & 0xFFF;
  ev->triggerData.src_id = (buf[0] >> 8) & 0xFFF;

  ev->triggerData.orbit_num = (buf[1] >> 32) & 0xFFFFFFFF;
  ev->triggerData.revision_num = (buf[1] >> 24) & 0xFF;

  ev->triggerData.run_num = (buf[2] >> 32) & 0xFFFFFFFF;
  ev->triggerData.trigger_num = (buf[2] >> 0) & 0xFFFFFFFF;

  ev->triggerData.inhibited_triggers_num = (buf[3] >> 32) & 0xFFFFFFFF;
  ev->triggerData.input_status_bits = (buf[3] >> 0) & 0xFFFFFFFF;

#ifdef DEBUG
  printf(">> SRSFile::ProcessLoneGFrame > size = %li\n", size);
  printf("\ttype = %x, event number = %x, bunch number = %x, id = %x\n",
    ev->triggerData.type, ev->triggerData.event_num, ev->triggerData.bunch_num, ev->triggerData.src_id);
  printf("\torbit number = %x, revision = %x\n",
    ev->triggerData.orbit_num, ev->triggerData.revision_num);
  printf("\trun number = %x, trigger number = %x\n",
    ev->triggerData.run_num, ev->triggerData.trigger_num);
  printf("\tinhibited triggers = %x, input status bits = %x\n",
    ev->triggerData.inhibited_triggers_num, ev->triggerData.input_status_bits);
#endif

  return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned char SRSFile::GetEvent(unsigned long n, RawEvent *event)
{
  if (n >= positions.size())
  {
    ERROR("SRSFile::GetEvent") << "Requested event number (" << n
      << ") is larger than event count (" << positions.size() << ")." << c_endl;
    return 1;
  }

  if (!infile)
  {
    ERROR("SRSFile::GetEvent") << "No file open." << c_endl;
    return 1;
  }

  if (infile->Seek(positions[n]))
  {
    ERROR("SRSFile::GetEvent") << "Seek to pos " << positions[n] << " unsuccessful (ftell="
      << infile->CurrentPosition() << ")." << c_endl;
    return 1;
  }

  return GetNextEvent(event);
}

//----------------------------------------------------------------------------------------------------

void SRSFile::Rewind()
{
  infile->Seek(0);

  corruptedEventCounter = 0;
  positions.clear();
  indexStatus = isNotIndexed;
}


} // namespace
