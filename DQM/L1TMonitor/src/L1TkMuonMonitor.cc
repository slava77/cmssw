/*
 * \file L1TkMuonMonitor.cc
 *
 * Date: 2015/06/16 12:00:00
 * Revision: 0.03
 * \author S.Baranov
 *
 */

#include "DQM/L1TMonitor/interface/L1TkMuonMonitor.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

using namespace std;
using namespace edm;
using namespace reco;

typedef edm::View<reco::LeafCandidate>  CandView;
 
const double L1TkMuonMonitor::piconv_ = 180./M_PI;

L1TkMuonMonitor::L1TkMuonMonitor(const ParameterSet& ps)
  : gmtSource_( ps.getParameter< InputTag >("gmtSource") )
 {

  // verbosity switch
  verbose_ = ps.getUntrackedParameter<bool>("verbose", false);

  if(verbose_) cout << "L1TkMuonMonitor: constructor...." << endl;

  dbe = NULL;
  if ( ps.getUntrackedParameter<bool>("DQMStore", false) ) 
  {
    dbe = Service<DQMStore>().operator->();
    dbe->setVerbose(0);
  }

  outputFile_ = ps.getUntrackedParameter<string>("outputFile", "");
  if ( outputFile_.size() != 0 ) {
    LogInfo("L1TkMuonMonitor") << "L1T Monitoring histograms will be saved to " << outputFile_.c_str();
  }

  bool disable = ps.getUntrackedParameter<bool>("disableROOToutput", false);
  if(disable){
    outputFile_="";
  }

  // input tags
  candInputTag_ = ps.getParameter<edm::InputTag>("candInputTag");
  LogInfo("L1TkMuonMonitor") << " L1TkMuonMonitor::L1TkMuonMonitor candInputTag " << candInputTag_;

  if ( dbe !=NULL ) {
    dbe->setCurrentFolder("L1T/L1TkMuon");
  }

}

L1TkMuonMonitor::~L1TkMuonMonitor()
{
}

void L1TkMuonMonitor::beginJob()
{
  nev_ = 0;
}

void L1TkMuonMonitor::beginRun(const edm::Run& r, const edm::EventSetup& c)
{
  if(nev_==0) {
      book_(c);
  }
}

void L1TkMuonMonitor::endJob(void)
{
  if(verbose_) cout << "L1TkMuonMonitor: end job...." << endl;
  LogInfo("EndJob") << "analyzed " << nev_ << " events"; 

 if ( outputFile_.size() != 0  && dbe ) dbe->save(outputFile_);

 return;
}

void L1TkMuonMonitor::analyze(const Event& e, const EventSetup& c)
{
  
  nev_++; 

  edm::Handle<CandView> aH; 
  e.getByLabel(candInputTag_, aH);

  if(!aH.isValid()){
    LogDebug("L1TkMuonMonitor") << " ::analyze Can't find CandView with label **candInputTag**";
  }

  const CandView& cands(*aH.product());

  int track = 0, trackPt=0;
  double maxPt = 0.;
  double dxy;
  for (auto cand : cands){
    track++;
    if (cand.pt() >= maxPt){
      trackPt = track;
      maxPt = cand.pt();
    }
  }

  track = 0;
  for (auto cand : cands){
    track++;
    if ( track == trackPt ){
      dxy = cand.vx()*sin(cand.phi())-cand.vy()*cos(cand.phi());

      LogDebug("L1TkMuonMonitor") << " ::analyze track " 
          << track << " Pt " << cand.pt() << " Phi " << cand.phi() << " Eta " << cand.eta() << " Dxy " << dxy 
          << " Z " << cand.vz() << " Q " << cand.charge() << " cand.pdgId " << cand.pdgId();

      l1tkmu_Pt->Fill(cand.pt());
      l1tkmu_Phi->Fill(cand.phi());
      l1tkmu_Eta->Fill(cand.eta());
      l1tkmu_Dxy->Fill(dxy);
      l1tkmu_Z->Fill(cand.vz());
      l1tkmu_Q->Fill(float(cand.charge()));
      l1tkmu_Pdgid->Fill(float(cand.pdgId()));
 
      if ( std::abs(cand.eta()) <= 1.1 ){
	// Barrel
        l1tkmu_Pt_barrel->Fill(cand.pt());
        l1tkmu_Phi_barrel->Fill(cand.phi());
        l1tkmu_Eta_barrel->Fill(cand.eta());
        l1tkmu_Dxy_barrel->Fill(dxy);
        l1tkmu_Z_barrel->Fill(cand.vz());
        l1tkmu_Q_barrel->Fill(float(cand.charge()));
        l1tkmu_Pdgid_barrel->Fill(float(cand.pdgId()));
      } else {
        // Endcap
        l1tkmu_Pt_endcap->Fill(cand.pt());
        l1tkmu_Phi_endcap->Fill(cand.phi());
        l1tkmu_Eta_endcap->Fill(cand.eta());
        l1tkmu_Dxy_endcap->Fill(dxy);
        l1tkmu_Z_endcap->Fill(cand.vz());
        l1tkmu_Q_endcap->Fill(float(cand.charge()));
        l1tkmu_Pdgid_endcap->Fill(float(cand.pdgId()));
      }
    }
  }
}

