#include "DataFormats/TrackerRecHit2D/interface/Phase2TrackerRecHit1D.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHitCollection.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidateCollection.h"
#include "DataFormats/TrackReco/interface/SeedStopInfo.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "RecoTracker/LST/interface/LSTPhase2OTHitsInput.h"
#include "RecoTracker/LST/interface/LSTOutput.h"
#include "RecoTracker/TkSeedingLayers/interface/SeedingHitSet.h"

#include "RecoTracker/TkSeedGenerator/interface/SeedCreator.h"
#include "RecoTracker/TkSeedGenerator/interface/SeedCreatorFactory.h"

#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"


class LSTOutputConverter : public edm::global::EDProducer<> {
public:
  explicit LSTOutputConverter(edm::ParameterSet const& iConfig);
  ~LSTOutputConverter() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const override;

  const edm::EDGetTokenT<LSTOutput> lstOutputToken_;
  const edm::EDGetTokenT<LSTPhase2OTHitsInput> lstPhase2OTHitsInputToken_;
  const edm::EDGetTokenT<TrajectorySeedCollection> lstPixelSeedToken_;
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> mfToken_;
  edm::ESGetToken<Propagator, TrackingComponentsRecord> propagatorAlongToken_;
  edm::ESGetToken<Propagator, TrackingComponentsRecord> propagatorOppositeToken_;
  //const edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  std::unique_ptr<SeedCreator> seedCreator_;
  const edm::EDPutTokenT<TrackCandidateCollection> trackCandidatePutToken_;
  const edm::EDPutTokenT<std::vector<SeedStopInfo>> seedStopInfoPutToken_;
};

LSTOutputConverter::LSTOutputConverter(edm::ParameterSet const& iConfig)
    : lstOutputToken_(consumes<LSTOutput>(iConfig.getUntrackedParameter<edm::InputTag>("lstOutput"))),
      lstPhase2OTHitsInputToken_{consumes<LSTPhase2OTHitsInput>(iConfig.getUntrackedParameter<edm::InputTag>("phase2OTHits"))},
      lstPixelSeedToken_{consumes<TrajectorySeedCollection>(iConfig.getUntrackedParameter<edm::InputTag>("lstPixelSeeds"))},
      mfToken_(esConsumes()),
      propagatorAlongToken_{esConsumes<Propagator, TrackingComponentsRecord>(iConfig.getParameter<edm::ESInputTag>("propagatorAlong"))},
      propagatorOppositeToken_{esConsumes<Propagator, TrackingComponentsRecord>(iConfig.getParameter<edm::ESInputTag>("propagatorOpposite"))},
      //beamSpotToken_(consumes<reco::BeamSpot>(iConfig.getUntrackedParameter<edm::InputTag>("beamSpot"))),
      seedCreator_(SeedCreatorFactory::get()->create("SeedFromConsecutiveHitsCreator", iConfig.getParameter<edm::ParameterSet>("SeedCreatorPSet"), consumesCollector())),
      trackCandidatePutToken_(produces<TrackCandidateCollection>()),
      seedStopInfoPutToken_(produces<std::vector<SeedStopInfo>>()) {}

void LSTOutputConverter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.addUntracked<edm::InputTag>("lstOutput", edm::InputTag("lstProducer"));
  desc.addUntracked<edm::InputTag>("phase2OTHits", edm::InputTag("lstPhase2OTHitsInputProducer"));
  desc.addUntracked<edm::InputTag>("lstPixelSeeds", edm::InputTag("lstPixelSeedInputProducer"));
  desc.add("propagatorAlong", edm::ESInputTag{"", "PropagatorWithMaterial"});
  desc.add("propagatorOpposite", edm::ESInputTag{"", "PropagatorWithMaterialOpposite"});
  //desc.addUntracked<edm::InputTag>("beamSpot", edm::InputTag("offlineBeamSpot"));

  edm::ParameterSetDescription psd0;
  psd0.add<std::string>("ComponentName", std::string("SeedFromConsecutiveHitsCreator"));
  psd0.add<std::string>("propagator", std::string("PropagatorWithMaterial"));
  psd0.add<double>("SeedMomentumForBOFF", 5.0);
  psd0.add<double>("OriginTransverseErrorMultiplier", 1.0);
  psd0.add<double>("MinOneOverPtError", 1.0);
  psd0.add<std::string>("magneticField", std::string(""));
  psd0.add<std::string>("TTRHBuilder", std::string("WithTrackAngle"));
  psd0.add<bool>("forceKinematicWithRegionDirection", false);
  desc.add<edm::ParameterSetDescription>("SeedCreatorPSet", psd0);

  descriptions.addWithDefaultLabel(desc);
}

