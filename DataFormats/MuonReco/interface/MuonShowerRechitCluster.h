#ifndef DataFormats_MuonShowerRechitCluster_h
#define DataFormats_MuonShowerRechitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"


namespace reco {

  class MuonShowerRechitCluster  {
  public:
    //TODO: think about how to keep the constituents for both CSC/DT rechits
    typedef edm::Ptr<TrackingRecHit> Constituent;
    typedef std::vector<Constituent> Constituents;

    //default constructor
    MuonShowerRechitCluster():eta_(0.),phi_(0.),x_(0.),y_(0.),z_(0.),time_(-999.),size_(0),nME11_12_(0){}
    
    MuonShowerRechitCluster(const double eta,const double phi,const float x, const float y,const float z, const float time, const int size, const int nME11_12);

    //destructor
    ~MuonShowerRechitCluster() ;

    double eta()    const {return eta_;}
    double phi()    const {return phi_;}
    float x() const {return x_;}
    float y() const {return y_;}
    float z() const {return z_;}
    float time()   const {return time_;}
    int size()     const {return size_;}
    int nME11_12() const {return nME11_12_;}
    void addDaughter(const Constituent &);

   private:
    double eta_;      
    double phi_;      
    float x_;      
    float y_;
    float z_;      
    float time_;      
    int size_;      
    int nME11_12_;     
    Constituents rechits_;     
 
  };
  inline void MuonShowerRechitCluster::addDaughter( const Constituent & cand ) { 
    rechits_.push_back( cand ); 
  }
   typedef std::vector<MuonShowerRechitCluster> MuonShowerRechitClusterCollection;
}
#endif
