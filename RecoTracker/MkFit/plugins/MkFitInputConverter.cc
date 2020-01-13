#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/SiStripCluster/interface/SiStripClusterTools.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2DCollection.h"

#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/TrackerCommon/interface/TrackerDetSide.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "RecoTracker/MkFit/interface/MkFitInputWrapper.h"
#include "RecoTracker/MkFit/interface/MkFitGeometry.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"

// ROOT
#include "Math/SVector.h"
#include "Math/SMatrix.h"

// mkFit includes
#include "Hit.h"
#include "Track.h"
#include "LayerNumberConverter.h"

class MkFitInputConverter: public edm::global::EDProducer<> {
public:
  explicit MkFitInputConverter(edm::ParameterSet const& iConfig);
  ~MkFitInputConverter() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const override;

  template <typename HitCollection>
  void convertHits(const HitCollection& hits,
                   std::vector<mkfit::HitVec>& mkFitHits,
                   MkFitHitIndexMap& hitIndexMap,
                   int& totalHits,
                   const TrackerTopology& ttopo,
                   const TransientTrackingRecHitBuilder& ttrhBuilder,
                   const MkFitGeometry& mkFitGeom) const;

  float clusterCharge(const SiStripRecHit2D& hit, DetId hitId) const;
  nullptr_t clusterCharge(const SiPixelRecHit& hit, DetId hitId) const;

  bool passCCC(float charge) const;
  bool passCCC(nullptr_t) const; //pixel

  void setDetails(mkfit::Hit& mhit, const SiPixelCluster& cluster, const int shortId, nullptr_t) const;
  void setDetails(mkfit::Hit& mhit, const SiStripCluster& cluster, const int shortId, float charge) const;

  mkfit::TrackVec convertSeeds(const edm::View<TrajectorySeed>& seeds,
                               const MkFitHitIndexMap& hitIndexMap,
                               const TransientTrackingRecHitBuilder& ttrhBuilder,
                               const MagneticField& mf) const;

  using SVector3 = ROOT::Math::SVector<float, 3>;
  using SMatrixSym33 = ROOT::Math::SMatrix<float,3,3,ROOT::Math::MatRepSym<float,3> >;
  using SMatrixSym66 = ROOT::Math::SMatrix<float,6,6,ROOT::Math::MatRepSym<float,6> >;

  edm::EDGetTokenT<SiPixelRecHitCollection> pixelRecHitToken_;
  edm::EDGetTokenT<SiStripRecHit2DCollection> stripRphiRecHitToken_;
  edm::EDGetTokenT<SiStripRecHit2DCollection> stripStereoRecHitToken_;
  edm::EDGetTokenT<edm::View<TrajectorySeed> > seedToken_;
  edm::EDPutTokenT<MkFitInputWrapper> putToken_;
  std::string ttrhBuilderName_;
  const float minGoodStripCharge_;
};

MkFitInputConverter::MkFitInputConverter(edm::ParameterSet const& iConfig)
    : pixelRecHitToken_{consumes<SiPixelRecHitCollection>(iConfig.getParameter<edm::InputTag>("pixelRecHits"))},
      stripRphiRecHitToken_{
          consumes<SiStripRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("stripRphiRecHits"))},
      stripStereoRecHitToken_{
          consumes<SiStripRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("stripStereoRecHits"))},
      seedToken_{consumes<edm::View<TrajectorySeed>>(iConfig.getParameter<edm::InputTag>("seeds"))},
      putToken_{produces<MkFitInputWrapper>()},
      ttrhBuilderName_{iConfig.getParameter<std::string>("ttrhBuilder")},
      minGoodStripCharge_{static_cast<float>(
          iConfig.getParameter<edm::ParameterSet>("minGoodStripCharge").getParameter<double>("value"))} {}

void MkFitInputConverter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add("pixelRecHits", edm::InputTag("siPixelRecHits"));
  desc.add("stripRphiRecHits", edm::InputTag("siStripMatchedRecHits", "rphiRecHit"));
  desc.add("stripStereoRecHits", edm::InputTag("siStripMatchedRecHits", "stereoRecHit"));
  desc.add("seeds", edm::InputTag("initialStepSeeds"));
  desc.add<std::string>("ttrhBuilder", "WithTrackAngle");

  edm::ParameterSetDescription descCCC;
  descCCC.add<double>("value");
  desc.add("minGoodStripCharge", descCCC);

  descriptions.add("mkFitInputConverterDefault", desc);
}

