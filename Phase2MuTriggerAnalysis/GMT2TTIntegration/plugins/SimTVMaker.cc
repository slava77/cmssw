#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"

#include "etaPhiPropagators.h"

using namespace std;
using namespace edm;

class SimTVMaker : public edm::EDProducer {
public:
  typedef math::XYZTLorentzVectorF LorentzVector;
  typedef math::XYZTLorentzVectorD LorentzVectorD;
  typedef vector<SimTrack>      SimTracks;
  typedef vector<SimVertex>     SimVertices;
  explicit SimTVMaker (const edm::ParameterSet&);
  ~SimTVMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag simtInputTag_;
  edm::InputTag simvInputTag_;
  std::string aliasprefix_;
  bool saveOnlyMuons_;
  unique_ptr<etaPhiProp> propTFS2;
};


SimTVMaker::SimTVMaker(const edm::ParameterSet& iConfig) {
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix", "sims");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos)
    branchprefix.replace(branchprefix.find("_"),1,"");

  produces<vector<float> > (branchprefix+"pt" ).setBranchAlias(aliasprefix_+"_pt" );
  produces<vector<float> > (branchprefix+"eta" ).setBranchAlias(aliasprefix_+"_eta" );
  produces<vector<float> > (branchprefix+"phi" ).setBranchAlias(aliasprefix_+"_phi" );
  produces<vector<float> > (branchprefix+"tfs2eta" ).setBranchAlias(aliasprefix_+"_tfs2_eta" );
  produces<vector<float> > (branchprefix+"tfs2phi" ).setBranchAlias(aliasprefix_+"_tfs2_phi" );
  produces<vector<LorentzVector> > (branchprefix+"v4" ).setBranchAlias(aliasprefix_+"_v4" );
  produces<vector<int> > (branchprefix+"q" ).setBranchAlias(aliasprefix_+"_q" );
  produces<vector<int> > (branchprefix+"pdgid" ).setBranchAlias(aliasprefix_+"_pdgid" );
  produces<vector<int> > (branchprefix+"vtype" ).setBranchAlias(aliasprefix_+"_vtype" );
  produces<vector<int> > (branchprefix+"idst" ).setBranchAlias(aliasprefix_+"_idst" );
  produces<vector<int> > (branchprefix+"idgen" ).setBranchAlias(aliasprefix_+"_idgen" );
  produces<vector<int> > (branchprefix+"iv" ).setBranchAlias(aliasprefix_+"_iv" );

  // input tags
  simtInputTag_ = iConfig.getParameter<edm::InputTag>("simtInputTag");
  simvInputTag_ = iConfig.getParameter<edm::InputTag>("simvInputTag");
  saveOnlyMuons_ = iConfig.getParameter<bool>("saveOnlyMuons");

  propTFS2 = std::unique_ptr<etaPhiProp>( new etaPhiToStation2Run1TF());
}


void SimTVMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<float> > sims_pt                        (new vector<float>  );
  auto_ptr<vector<float> > sims_eta                        (new vector<float>  );
  auto_ptr<vector<float> > sims_phi                        (new vector<float>  );
  auto_ptr<vector<float> > sims_tfs2_eta                        (new vector<float>  );
  auto_ptr<vector<float> > sims_tfs2_phi                        (new vector<float>  );
  auto_ptr<vector<LorentzVector> > sims_v4                        (new vector<LorentzVector>  );
  auto_ptr<vector<int> >           sims_q  (new vector<int>);
  auto_ptr<vector<int> >           sims_pdgid  (new vector<int>);
  auto_ptr<vector<int> >           sims_vtype  (new vector<int>);
  auto_ptr<vector<int> >           sims_idst  (new vector<int>);
  auto_ptr<vector<int> >           sims_idgen  (new vector<int>);
  auto_ptr<vector<int> >           sims_iv  (new vector<int>);

  Handle<SimTracks> simtH;
  ev.getByLabel(simtInputTag_, simtH);
  const SimTracks& simts(*simtH.product());

  Handle<SimVertices> simvH;
  ev.getByLabel(simvInputTag_, simvH);
  const SimVertices& simvs(*simvH.product());

  LorentzVectorD dummy(1E6,1E6,1E6,4E6);
  for (auto simt : simts){
    if (std::abs(simt.type())!=13 &&  saveOnlyMuons_ ) continue;
    LorentzVectorD a4d(simt.momentum());
    sims_pt->push_back(a4d.pt());
    sims_eta->push_back(a4d.eta());
    sims_phi->push_back(a4d.phi());
    a4d = simt.vertIndex() < 0 ? dummy : simvs[simt.vertIndex()].position();
    sims_v4->push_back(LorentzVector(a4d.x(), a4d.y(), a4d.z(), a4d.t()));
    sims_q->push_back(simt.charge());

    auto pState = propTFS2->propagate(sims_pt->back(), sims_eta->back(), sims_phi->back(), sims_q->back(), sims_v4->back().z());
    sims_tfs2_eta->push_back(pState.eta);
    sims_tfs2_phi->push_back(pState.phi);
    sims_pdgid->push_back(simt.type());
    sims_vtype->push_back(-1);// FIXME FOR 7X simt.vertIndex() < 0 ? 0 : simvs[simt.vertIndex()].processType());
    sims_idst->push_back(simt.trackId());
    sims_idgen->push_back(simt.genpartIndex());
    sims_iv->push_back(simt.vertIndex());
  }

  ev.put(sims_pt, aliasprefix_+"pt");
  ev.put(sims_eta, aliasprefix_+"eta");
  ev.put(sims_phi, aliasprefix_+"phi");
  ev.put(sims_tfs2_eta, aliasprefix_+"tfs2eta");
  ev.put(sims_tfs2_phi, aliasprefix_+"tfs2phi");
  ev.put(sims_v4, aliasprefix_+"v4");
  ev.put(sims_q , aliasprefix_+"q");
  ev.put(sims_pdgid , aliasprefix_+"pdgid");
  ev.put(sims_vtype , aliasprefix_+"vtype");
  ev.put(sims_idst , aliasprefix_+"idst");
  ev.put(sims_idgen , aliasprefix_+"idgen");
  ev.put(sims_iv , aliasprefix_+"iv");
}

DEFINE_FWK_MODULE(SimTVMaker);

