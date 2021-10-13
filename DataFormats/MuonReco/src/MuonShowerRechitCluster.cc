#include "DataFormats/MuonReco/interface/MuonShowerRechitCluster.h"

/*
 * Create LeafCandidate with (eta,phi) only as CSC rechits has no energy/momentum measurement
 * Pt is set to 1.0 as a place holder, mass is set at 0.
 * Vertex associated with the CSC rechit is set to the origin.
 *
 */
reco::MuonShowerRechitCluster::MuonShowerRechitCluster(const double eta,const double phi,const float x,const float y,const float z, const float time,const int size,const int nME11_12){
    x_ = x;
    y_ = y;
    z_ = z;
    time_ = time;
    nME11_12_ = nME11_12;
    eta_ = eta;
    phi_ = phi;
    size_ = size;
    

}

reco::MuonShowerRechitCluster::~MuonShowerRechitCluster() {

}
