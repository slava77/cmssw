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
                         const int nStation10,
                         const float avgStation10,
                         const float time,
                         const float timeSpread,
                         const int nME11,
                         const int nME12,
                         const Rechits& rh);

    //destructor
    ~MuonCSCRecHitCluster();

    int nME11() const { return nME11_; }
    int nME12() const { return nME12_; }
    float time() const { return time_; }
    float timeSpread() const { return timeSpread_; }
    void addDaughter(const RechitRef&);
    Rechits getConstituents() { return rechits_; }

  private:
    Rechits rechits_;
    float time_;
    float timeSpread_;
    int nME11_;
    int nME12_;
  };
  inline void MuonCSCRecHitCluster::addDaughter(const RechitRef& cand) { rechits_.push_back(cand); }

  typedef std::vector<MuonCSCRecHitCluster> MuonCSCRecHitClusterCollection;
}  // namespace reco
#endif
