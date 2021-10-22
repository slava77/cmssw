#include "DataFormats/MuonReco/interface/MuonShowerCluster.h"

reco::MuonShowerCluster::MuonShowerCluster(const float x,const float y, const float z,const int size){
    x_  = x;
    y_  = y;
    z_  = z;
    size_ = size;

}

reco::MuonShowerCluster::~MuonShowerCluster() {

}
