#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"

reco::MuonRecHitCluster::MuonRecHitCluster(
    const float x, const float y, const float z, const int size, const int nStation10, const float avgStation10) {
  x_ = x;
  y_ = y;
  z_ = z;
  size_ = size;
  nStation10_ = nStation10;
  avgStation10_ = avgStation10;
}

reco::MuonRecHitCluster::~MuonRecHitCluster() {}
