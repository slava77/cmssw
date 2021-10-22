// -*- C++ -*-
//
// Package:    RecoMuon/MuonShowerProducer
// Class:      MuonShowerProducer
// 
/**\class MuonShowerProducer MuonShowerProducer.cc RecoMuon/MuonShowerProducer/plugins/MuonShowerProducer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ka Hei Martin Kwok
//         Created:  Mon, 11 Oct 2021 19:52:09 GMT
//
//


// system include files
#include <memory>
#include <cmath>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include <FWCore/MessageLogger/interface/MessageLogger.h>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/DetId/interface/DetId.h"
#include <DataFormats/DTRecHit/interface/DTRecHitCollection.h>
#include "DataFormats/MuonReco/interface/MuonShowerDTCluster.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
//
// class declaration
//
typedef edm::Ref<DTRecHitCollection>  rechitRef;
typedef std::vector<rechitRef>  rechits;

class DTRechitClusterProducer : public edm::stream::EDProducer<> {
   public:
      explicit DTRechitClusterProducer(const edm::ParameterSet&);
      ~DTRechitClusterProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      edm::EDGetTokenT<DTRecHitCollection> dtRechitInputToken_;
      const edm::ESGetToken<DTGeometry, MuonGeometryRecord> m_dtGeometryToken;
      typedef std::vector<reco::MuonShowerDTCluster> MuonShowerDTClusterCollection;

      // ----------member data ---------------------------
   protected:

    double rParam_ ;            // distance paramter
    int    nRechitMin_;         // min number of rechits 
    std::vector<rechitRef> inputs_;
    std::vector<rechitRef> getConstituents(const std::vector<fastjet::PseudoJet>& fjConstituents);

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DTRechitClusterProducer::DTRechitClusterProducer(const edm::ParameterSet& iConfig)
 : m_dtGeometryToken(esConsumes<DTGeometry,MuonGeometryRecord>()) 
{
  
   rParam_ = iConfig.getParameter<double>("rParam");
   nRechitMin_ = iConfig.getParameter<int>("nRechitMin");

   dtRechitInputToken_ = consumes<DTRecHitCollection>(edm::InputTag("dt1DRecHits")),
   produces<MuonShowerDTClusterCollection>();
  
}


DTRechitClusterProducer::~DTRechitClusterProducer(){
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
DTRechitClusterProducer::produce(edm::Event& ev, const edm::EventSetup& iSetup)
{
  edm::ESHandle<DTGeometry> dtG;
  dtG = iSetup.getHandle(m_dtGeometryToken);

  edm::Handle<DTRecHitCollection> dtRechits;

  ev.getByToken(dtRechitInputToken_,dtRechits);

  std::vector<fastjet::PseudoJet> fjInput;

  fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm, rParam_);

  int nRecHits = dtRechits->size();
  DTRecHitCollection::const_iterator recIt;


  inputs_.clear();
  fjInput.clear();

  for (recIt = dtRechits->begin(); recIt != dtRechits->end(); recIt++) {
    auto dtRechit = (*recIt);
    LocalPoint  localPosition       = dtRechit.localPosition();
    DetId geoid = dtRechit.geographicalId();
    DTChamberId dtdetid = DTChamberId(geoid);
    const DTChamber * dtchamber = dtG->chamber(dtdetid);
    if (dtchamber) {
        GlobalPoint globalPosition = dtchamber->toGlobal(localPosition);
        float x = globalPosition.x();
        float y = globalPosition.y();
        float z = globalPosition.z();
        rechitRef dt_ref = rechitRef(dtRechits, recIt-dtRechits->begin());
        inputs_.push_back( dt_ref );
        fjInput.push_back(fastjet::PseudoJet( x, y, z, 1.0));
        fjInput.back().set_user_index(recIt-dtRechits->begin());
    }
  }
  fastjet::ClusterSequence clus_seq ( fjInput , jet_def);

  double ptmin = 0.0;
  std::vector<fastjet::PseudoJet> fjJets = fastjet::sorted_by_pt(clus_seq.inclusive_jets(ptmin));

  auto DTclusters = std::make_unique<MuonShowerDTClusterCollection>();
  if (!fjJets.empty()){
    for (unsigned int ijet = 0; ijet < fjJets.size(); ++ijet) {

      // get the fastjet jet
      const fastjet::PseudoJet& fjJet = fjJets[ijet];

      // skip if the cluster has too few rechits
      if (int(fjJet.constituents().size()) < nRechitMin_) continue;
      // get the constituents from fastjet
      std::vector<fastjet::PseudoJet> const& fjConstituents = fastjet::sorted_by_pt(fjJet.constituents());

      std::vector<rechitRef > const& rechits = getConstituents(fjConstituents);

      //Derive cluster properties
      int nMB1 =0;
      for (auto & rechit : rechits){

        DetId geoid = rechit->geographicalId();
        DTChamberId dtdetid = DTChamberId(geoid);
        
        if( dtdetid.station()==1 ) nMB1++;
      }
      
      float jetX = fjJet.px()/rechits.size(); 
      float jetY = fjJet.py()/rechits.size(); 
      float jetZ = fjJet.pz()/rechits.size(); 
     
      reco::MuonShowerDTCluster cls(jetX,jetY,jetZ,rechits.size(),nMB1,rechits);
      //std::cout<<"CSCrechitCluster" << " my phi =  " <<   phi  << " my eta =  " <<    eta ;
      //std::cout<< " cls phi =  " <<    cls.phi()  << " cls eta =  " <<    (cls.eta()) <<std::endl;

      
      DTclusters->push_back(cls);
    }
  }
  ev.put(std::move(DTclusters));
 
}

std::vector<rechitRef > DTRechitClusterProducer::getConstituents(const std::vector<fastjet::PseudoJet>& fjConstituents) {

  std::vector<rechitRef> result;
  for (unsigned int i = 0; i < fjConstituents.size(); i++) {
    auto index = fjConstituents[i].user_index();
    if (index >= 0 && static_cast<unsigned int>(index) < inputs_.size()) {
      result.emplace_back(inputs_[index]);
    }
  }
  return result;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DTRechitClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DTRechitClusterProducer);
