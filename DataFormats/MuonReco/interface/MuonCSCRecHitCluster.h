#ifndef DataFormats_MuonCSCRecHitCluster_h
#define DataFormats_MuonCSCRecHitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"

#include <DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h>
#include "DataFormats/MuonReco/interface/MuonRecHitCluster.h"

namespace reco {

  class MuonCSCRecHitCluster : public MuonRecHitCluster {
  public:
    typedef edm::Ref<CSCRecHit2DCollection>  rechitRef;
    typedef std::vector<rechitRef>  rechits;

    //default constructor
    MuonCSCRecHitCluster(){}
    
    MuonCSCRecHitCluster(const float x, const float y,const float z, const int size, const float time, const int nME11_12, rechits rh);

    //destructor
    ~MuonCSCRecHitCluster() ;

    int nME11_12() const {return nME11_12_;}
    float time()   const {return time_;}
    void addDaughter(const rechitRef &);
    rechits getConstituents(){return rechits_;}

   private:
    rechits rechits_;     
    float time_;      
    int nME11_12_;     
 
  };
  inline void MuonCSCRecHitCluster::addDaughter( const rechitRef & cand ) { 
    rechits_.push_back( cand ); 
  }

   typedef std::vector<MuonCSCRecHitCluster> MuonCSCRecHitClusterCollection;
}
#endif
