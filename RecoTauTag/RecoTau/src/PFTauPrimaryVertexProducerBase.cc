#include "RecoTauTag/RecoTau/interface/PFTauPrimaryVertexProducerBase.h"

PFTauPrimaryVertexProducerBase::PFTauPrimaryVertexProducerBase(const edm::ParameterSet& iConfig):
  pftauToken_(consumes<std::vector<reco::PFTau> >(iConfig.getParameter<edm::InputTag>("PFTauTag"))),
  electronToken_(consumes<edm::View<reco::Electron> >(iConfig.getParameter<edm::InputTag>("ElectronTag"))),
  muonToken_(consumes<edm::View<reco::Muon> >(iConfig.getParameter<edm::InputTag>("MuonTag"))),
  pvToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("PVTag"))),
  beamSpotToken_(consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"))),
  algorithm_(iConfig.getParameter<int>("Algorithm")),
  qualityCutsPSet_(iConfig.getParameter<edm::ParameterSet>("qualityCuts")),
  useBeamSpot_(iConfig.getParameter<bool>("useBeamSpot")),
  useSelectedTaus_(iConfig.getParameter<bool>("useSelectedTaus")),
  removeMuonTracks_(iConfig.getParameter<bool>("RemoveMuonTracks")),
  removeElectronTracks_(iConfig.getParameter<bool>("RemoveElectronTracks"))
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<edm::ParameterSet> discriminators =iConfig.getParameter<std::vector<edm::ParameterSet> >("discriminators");
  // Build each of our cuts
  BOOST_FOREACH(const edm::ParameterSet &pset, discriminators) {
    DiscCutPair* newCut = new DiscCutPair();
    newCut->inputToken_ =consumes<reco::PFTauDiscriminator>(pset.getParameter<edm::InputTag>("discriminator"));

    if ( pset.existsAs<std::string>("selectionCut") ) newCut->cutFormula_ = new TFormula("selectionCut", pset.getParameter<std::string>("selectionCut").data());
    else newCut->cut_ = pset.getParameter<double>("selectionCut");
    discriminators_.push_back(newCut);
  }
  // Build a string cut if desired
  if (iConfig.exists("cut")) cut_.reset(new StringCutObjectSelector<reco::PFTau>(iConfig.getParameter<std::string>( "cut" )));
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  produces<edm::AssociationVector<reco::PFTauRefProd, std::vector<reco::VertexRef> > >();
  produces<reco::VertexCollection>("PFTauPrimaryVertices"); 

  vertexAssociator_.reset(new reco::tau::RecoTauVertexAssociator(qualityCutsPSet_,consumesCollector()));
}

PFTauPrimaryVertexProducerBase::~PFTauPrimaryVertexProducerBase(){}

namespace {
  edm::Ptr<reco::TrackBase> getTrack(const reco::Candidate& cand) {
    const reco::PFCandidate* pfCandPtr = dynamic_cast<const reco::PFCandidate*>(&cand);
    if (pfCandPtr) {
      if      ( pfCandPtr->trackRef().isNonnull()    ) return edm::refToPtr(pfCandPtr->trackRef());
      else if ( pfCandPtr->gsfTrackRef().isNonnull() ) return edm::refToPtr(pfCandPtr->gsfTrackRef());
      else return edm::Ptr<reco::TrackBase>();
    }
    const pat::PackedCandidate* pCand = dynamic_cast<const pat::PackedCandidate*>(&cand);
    if (pCand && pCand->hasTrackDetails()) {
      const reco::TrackBase* trkPtr = &pCand->pseudoTrack();
      return edm::Ptr<reco::TrackBase>(trkPtr,0);
    }
    return edm::Ptr<reco::TrackBase>();
  }
}

