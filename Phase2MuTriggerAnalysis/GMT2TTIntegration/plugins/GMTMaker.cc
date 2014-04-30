#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"

using namespace std;
using namespace edm;

class GMTMaker : public edm::EDProducer {
public:
  typedef vector<L1MuGMTCand>      GMTs;
  explicit GMTMaker (const edm::ParameterSet&);
  ~GMTMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag gmtInputTag_;
  std::string aliasprefix_;
};


GMTMaker::GMTMaker(const edm::ParameterSet& iConfig) {
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix", "gmts");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos)
    branchprefix.replace(branchprefix.find("_"),1,"");

  produces<std::vector<float> > (branchprefix+"pt" ).setBranchAlias(aliasprefix_+"_pt" );
  produces<std::vector<float> > (branchprefix+"eta" ).setBranchAlias(aliasprefix_+"_eta" );
  produces<std::vector<float> > (branchprefix+"phi" ).setBranchAlias(aliasprefix_+"_phi" );
  produces<std::vector<int> > (branchprefix+"q" ).setBranchAlias(aliasprefix_+"_q" );
  produces<std::vector<int> > (branchprefix+"quality" ).setBranchAlias(aliasprefix_+"_quality" );
  produces<std::vector<int> > (branchprefix+"bx" ).setBranchAlias(aliasprefix_+"_bx" );

  // input tags
  gmtInputTag_ = iConfig.getParameter<edm::InputTag>("gmtInputTag");
}


void GMTMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<float> > gmts_pt                        (new vector<float>  );
  auto_ptr<vector<float> > gmts_eta                        (new vector<float>  );
  auto_ptr<vector<float> > gmts_phi                        (new vector<float>  );
  auto_ptr<vector<int> >           gmts_q  (new vector<int>);
  auto_ptr<vector<int> >           gmts_quality  (new vector<int>);
  auto_ptr<vector<int> >           gmts_bx  (new vector<int>);


  Handle<GMTs> aH;
  ev.getByLabel(gmtInputTag_, aH);
  const GMTs& gmts(*aH.product());

  for (auto gmt : gmts){
    gmts_pt->push_back(gmt.ptValue());
    gmts_eta->push_back(gmt.etaValue());
    gmts_phi->push_back(gmt.phiValue());
    gmts_q->push_back(gmt.charge_valid() ? gmt.charge() : 0);
    gmts_quality->push_back(gmt.quality());
    gmts_bx->push_back(gmt.bx());
  }

  ev.put(gmts_pt, aliasprefix_+"pt");
  ev.put(gmts_eta, aliasprefix_+"eta");
  ev.put(gmts_phi, aliasprefix_+"phi");
  ev.put(gmts_q , aliasprefix_+"q");
  ev.put(gmts_quality , aliasprefix_+"quality");
  ev.put(gmts_bx , aliasprefix_+"bx");

}

DEFINE_FWK_MODULE(GMTMaker);

