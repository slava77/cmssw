// -*- C++ -*-
//
// Package:    SiPixelTrackProbQXYProducer
// Class:      SiPixelTrackProbQXYProducer
//
/**\class SiPixelTrackProbQXYProducer  SiPixelTrackProbQXYProducer.cc RecoTracker/DeDx/plugins/SiPixelTrackProbQXYProducer.cc

 Description: SiPixel Charge and shape probabilities combined for tracks

*/
//
// Original Author:  Tamas Almos Vami
//         Created:  Mon Nov 17 14:09:02 CEST 2021
//

// system include files
#include <memory>

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackReco/interface/DeDxHitInfo.h"
#include "DataFormats/TrackReco/interface/SiPixelTrackProbQXY.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/EDPutToken.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "RecoLocalTracker/Records/interface/TkPixelCPERecord.h"
#include "RecoLocalTracker/ClusterParameterEstimator/interface/PixelClusterParameterEstimator.h"
//
// class declaration
//

class SiPixelTrackProbQXYProducer : public edm::global::EDProducer<> {
public:
  explicit SiPixelTrackProbQXYProducer(const edm::ParameterSet&);
  ~SiPixelTrackProbQXYProducer() override = default;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;

  // ----------member data ---------------------------
  const bool debugFlag_;
  const std::string pixelCPE_;
  const edm::EDGetTokenT<reco::TrackCollection> trackToken_;
  const edm::EDGetTokenT<reco::DeDxHitInfoAss> gt2dedxHitInfo_;
  const edm::EDPutTokenT<edm::ValueMap<reco::SiPixelTrackProbQXY>> putProbQXYVMToken_;
  const edm::EDPutTokenT<edm::ValueMap<reco::SiPixelTrackProbQXY>> putProbQXYNoLayer1VMToken_;
};

using namespace reco;
using namespace std;
using namespace edm;

SiPixelTrackProbQXYProducer::SiPixelTrackProbQXYProducer(const edm::ParameterSet& iConfig)
    : debugFlag_(iConfig.getUntrackedParameter<bool>("debug", false)),
      pixelCPE_(iConfig.getParameter<std::string>("pixelCPE")),
      trackToken_(consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("tracks"))),
      gt2dedxHitInfo_(consumes<reco::DeDxHitInfoAss>(iConfig.getParameter<edm::InputTag>("dedxInfos"))),
      putProbQXYVMToken_(produces<edm::ValueMap<reco::SiPixelTrackProbQXY>>()),
      putProbQXYNoLayer1VMToken_(produces<edm::ValueMap<reco::SiPixelTrackProbQXY>>("NoLayer1")) {}

