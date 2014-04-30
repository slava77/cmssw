#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/SLHC/interface/L1TkTrack.h"
#include "SimDataFormats/SLHC/interface/StackedTrackerTypes.h"

class FlatTTMaker : public edm::EDProducer {
public:
  typedef math::XYZTLorentzVectorF LorentzVector;
  typedef L1TkTrack_PixelDigi_ L1Track;
  typedef L1TkTrack_PixelDigi_Collection L1TrackCollection;
  explicit FlatTTMaker (const edm::ParameterSet&);
  ~FlatTTMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag ttInputTag;
  std::string aliasprefix_;
};


FlatTTMaker::FlatTTMaker(const edm::ParameterSet& iConfig) {
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix", "tts");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos)
    branchprefix.replace(branchprefix.find("_"),1,"");

  produces<std::vector<LorentzVector> > (branchprefix+"p4" ).setBranchAlias(aliasprefix_+"_p4" );
  produces<std::vector<int> > (branchprefix+"q" ).setBranchAlias(aliasprefix_+"_q" );
  produces<std::vector<int> > (branchprefix+"nstubs" ).setBranchAlias(aliasprefix_+"_nstubs" );
  produces<std::vector<float> >(branchprefix+"chi2" ).setBranchAlias(aliasprefix_+"_chi2" );

  // input tags
  ttInputTag = iConfig.getParameter<edm::InputTag>("ttInputTag");
}

using namespace std;
using namespace edm;

void FlatTTMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<LorentzVector> > tts_p4                        (new vector<LorentzVector>  );
  auto_ptr<vector<int> >           tts_q  (new vector<int>);
  auto_ptr<vector<int> >           tts_nstubs  (new vector<int>);
  auto_ptr<vector<float> >         tts_chi2  (new vector<float>);

  Handle<L1TrackCollection> ttH;
  ev.getByLabel(ttInputTag, ttH);
  const L1TrackCollection& tts(*ttH.product());

  for (auto tt : tts){
    auto ttp3 = tt.getMomentum();
    
    tts_p4->push_back(LorentzVector(ttp3.x(), ttp3.y(), ttp3.z(), ttp3.mag()));
    tts_q->push_back(tt.getRInv()>0? 1: -1);
    tts_nstubs->push_back(tt.getStubPtrs().size());
    tts_chi2->push_back(tt.getChi2());
  }

  ev.put(tts_p4, aliasprefix_+"p4");
  ev.put(tts_q , aliasprefix_+"q");
  ev.put(tts_nstubs, aliasprefix_+"nstubs");
  ev.put(tts_chi2, aliasprefix_+"chi2");

}

DEFINE_FWK_MODULE(FlatTTMaker);

