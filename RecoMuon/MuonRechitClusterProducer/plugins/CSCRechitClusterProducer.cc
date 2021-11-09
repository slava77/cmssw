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
typedef edm::Ref<CSCRecHit2DCollection> rechitRef;
typedef std::vector<rechitRef> rechits;

class CSCRechitClusterProducer : public edm::stream::EDProducer<> {
public:
  explicit CSCRechitClusterProducer(const edm::ParameterSet&);
  ~CSCRechitClusterProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
  edm::EDGetTokenT<CSCRecHit2DCollection> cscRechitInputToken_;
  const edm::ESGetToken<CSCGeometry, MuonGeometryRecord> m_cscGeometryToken;
  typedef std::vector<reco::MuonCSCRecHitCluster> MuonCSCRecHitClusterCollection;

  // ----------member data ---------------------------
protected:
  double rParam_;   // distance paramter
  int nRechitMin_;  // min number of rechits
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
    : m_cscGeometryToken(esConsumes<CSCGeometry, MuonGeometryRecord>()) {
  rParam_ = iConfig.getParameter<double>("rParam");
  nRechitMin_ = iConfig.getParameter<int>("nRechitMin");

  cscRechitInputToken_ = consumes<CSCRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("cscRecHitLabel"));
  produces<MuonCSCRecHitClusterCollection>();
}

CSCRechitClusterProducer::~CSCRechitClusterProducer() {}

//
// member functions
//

// ------------ method called to produce the data  ------------
void CSCRechitClusterProducer::produce(edm::Event& ev, const edm::EventSetup& iSetup) {
  edm::ESHandle<CSCGeometry> cscG;
  cscG = iSetup.getHandle(m_cscGeometryToken);

  edm::Handle<CSCRecHit2DCollection> cscRechits;

  ev.getByToken(cscRechitInputToken_, cscRechits);

  std::vector<fastjet::PseudoJet> fjInput;

  fastjet::JetDefinition jet_def(fastjet::cambridge_algorithm, rParam_);

  CSCRecHit2DCollection::const_iterator recIt;

  inputs_.clear();
  fjInput.clear();

  for (recIt = cscRechits->begin(); recIt != cscRechits->end(); recIt++) {
    auto cscRechit = (*recIt);
    LocalPoint cscRecHitLocalPosition = cscRechit.localPosition();
    CSCDetId cscdetid = cscRechit.cscDetId();
    const CSCChamber* cscchamber = cscG->chamber(cscdetid);
    if (cscchamber) {
      GlobalPoint globalPosition = cscchamber->toGlobal(cscRecHitLocalPosition);
      float x = globalPosition.x();
      float y = globalPosition.y();
      float z = globalPosition.z();
      rechitRef csc_ref = rechitRef(cscRechits, recIt - cscRechits->begin());
      inputs_.push_back(csc_ref);
      fjInput.push_back(fastjet::PseudoJet(x, y, z, 1.0));
      fjInput.back().set_user_index(recIt - cscRechits->begin());
    }
  }
  fastjet::ClusterSequence clus_seq(fjInput, jet_def);

  double ptmin = 0.0;
  std::vector<fastjet::PseudoJet> fjJets = fastjet::sorted_by_pt(clus_seq.inclusive_jets(ptmin));

  auto CSCclusters = std::make_unique<MuonCSCRecHitClusterCollection>();
  if (!fjJets.empty()) {
    for (unsigned int ijet = 0; ijet < fjJets.size(); ++ijet) {
      // get the fastjet jet
      const fastjet::PseudoJet& fjJet = fjJets[ijet];

      // skip if the cluster has too few rechits
      if (int(fjJet.constituents().size()) < nRechitMin_)
        continue;
      // get the constituents from fastjet
      std::vector<fastjet::PseudoJet> const& fjConstituents = fastjet::sorted_by_pt(fjJet.constituents());

      std::vector<rechitRef> const& rechits = getConstituents(fjConstituents);

      //Derive cluster properties
      float time = 0.0;
      float time_strip = 0.0;  // for timeSpread calculation
      int nME11 = 0;
      int nME12 = 0;
      int nStation10 = 0;
      int totStation10 = 0;
      float avgStation10 = 0.0;
      float timeSpread = 0.0;
      std::map<int, int> station_count_map;
      for (auto& rechit : rechits) {
        CSCDetId cscdetid = rechit->cscDetId();
        int endcap = CSCDetId::endcap(cscdetid) == 1 ? 1 : -1;
        int chamber = endcap * (CSCDetId::station(cscdetid) * 10 + CSCDetId::ring(cscdetid));
        station_count_map[CSCDetId::station(cscdetid)]++;
        if (abs(chamber) == 11)
          nME11++;
        if (abs(chamber) == 12)
          nME12++;
        time += (rechit->tpeak() + rechit->wireTime());
        time_strip += rechit->tpeak();
      }
      //chamber statistics
      std::map<int, int>::iterator it;
      for (it = station_count_map.begin(); it != station_count_map.end(); it++) {
        if (it->second >= 10) {
          nStation10++;
          avgStation10 += (it->first) * (it->second);
          totStation10 += (it->second);
        }
      }
      if (nStation10 != 0) {
        avgStation10 = avgStation10 / totStation10;
      }
      time = time / (2 * rechits.size());
      time_strip = time_strip / (rechits.size());

      //derive cluster statistics
      for (auto& rechit : rechits) {
        timeSpread += (rechit->tpeak() - time_strip) * (rechit->tpeak() - time_strip);
      }
      timeSpread = std::sqrt(timeSpread / rechits.size());

      float jetX = fjJet.px() / rechits.size();
      float jetY = fjJet.py() / rechits.size();
      float jetZ = fjJet.pz() / rechits.size();

      reco::MuonCSCRecHitCluster cls(
          jetX, jetY, jetZ, rechits.size(), nStation10, avgStation10, time, timeSpread, nME11, nME12, rechits);
      //std::cout<<"CSCrechitCluster" ;
      //std::cout<< " my phi =  " <<   phi  << " my eta =  " <<    eta ;
      //std::cout<< " cls phi =  " <<    cls.phi()  << " cls eta =  " <<    (cls.eta())
      //std::cout<< " cls nStation10 =  " <<    cls.nStation10()  << " cls avgStation10 =  " <<    (cls.avgStation10());
      //std::cout<< " cls time =  " <<    cls.time()  << " cls timeSpread =  " <<    (cls.timeSpread()) <<std::endl;

      CSCclusters->push_back(cls);
    }
  }
  ev.put(std::move(CSCclusters));
}

std::vector<rechitRef> CSCRechitClusterProducer::getConstituents(const std::vector<fastjet::PseudoJet>& fjConstituents) {
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
void CSCRechitClusterProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<int>("nRechitMin", 50);
  desc.add<double>("rParam", 0.4);
  desc.add<edm::InputTag>("cscRecHitLabel", edm::InputTag("csc2DRecHits"));
  descriptions.add("CSCRechitClusterProducer", desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCRechitClusterProducer);
