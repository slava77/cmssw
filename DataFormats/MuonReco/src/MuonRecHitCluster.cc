#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"

reco::MuonRecHitCluster::MuonRecHitCluster(const float x,const float y, const float z,const int size){
    x_  = x;
    y_  = y;
    z_  = z;
    size_ = size;

}

reco::MuonRecHitCluster::~MuonRecHitCluster() {

}