void SiPixelTrackProbQXYProducer::produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  // Retrieve track collection from the event
  auto trackCollectionHandle = iEvent.getHandle(trackToken_);
  const TrackCollection& trackCollection(*trackCollectionHandle.product());
  int numTrack = 0;
  int numTrackWSmallProbQ = 0;

  auto const& gt2dedxHitInfo = iEvent.get(gt2dedxHitInfo_);

  // Retrieve tracker topology from geometry
  edm::ESHandle<TrackerTopology> TopoHandle;
  iSetup.get<TrackerTopologyRcd>().get(TopoHandle);
  const TrackerTopology* tTopo = TopoHandle.product();

  edm::ESHandle<PixelClusterParameterEstimator> pixelCPE;
  iSetup.get<TkPixelCPERecord>().get(pixelCPE_, pixelCPE);


  edm::ESHandle<TrackerGeometry> tkGeometry;
  iSetup.get<TrackerDigiGeometryRecord>().get(tkGeometry);

  // Creates the output collection
  auto resultSiPixelTrackProbQXYColl = std::make_unique<reco::SiPixelTrackProbQXYCollection>();
  auto resultSiPixelTrackProbQXYNoLayer1Coll = std::make_unique<reco::SiPixelTrackProbQXYCollection>();

  // Loop through the tracks
  for (const auto& track : trackCollection) {
    reco::TrackRef trackRef(trackCollectionHandle, numTrack);
    numTrack++;
    float probQonTrack = 0.0;
    float probXYonTrack = 0.0;
    float probQonTrackNoLayer1 = 0.0;
    float probXYonTrackNoLayer1 = 0.0;
    int numRecHits = 0;
    int numRecHitsNoLayer1 = 0;
    float probQonTrackWMulti = 1;
    float probXYonTrackWMulti = 1;
    float probQonTrackWMultiNoLayer1 = 1;
    float probXYonTrackWMultiNoLayer1 = 1;

    auto const& dedxRef = gt2dedxHitInfo[trackRef];
    // Loop through the rechits on the given track
    if(debugFlag_) {
       LogPrint("SiPixelTrackProbQXYProducer") << "  -----------------------------------------------";
       LogPrint("SiPixelTrackProbQXYProducer") << "  For track " << numTrack << " -1 ( "<<numTrack-1<<" dedxinfoRef is "<<(!dedxRef.isNull());
       if (!dedxRef.isNull()) {
         LogPrint("SiPixelTrackProbQXYProducer") << "track : "<<track.pt()<<" "<<track.eta()<<" "<<track.phi()<<" dedx: size "<<dedxRef->size();
       }
    }

    int iHit = -1;
    for (auto const& hit : track.recHits()) {
      const SiPixelRecHit* pixhit = dynamic_cast<const SiPixelRecHit*>(hit);
      if (pixhit == nullptr)
        continue;
      if (!pixhit->isValid())
        continue;
      iHit++;
      
      DetId hitDetId = pixhit->detUnit()->geographicalId();
      if (debugFlag_) {
        if (!dedxRef.isNull()) {
          DetId dedxId = dedxRef->detId(iHit);
          bool isSame = hitDetId.rawId()==dedxId.rawId();
          LogPrint("SiPixelTrackProbQXYProducer") 
            << "  >>>>> hit IDs tkHit "<<hitDetId.rawId()<<" dedx "<<dedxId.rawId()<<" eq "<<isSame;
          if (isSame) {
            auto const* dedxClu = dedxRef->pixelCluster(iHit);
            if (dedxClu == nullptr) {
              LogPrint("SiPixelTrackProbQXYProducer")
                << "  >>>>>> FAILED to get dedxClu";
            } else {
              auto const* hitClu = &*(pixhit->cluster());
              LogPrint("SiPixelTrackProbQXYProducer")
                << "  >>>>>> dedx "<<dedxClu->x()<<" "<<dedxClu->y()<<" "<<dedxClu->charge()<<" "<<dedxClu->size()
                << " tkHit "<<hitClu->x()<<" "<<hitClu->y()<<" "<<hitClu->charge()<<" "<<hitClu->size();
              const GeomDetUnit& geomDet = *tkGeometry->idToDetUnit(dedxId);
              //NOT GOOD FOR TEMPLATE              auto qual_0 =  std::get<2>(pixelCPE->getParameters(*hitClu, geomDet));
              //lazy option, no propagation
              LocalVector lv = geomDet.toLocal(GlobalVector(track.px(), track.py(), track.pz()));
              auto qual_1 = std::get<2>(pixelCPE->getParameters(*hitClu, geomDet, 
                                                                LocalTrajectoryParameters(dedxRef->pos(iHit), lv, track.charge())));
              LogPrint("SiPixelTrackProbQXYProducer")
                << "  >>>>>> probs dedx "<<SiPixelRecHitQuality::thePacking.probabilityQ(qual_1)
                << " tkHit "<<pixhit->probabilityQ();
              
            }
          }
        }
      }

      // Have a separate variable that excludes Layer 1
      // Layer 1 was very noisy in 2017/2018
      if ((pixhit->geographicalId().subdetId() == PixelSubdetector::PixelEndcap) ||
         (pixhit->geographicalId().subdetId() == PixelSubdetector::PixelBarrel &&
          tTopo->pxbLayer(pixhit->geographicalId()) != 1)) {
        float probQNoLayer1 = pixhit->probabilityQ();
        float probXYNoLayer1 = pixhit->probabilityXY();
        if (probQNoLayer1 > 0.f) {  // only save the non-zero rechits
          numRecHitsNoLayer1++;
          // Calculate alpha term needed for the combination
          probQonTrackWMultiNoLayer1 *= probQNoLayer1;
          probXYonTrackWMultiNoLayer1 *= probXYNoLayer1;
          if(debugFlag_) {
            LogPrint("SiPixelTrackProbQXYProducer") << "    >>>> For rechit excluding Layer 1: " << numRecHitsNoLayer1 << " ProbQ = " << probQNoLayer1;
      }

        }
      }

      // Have a variable that includes all layers and disks
      float probQ = pixhit->probabilityQ();
      float probXY = pixhit->probabilityXY();

      if (probQ == 0) {
        continue;  // if any of the rechits have zero probQ, skip them
      }
      numRecHits++;

      if(debugFlag_) {
        LogPrint("SiPixelTrackProbQXYProducer") << "    >>>> For rechit: " << numRecHits << " ProbQ = " << probQ;
      }

      // Calculate alpha term needed for the combination
      probQonTrackWMulti *= probQ;
      probXYonTrackWMulti *= probXY;

    }  // end looping on the rechits

    // Combine the probabilities into a track level quantity
    float logprobQonTrackWMulti = probQonTrackWMulti > 0.f ? log(probQonTrackWMulti) : 0.f;
    float logprobXYonTrackWMulti = probXYonTrackWMulti > 0.f ? log(probXYonTrackWMulti) : 0.f;
    float factQ = -logprobQonTrackWMulti;
    float factXY = -logprobXYonTrackWMulti;
    float probQonTrackTerm = 0.f;
    float probXYonTrackTerm = 0.f;

    if (numRecHits == 1 ) {
      probQonTrackTerm = 1.f;
      probXYonTrackTerm = 1.f;
    } else if (numRecHits > 1 ) {
      probQonTrackTerm =  1.f + factQ;
      probXYonTrackTerm = 1.f + factXY;
      for (int iTkRh = 2; iTkRh < numRecHits; ++iTkRh) {
         factQ *= -logprobQonTrackWMulti / float(iTkRh);
         factXY *= -logprobXYonTrackWMulti / float(iTkRh);
         probQonTrackTerm += factQ;
         probXYonTrackTerm += factXY;
      }
    }
    probQonTrack = probQonTrackWMulti * probQonTrackTerm;
    probXYonTrack = probXYonTrackWMulti * probXYonTrackTerm;

    // Count the number of tracks with small probQonTrack
    if(probQonTrack < 0.1) {
       numTrackWSmallProbQ++;
    }

    if(debugFlag_) {
       LogPrint("SiPixelTrackProbQXYProducer") << "  >> probQonTrack = " << probQonTrack
        << " = " << probQonTrackWMulti << " * " << probQonTrackTerm;
    }

    // Repeat the above excluding Layer 1
    float logprobQonTrackWMultiNoLayer1 = probQonTrackWMultiNoLayer1 > 0 ? log(probQonTrackWMultiNoLayer1) : 0;
    float logprobXYonTrackWMultiNoLayer1 = probXYonTrackWMultiNoLayer1 > 0 ? log(probXYonTrackWMultiNoLayer1) : 0;

    float factQNoLayer1 = -logprobQonTrackWMultiNoLayer1;
    float factXYNoLayer1 = -logprobXYonTrackWMultiNoLayer1;
    float probQonTrackTermNoLayer1 = 0.f;
    float probXYonTrackTermNoLayer1 = 0.f;

    if (numRecHitsNoLayer1 == 1 ) {
      probQonTrackTermNoLayer1 = 1.f;
      probXYonTrackTermNoLayer1 = 1.f;
    } else if (numRecHitsNoLayer1 > 1 ) {
      probQonTrackTermNoLayer1 =  1.f + factQNoLayer1;
      probXYonTrackTermNoLayer1 = 1.f + factXYNoLayer1;
      for (int iTkRh = 2; iTkRh < numRecHitsNoLayer1; ++iTkRh) {
        factQNoLayer1 *= -logprobQonTrackWMultiNoLayer1 / float(iTkRh);
        factXYNoLayer1 *= -logprobXYonTrackWMultiNoLayer1 / float(iTkRh);
        probQonTrackTermNoLayer1 += factQNoLayer1;
        probXYonTrackTermNoLayer1 += factXYNoLayer1;
      }
    }

    probQonTrackNoLayer1 = probQonTrackWMultiNoLayer1 * probQonTrackTermNoLayer1;
    probXYonTrackNoLayer1 = probXYonTrackWMultiNoLayer1 * probXYonTrackTermNoLayer1;

    if(debugFlag_) {
       LogPrint("SiPixelTrackProbQXYProducer") << "  >> probQonTrackNoLayer1 = " << probQonTrackNoLayer1
       << " = " << probQonTrackWMultiNoLayer1 << " * " << probQonTrackTermNoLayer1;
    }

    // To save space let's zero out the low pt tracks
    if (track.pt() < 5) {
      probQonTrack = 0;
      probXYonTrack = 0;
      probQonTrackNoLayer1 = 0;
      probXYonTrackNoLayer1 = 0;
    }

    // Store the values in the collection
    resultSiPixelTrackProbQXYColl->emplace_back(probQonTrack, probXYonTrack);
    resultSiPixelTrackProbQXYNoLayer1Coll->emplace_back(probQonTrackNoLayer1, probXYonTrackNoLayer1);
  }  // end loop on track collection

  if(debugFlag_) {
     LogPrint("SiPixelTrackProbQXYProducer") << "In this event the ratio of low probQ tracks / all tracks " << numTrackWSmallProbQ/float(numTrack);
  }

  // Populate the event with the value map
  auto trackProbQXYMatch = std::make_unique<edm::ValueMap<reco::SiPixelTrackProbQXY>>();
  edm::ValueMap<reco::SiPixelTrackProbQXY>::Filler filler(*trackProbQXYMatch);
  filler.insert(trackCollectionHandle, resultSiPixelTrackProbQXYColl->begin(), resultSiPixelTrackProbQXYColl->end());
  filler.fill();
  iEvent.put(putProbQXYVMToken_, std::move(trackProbQXYMatch));

  auto trackProbQXYMatchNoLayer1 = std::make_unique<edm::ValueMap<reco::SiPixelTrackProbQXY>>();
  edm::ValueMap<reco::SiPixelTrackProbQXY>::Filler fillerNoLayer1(*trackProbQXYMatchNoLayer1);
  fillerNoLayer1.insert(trackCollectionHandle,
                        resultSiPixelTrackProbQXYNoLayer1Coll->begin(),
                        resultSiPixelTrackProbQXYNoLayer1Coll->end());
  fillerNoLayer1.fill();
  iEvent.put(putProbQXYNoLayer1VMToken_, std::move(trackProbQXYMatchNoLayer1));
}

void SiPixelTrackProbQXYProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.setComment("Producer that creates SiPixel Charge and shape probabilities combined for tracks");
  desc.addUntracked<bool>("debug", false);
  desc.add<std::string>("pixelCPE", {"PixelCPEClusterRepair"})
      ->setComment("PixelCPE name");
  desc.add<edm::InputTag>("tracks", edm::InputTag("generalTracks"))
      ->setComment("Input track collection for the producer");
  desc.add<edm::InputTag>("dedxInfos", edm::InputTag("dedxHitInfo"))
      ->setComment("Input track dedxHitInfo");
  descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiPixelTrackProbQXYProducer);
