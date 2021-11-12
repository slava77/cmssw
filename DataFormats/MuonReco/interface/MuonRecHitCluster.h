#ifndef DataFormats_MuonRecHitCluster_h
#define DataFormats_MuonRecHitCluster_h

#include <vector>
#include "DataFormats/Common/interface/SortedCollection.h"
#include "DataFormats/Math/interface/PtEtaPhiMass.h"

namespace reco {

  class MuonRecHitCluster {
  public:
    //default constructor
    MuonRecHitCluster() : x_(0), y_(0), z_(0), size_(0) {}

    MuonRecHitCluster(
        const float x, const float y, const float z, const int size, const int nStation10, const float avgStation10);

    //destructor
    ~MuonRecHitCluster();

    double eta() const { return etaFromXYZ(x_, y_, z_); }
    double phi() const { return std::atan2(y_, x_); }
    double x() const { return x_; }
    double y() const { return y_; }
    double z() const { return z_; }
    double r() const { return std::sqrt(x_ * x_ + y_ * y_); }
    int size() const { return size_; }
    int nStation10() const { return nStation10_; }  // number of stations with at least 10 rechits
    float avgStation10() const {
      return avgStation10_;
    }  // station number with at least 10 rechits weighted by n rechits

  private:
    double x_;
    double y_;
    double z_;
    int size_;
    int nStation10_;
    float avgStation10_;
  };

  typedef std::vector<MuonRecHitCluster> MuonRecHitClusterCollection;
}  // namespace reco
#endif
