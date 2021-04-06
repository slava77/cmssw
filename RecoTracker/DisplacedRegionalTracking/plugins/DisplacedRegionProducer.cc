#include "RecoTracker/DisplacedRegionalTracking/plugins/DisplacedRegionProducer.h"

using namespace std;

DisplacedRegionProducer::DisplacedRegionProducer(const edm::ParameterSet &cfg, const DisplacedRegionTFCache *cache)
    : rParam_(cfg.getParameter<double>("rParam")),

      minRadius_(cfg.getParameter<double>("minRadius")),
      discriminatorCut_(cfg.getParameter<double>("discriminatorCut")),
      input_names_(cfg.getParameter<vector<string> >("input_names")),
      output_names_(cfg.getParameter<vector<string> >("output_names")) {
  beamSpotToken_ = consumes<reco::BeamSpot>(cfg.getParameter<edm::InputTag>("beamSpot"));
  trackClustersToken_ =
      consumes<edm::View<reco::VertexCompositeCandidate> >(cfg.getParameter<edm::InputTag>("trackClusters"));

  unsigned nThreads = cfg.getParameter<unsigned>("nThreads");
  string singleThreadPool = cfg.getParameter<string>("singleThreadPool");
  tensorflow::SessionOptions sessionOptions;
  tensorflow::setThreading(sessionOptions, nThreads, singleThreadPool);
  session_ = tensorflow::createSession(cache->graphDef, sessionOptions);

  produces<vector<reco::Vertex> >();
}

DisplacedRegionProducer::~DisplacedRegionProducer() {
  if (session_ != nullptr)
    tensorflow::closeSession(session_);
}

unique_ptr<DisplacedRegionTFCache> DisplacedRegionProducer::initializeGlobalCache(const edm::ParameterSet &cfg) {
  tensorflow::setLogging("3");
  string pbFile = cfg.getParameter<edm::FileInPath>("graph_path").fullPath();
  DisplacedRegionTFCache *cache = new DisplacedRegionTFCache();
  cache->graphDef = tensorflow::loadGraphDef(pbFile);

  return unique_ptr<DisplacedRegionTFCache>(cache);
}

void DisplacedRegionProducer::globalEndJob(const DisplacedRegionTFCache *cache) {
  if (cache->graphDef != nullptr)
    delete cache->graphDef;
}

void DisplacedRegionProducer::produce(edm::Event &event, const edm::EventSetup &setup) {
  edm::Handle<reco::BeamSpot> beamSpot;
  edm::Handle<edm::View<reco::VertexCompositeCandidate> > trackClusters;

  event.getByToken(beamSpotToken_, beamSpot);
  const math::XYZVector bs(beamSpot->x0(), beamSpot->y0(), beamSpot->z0());
  event.getByToken(trackClustersToken_, trackClusters);

  // Initialize distances.
  list<DisplacedRegion> pseudoROIs;
  list<Distance> distances;
  const double minTrackClusterRadius = minRadius_ - rParam_;
  for (unsigned i = 0; i < trackClusters->size(); i++) {
    const reco::VertexCompositeCandidate &trackCluster = trackClusters->at(i);
    const math::XYZVector x(trackCluster.vx(), trackCluster.vy(), trackCluster.vz());
    if (minRadius_ < 0.0 || minTrackClusterRadius < 0.0 || (x - bs).rho() > minTrackClusterRadius)
      pseudoROIs.emplace_back(trackClusters, i, rParam_);
  }
  if (pseudoROIs.size() > 1) {
    DisplacedRegionItr secondToLast = pseudoROIs.end();
    secondToLast--;
    for (DisplacedRegionItr i = pseudoROIs.begin(); i != secondToLast; i++) {
      DisplacedRegionItr j = i;
      j++;
      for (; j != pseudoROIs.end(); j++)
        distances.emplace_back(i, j);
    }
  }

  // Track clusters farther apart than 4 times rParam_ cannot wind up in the
  // same ROI, so remove these pairs.
  const auto pred = [&](const Distance &a) { return a.distance() > 4.0 * rParam_; };
  distances.remove_if(pred);

  // Do clustering.
  while (!distances.empty()) {
    const auto comp = [](const Distance &a, const Distance &b) { return a.distance() <= b.distance(); };
    distances.sort(comp);
    DistanceItr d = distances.begin();
    if (d->distance() > rParam_)
      break;

    d->entities().first->merge(*d->entities().second);
    d->entities().second->setInvalid();

    const auto distancePred = [](const Distance &a) {
      return (!a.entities().first->valid() || !a.entities().second->valid());
    };
    const auto pseudoROIPred = [](const DisplacedRegion &a) { return !a.valid(); };
    distances.remove_if(distancePred);
    pseudoROIs.remove_if(pseudoROIPred);
  }

  // Remove invalid ROIs.
  const auto roiPred = [&](const DisplacedRegion &roi) {
    if (!roi.valid())
      return true;
    const math::XYZVector x(roi.vx(), roi.vy(), roi.vz());
    if ((x - bs).rho() < minRadius_)
      return true;
    const double discriminatorValue = ((discriminatorCut_ > 0.0) ? getDiscriminatorValue(roi, *beamSpot) : 1.0);
    if (discriminatorValue < discriminatorCut_)
      return true;
    return false;
  };
  pseudoROIs.remove_if(roiPred);

  auto regionsOfInterest = make_unique<vector<reco::Vertex> >();
  regionsOfInterest->clear();

  const vector<double> error = {1.0, 0.0, 1.0, 0.0, 0.0, 1.0};

  for (const auto &roi : pseudoROIs)
    regionsOfInterest->emplace_back(reco::Vertex::Point(roi.vx(), roi.vy(), roi.vz()),
                                    reco::Vertex::Error(error.begin(), error.end(), true, true));

  event.put(move(regionsOfInterest));
}

