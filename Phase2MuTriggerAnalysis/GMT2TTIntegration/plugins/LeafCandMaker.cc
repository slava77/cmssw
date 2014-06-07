#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/LeafCandidate.h"

using namespace std;
using namespace edm;
using namespace reco;

class LeafCandMaker : public edm::EDProducer {
public:
  typedef edm::View<LeafCandidate>      CandView;
  explicit LeafCandMaker (const edm::ParameterSet&);
  ~LeafCandMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag candInputTag_;
  std::string aliasprefix_;
};


LeafCandMaker::LeafCandMaker(const edm::ParameterSet& iConfig) {
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix", "cands");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos)
    branchprefix.replace(branchprefix.find("_"),1,"");

  produces<std::vector<float> > (branchprefix+"vx" ).setBranchAlias(aliasprefix_+"_vx" );
  produces<std::vector<float> > (branchprefix+"vy" ).setBranchAlias(aliasprefix_+"_vy" );
  produces<std::vector<float> > (branchprefix+"vz" ).setBranchAlias(aliasprefix_+"_vz" );
  produces<std::vector<float> > (branchprefix+"pt" ).setBranchAlias(aliasprefix_+"_pt" );
  produces<std::vector<float> > (branchprefix+"eta" ).setBranchAlias(aliasprefix_+"_eta" );
  produces<std::vector<float> > (branchprefix+"phi" ).setBranchAlias(aliasprefix_+"_phi" );
  produces<std::vector<int> > (branchprefix+"q" ).setBranchAlias(aliasprefix_+"_q" );
  produces<std::vector<int> > (branchprefix+"pdgid" ).setBranchAlias(aliasprefix_+"_pdgid" );

  // input tags
  candInputTag_ = iConfig.getParameter<edm::InputTag>("candInputTag");
}


void LeafCandMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<float> > cands_vx                 (new vector<float>  );
  auto_ptr<vector<float> > cands_vy                 (new vector<float>  );
  auto_ptr<vector<float> > cands_vz                 (new vector<float>  );
  auto_ptr<vector<float> > cands_pt                        (new vector<float>  );
  auto_ptr<vector<float> > cands_eta                        (new vector<float>  );
  auto_ptr<vector<float> > cands_phi                        (new vector<float>  );
  auto_ptr<vector<int> >   cands_q  (new vector<int>);
  auto_ptr<vector<int> >   cands_pdgid  (new vector<int>);


  Handle<CandView> aH;
  ev.getByLabel(candInputTag_, aH);
  const CandView& cands(*aH.product());

  for (auto cand : cands){
    cands_vx->push_back(cand.vx());
    cands_vy->push_back(cand.vy());
    cands_vz->push_back(cand.vz());
    cands_pt->push_back(cand.pt());
    cands_eta->push_back(cand.eta());
    cands_phi->push_back(cand.phi());
    cands_q->push_back(cand.charge());
    cands_pdgid->push_back(cand.pdgId());
  }

  ev.put(cands_vx, aliasprefix_+"vx");
  ev.put(cands_vy, aliasprefix_+"vy");
  ev.put(cands_vz, aliasprefix_+"vz");
  ev.put(cands_pt, aliasprefix_+"pt");
  ev.put(cands_eta, aliasprefix_+"eta");
  ev.put(cands_phi, aliasprefix_+"phi");
  ev.put(cands_q , aliasprefix_+"q");
  ev.put(cands_pdgid, aliasprefix_+"pdgid");

}

DEFINE_FWK_MODULE(LeafCandMaker);

