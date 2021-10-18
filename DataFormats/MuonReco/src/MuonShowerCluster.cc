#include "DataFormats/MuonReco/interface/MuonShowerCluster.h"

/*
 * Create LeafCandidate with (eta,phi) only as CSC rechits has no energy/momentum measurement
 * Pt is set to 1.0 as a place holder, mass is set at 0.
 * Vertex associated with the CSC rechit is set to the origin.
 *
 */
reco::MuonShowerCluster::MuonShowerCluster(const float x,const float y, const float z, const float time,const int size){
    x_  = x;
    y_  = y;
    z_  = z;
    time_ = time;
    size_ = size;

}

reco::MuonShowerCluster::~MuonShowerCluster() {

}