void LSTOutputConverter::produce(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // Setup
  auto const& lstOutput = iEvent.get(lstOutputToken_);
  auto const& phase2OTRecHits = iEvent.get(lstPhase2OTHitsInputToken_);
  auto const& pixelSeeds = iEvent.get(lstPixelSeedToken_);
  const auto& mf = iSetup.getData(mfToken_);
  const auto& propAlo = iSetup.getData(propagatorAlongToken_);
  const auto& propOppo = iSetup.getData(propagatorOppositeToken_);
  //auto const& bs = iEvent.get(beamSpotToken_);

  // Vector definitions
  std::vector<std::vector<unsigned int>> const& lstTC_hitIdx = lstOutput.hitIdx();
  std::vector<unsigned int> const& lstTC_len = lstOutput.len();
  std::vector<float> const& lstTC_pt = lstOutput.pt();
  std::vector<float> const& lstTC_eta = lstOutput.eta();
  std::vector<float> const& lstTC_phi = lstOutput.phi();
  std::vector<int> const& lstTC_seedIdx = lstOutput.seedIdx();

  TrackCandidateCollection output;
  output.reserve(lstTC_len.size());

  auto const& OTHits = phase2OTRecHits.hits();

  //auto seedsFromT5 = std::make_unique<TrajectorySeedCollection>();
  //GlobalPoint vtxOfBs(bs.x0(), bs.y0(), bs.z0()); 
  LogDebug("LSTOutputConverter")<<"lstTC size "<<lstTC_len.size();
  for (unsigned int i=0; i<lstTC_len.size(); i++) {
    LogDebug("LSTOutputConverter")<<" cand "<<i<<" "<<lstTC_len[i]<<" "<<lstTC_pt[i]<<" "<<lstTC_eta[i]<<" "<<lstTC_phi[i]<<" "<<lstTC_seedIdx[i];

    TrajectorySeed seed;
    if (lstTC_seedIdx[i] != - 1)
      seed = pixelSeeds[lstTC_seedIdx[i]];

    edm::OwnVector<TrackingRecHit> recHits;
    if (lstTC_seedIdx[i] != - 1) {
      for (auto const& hit : seed.recHits())
        recHits.push_back(hit.clone());
    }

    unsigned int const nPixelHits = lstTC_seedIdx[i] == -1 ? 0 : recHits.size();
    for (unsigned int j=nPixelHits; j<lstTC_hitIdx[i].size(); j++)
      recHits.push_back(OTHits[lstTC_hitIdx[i][j]]->clone());

    recHits.sort([](const auto& a, const auto& b) {
      const auto asub = a.det()->subDetector();
      const auto bsub = b.det()->subDetector();
      if (GeomDetEnumerators::isInnerTracker(asub) && GeomDetEnumerators::isOuterTracker(bsub)) {
        return true;
      } else if (GeomDetEnumerators::isOuterTracker(asub) && GeomDetEnumerators::isInnerTracker(bsub)) {
        return false;
      } else if (asub != bsub) {
        return asub < bsub;
      } else {
        const auto& apos = a.surface();
        const auto& bpos = b.surface();
        if (GeomDetEnumerators::isBarrel(asub)) {
          return apos->rSpan().first < bpos->rSpan().first;
        } else {
          return std::abs(apos->zSpan().first) < std::abs(bpos->zSpan().first);
        }
      }
    });

    if (lstTC_seedIdx[i] == -1) { // T5
      using Hit = SeedingHitSet::ConstRecHitPointer;
      std::vector<Hit> hitsFromT5;
      hitsFromT5.reserve(lstTC_len[i]);
      for (auto const& hit : recHits)
        hitsFromT5.emplace_back(dynamic_cast<Hit>(&hit));

      TrajectorySeedCollection seeds;
      seedCreator_->init(GlobalTrackingRegion(), iSetup, nullptr);
      seedCreator_->makeSeed(seeds, hitsFromT5);
      if (seeds.empty()) {
        edm::LogInfo("LSTOutputConverter")<<"failed to convert a T5 to a seed"<<i<<" "<<lstTC_len[i]
                                             <<" "<<lstTC_pt[i]<<" "<<lstTC_eta[i]<<" "<<lstTC_phi[i]<<" "<<lstTC_seedIdx[i];
        continue;
      }
      seed = seeds[0];
      auto const& ss = seed.startingState();
      LogDebug("LSTOutputConverter")<<"Created a seed with "<<seed.nHits()<<" "<<ss.detId()<<" "<<ss.pt()
                                           <<" "<<ss.parameters().vector()<<" "<<ss.error(0);
    }


    TrajectoryStateOnSurface tsos = trajectoryStateTransform::transientState(seed.startingState(), (seed.recHits().end()-1)->surface(), &mf);
    auto tsosPair = propOppo.propagateWithPath(tsos, *recHits[0].surface());
    if (!tsosPair.first.isValid()) {
      LogDebug("LSTOutputConverter") << "Propagating to startingState opposite to momentum failed, trying along next";
      tsosPair = propAlo.propagateWithPath(tsos, *recHits[0].surface());
    }
    if (tsosPair.first.isValid()) {
      PTrajectoryStateOnDet st = trajectoryStateTransform::persistentState(tsosPair.first, recHits[0].det()->geographicalId().rawId());

      output.emplace_back(TrackCandidate(recHits,seed,st));
    } else {
      edm::LogInfo("LSTOutputConverter")<<"Failed to make a candidate initial state. Seed state is "<<tsos
                                           <<" TC cand "<<i<<" "<<lstTC_len[i]
                                           <<" "<<lstTC_pt[i]<<" "<<lstTC_eta[i]<<" "<<lstTC_phi[i]<<" "<<lstTC_seedIdx[i]
                                           <<" first hit "<<recHits.front().globalPosition()
                                           <<" last hit "<<recHits.back().globalPosition();
    }
  }

  LogDebug("LSTOutputConverter")<<"done with conversion: output size "<<output.size();
  iEvent.emplace(trackCandidatePutToken_, std::move(output));
  iEvent.emplace(seedStopInfoPutToken_, 0U); //dummy stop info
}

DEFINE_FWK_MODULE(LSTOutputConverter);