void PFTauPrimaryVertexProducerBase::produce(edm::Event& iEvent,const edm::EventSetup& iSetup){
  
  beginEvent(iEvent, iSetup);

  // Obtain Collections
  edm::ESHandle<TransientTrackBuilder> transTrackBuilder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder",transTrackBuilder);
  
  edm::Handle<std::vector<reco::PFTau> > pfTaus;
  iEvent.getByToken(pftauToken_,pfTaus);

  edm::Handle<edm::View<reco::Electron> > electrons;
  if(removeElectronTracks_) iEvent.getByToken(electronToken_,electrons);

  edm::Handle<edm::View<reco::Muon> > muons;
  if(removeMuonTracks_) iEvent.getByToken(muonToken_,muons);

  edm::Handle<reco::VertexCollection > vertices;
  iEvent.getByToken(pvToken_,vertices);

  edm::Handle<reco::BeamSpot> beamSpot;
  if(useBeamSpot_) iEvent.getByToken(beamSpotToken_,beamSpot);

  // Set Association Map
  auto avPFTauPV = std::make_unique<edm::AssociationVector<reco::PFTauRefProd, std::vector<reco::VertexRef>>>(reco::PFTauRefProd(pfTaus));
  auto vertexCollection_out = std::make_unique<reco::VertexCollection>();
  reco::VertexRefProd vertexRefProd_out = iEvent.getRefBeforePut<reco::VertexCollection>("PFTauPrimaryVertices");

  // Load each discriminator
  BOOST_FOREACH(DiscCutPair *disc, discriminators_) {
    edm::Handle<reco::PFTauDiscriminator> discr;
    iEvent.getByToken(disc->inputToken_, discr);
    disc->discr_ = &(*discr);
  }

  // Set event for VerexAssociator if needed
  if(useInputPV==algorithm_)
    vertexAssociator_->setEvent(iEvent);

  // For each Tau Run Algorithim 
  if(pfTaus.isValid()){
    for(reco::PFTauCollection::size_type iPFTau = 0; iPFTau < pfTaus->size(); iPFTau++) {
      reco::PFTauRef tau(pfTaus, iPFTau);
      reco::VertexRef thePVRef;
      if(useInputPV==algorithm_){
	thePVRef = vertexAssociator_->associatedVertex(*tau); 
      }
      else if(useFrontPV==algorithm_){
	thePVRef = reco::VertexRef(vertices,0);
      }
      reco::Vertex thePV = *thePVRef;
      ///////////////////////
      // Check if it passed all the discrimiantors
      bool passed(true); 
      BOOST_FOREACH(const DiscCutPair* disc, discriminators_) {
        // Check this discriminator passes
	bool passedDisc = true;
	if ( disc->cutFormula_ )passedDisc = (disc->cutFormula_->Eval((*disc->discr_)[tau]) > 0.5);
	else passedDisc = ((*disc->discr_)[tau] > disc->cut_);
        if ( !passedDisc ){passed = false; break;}
      }
      if (passed && cut_.get()){passed = (*cut_)(*tau);}
      if (passed){
	std::vector<edm::Ptr<reco::TrackBase> > signalTracks;
	for(reco::PFTauCollection::size_type jPFTau = 0; jPFTau < pfTaus->size(); jPFTau++) {
	  if(useSelectedTaus_ || iPFTau==jPFTau){
	    reco::PFTauRef pfTauRef(pfTaus, jPFTau);
	    ///////////////////////////////////////////////////////////////////////////////////////////////
	    // Get tracks from PFTau daughters
	    for(const auto& pfcand : pfTauRef->signalChargedHadrCands()) {
	      if(pfcand.isNull()) continue;
	      const edm::Ptr<reco::TrackBase>& trackPtr = getTrack(*pfcand);
	      if(trackPtr.isNonnull()) signalTracks.push_back(trackPtr);
	    }
	  }
	}
	// Get Muon tracks
	if(removeMuonTracks_){
	  if(muons.isValid()) {
	    for(const auto& muon: *muons){
	      if(muon.track().isNonnull()) signalTracks.push_back(edm::refToPtr(muon.track()));
	    }
	  }
	}
	// Get Electron Tracks
	if(removeElectronTracks_){
	  if(electrons.isValid()) {
	    for(const auto& electron: *electrons){
	      if(electron.track().isNonnull()) signalTracks.push_back(edm::refToPtr(electron.track()));
	    }
	  }
	}
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Get Non-Tau tracks
	std::vector<const reco::Track*> nonTauTracks;
	nonTauTracksInPV(thePVRef,signalTracks,nonTauTracks);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Refit the vertex
	TransientVertex transVtx;
	std::vector<reco::TransientTrack> transTracks;
	for(const auto track: nonTauTracks){
	  transTracks.push_back(transTrackBuilder->build(*track));
	}
	bool fitOK(true);
	if ( transTracks.size() >= 2 ) {
	  AdaptiveVertexFitter avf;
	  avf.setWeightThreshold(0.1); //weight per track. allow almost every fit, else --> exception
	  try {
	    if ( !useBeamSpot_ ){
	      transVtx = avf.vertex(transTracks);
	    } else {
	      transVtx = avf.vertex(transTracks, *beamSpot);
	    }
	  } catch (...) {
	    fitOK = false;
	  }
	} else fitOK = false;
	if ( fitOK ) thePV = transVtx;
      }
      reco::VertexRef vtxRef = reco::VertexRef(vertexRefProd_out, vertexCollection_out->size());
      vertexCollection_out->push_back(thePV);
      avPFTauPV->setValue(iPFTau, vtxRef);
    }
  }
  iEvent.put(std::move(vertexCollection_out),"PFTauPrimaryVertices");
  iEvent.put(std::move(avPFTauPV));
}

