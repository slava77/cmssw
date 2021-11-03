#include "DataFormats/MuonReco/interface/MuonDTRecHitCluster.h"

reco::MuonDTRecHitCluster::MuonDTRecHitCluster(
    const float x, const float y, const float z, const int size, const int nMB1, const Rechits& rh)
    : MuonRecHitCluster(x, y, z, size) {
  nMB1_ = nMB1;
  for (unsigned i = 0; i < rh.size(); i++) {
    addDaughter(rh[i]);
  }
}

reco::MuonDTRecHitCluster::~MuonDTRecHitCluster() {}
