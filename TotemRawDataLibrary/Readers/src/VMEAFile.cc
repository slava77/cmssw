/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*    
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/VMEAFile.h"
#include "TotemRawDataLibrary/Readers/interface/VME2File.h"

// the RunI version
#include "TotemRawDataLibrary/DAQA/interface/event_3_9.h"

using namespace std;

namespace Totem {

const unsigned int VMEAFile::eventHeaderSize = sizeof(eventHeaderStruct);

//----------------------------------------------------------------------------------------------------

VMEAFile::VMEAFile() : dataPtr(NULL), dataPtrSize(0), infile(NULL)
{
#ifdef DEBUG
  printf(">> VMEAFile::VMEAFile \n");
#endif
}

//----------------------------------------------------------------------------------------------------

VMEAFile::~VMEAFile()
{
#ifdef DEBUG
  printf(">> VMEAFile::~VMEAFile, this = %p, dataPtr = %p\n", (void *) this, dataPtr);
#endif

  Close();

  if(dataPtr != NULL)
    delete [] dataPtr;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VMEAFile::Open(const std::string &fn)
{
  // check if the source is a VMEA File
  size_t dotPos = fn.rfind('.');
  string extension = (dotPos == string::npos) ? "" : fn.substr(dotPos);
  if (extension.compare(".vmea") != 0)
    return osWrongFormat;

  infile = StorageFile::CreateInstance(fn);
  if(!infile) {
    return osCannotOpen;
  }

  infile->OpenFile();

  if (!infile->IsOpened()) {
    infile->PrintError("Error while opening file in VMEAFile::Open");
    return osCannotOpen;
  }

  // set maximum of data counters
  dataEventNumberMax = 0xFFFFFFFF;
  dataConfNumberMax = 0;

  // reset counters
  indexStatus = isNotIndexed;
  corruptedEventCounter = 0;

  filename = fn;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VMEAFile::Open(StorageFile *storageFile)
{
  std::string fn = storageFile->GetURLPath();

  // check if the source is a VMEA File
  size_t dotPos = fn.rfind('.');
  string extension = (dotPos == string::npos) ? "" : fn.substr(dotPos);
  if (extension.compare(".vmea") != 0)
    return osWrongFormat;

  infile = storageFile;
  infile->OpenFile();

  if (!infile->IsOpened()) {
    infile->PrintError("Error while opening file in VMEAFile::Open");
    return osCannotOpen;
  }

  // set maximum of data counters
  dataEventNumberMax = 0xFFFFFFFF;
  dataConfNumberMax = 0;

  // reset counters
  indexStatus = isNotIndexed;
  corruptedEventCounter = 0;

  filename = fn;

  return osOK;
}

//----------------------------------------------------------------------------------------------------

void VMEAFile::Close()
{
#ifdef DEBUG
  printf(">> VMEAFile::Close, this = %p", (void*) this);
#endif

  if(infile && infile->CloseFile()==EOF) {
      ERROR("VMEAFile::Close") << "Cannot close the file." << c_endl;
    delete infile;
  }
}

//----------------------------------------------------------------------------------------------------

unsigned char VMEAFile::ReadToBuffer(unsigned int bytesToRead, unsigned int offset)
{
#ifdef DEBUG
  printf(">> VMEAFile::ReadToBuffer(%u, %u), this = %p, dataPtr = %p, dataPtrSize = %u\n", 
      bytesToRead, offset, (void*) this, dataPtr, dataPtrSize);
#endif

  // allocate new memory block if current one is too small
  if(dataPtrSize<bytesToRead+offset) {
    char *newPtr = NULL;
    try {
      newPtr = new char[bytesToRead+offset];
    }
    catch (bad_alloc& ba) {
      ERROR("VMEAFile::ReadToBuffer") << "Cannot allocate buffer large enough." << c_endl;
      return 2;
    }

    if (dataPtr) {
      if (offset > 0)
        memcpy(newPtr, dataPtr, offset);
      delete [] dataPtr;
    }

    dataPtr = newPtr;
    dataPtrSize = bytesToRead+offset;
  }

  // read data at given offset
  unsigned int bytesRead = infile->ReadData(dataPtr + offset, sizeof(char), bytesToRead);
  int eofFlag = infile->CheckEOF();

  if (bytesRead != bytesToRead && !(bytesRead == 0 && eofFlag)) 
  {
    ERROR("VMEAFile::ReadToBuffer") << "Reading from file to buffer failed. Only " << bytesRead
  			      << " B read from " << bytesToRead << " B." << c_endl;
    return 1;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned char VMEAFile::GetNextEvent(RawEvent *event)
{
#ifdef DEBUG
  printf(">> VMEAFile::GetNextEvent, this = %p\n", (void*)this);
  printf("\teventHeaderSize = %u\n", eventHeaderSize);
#endif

  eventHeaderStruct *eventHeader = NULL;

  long int currentPos = infile->CurrentPosition();
  while (!infile->CheckEOF()) {
    // read next header
    if (ReadToBuffer(eventHeaderSize, 0) != 0)
      return 10;

    eventHeader = (eventHeaderStruct *) dataPtr;

    // check the sanity of header data
    if (eventHeader->eventMagic != EVENT_MAGIC_NUMBER) {
      ERROR("VMEAFile::GetNextEvent") << "Event magic check failed (" << hex << eventHeader->eventMagic << "!=" << EVENT_MAGIC_NUMBER << dec << "). Exiting." << c_endl;
      return 1;
    }

    unsigned int N = eventHeader->eventSize;
    if (N<eventHeaderSize) {
      ERROR("VMEAFile::GetNextEvent") << "Event size (" << N << ") smaller than header size (" << eventHeaderSize << "). Exiting." << c_endl;
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
  OptoRxVFATFrameCollection *oc = (OptoRxVFATFrameCollection *) event->frames;
  oc->Invalidate();
  unsigned int errorCounter = ProcessVMEAEvent(dataPtr, oc, event);

#ifdef DEBUG
  printf("* %u, %u, %u\n",
    EVENT_ID_GET_NB_IN_RUN( eventHeader->eventId ),
    EVENT_ID_GET_BURST_NB( eventHeader->eventId ),
    EVENT_ID_GET_NB_IN_BURST( eventHeader->eventId )
    );
#endif

  // notify about errors
  if (errorCounter > 0)
  {
    ERROR("VMEAFile::GetNextEvent") << errorCounter << " GOH blocks have failed consistency checks in event "
      << event->dataEventNumber << "." << c_endl;
    corruptedEventCounter++;
  }

  // save event position
  if (indexStatus == isIndexing)
    positions.push_back(currentPos);

  return 0;
}

//----------------------------------------------------------------------------------------------------

unsigned int VMEAFile::ProcessVMEAEvent(char *ptr, OptoRxVFATFrameCollection *oc, RawEvent *event)
{
  eventHeaderStruct *eventHeader = (eventHeaderStruct *) ptr;
  bool superEvent = TEST_ANY_ATTRIBUTE(eventHeader->eventTypeAttribute, ATTR_SUPER_EVENT);

#ifdef DEBUG
  printf(">> VMEAFile::ProcessVMEAEvent\n");

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
  
  // store important GDC data
  event->dataEventNumber = EVENT_ID_GET_NB_IN_RUN(eventHeader->eventId) - 1;
  event->timestamp = eventHeader->eventTimestamp;

  eventSizeType eventSize = eventHeader->eventSize;
  eventHeadSizeType headSize = eventHeader->eventHeadSize;

  // process all sub-events (LDC frames)
  unsigned int errorCounter = 0;
  if (superEvent) {
    unsigned int offset = headSize;
    while (offset < eventSize) {
#ifdef DEBUG 
      printf("\t> offset before %i\n", offset);
#endif
      eventStruct *subEvPtr = (eventStruct *) (ptr + offset); 
      eventSizeType subEvSize = subEvPtr->eventHeader.eventSize;

      errorCounter += ProcessSubEvent(ptr + offset, oc, event);

      offset += subEvSize;
#ifdef DEBUG 
      printf("\t> offset after %i\n", offset);
#endif
    }
  } else
    errorCounter += ProcessSubEvent(ptr, oc, event);

  return errorCounter;
}

//----------------------------------------------------------------------------------------------------

unsigned int VMEAFile::ProcessSubEvent(char *ptr, OptoRxVFATFrameCollection *oc, RawEvent *event)
{
  eventHeaderStruct *eventHeader = (eventHeaderStruct *) ptr;

#ifdef DEBUG 
  printf("\t\t>> ProcessSubEvent\n");

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
  while (offset < subEvSize) {
#ifdef DEBUG 
    printf("\t\toffset (before) %lu\n", offset);
#endif
    
    equipmentHeaderStruct *eq = (equipmentHeaderStruct *) (ptr + offset);
    equipmentSizeType equipmentHeaderStructSize = sizeof(equipmentHeaderStruct);
    signed long payloadSize = eq->equipmentSize - equipmentHeaderStructSize;
    unsigned long long *payloadPtr = (unsigned long long *)(ptr + offset + equipmentHeaderStructSize);

#ifdef DEBUG 
    printf("\t\t\tequipmentSize = %u\n", eq->equipmentSize);
    printf("\t\t\tequipmentType = %u\n", eq->equipmentType);
    printf("\t\t\tequipmentId = %u\n", eq->equipmentId);
    printf("\t\t\tequipmentTypeAttribute = %p\n", (void*) eq->equipmentTypeAttribute);
    printf("\t\t\tequipmentBasicElementSize = %u\n", eq->equipmentBasicElementSize);
    printf("\t\t\t\t\tpayload size = %li\n", payloadSize);
    printf("\t\t\t\t\tpayload ptr = %p\n", (void*) payloadPtr);
#endif

    if (payloadSize > 0) {
      payloadSize /= 8; // bytes -> words

      switch (eq->equipmentType) {
        case etOptoRxOld:
        case etOptoRx:
          errorCounter += VME2File::ProcessOptoRxFrame(payloadPtr, payloadSize, oc, event);
          break;

        case etLoneG:
          ProcessLoneGFrame(payloadPtr, payloadSize, event);
          break;

        default:
          ERROR("VMEAFile::ProcessSubEvent") << "Unknown equipment type: " << eq->equipmentType << ". Skipping." << c_endl;
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

unsigned char VMEAFile::GetEvent(unsigned long n, RawEvent *event)
{
  if (n >= positions.size()) {
    ERROR("VMEAFile::GetEvent") << "Requested event number (" << n
      << ") is larger than event count (" << positions.size() << ")." << c_endl;
    return 1;  
  }

  if (!infile) {
    ERROR("VMEAFile::GetEvent") << "No file open." << c_endl;
    return 1;
  }

  if (infile->Seek(positions[n])) {
    ERROR("VMEAFile::GetEvent") << "Seek to pos " << positions[n] << " unsuccessful (ftell="
      << infile->CurrentPosition() << ")." << c_endl;
    return 1;
  }

  return GetNextEvent(event);
}

//----------------------------------------------------------------------------------------------------

void VMEAFile::Rewind()
{
  infile->Seek(0);

  corruptedEventCounter = 0;
  positions.clear();
  indexStatus = isNotIndexed;
}

//----------------------------------------------------------------------------------------------------

unsigned int VMEAFile::ProcessLoneGFrame(VMEAFile::word *buf, signed long size, RawEvent *ev)
{
  if (size != 5) {
    ERROR("VMEAFile::ProcessLoneGFrame") << "Wrong LoneG frame size: " << size << " (shall be 5)." << c_endl;
    return 1;
  }

  if (!ev)
    return 0;

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
  printf(">> VMEAFile::ProcessLoneGFrame > size = %li\n", size);
  printf("\ttype = %i, event number = %u, bunch number = %u, id = %u\n",
    ev->triggerData.type, ev->triggerData.event_num, ev->triggerData.bunch_num, ev->triggerData.src_id);
  printf("\torbit number = %u, revision = %u\n",
    ev->triggerData.orbit_num, ev->triggerData.revision_num);
  printf("\trun number = %u, trigger number = %u\n",
    ev->triggerData.run_num, ev->triggerData.trigger_num);
  printf("\tinhibited triggers = %u, input status bits = %u\n",
    ev->triggerData.inhibited_triggers_num, ev->triggerData.input_status_bits);
#endif

  return 0;
}


} // namespace
