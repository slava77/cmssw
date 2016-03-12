#ifndef _TotemAnalysis_TotemNtuplizer_RawDataFormats_h_
#define _TotemAnalysis_TotemNtuplizer_RawDataFormats_h_

#include "TObject.h"
#include <vector>

struct EventMetaData
{
  unsigned long run_no;                 ///< run number in form [run number]*1E4 + [raw-data file index]
  unsigned long event_no;               ///< event number assigned by CMSSW (RawDataSource), counts from 1
  unsigned long daq_event_number;       ///< event number assigned by DAQ
  unsigned long long timestamp;         ///< timestamp of the event (UNIX timestamp), 1s resolution
  std::vector<unsigned int> optoRx_Id;  ///< ID of a given OptoRx (the index of the array)
  std::vector<unsigned int> optoRx_BX;  ///< bunch-crossing number reported by a given OptoRx
  std::vector<unsigned int> optoRx_LV1; ///< LV1 as reported by a given OptoRx

  EventMetaData() : run_no(0), event_no(0), daq_event_number(0), timestamp(0) {}
  virtual ~EventMetaData() {}

  ClassDef(EventMetaData,1)
};

#endif
