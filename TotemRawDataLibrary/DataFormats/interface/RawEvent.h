/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*    
****************************************************************************/

#ifndef _Totem_RawEvent_h_
#define _Totem_RawEvent_h_

#include "TotemRawDataLibrary/DataFormats/interface/VFATFrameCollection.h"

#include <ctime>
#include <map>

namespace Totem {



/**
 * Additional data provided in OptoRx headers and footers.
**/
struct OptoRxMetaData
{
  unsigned int BX, LV1;
};

//----------------------------------------------------------------------------------------------------

/**
 * Trigger data provided by LoneG.
**/
struct TriggerData
{
  unsigned char type;
  unsigned int event_num, bunch_num, src_id;
  unsigned int orbit_num;
  unsigned char revision_num;
  unsigned int run_num, trigger_num, inhibited_triggers_num, input_status_bits;
};

//----------------------------------------------------------------------------------------------------

/**
 * Class for raw (unprocessed) event. Consists of VFATFrameColl plus additional information (event number, etc.).
**/
class RawEvent
{
  public:
    /// collection of VFAT frames
    VFATFrameCollection *frames;
    
    /// flag whether the `frames' collection shall be freed by the destructor
    bool ownsCollection;

    /// number of event in the datafile (0xb tag)
    unsigned long dataEventNumber;
   
    /// number of configuration in the datafile (0xb tag)
    unsigned long dataConfNumber;
    
    /// timestamp
    time_t timestamp;

    /// additional data from OptoRx frames, indexed by OptoRxId
    std::map<unsigned int, OptoRxMetaData> optoRxMetaData;

    /// trigger (LoneG) data
    TriggerData triggerData;

    /// map: LDC id -> LDC timestamp
    std::map<unsigned int, time_t> ldcTimeStamps;
    
    inline RawEvent(VFATFrameCollection *_f = NULL, bool _o = true);

    /// copy constructor, the copy object will NOT own the collection
    inline RawEvent(const RawEvent &orig);
    inline ~RawEvent();
};

//----------------------------------------------------------------------------------------------------

RawEvent::RawEvent(VFATFrameCollection *_f, bool _o) : frames(_f), ownsCollection(_o), dataEventNumber(0), dataConfNumber(0)
{
#ifdef DEBUG
  printf(">> RawEvent::RawEvent, this = %p, frames = %p, ownsCollection = %i\n", (void *)this, (void *)frames, ownsCollection);
#endif
}

//----------------------------------------------------------------------------------------------------

RawEvent::RawEvent(const RawEvent &orig) : frames(orig.frames), ownsCollection(false), dataEventNumber(0), dataConfNumber(0)
{
#ifdef DEBUG
  printf(">> RawEvent::RawEvent (copy), this = %p, frames = %p\n", (void *)this, (void *)frames);
#endif
}

//----------------------------------------------------------------------------------------------------

RawEvent::~RawEvent()
{
#ifdef DEBUG
  printf(">> RawEvent::~RawEvent, this = %p, frames = %p, ownsCollection = %i\n", (void *)this, (void *)frames, ownsCollection);
#endif

  if (ownsCollection && frames)
    delete frames;
}

} // namespace

#endif
