/*
 * \file L1MUTK.cc
 *
 * $Date: 2015/06/16 12:00:00 $
 * $Revision: 0.03 $
 * \author S.Baranov
 *
 */

#include "DQM/L1TMonitor/interface/L1MUTK.h"
#include "DQMServices/Core/interface/DQMStore.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

using namespace std;
using namespace edm;
using namespace reco;

const double L1MUTK::piconv_ = 180. / acos(-1.);

L1MUTK::L1MUTK(const ParameterSet& ps)
  : gmtSource_( ps.getParameter< InputTag >("gmtSource") )
 {

  // verbosity switch
  verbose_ = ps.getUntrackedParameter<bool>("verbose", false);

  if(verbose_) cout << "L1MUTK: constructor...." << endl;

  dbe = NULL;
  if ( ps.getUntrackedParameter<bool>("DQMStore", false) ) 
  {
    dbe = Service<DQMStore>().operator->();
    dbe->setVerbose(0);
  }

  outputFile_ = ps.getUntrackedParameter<string>("outputFile", "");
  if ( outputFile_.size() != 0 ) {
    cout << "L1T Monitoring histograms will be saved to " << outputFile_.c_str() << endl;
  }

  bool disable = ps.getUntrackedParameter<bool>("disableROOToutput", false);
  if(disable){
    outputFile_="";
  }

  // input tags
  candInputTag_ = ps.getParameter<edm::InputTag>("candInputTag");
  cout << " L1MUTK::L1MUTK candInputTag " << candInputTag_ << endl;

  if ( dbe !=NULL ) {
    dbe->setCurrentFolder("L1T/L1MUTK");
  }

}

L1MUTK::~L1MUTK()
{
}

void L1MUTK::beginJob()
{
  nev_ = 0;
}

void L1MUTK::beginRun(const edm::Run& r, const edm::EventSetup& c)
{
  if(nev_==0) {
      book_(c);
  }
}

void L1MUTK::endJob(void)
{
  if(verbose_) cout << "L1MUTK: end job...." << endl;
  LogInfo("EndJob") << "analyzed " << nev_ << " events"; 

 if ( outputFile_.size() != 0  && dbe ) dbe->save(outputFile_);

 return;
}

