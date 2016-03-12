#ifndef RecoTotemRP_RPRecoDataFormats_RPTrackCandidateCollection_h
#define RecoTotemRP_RPRecoDataFormats_RPTrackCandidateCollection_h
 
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"

#include <map>
 
class RPTrackCandidateCollection : public std::map<RPId, RPTrackCandidate>
{
  public:
    //inline RPId RomanPotId() {return rp_id_;}
    //inline void RomanPotId(RPId rp_id) {rp_id_ = rp_id;}
  private:
    //RPId rp_id_;
};

#endif
