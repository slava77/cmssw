/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/VMEAStream.h"
#include "TotemRawDataLibrary/Readers/interface/VMEAFile.h"

// the RunI version
#include "TotemRawDataLibrary/DAQA/interface/event_3_9.h"

#ifdef USE_DAQA
  #include "TotemRawDataLibrary/DAQA/interface/monitor.h"
  #include <cstdlib>
#endif


//#define DEBUG 1

using namespace std;

namespace Totem {

//----------------------------------------------------------------------------------------------------

VMEAStream::VMEAStream()
{
}

//----------------------------------------------------------------------------------------------------

VMEAStream::~VMEAStream()
{
  Close();
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VMEAStream::Open(const std::string &filename)
{
#ifdef DEBUG
  printf(">> VMEAStream::Open\n");
#endif
  // check if the source is a VMEA Stream
  size_t ddotPos = filename.rfind("://");
  string prefix = filename.substr(0, ddotPos);
  if (prefix.compare("vmeastream") != 0)
    return osWrongFormat;
  
  string fn = filename.substr(ddotPos+3).c_str();
  
#ifdef USE_DAQA
  // open the stream
#ifdef DEBUG
  printf("\tmonitorSetDataSource(%s)\n", fn.c_str());
#endif
  int status = monitorSetDataSource((char *) fn.c_str());
#ifdef DEBUG
  printf("\t\tstatus = %i\n", status);
#endif
  if (status != 0)
    return osCannotOpen;

  // set maximum of data counters
  dataEventNumberMax = 0xFFFFFFFF;
  dataConfNumberMax = 0x0;

  // reset counters
  corruptedEventCounter = 0;
  eventCounter = 0;

  return osOK;
#else
  ERROR("VMEAStream::Open") << "Sorry, the class has been compiled without DAQA support." << c_endl;
  return osCannotOpen;
#endif
}

//----------------------------------------------------------------------------------------------------

DataFile::OpenStatus VMEAStream::Open(StorageFile*)
{
  //todo: does it make no sense?
  return osOK;
}

//----------------------------------------------------------------------------------------------------

void VMEAStream::Close()
{
}

//----------------------------------------------------------------------------------------------------

unsigned char VMEAStream::GetNextEvent(RawEvent*
  #ifdef USE_DAQA
    event
  #endif
  )
{
#ifdef DEBUG
  printf(">> VMEAStream::GetNextEvent\n");
#endif

#ifdef USE_DAQA
  void *ptr = NULL;
  eventHeaderStruct *eventHeader;
  while (true) {
    // get next event from the stream
#ifdef DEBUG
    printf("\tmonitorGetEventDynamic\n");
#endif
    int status = monitorGetEventDynamic(&ptr);
#ifdef DEBUG
    printf("\t\tstatus = %i\n", status);
#endif
    if (status != 0) {
      return 1;
    }

    // check whether it is a PHYSICS event
    eventHeader = (eventHeaderStruct *) ptr;
    if (eventHeader->eventType != PHYSICS_EVENT) {
      free(ptr);
    } else
      break;
  }

  // process the data
  OptoRxVFATFrameCollection *oc = (OptoRxVFATFrameCollection *) event->frames;
  oc->Invalidate();
  unsigned int errorCounter = VMEAFile::ProcessVMEAEvent((char *)ptr, oc, event); 
  event->dataEventNumber = EVENT_ID_GET_NB_IN_RUN(eventHeader->eventId) - 1;
  event->timestamp = eventHeader->eventTimestamp;
  
  if (errorCounter > 0) {
    ERROR("VMEAStream::GetNextEvent") << errorCounter << " GOH blocks have failed consistency checks in event "
      << event->dataEventNumber << "." << c_endl;
    corruptedEventCounter++;
  }

  free(ptr);
  eventCounter++;
  return 0;
#else
  ERROR("VMEAStream::GetNextEvent") << "Sorry, the class has been compiled without DAQA support." << c_endl;
  return 1;
#endif
}
    
//----------------------------------------------------------------------------------------------------

/// random event access not supported by this class
void VMEAStream::Rewind()
{
  ERROR("VMEAStream::Rewind") << "This method is not supported." << c_endl;
}

//----------------------------------------------------------------------------------------------------
    
/// random event access not supported by this class
unsigned char VMEAStream::GetEvent(unsigned long, RawEvent*)
{
  ERROR("VMEAStream::GetEvent") << "This method is not supported." << c_endl;
  return 1;
}

} // namespace

