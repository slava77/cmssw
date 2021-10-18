#ifndef DataFormats_MuonShowerCSCCluster_h
#define DataFormats_MuonShowerCSCCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"

#include <DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h>
#include "DataFormats/MuonReco/interface/MuonShowerCluster.h"

namespace reco {

  class MuonShowerCSCCluster : public MuonShowerCluster {
  public:
    typedef edm::Ref<CSCRecHit2DCollection>  rechitRef;
    typedef std::vector<rechitRef>  rechits;

    //default constructor
    MuonShowerCSCCluster(){}
    
    MuonShowerCSCCluster(const float x, const float y,const float z, const float time, const int size, const int nME11_12, rechits rh);

    //destructor
    ~MuonShowerCSCCluster() ;

    void addDaughter(const rechitRef &);
    rechits getConstituents(){return rechits_;}

   private:
    rechits rechits_;     
    int nME11_12_;     
 
  };
  inline void MuonShowerCSCCluster::addDaughter( const rechitRef & cand ) { 
    rechits_.push_back( cand ); 
  }

   typedef std::vector<MuonShowerCSCCluster> MuonShowerCSCClusterCollection;
}
#endif
