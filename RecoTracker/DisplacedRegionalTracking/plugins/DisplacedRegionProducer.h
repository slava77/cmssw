#ifndef RecoTracker_DisplacedRegionalTracking_DisplacedRegionProducer_h
#define RecoTracker_DisplacedRegionalTracking_DisplacedRegionProducer_h

#include <list>
#include <vector>
#include <limits>
#include <string>
#include <atomic>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Candidate/interface/VertexCompositeCandidate.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/VertexReco/interface/Vertex.h"

#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"

#include "RecoTracker/DisplacedRegionalTracking/interface/DisplacedRegion.h"

struct DisplacedRegionTFCache {
  DisplacedRegionTFCache() : graphDef(nullptr) {}

  std::atomic<tensorflow::GraphDef *> graphDef;
};

class DisplacedRegionProducer : public edm::stream::EDProducer<edm::GlobalCache<DisplacedRegionTFCache> > {
public:
  DisplacedRegionProducer(const edm::ParameterSet &, const DisplacedRegionTFCache *);
  ~DisplacedRegionProducer() override;
  static std::unique_ptr<DisplacedRegionTFCache> initializeGlobalCache(const edm::ParameterSet &);
  static void globalEndJob(const DisplacedRegionTFCache *);
  void produce(edm::Event &, const edm::EventSetup &) override;

private:
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  edm::EDGetTokenT<edm::View<reco::VertexCompositeCandidate> > trackClustersToken_;

  // clustering parameters
  double rParam_;

  // selection parameters
  double minRadius_;
  double discriminatorCut_;
  std::vector<std::string> input_names_;
  std::vector<std::string> output_names_;

  tensorflow::Session *session_;

  const double getDiscriminatorValue(const DisplacedRegion &, const reco::BeamSpot &) const;
};

#endif