void L1TkMuonMonitor::book_(const EventSetup& c)
{
  // get hold of back-end interface
  DQMStore* dbe = 0;
  dbe = Service<DQMStore>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("L1T/L1TkMuon");
    dbe->rmdir("L1T/L1TkMuon");
  }

  if ( dbe ) 
  {
    dbe->setCurrentFolder("L1T/L1TkMuon");

    /*
     *  All
     */
    l1tkmu_Pt    = dbe->book1D("l1tkmu_Pt","L1 Muon Pt",       100,  0.,250. );
    l1tkmu_Pt->setAxisTitle("l1tkmu_Pt",1);

    l1tkmu_Phi   = dbe->book1D("l1tkmu_Phi","L1 Muon Phi",     100, -3.,3. );
    l1tkmu_Phi->setAxisTitle("l1tkmu_Phi",1);

    l1tkmu_Eta   = dbe->book1D("l1tkmu_Eta","L1 Muon Eta",     100, -3.,3. );
    l1tkmu_Eta->setAxisTitle("l1tkmu_Eta",1);

    l1tkmu_Dxy   = dbe->book1D("l1tkmu_Dxy","L1 Muon Dxy",     100,-15.,15. );
    l1tkmu_Dxy->setAxisTitle("l1tkmu_Dxy",1);

    l1tkmu_Z     = dbe->book1D("l1tkmu_Z","L1 Muon Z",         100,-20.,20. );
    l1tkmu_Z->setAxisTitle("l1tkmu_Z",1);

    l1tkmu_Q     = dbe->book1D("l1tkmu_Q","L1 Muon Q",           4, -2.,2. );
    l1tkmu_Q->setAxisTitle("l1tkmu_Q",1);

    l1tkmu_Pdgid = dbe->book1D("l1tkmu_Pdgid","L1 Muon Pdgid", 100,-50.,50. );
    l1tkmu_Pdgid->setAxisTitle("l1tkmu_Pdgid",1);

    /*
     *  "barrel" (|eta|<=1.1)
     */
    l1tkmu_Pt_barrel    = dbe->book1D("l1tkmu_Pt_barrel","L1 Muon Pt barrel",       100,  0.,250. );
    l1tkmu_Pt_barrel->setAxisTitle("l1tkmu_Pt_barrel",1);

    l1tkmu_Phi_barrel   = dbe->book1D("l1tkmu_Phi_barrel","L1 Muon Phi barrel",     100, -3.,3. );
    l1tkmu_Phi_barrel->setAxisTitle("l1tkmu_Phi_barrel",1);

    l1tkmu_Eta_barrel   = dbe->book1D("l1tkmu_Eta_barrel","L1 Muon Eta barrel",     100, -3.,3. );
    l1tkmu_Eta_barrel->setAxisTitle("l1tkmu_Eta_barrel",1);

    l1tkmu_Dxy_barrel   = dbe->book1D("l1tkmu_Dxy_barrel","L1 Muon Dxy barrel",     100,-15.,15. );
    l1tkmu_Dxy_barrel->setAxisTitle("l1tkmu_Dxy_barrel",1);

    l1tkmu_Z_barrel     = dbe->book1D("l1tkmu_Z_barrel","L1 Muon Z barrel",         100,-20.,20. );
    l1tkmu_Z_barrel->setAxisTitle("l1tkmu_Z_barrel",1);

    l1tkmu_Q_barrel     = dbe->book1D("l1tkmu_Q_barrel","L1 Muon Q barrel",           4, -2.,2. );
    l1tkmu_Q_barrel->setAxisTitle("l1tkmu_Q_barrel",1);

    l1tkmu_Pdgid_barrel = dbe->book1D("l1tkmu_Pdgid_barrel","L1 Muon Pdgid barrel", 100,-50.,50. );
    l1tkmu_Pdgid_barrel->setAxisTitle("l1tkmu_Pdgid_barrel",1);

    /*
     *  "endcap" (|eta|>1.1)
     */
    l1tkmu_Pt_endcap    = dbe->book1D("l1tkmu_Pt_endcap","L1 Muon Pt endcap",       100,0.,250. );
    l1tkmu_Pt_endcap->setAxisTitle("l1tkmu_Pt_endcap",1);

    l1tkmu_Phi_endcap   = dbe->book1D("l1tkmu_Phi_endcap","L1 Muon Phi endcap",     100,-3.,3. );
    l1tkmu_Phi_endcap->setAxisTitle("l1tkmu_Phi_endcap",1);

    l1tkmu_Eta_endcap   = dbe->book1D("l1tkmu_Eta_endcap","L1 Muon Eta endcap",     100,-3.,3. );
    l1tkmu_Eta_endcap->setAxisTitle("l1tkmu_Eta_endcap",1);

    l1tkmu_Dxy_endcap   = dbe->book1D("l1tkmu_Dxy_endcap","L1 Muon Dxy endcap",     100,-15.,15. );
    l1tkmu_Dxy_endcap->setAxisTitle("l1tkmu_Dxy_endcap",1);

    l1tkmu_Z_endcap     = dbe->book1D("l1tkmu_Z_endcap","L1 Muon Z endcap",         100,-20.,20. );
    l1tkmu_Z_endcap->setAxisTitle("l1tkmu_Z_endcap",1);

    l1tkmu_Q_endcap     = dbe->book1D("l1tkmu_Q_endcap","L1 Muon Q endcap",           4, -2.,2. );
    l1tkmu_Q_endcap->setAxisTitle("l1tkmu_Q_endcap",1);

    l1tkmu_Pdgid_endcap = dbe->book1D("l1tkmu_Pdgid_endcap","L1 Muon Pdgid endcap", 100,-50.,50. );
    l1tkmu_Pdgid_endcap->setAxisTitle("l1tkmu_Pdgid_endcap",1);

  }  
}

DEFINE_FWK_MODULE(L1TkMuonMonitor);
