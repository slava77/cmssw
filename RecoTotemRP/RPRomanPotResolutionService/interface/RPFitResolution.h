#ifndef RecoTotemRP_RPRomanPotResolutionService_RPFitResolution_h
#define RecoTotemRP_RPRomanPotResolutionService_RPFitResolution_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"

//Data Formats
#include <iostream>
#include <string>
#include <vector>


class RPFitResolution
{
  public:
    RPFitResolution(const edm::ParameterSet& conf);
    RP2DHit Create2DHit(RPId rp_id, const RPFittedTrack &track);
  private:
    double strip_alignment_res_degradation_;
    double var_degrad_;
    int verbosity_;
};

#endif

