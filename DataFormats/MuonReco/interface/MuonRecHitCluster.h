#ifndef DataFormats_MuonRecHitCluster_h
#define DataFormats_MuonRecHitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/Math/interface/PtEtaPhiMass.h"


namespace reco {

  class MuonRecHitCluster  {
  public:

    //default constructor
    MuonRecHitCluster():x_(0),y_(0),z_(0),size_(0){}
    
    MuonRecHitCluster(const float x, const float y, const float z, const int size);

    //destructor
    ~MuonRecHitCluster() ;

    double eta()   const {return etaFromXYZ(x_,y_,z_);}
    double phi()   const {return std::atan2(y_,x_);}
    double x()      const {return x_;}
    double y()      const {return y_;}
    double z()      const {return z_;}
    double r()      const {return std::sqrt(x_*x_+y_*y_);}
    int size()     const {return size_;}

   private:
    double x_; 
    double y_; 
    double z_; 
    int size_;      
 
  };

   typedef std::vector<MuonRecHitCluster> MuonRecHitClusterCollection;
}
#endif
