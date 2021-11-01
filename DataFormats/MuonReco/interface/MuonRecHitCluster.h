#ifndef DataFormats_MuonRecHitCluster_h
#define DataFormats_MuonRecHitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/Math/interface/Vector3D.h"


namespace reco {

  class MuonRecHitCluster  {
  public:

    typedef math::XYZVector Vector;
    //default constructor
    MuonRecHitCluster():x_(0),y_(0),z_(0),size_(0){}
    
    MuonRecHitCluster(const float x, const float y, const float z, const int size);

    //destructor
    ~MuonRecHitCluster() ;

    double eta()   const {return Vector(x_,y_,z_).eta();}
    double phi()   const {return Vector(x_,y_,z_).phi();}
    float x()      const {return x_;}
    float y()      const {return y_;}
    float z()      const {return z_;}
    float r()      const {return std::sqrt(x_*x_+y_*y_);}
    int size()     const {return size_;}

   private:
    float x_; 
    float y_; 
    float z_; 
    int size_;      
 
  };

   typedef std::vector<MuonRecHitCluster> MuonRecHitClusterCollection;
}
#endif
