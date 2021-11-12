#include "DataFormats/MuonReco/interface/MuonCSCRecHitCluster.h"

/*
 * Create LeafCandidate with (eta,phi) only as CSC rechits has no energy/momentum measurement
 * Pt is set to 1.0 as a place holder, mass is set at 0.
 * Vertex associated with the CSC rechit is set to the origin.
 *
 */
reco::MuonCSCRecHitCluster::MuonCSCRecHitCluster(const float x,
                                                 const float y,
                                                 const float z,
                                                 const int size,
                                                 const int nStation10,
                                                 const float avgStation10,
                                                 const float time,
                                                 const float timeSpread,
                                                 const int nME11,
                                                 const int nME12,
                                                 const Rechits& rh)
    : MuonRecHitCluster(x, y, z, size, nStation10, avgStation10) {
  time_ = time;
  timeSpread_ = timeSpread;
  nME11_ = nME11;
  nME12_ = nME12;
  for (unsigned i = 0; i < rh.size(); i++) {
    addDaughter(rh[i]);
  }
}

reco::MuonCSCRecHitCluster::~MuonCSCRecHitCluster() {}
