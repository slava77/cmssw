#ifndef DataFormats_MuonShowerDTCluster_h
#define DataFormats_MuonShowerDTCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"

#include <DataFormats/DTRecHit/interface/DTRecHitCollection.h>
#include "DataFormats/MuonReco/interface/MuonShowerCluster.h"

namespace reco {

  class MuonShowerDTCluster : public MuonShowerCluster {
  public:
    typedef edm::Ref<DTRecHitCollection>  rechitRef;
    typedef std::vector<rechitRef>  rechits;

    //default constructor
    MuonShowerDTCluster(){}
    
    MuonShowerDTCluster(const float x, const float y,const float z, const int size, const int nMB1, rechits rh);

    //destructor
    ~MuonShowerDTCluster() ;

    int nMB1() const {return nMB1_;}
    void addDaughter(const rechitRef &);
    rechits getConstituents(){return rechits_;}

   private:
    rechits rechits_;     
    int nMB1_;     
 
  };
  inline void MuonShowerDTCluster::addDaughter( const rechitRef & cand ) { 
    rechits_.push_back( cand ); 
  }

   typedef std::vector<MuonShowerDTCluster> MuonShowerDTClusterCollection;
}
#endif
