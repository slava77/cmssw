#include "DataFormats/MuonReco/interface/MuonDTRecHitCluster.h"

reco::MuonDTRecHitCluster::MuonDTRecHitCluster(const float x,
                                               const float y,
                                               const float z,
                                               const int size,
                                               const int nStation10,
                                               const float avgStation10,
                                               const int nMB1,
                                               const int nMB2,
                                               const Rechits& rh)
    : MuonRecHitCluster(x, y, z, size, nStation10, avgStation10) {
  nMB1_ = nMB1;
  nMB2_ = nMB2;
  for (unsigned i = 0; i < rh.size(); i++) {
    addDaughter(rh[i]);
  }
}

reco::MuonDTRecHitCluster::~MuonDTRecHitCluster() {}
