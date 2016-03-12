#ifndef RecoTotemRP_RPRecoDataFormats_RPTrackCandidateDistinctCollectionsSet_h
#define RecoTotemRP_RPRecoDataFormats_RPTrackCandidateDistinctCollectionsSet_h
 
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include <vector>
#include <map>


class RPTrackCandidateDistinctCollectionsSet
   : public std::map<RPId, std::vector<RPTrackCandidateCollection> >
{
  public:
    //inline RPId RomanPotId() {return rp_id_;}
    //inline void RomanPotId(RPId rp_id) {rp_id_ = rp_id;}
  private:
    //RPId rp_id_;
};


#endif
