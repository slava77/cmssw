/*
 * \file L1MUTK.cc
 *
 * $Date: 2015/05/29 12:00:00 $
 * $Revision: 0.01 $
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
  LogWarning("L1MUTK") << " ::analyze ";

  edm::Handle<CandView> aH; 
  e.getByLabel(candInputTag_, aH);
  const CandView& cands(*aH.product());

  for (auto cand : cands){
    cout << " L1MUTK: cand.pt " << cand.pt() << " cand.pdgId " << cand.pdgId() << endl;
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
    
    std::string hname("");
    std::string htitle("");
    
    regional_triggers = dbe->book1D("Regional_trigger","Muon trigger contribution", 27, 0., 27.);
    regional_triggers->setAxisTitle("regional trigger",1);
    int ib=1;
    regional_triggers->setBinLabel(ib++,"All muons",1);

  }  
}