void MkFitInputConverter::produce(edm::StreamID iID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // Then import hits
  edm::Handle<SiPixelRecHitCollection> pixelHits;
  iEvent.getByToken(pixelRecHitToken_, pixelHits);

  edm::Handle<SiStripRecHit2DCollection> stripRphiHits;
  iEvent.getByToken(stripRphiRecHitToken_, stripRphiHits);

  edm::Handle<SiStripRecHit2DCollection> stripStereoHits;
  iEvent.getByToken(stripStereoRecHitToken_, stripStereoHits);

  edm::ESHandle<TransientTrackingRecHitBuilder> ttrhBuilder;
  iSetup.get<TransientRecHitRecord>().get(ttrhBuilderName_, ttrhBuilder);

  edm::ESHandle<TrackerTopology> ttopo;
  iSetup.get<TrackerTopologyRcd>().get(ttopo);

  edm::ESHandle<MkFitGeometry> mkFitGeom;
  iSetup.get<TrackerRecoGeometryRecord>().get(mkFitGeom);

  std::vector<mkfit::HitVec> mkFitHits(mkFitGeom->layerNumberConverter().nLayers());
  MkFitHitIndexMap hitIndexMap;
  int totalHits = 0; // I need to have a global hit index in order to have the hit remapping working?
  convertHits(*stripRphiHits, mkFitHits, hitIndexMap, totalHits, *ttopo, *ttrhBuilder, *mkFitGeom);
  convertHits(*stripStereoHits, mkFitHits, hitIndexMap, totalHits, *ttopo, *ttrhBuilder, *mkFitGeom);
  convertHits(*pixelHits, mkFitHits, hitIndexMap, totalHits, *ttopo, *ttrhBuilder, *mkFitGeom);

  // Then import seeds
  edm::Handle<edm::View<TrajectorySeed>> seeds;
  iEvent.getByToken(seedToken_, seeds);

  edm::ESHandle<MagneticField> mf;
  iSetup.get<IdealMagneticFieldRecord>().get(mf);

  auto mkFitSeeds = convertSeeds(*seeds, hitIndexMap, *ttrhBuilder, *mf);

  iEvent.emplace(putToken_, std::move(hitIndexMap), std::move(mkFitHits), std::move(mkFitSeeds));
}

float MkFitInputConverter::clusterCharge(const SiStripRecHit2D& hit, DetId hitId) const {
  return siStripClusterTools::chargePerCM(hitId, hit.firstClusterRef().stripCluster());
}
nullptr_t MkFitInputConverter::clusterCharge(const SiPixelRecHit& hit, DetId hitId) const {
  return nullptr;
}

bool MkFitInputConverter::passCCC(float charge) const {
  return charge > minGoodStripCharge_;
}

bool MkFitInputConverter::passCCC(nullptr_t) const {
  return true;
}

void MkFitInputConverter::setDetails(mkfit::Hit& mhit, const SiPixelCluster& cluster, int shortId, nullptr_t) const {
  mhit.setupAsPixel(shortId, cluster.sizeX(), cluster.sizeY());
}

void MkFitInputConverter::setDetails(mkfit::Hit& mhit, const SiStripCluster& cluster, int shortId, float charge) const {
  mhit.setupAsStrip(shortId, charge, cluster.amplitudes().size());
}


