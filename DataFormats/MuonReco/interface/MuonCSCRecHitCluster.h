#ifndef DataFormats_MuonCSCRecHitCluster_h
#define DataFormats_MuonCSCRecHitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"

#include "DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h"
#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"

namespace reco {

  class MuonCSCRecHitCluster : public MuonRecHitCluster {
  public:
    typedef edm::Ref<CSCRecHit2DCollection> RechitRef;
    typedef std::vector<RechitRef> Rechits;

    //default constructor
    MuonCSCRecHitCluster() = default;

    MuonCSCRecHitCluster(const float x,
                         const float y,
                         const float z,
                         const int size,
                         const float time,
                         const int nME11_12,
                         const Rechits& rh);

    //destructor
    ~MuonCSCRecHitCluster();

    int nME11_12() const { return nME11_12_; }
    float time() const { return time_; }
    void addDaughter(const RechitRef&);
    Rechits getConstituents() { return rechits_; }

  private:
    Rechits rechits_;
    float time_;
    int nME11_12_;
  };
  inline void MuonCSCRecHitCluster::addDaughter(const RechitRef& cand) { rechits_.push_back(cand); }

  typedef std::vector<MuonCSCRecHitCluster> MuonCSCRecHitClusterCollection;
}  // namespace reco
#endif