const double DisplacedRegionProducer::getDiscriminatorValue(const DisplacedRegion &roi,
                                                            const reco::BeamSpot &bs) const {
  // The network takes in two maps of data features, one with information
  // related to the pairwise track vertices and one with information related to
  // the tracks in an isolation annulus.

  const int maxNVertices = 40;     // maximum number of pairwise track vertices per ROI
  const int nVertexFeatures = 23;  // number of features per pairwise track vertex
  const int nDimVertices = 3;      // number of tensor dimensions for the map of pairwise track vertices

  const int maxNAnnulusTracks = 10;     // maximum number of annulus tracks per ROI
  const int nAnnulusTrackFeatures = 8;  // number of features per annulus track
  const int nDimAnnulusTracks = 3;      // number of tensor dimensions for the map of annulus tracks

  tensorflow::Tensor vertexTensor(tensorflow::DT_FLOAT, tensorflow::TensorShape({1, maxNVertices, nVertexFeatures}));
  auto vertex_map = vertexTensor.tensor<float, nDimVertices>();
  tensorflow::Tensor annulusTensor(tensorflow::DT_FLOAT,
                                   tensorflow::TensorShape({1, maxNAnnulusTracks, nAnnulusTrackFeatures}));
  auto annulus_map = annulusTensor.tensor<float, nDimAnnulusTracks>();

  for (int i = 0, map_i = 0; map_i < maxNVertices; i++, map_i++) {
    if (i >= static_cast<int>(roi.nConstituents()))
      for (unsigned j = 0; j < nVertexFeatures; j++)
        vertex_map(0, map_i, j) = 0.0;
    else {
      const auto &trackClusterRef = roi.constituent(i);
      const auto &track0 = *trackClusterRef->daughter(0)->bestTrack();
      const auto &track1 = *trackClusterRef->daughter(1)->bestTrack();

      vertex_map(0, map_i, 0) = trackClusterRef->vx() - bs.x0();
      vertex_map(0, map_i, 1) = trackClusterRef->vy() - bs.y0();
      vertex_map(0, map_i, 2) = trackClusterRef->vz() - bs.z0();

      vertex_map(0, map_i, 3) = trackClusterRef->vertexCovariance()(0, 0);
      vertex_map(0, map_i, 4) = trackClusterRef->vertexCovariance()(0, 1);
      vertex_map(0, map_i, 5) = trackClusterRef->vertexCovariance()(0, 2);
      vertex_map(0, map_i, 6) = trackClusterRef->vertexCovariance()(1, 1);
      vertex_map(0, map_i, 7) = trackClusterRef->vertexCovariance()(1, 2);
      vertex_map(0, map_i, 8) = trackClusterRef->vertexCovariance()(2, 2);

      vertex_map(0, map_i, 9) = track0.charge() * track0.pt();
      vertex_map(0, map_i, 10) = track0.eta();
      vertex_map(0, map_i, 11) = track0.phi();
      vertex_map(0, map_i, 12) = track0.dxy(bs);
      vertex_map(0, map_i, 13) = track0.dz(reco::TrackBase::Point(bs.x0(), bs.y0(), bs.z0()));
      vertex_map(0, map_i, 14) = track0.normalizedChi2();
      vertex_map(0, map_i, 15) = track0.quality(reco::Track::highPurity) ? 1 : 0;

      vertex_map(0, map_i, 16) = track1.charge() * track1.pt();
      vertex_map(0, map_i, 17) = track1.eta();
      vertex_map(0, map_i, 18) = track1.phi();
      vertex_map(0, map_i, 19) = track1.dxy(bs);
      vertex_map(0, map_i, 20) = track1.dz(reco::TrackBase::Point(bs.x0(), bs.y0(), bs.z0()));
      vertex_map(0, map_i, 21) = track1.normalizedChi2();
      vertex_map(0, map_i, 22) = track1.quality(reco::Track::highPurity) ? 1 : 0;
    }
  }

  for (int i = 0; i < maxNAnnulusTracks; i++)
    for (unsigned j = 0; j < nAnnulusTrackFeatures; j++)
      annulus_map(0, i, j) = 0.0;

  tensorflow::NamedTensorList input_tensors;
  input_tensors.resize(2);
  input_tensors[0] = tensorflow::NamedTensor(input_names_.at(0), vertexTensor);
  input_tensors[1] = tensorflow::NamedTensor(input_names_.at(1), annulusTensor);
  vector<tensorflow::Tensor> outputs;
  tensorflow::run(session_, input_tensors, output_names_, &outputs);

  return (outputs.at(0).flat<float>()(1));
}

const double Distance::distance() const {
  if (entities_.first->valid() && entities_.second->valid())
    return (entities_.first->centerOfMass() - entities_.second->centerOfMass()).r();
  return numeric_limits<double>::max();
}

DEFINE_FWK_MODULE(DisplacedRegionProducer);
