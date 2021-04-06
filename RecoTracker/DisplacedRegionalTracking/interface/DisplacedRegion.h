#ifndef RecoTracker_DisplacedRegionalTracking_DisplacedRegion_h
#define RecoTracker_DisplacedRegionalTracking_DisplacedRegion_h

#include <cmath>
#include <limits>
#include <list>
#include <utility>

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/GeometrySurface/interface/GloballyPositioned.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/Math/interface/Vector3D.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateClosestToPoint.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#define INVALID_DOUBLE (static_cast<double>(std::nan("")))

struct DisplacedRegion {
public:
  DisplacedRegion() : valid_(false), rParam_(INVALID_DOUBLE), sumOfCenters_(0.0, 0.0, 0.0) {}

  DisplacedRegion(const edm::Handle<edm::View<reco::VertexCompositeCandidate> > &, const unsigned, const double);

  ~DisplacedRegion() { constituents_.clear(); }

  const bool valid() const { return valid_; }
  const double rParam() const { return rParam_; }
  const edm::RefVector<edm::View<reco::VertexCompositeCandidate> > &constituents() const { return constituents_; }
  const edm::Ref<edm::View<reco::VertexCompositeCandidate> > constituent(const unsigned i) const {
    return constituents_.at(i);
  }
  const unsigned nConstituents() const { return constituents_.size(); }
  const math::XYZVector &sumOfCenters() const { return sumOfCenters_; }
  const math::XYZVector centerOfMass() const { return sumOfCenters_ * (1.0 / constituents_.size()); }

  const double vx() const { return centerOfMass().x(); }
  const double vy() const { return centerOfMass().y(); }
  const double vz() const { return centerOfMass().z(); }

  void merge(const DisplacedRegion &other);
  void setInvalid() { valid_ = false; }

  template <class T>
  const bool inRegion(const T &) const;
  const bool inRegion(const reco::TransientTrack &, double &) const;

private:
  bool valid_;
  double rParam_;
  edm::RefVector<edm::View<reco::VertexCompositeCandidate> > constituents_;
  math::XYZVector sumOfCenters_;
};

template <class T>
const bool DisplacedRegion::inRegion(const T &other) const {
  const double distance = hypot(hypot(other.vx() - vx(), other.vy() - vy()), other.vz() - vz());
  return (distance < rParam());
}

typedef std::list<DisplacedRegion>::iterator DisplacedRegionItr;

struct Distance {
public:
  Distance(DisplacedRegionItr entity0, DisplacedRegionItr entity1) : entities_(entity0, entity1) {}
  ~Distance() {}
  const double distance() const;
  std::pair<DisplacedRegionItr, DisplacedRegionItr> &entities() { return entities_; }
  const std::pair<DisplacedRegionItr, DisplacedRegionItr> &entities() const { return entities_; }

private:
  std::pair<DisplacedRegionItr, DisplacedRegionItr> entities_;
};

typedef std::list<Distance>::iterator DistanceItr;

#endif
