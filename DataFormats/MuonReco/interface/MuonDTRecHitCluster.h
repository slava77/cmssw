#ifndef DataFormats_MuonDTRecHitCluster_h
#define DataFormats_MuonDTRecHitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"

#include "DataFormats/DTRecHit/interface/DTRecHitCollection.h"
#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"

namespace reco {

  class MuonDTRecHitCluster : public MuonRecHitCluster {
  public:
    typedef edm::Ref<DTRecHitCollection>  RechitRef;
    typedef std::vector<RechitRef>  Rechits;

    //default constructor
    MuonDTRecHitCluster() = default;
    
    MuonDTRecHitCluster(const float x, const float y,const float z, const int size, const int nMB1,const Rechits& rh);

    //destructor
    ~MuonDTRecHitCluster() ;

    int nMB1() const {return nMB1_;}
    void addDaughter(const RechitRef &);
    Rechits getConstituents(){return rechits_;}

   private:
    Rechits rechits_;     
    int nMB1_;     
 
  };
  inline void MuonDTRecHitCluster::addDaughter( const RechitRef & cand ) { 
    rechits_.push_back( cand ); 
  }

   typedef std::vector<MuonDTRecHitCluster> MuonDTRecHitClusterCollection;
}
#endif
