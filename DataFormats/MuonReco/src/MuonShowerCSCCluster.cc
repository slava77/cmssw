#include "DataFormats/MuonReco/interface/MuonShowerCSCCluster.h"

/*
 * Create LeafCandidate with (eta,phi) only as CSC rechits has no energy/momentum measurement
 * Pt is set to 1.0 as a place holder, mass is set at 0.
 * Vertex associated with the CSC rechit is set to the origin.
 *
 */
reco::MuonShowerCSCCluster::MuonShowerCSCCluster(const float x,const float y,const float z,const int size, const float time,const int nME11_12, rechits rh)
: MuonShowerCluster(x,y,z,size){

  time_     = time;
  nME11_12_ = nME11_12;
  for(unsigned i=0; i< rh.size();i++){
    addDaughter(rh[i]);
  }   

}

reco::MuonShowerCSCCluster::~MuonShowerCSCCluster() {

}
