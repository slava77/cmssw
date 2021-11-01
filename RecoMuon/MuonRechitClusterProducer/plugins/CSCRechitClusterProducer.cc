// -*- C++ -*-
//
// Package:    RecoMuon/MuonRecHitProducer
// Class:      MuonRecHitProducer
// 
/**\class MuonRecHitProducer MuonRecHitProducer.cc RecoMuon/MuonRecHitProducer/plugins/MuonRecHitProducer.cc

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

#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include <DataFormats/CSCRecHit/interface/CSCRecHit2D.h>
#include <DataFormats/CSCRecHit/interface/CSCRecHit2DCollection.h>
#include "DataFormats/MuonReco/interface/MuonCSCRecHitCluster.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"

#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
//
// class declaration
//
typedef edm::Ref<CSCRecHit2DCollection>  rechitRef;
typedef std::vector<rechitRef>  rechits;

class CSCRechitClusterProducer : public edm::stream::EDProducer<> {
   public:
      explicit CSCRechitClusterProducer(const edm::ParameterSet&);
      ~CSCRechitClusterProducer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void produce(edm::Event&, const edm::EventSetup&) override;
      edm::EDGetTokenT<CSCRecHit2DCollection> cscRechitInputToken_;
      const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> m_cscGeometryToken;
      typedef std::vector<reco::MuonCSCRecHitCluster> MuonCSCRecHitClusterCollection;

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
CSCRechitClusterProducer::CSCRechitClusterProducer(const edm::ParameterSet& iConfig)
 : m_cscGeometryToken(esConsumes<CSCGeometry,MuonGeometryRecord>()) 
{
  
   rParam_ = iConfig.getParameter<double>("rParam");
   nRechitMin_ = iConfig.getParameter<int>("nRechitMin");

   cscRechitInputToken_ = consumes<CSCRecHit2DCollection>(edm::InputTag("csc2DRecHits")),
   produces<MuonCSCRecHitClusterCollection>();
  
}


CSCRechitClusterProducer::~CSCRechitClusterProducer(){
}


//
// member functions
//

// ------------ method called to produce the data  ------------
void
CSCRechitClusterProducer::produce(edm::Event& ev, const edm::EventSetup& iSetup)
{
  edm::ESHandle<CSCGeometry> cscG;
  cscG = iSetup.getHandle(m_cscGeometryToken);

  edm::Handle<CSCRecHit2DCollection> cscRechits;

  ev.getByToken(cscRechitInputToken_,cscRechits);

  std::vector<fastjet::PseudoJet> fjInput;

  fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm, rParam_);

  int nRecHits = cscRechits->size();
  CSCRecHit2DCollection::const_iterator recIt;


  inputs_.clear();
  fjInput.clear();

  for (recIt = cscRechits->begin(); recIt != cscRechits->end(); recIt++) {
    auto cscRechit = (*recIt);
    LocalPoint  cscRecHitLocalPosition       = cscRechit.localPosition();
    CSCDetId cscdetid = cscRechit.cscDetId();
    const CSCChamber* cscchamber = cscG->chamber(cscdetid);
    if (cscchamber) {
        GlobalPoint globalPosition = cscchamber->toGlobal(cscRecHitLocalPosition);
        float x = globalPosition.x();
        float y = globalPosition.y();
        float z = globalPosition.z();
        rechitRef csc_ref = rechitRef(cscRechits, recIt-cscRechits->begin());
        inputs_.push_back( csc_ref );
        fjInput.push_back(fastjet::PseudoJet( x, y, z, 1.0));
        fjInput.back().set_user_index(recIt-cscRechits->begin());
    }
  }
  fastjet::ClusterSequence clus_seq ( fjInput , jet_def);

  double ptmin = 0.0;
  std::vector<fastjet::PseudoJet> fjJets = fastjet::sorted_by_pt(clus_seq.inclusive_jets(ptmin));

  auto CSCclusters = std::make_unique<MuonCSCRecHitClusterCollection>();
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
      float time=0.0;
      int nME11_12 =0;
      for (auto & rechit : rechits){

        CSCDetId cscdetid = rechit->cscDetId();
        int endcap = CSCDetId::endcap(cscdetid) == 1 ? 1 : -1;
        int  chamber =  endcap * (CSCDetId::station(cscdetid)*10 + CSCDetId::ring(cscdetid));
        if( abs(chamber)==11 || abs(chamber)==12) nME11_12++;
        time+= (rechit->tpeak() + rechit->wireTime());
      }
      time = time/(2*rechits.size()); 

      
      float jetX = fjJet.px()/rechits.size(); 
      float jetY = fjJet.py()/rechits.size(); 
      float jetZ = fjJet.pz()/rechits.size(); 
     
      reco::MuonCSCRecHitCluster cls(jetX,jetY,jetZ,rechits.size(),time,nME11_12,rechits);
      //std::cout<<"CSCrechitCluster" << " my phi =  " <<   phi  << " my eta =  " <<    eta ;
      //std::cout<< " cls phi =  " <<    cls.phi()  << " cls eta =  " <<    (cls.eta()) <<std::endl;

      
      CSCclusters->push_back(cls);
    }
  }
  ev.put(std::move(CSCclusters));
 
}

std::vector<rechitRef > CSCRechitClusterProducer::getConstituents(const std::vector<fastjet::PseudoJet>& fjConstituents) {

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
CSCRechitClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCRechitClusterProducer);
