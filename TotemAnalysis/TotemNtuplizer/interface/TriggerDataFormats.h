#ifndef _TotemAnalysis_TotemNtuplizer_TriggerDataFormats_h_
#define _TotemAnalysis_TotemNtuplizer_TriggerDataFormats_h_

#include "TObject.h"

/// a copy of Totem::TriggerData from TotemRawDataLibrary/DataFormats/interface/RawEvent.h
struct TriggerData
{
  unsigned char type;                   ///<
  unsigned int event_num;               ///< incremental counter of triggers accepted by DAQ (thus event counter)
  unsigned int bunch_num;               ///< the number of bunch(-pair) collided in this event
  unsigned int src_id;                  ///<
  unsigned int orbit_num;               ///<
  unsigned char revision_num;           ///<
  unsigned int run_num;                 ///< the run number (without the raw-file index extension)
  unsigned int trigger_num;             ///< incremental trigger counter
  unsigned int inhibited_triggers_num;  ///< incremental counter of triggers rejected by DAQ
  unsigned int input_status_bits;       ///< result of the trigger logic (each bit corresponds to one entry in the trigger menu)

  TriggerData() {}
  virtual ~TriggerData() {}

  ClassDef(TriggerData,1)
};

#endif
