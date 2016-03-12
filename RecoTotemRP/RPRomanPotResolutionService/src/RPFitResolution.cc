#include "RecoTotemRP/RPRomanPotResolutionService/interface/RPFitResolution.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"

#include <iostream>

RPFitResolution::RPFitResolution(const edm::ParameterSet& conf)
{
  strip_alignment_res_degradation_ = conf.getParameter<double>("StripAlignmentResolutionDegradation");
  verbosity_ = conf.getParameter<int>("Verbosity");
  var_degrad_ = strip_alignment_res_degradation_*strip_alignment_res_degradation_;
}


RP2DHit RPFitResolution::Create2DHit(RPId rp_id, const RPFittedTrack &track)
{
  RP2DHit tmp(track.X0(), track.Y0(), track.X0Variance()*var_degrad_, 
      track.Y0Variance()*var_degrad_, track.Z0());

  if(verbosity_)
  {
    std::cout<<"RPFitResolution::Create2DHit"<<std::endl;
    std::cout<<"rp_id="<<rp_id<<" pos=("<<track.X0()<<","<<track.Y0()<<","<<track.Z0()<<")"<<std::endl;
    std::cout<<"\t sigma x="<<track.X0Sigma()<<" sigma y="<<track.Y0Sigma()<<std::endl;
    std::cout<<"\t oryg variance x="<<track.X0Variance()<<" sigma y="<<track.Y0Variance()<<std::endl;
    std::cout<<"\t degrad var x="<<track.X0Variance()*var_degrad_<<" sigma y="<<track.Y0Variance()*var_degrad_<<std::endl;
    std::cout<<"\tvar_degrad_="<<var_degrad_<<std::endl;
    std::cout<<std::endl;
    std::cout<<tmp<<std::endl;
  }
  return tmp;
}

