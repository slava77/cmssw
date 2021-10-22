#include "DataFormats/MuonReco/interface/MuonShowerDTCluster.h"

reco::MuonShowerDTCluster::MuonShowerDTCluster(const float x,const float y,const float z,const int size,const int nMB1, rechits rh)
: MuonShowerCluster(x,y,z,size){

  nMB1_ = nMB1;
  for(unsigned i=0; i< rh.size();i++){
    addDaughter(rh[i]);
  }   

}

reco::MuonShowerDTCluster::~MuonShowerDTCluster() {

}