void L1MUTK::analyze(const Event& e, const EventSetup& c)
{
  
  nev_++; 

  edm::Handle<CandView> aH; 
  e.getByLabel(candInputTag_, aH);

  if(!aH.isValid()){
    LogDebug("L1MUTK") << " ::analyze Can't find CandView with label **candInputTag**";
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

  cout << " trackPt " << trackPt << " " << maxPt << endl;

  track = 0;
  for (auto cand : cands){
    track++;
    if ( track == trackPt ){
      dxy = cand.vx()*sin(cand.phi())-cand.vy()*cos(cand.phi());

      LogDebug("L1MUTK") << " ::analyze track " 
          << track << " Pt " << cand.pt() << " Phi " << cand.phi() << " Eta " << cand.eta() << " Dxy " << dxy 
          << " Z " << cand.vz() << " Q " << cand.charge() << " cand.pdgId " << cand.pdgId();

      gem_Pt->Fill(cand.pt());
      gem_Phi->Fill(cand.phi());
      gem_Eta->Fill(cand.eta());
      gem_Dxy->Fill(dxy);
      gem_Z->Fill(cand.vz());
      gem_Q->Fill(float(cand.charge()));
      gem_Pdgid->Fill(float(cand.pdgId()));
 
      if ( std::abs(cand.eta()) <= 1.1 ){
	// Barrel
        gem_Pt_barrel->Fill(cand.pt());
        gem_Phi_barrel->Fill(cand.phi());
        gem_Eta_barrel->Fill(cand.eta());
        gem_Dxy_barrel->Fill(dxy);
        gem_Z_barrel->Fill(cand.vz());
        gem_Q_barrel->Fill(float(cand.charge()));
        gem_Pdgid_barrel->Fill(float(cand.pdgId()));
      } else {
        // Endcap
        gem_Pt_endcap->Fill(cand.pt());
        gem_Phi_endcap->Fill(cand.phi());
        gem_Eta_endcap->Fill(cand.eta());
        gem_Dxy_endcap->Fill(dxy);
        gem_Z_endcap->Fill(cand.vz());
        gem_Q_endcap->Fill(float(cand.charge()));
        gem_Pdgid_endcap->Fill(float(cand.pdgId()));
      }
    }
  }
}

void L1MUTK::book_(const EventSetup& c)
{
  // get hold of back-end interface
  DQMStore* dbe = 0;
  dbe = Service<DQMStore>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("L1T/L1MUTK");
    dbe->rmdir("L1T/L1MUTK");
  }

  if ( dbe ) 
  {
    dbe->setCurrentFolder("L1T/L1MUTK");

    /*
     *  All
     */
    gem_Pt    = dbe->book1D("gem_Pt","L1 Muon GEM Pt",   100,0.,70. );
    gem_Pt->setAxisTitle("gem_Pt",1);

    gem_Phi   = dbe->book1D("gem_Phi","L1 Muon GEM Phi", 100,-3.,3. );
    gem_Phi->setAxisTitle("gem_Phi",1);

    gem_Eta   = dbe->book1D("gem_Eta","L1 Muon GEM Eta", 100,-3.,3. );
    gem_Eta->setAxisTitle("gem_Eta",1);

    gem_Dxy   = dbe->book1D("gem_Dxy","L1 Muon GEM Dxy", 100,-15.,15. );
    gem_Dxy->setAxisTitle("gem_Dxy",1);

    gem_Z     = dbe->book1D("gem_Z","L1 Muon GEM Z", 100,-15.,15. );
    gem_Z->setAxisTitle("gem_Z",1);

    gem_Q     = dbe->book1D("gem_Q","L1 Muon GEM Q", 4,-2.,2. );
    gem_Q->setAxisTitle("gem_Q",1);

    gem_Pdgid = dbe->book1D("gem_Pdgid","L1 Muon GEM Pdgid", 100,-50.,50. );
    gem_Pdgid->setAxisTitle("gem_Pdgid",1);

    /*
     *  "barrel" (|eta|<=1.1)
     */
    gem_Pt_barrel    = dbe->book1D("gem_Pt_barrel","L1 Muon GEM Pt barrel",   100,0.,70. );
    gem_Pt_barrel->setAxisTitle("gem_Pt_barrel",1);

    gem_Phi_barrel   = dbe->book1D("gem_Phi_barrel","L1 Muon GEM Phi barrel", 100,-3.,3. );
    gem_Phi_barrel->setAxisTitle("gem_Phi_barrel",1);

    gem_Eta_barrel   = dbe->book1D("gem_Eta_barrel","L1 Muon GEM Eta barrel", 100,-3.,3. );
    gem_Eta_barrel->setAxisTitle("gem_Eta_barrel",1);

    gem_Dxy_barrel   = dbe->book1D("gem_Dxy_barrel","L1 Muon GEM Dxy barrel", 100,-15.,15. );
    gem_Dxy_barrel->setAxisTitle("gem_Dxy_barrel",1);

    gem_Z_barrel     = dbe->book1D("gem_Z_barrel","L1 Muon GEM Z barrel", 100,-15.,15. );
    gem_Z_barrel->setAxisTitle("gem_Z_barrel",1);

    gem_Q_barrel     = dbe->book1D("gem__barrelQ","L1 Muon GEM Q barrel", 4,-2.,2. );
    gem_Q_barrel->setAxisTitle("gem_Q_barrel",1);

    gem_Pdgid_barrel = dbe->book1D("gem_Pdgid_barrel","L1 Muon GEM Pdgid barrel", 100,-50.,50. );
    gem_Pdgid_barrel->setAxisTitle("gem_Pdgid_barrel",1);

    /*
     *  "endcap" (|eta|>1.1)
     */
    gem_Pt_endcap    = dbe->book1D("gem_Pt_endcap","L1 Muon GEM Pt endcap",   100,0.,70. );
    gem_Pt_endcap->setAxisTitle("gem_Pt_endcap",1);

    gem_Phi_endcap   = dbe->book1D("gem_Phi_endcap","L1 Muon GEM Phi endcap", 100,-3.,3. );
    gem_Phi_endcap->setAxisTitle("gem_Phi_endcap",1);

    gem_Eta_endcap   = dbe->book1D("gem_Eta_endcap","L1 Muon GEM Eta endcap", 100,-3.,3. );
    gem_Eta_endcap->setAxisTitle("gem_Eta_endcap",1);

    gem_Dxy_endcap   = dbe->book1D("gem_Dxy_endcap","L1 Muon GEM Dxy endcap", 100,-15.,15. );
    gem_Dxy_endcap->setAxisTitle("gem_Dxy_endcap",1);

    gem_Z_endcap     = dbe->book1D("gem_Z_endcap","L1 Muon GEM Z endcap", 100,-15.,15. );
    gem_Z_endcap->setAxisTitle("gem_Z_endcap",1);

    gem_Q_endcap     = dbe->book1D("gem_Q_endcap","L1 Muon GEM Q endcap", 4,-2.,2. );
    gem_Q_endcap->setAxisTitle("gem_Q_endcap",1);

    gem_Pdgid_endcap = dbe->book1D("gem_Pdgid_endcap","L1 Muon GEM Pdgid endcap", 100,-50.,50. );
    gem_Pdgid_endcap->setAxisTitle("gem_Pdgid_endcap",1);

  }  
}
