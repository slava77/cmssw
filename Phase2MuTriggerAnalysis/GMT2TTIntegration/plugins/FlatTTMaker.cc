#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/SLHC/interface/StackedTrackerTypes.h"
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"

#include "etaPhiPropagators.h"

using namespace std;
using namespace edm;

class FlatTTMaker : public edm::EDProducer {
public:
  typedef math::XYZTLorentzVectorF LorentzVector;
  typedef TTTrack< Ref_PixelDigi_ >                     L1Track;
  typedef std::vector< L1Track >        L1TrackCollection;
  explicit FlatTTMaker (const edm::ParameterSet&);
  ~FlatTTMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag ttInputTag;
  std::string aliasprefix_;
  unique_ptr<etaPhiProp> propTFS2;
};


FlatTTMaker::FlatTTMaker(const edm::ParameterSet& iConfig) {
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix", "tts");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos)
    branchprefix.replace(branchprefix.find("_"),1,"");

  produces<std::vector<float> > (branchprefix+"pt" ).setBranchAlias(aliasprefix_+"_pt" );
  produces<std::vector<float> > (branchprefix+"eta" ).setBranchAlias(aliasprefix_+"_eta" );
  produces<std::vector<float> > (branchprefix+"phi" ).setBranchAlias(aliasprefix_+"_phi" );
  produces<std::vector<float> > (branchprefix+"tfs2eta" ).setBranchAlias(aliasprefix_+"_tfs2_eta" );
  produces<std::vector<float> > (branchprefix+"tfs2phi" ).setBranchAlias(aliasprefix_+"_tfs2_phi" );
  produces<std::vector<int> > (branchprefix+"q" ).setBranchAlias(aliasprefix_+"_q" );
  produces<std::vector<int> > (branchprefix+"nstubs" ).setBranchAlias(aliasprefix_+"_nstubs" );
  produces<std::vector<float> >(branchprefix+"chi2" ).setBranchAlias(aliasprefix_+"_chi2" );
  produces<std::vector<float> >(branchprefix+"vz" ).setBranchAlias(aliasprefix_+"_vz" );

  // input tags
  ttInputTag = iConfig.getParameter<edm::InputTag>("ttInputTag");
  propTFS2 = std::unique_ptr<etaPhiProp>( new etaPhiToStation2Run1TF());
}


void FlatTTMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<float> > tts_pt                        (new vector<float>  );
  auto_ptr<vector<float> > tts_eta                        (new vector<float>  );
  auto_ptr<vector<float> > tts_phi                        (new vector<float>  );
  auto_ptr<vector<float> > tts_tfs2_eta                        (new vector<float>  );
  auto_ptr<vector<float> > tts_tfs2_phi                        (new vector<float>  );
  auto_ptr<vector<int> >           tts_q  (new vector<int>);
  auto_ptr<vector<int> >           tts_nstubs  (new vector<int>);
  auto_ptr<vector<float> >         tts_chi2  (new vector<float>);
  auto_ptr<vector<float> >         tts_vz  (new vector<float>);

  Handle<L1TrackCollection> ttH;
  ev.getByLabel(ttInputTag, ttH);
  const L1TrackCollection& tts(*ttH.product());

  for (auto tt : tts){
    auto ttp3 = tt.getMomentum();
    LorentzVector tmp4(LorentzVector(ttp3.x(), ttp3.y(), ttp3.z(), ttp3.mag()));
    
    tts_pt->push_back(tmp4.pt());
    tts_eta->push_back(tmp4.eta());
    tts_phi->push_back(tmp4.phi());
    tts_q->push_back(tt.getRInv()>0? 1: -1);
    tts_nstubs->push_back(tt.getStubRefs().size());
    tts_chi2->push_back(tt.getChi2());
    tts_vz->push_back(tt.getPOCA().z());

    auto pState = propTFS2->propagate(tts_pt->back(), tts_eta->back(), tts_phi->back(), tts_q->back(), tts_vz->back());
    tts_tfs2_eta->push_back(pState.eta);
    tts_tfs2_phi->push_back(pState.phi);
  }

  ev.put(tts_pt, aliasprefix_+"pt");
  ev.put(tts_eta, aliasprefix_+"eta");
  ev.put(tts_phi, aliasprefix_+"phi");
  ev.put(tts_tfs2_eta, aliasprefix_+"tfs2eta");
  ev.put(tts_tfs2_phi, aliasprefix_+"tfs2phi");
  ev.put(tts_q , aliasprefix_+"q");
  ev.put(tts_nstubs, aliasprefix_+"nstubs");
  ev.put(tts_chi2, aliasprefix_+"chi2");
  ev.put(tts_vz, aliasprefix_+"vz");

}

DEFINE_FWK_MODULE(FlatTTMaker);