template <typename HitCollection>
void MkFitInputConverter::convertHits(const HitCollection& hits,
                                      std::vector<mkfit::HitVec>& mkFitHits,
                                      MkFitHitIndexMap& hitIndexMap,
                                      int& totalHits,
                                      const TrackerTopology& ttopo,
                                      const TransientTrackingRecHitBuilder& ttrhBuilder,
                                      const MkFitGeometry& mkFitGeom) const {
  if (hits.empty())
    return;
  auto isPlusSide = [&ttopo](const DetId& detid) {
    return ttopo.side(detid) == static_cast<unsigned>(TrackerDetSide::PosEndcap);
  };

  {
    const DetId detid{hits.ids().back()};
    const auto ilay =
      mkFitGeom.layerNumberConverter().convertLayerNumber(detid.subdetId(), ttopo.layer(detid), false, ttopo.isStereo(detid), isPlusSide(detid));
    // Do initial reserves to minimize further memory allocations
    const auto& lastClusterRef = hits.data().back().firstClusterRef();
    hitIndexMap.resizeByClusterIndex(lastClusterRef.id(), lastClusterRef.index());
    hitIndexMap.increaseLayerSize(ilay, hits.detsetSize(hits.ids().size() - 1));
  }

  for (const auto& detset : hits) {
    const DetId detid = detset.detId();
    const auto subdet = detid.subdetId();
    const auto layer = ttopo.layer(detid);
    const auto isStereo = ttopo.isStereo(detid);
    const auto ilay = mkFitGeom.layerNumberConverter().convertLayerNumber(subdet, layer, false, isStereo, isPlusSide(detid));
    const auto uniqueIdInLayer = mkFitGeom.uniqueIdInLayer(ilay, detid.rawId());
    hitIndexMap.increaseLayerSize(ilay, detset.size());  // to minimize memory allocations

    for(const auto& hit: detset) {
      const auto charge = clusterCharge(hit, detid);
      if(!passCCC(charge)) continue;

      const auto& gpos = hit.globalPosition();
      SVector3 pos(gpos.x(), gpos.y(), gpos.z());
      const auto& gerr = hit.globalPositionError();
      SMatrixSym33 err;
      err.At(0, 0) = gerr.cxx();
      err.At(1, 1) = gerr.cyy();
      err.At(2, 2) = gerr.czz();
      err.At(0, 1) = gerr.cyx();
      err.At(0, 2) = gerr.czx();
      err.At(1, 2) = gerr.czy();

      LogTrace("MkFitInputConverter") << "Adding hit detid " << detid.rawId() << " subdet " << subdet << " layer "
                                      << layer << " isStereo " << isStereo << " zplus " << isPlusSide(detid) << " ilay "
                                      << ilay;

      hitIndexMap.insert(hit.firstClusterRef().id(),
                         hit.firstClusterRef().index(),
                         MkFitHitIndexMap::MkFitHit{static_cast<int>(mkFitHits[ilay].size()), ilay},
                         &hit);
      mkFitHits[ilay].emplace_back(pos, err, totalHits);
      setDetails(mkFitHits[ilay].back(), *(hit.cluster()), uniqueIdInLayer, charge);
      ++totalHits;
    }
  }
}

mkfit::TrackVec MkFitInputConverter::convertSeeds(const edm::View<TrajectorySeed>& seeds,
                                                  const MkFitHitIndexMap& hitIndexMap,
                                                  const TransientTrackingRecHitBuilder& ttrhBuilder,
                                                  const MagneticField& mf) const {
  mkfit::TrackVec ret;
  ret.reserve(seeds.size());
  int index = 0;
  for(const auto& seed: seeds) {
    const auto hitRange = seed.recHits();
    const auto lastRecHit = ttrhBuilder.build(&*(hitRange.second-1));
    const auto tsos = trajectoryStateTransform::transientState( seed.startingState(), lastRecHit->surface(), &mf);
    const auto& stateGlobal = tsos.globalParameters();
    const auto& gpos = stateGlobal.position();
    const auto& gmom = stateGlobal.momentum();
    SVector3 pos(gpos.x(), gpos.y(), gpos.z());
    SVector3 mom(gmom.x(), gmom.y(), gmom.z());

    const auto cartError = tsos.cartesianError(); // returns a temporary, so can't chain with the following line
    const auto& cov = cartError.matrix();
    SMatrixSym66 err;
    for(int i=0; i<6; ++i) {
      for(int j=i; j<6; ++j) {
        err.At(i, j) = cov[i][j];
      }
    }

    mkfit::TrackState state(tsos.charge(), pos, mom, err);
    state.convertFromCartesianToCCS();
    ret.emplace_back(state, 0, index, 0, nullptr);

    // Add hits
    for(auto iHit = hitRange.first; iHit != hitRange.second; ++iHit) {
      const auto *hit = dynamic_cast<const BaseTrackerRecHit *>(&*iHit);
      if(hit == nullptr) {
        throw cms::Exception("Assert") << "Encountered a seed with a hit which is not BaseTrackerRecHit";
      }
      const auto& clusterRef = static_cast<const BaseTrackerRecHit&>(*iHit).firstClusterRef();
      const auto& mkFitHit = hitIndexMap.mkFitHit(clusterRef.id(), clusterRef.index());
      ret.back().addHitIdx(mkFitHit.index(), mkFitHit.layer(), 0);  // per-hit chi2 is not known
    }
    ++index;
  }
  return ret;
}

DEFINE_FWK_MODULE(MkFitInputConverter);
