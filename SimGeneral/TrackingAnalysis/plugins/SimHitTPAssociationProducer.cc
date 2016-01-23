#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <utility>

#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Common/interface/Handle.h"

#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFramePlaybackInfoExtended.h"
#include "SimGeneral/TrackingAnalysis/interface/TPtoSimHitPtr.h"

#include "SimGeneral/TrackingAnalysis/interface/SimHitTPAssociationProducer.h"

SimHitTPAssociationProducer::SimHitTPAssociationProducer(const edm::ParameterSet & cfg) 
  : _simHitSrc(cfg.getParameter<std::vector<edm::InputTag> >("simHitSrc")),
    _xframesSrc(cfg.getParameter<std::vector<edm::InputTag> >("xframesSrc")),
    _trackingParticleSrc(cfg.getParameter<edm::InputTag>("trackingParticleSrc"))
{
  produces<SimHitTPAssociationList>();
  produces<std::vector<TPtoSimHitPtr> >();
}

SimHitTPAssociationProducer::~SimHitTPAssociationProducer() {
}
		

void SimHitTPAssociationProducer::produce(edm::Event& iEvent, const edm::EventSetup& es) {
  std::auto_ptr<SimHitTPAssociationList> simHitTPList(new SimHitTPAssociationList);
  std::auto_ptr<std::vector<TPtoSimHitPtr> > simHitPTPList(new std::vector<TPtoSimHitPtr>);
 
  // TrackingParticle
  edm::Handle<TrackingParticleCollection>  TPCollectionH;
  iEvent.getByLabel(_trackingParticleSrc,  TPCollectionH);

  // prepare temporary map between SimTrackId and TrackingParticle index
  std::map<std::pair<size_t, EncodedEventId>, TrackingParticleRef> mapping;
  for (TrackingParticleCollection::size_type itp = 0; itp < TPCollectionH.product()->size(); ++itp) {
    TrackingParticleRef trackingParticle(TPCollectionH, itp);
    // SimTracks inside TrackingParticle
    EncodedEventId eid(trackingParticle->eventId());
    for (auto itrk  = trackingParticle->g4Track_begin(); itrk != trackingParticle->g4Track_end(); ++itrk) {
      std::pair<uint32_t, EncodedEventId> trkid(itrk->trackId(), eid);
      mapping.insert(std::make_pair(trkid, trackingParticle));
    }
  }

  // PSimHits
  for (auto psit=_simHitSrc.begin();psit<_simHitSrc.end();++psit) {
    edm::Handle<edm::PSimHitContainer>  PSimHitCollectionH;
    iEvent.getByLabel(*psit,  PSimHitCollectionH);
    for (unsigned int psimHit = 0;psimHit != PSimHitCollectionH->size();++psimHit) {
      TrackPSimHitRef pSimHitRef(PSimHitCollectionH,psimHit);
      std::pair<uint32_t, EncodedEventId> simTkIds(pSimHitRef->trackId(),pSimHitRef->eventId()); 
      auto ipos = mapping.find(simTkIds);
      if (ipos != mapping.end()) {
	simHitTPList->push_back(std::make_pair(ipos->second,pSimHitRef));
      }
    }
  } 
  
  std::sort(simHitTPList->begin(),simHitTPList->end(),simHitTPAssociationListGreater);
  iEvent.put(simHitTPList);

  edm::Handle<CrossingFramePlaybackInfoExtended> xfInfo;
  iEvent.getByLabel(edm::InputTag("mix","","@skipCurrentProcess"), xfInfo);
  for (auto xftag : _xframesSrc ){
    edm::Handle<CrossingFrame<PSimHit> > xfh;
    iEvent.getByLabel(xftag, xfh);
    auto const& xf = *xfh.product();
    auto nAll = xf.getNrSignals() + xf.getNrPileups();
    auto puOffBcr(xf.getPileupOffsetsBcr());
    auto puOffSrc(xf.getPileupOffsetsSource());
    auto const& offsets = xf.pileupOffsets();
    unsigned int iSource = 0;
    unsigned int iBcr = 0;
    unsigned int iPUEvent = 0;
    unsigned int ioff = 0;
    std::vector<std::vector<edm::EventID> > puids;
    while (puids.size()==0 && iSource < puOffSrc.size()){
      xfInfo->getEventStartInfo(puids, iSource);
      if (puids.size() == 0) iSource++;
    }
    while (puids[iBcr].size() == 0 && iBcr < puids.size()) iBcr++;

    for (auto iH = 0U; iH < nAll; ++iH){
      const PSimHit* hitP = &xf.getObject(iH);
      EncodedEventId eid;
      auto iPU = iH - xf.getNrSignals();
      if (iH < xf.getNrSignals()) eid = hitP->eventId();
      else {
	while (ioff + 1 < offsets.size() && offsets[ioff+1] <=  iPU){
	  ioff++;
	  iPUEvent++;
	  if (iPUEvent == puids[iBcr].size()){
	    iPUEvent=0;
	    iBcr++;
	    if (iBcr == puids.size()){
	      iBcr = 0;
	      iSource++;
	      if (iSource < puOffSrc.size()){
		xfInfo->getEventStartInfo(puids, iSource);
	      } else break;
	    }
	  }
	  if (iSource == puOffSrc.size()) edm::LogWarning("BadAssociation")<<"Something is wrong with iSource "<<iSource;
	  
	}
	eid = EncodedEventId(xf.getBunchRange().first+iBcr, iPUEvent+1);
      }
      std::pair<uint32_t, EncodedEventId> simTkIds(hitP->trackId(),eid);
      auto ipos = mapping.find(simTkIds);
      if (ipos != mapping.end()) {
	simHitPTPList->push_back(TPtoSimHitPtr(ipos->second, hitP));
      }
    }
  }
  std::sort(simHitPTPList->begin(),simHitPTPList->end(), TPtoSimHitPtr::greater );
  iEvent.put(simHitPTPList);
}

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(SimHitTPAssociationProducer);
