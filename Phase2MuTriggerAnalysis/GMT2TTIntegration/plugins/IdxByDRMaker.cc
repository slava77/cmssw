#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/deltaR.h"

using namespace std;
using namespace edm;

class IdxByDRMaker : public edm::EDProducer {
public:
  explicit IdxByDRMaker (const edm::ParameterSet&);
  ~IdxByDRMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag pt1InputTag;
  edm::InputTag pt2InputTag;
  edm::InputTag eta1InputTag;
  edm::InputTag eta2InputTag;
  edm::InputTag phi1InputTag;
  edm::InputTag phi2InputTag;
  std::string aliasprefix_;
  std::string branchprefix_;

  float drParConst_;
  float drParPt1In_;
  float drParPt2In_;

  float phiParConst_;
  float phiParPt1In_;
  float phiParPt2In_;

  float etaParConst_;
  float etaParPt1In_;
  float etaParPt2In_;

  bool useDr_;
};


IdxByDRMaker::IdxByDRMaker(const edm::ParameterSet& iConfig) : 
  drParConst_(0), drParPt1In_(0), drParPt2In_(0),
  phiParConst_(0), phiParPt1In_(0), phiParPt2In_(0),
  etaParConst_(0), etaParPt1In_(0), etaParPt2In_(0)
 {
  aliasprefix_ = iConfig.getParameter<std::string>("aliasPrefix");
  branchprefix_ = aliasprefix_;
  if(branchprefix_.find("_") != std::string::npos)
    branchprefix_.replace(branchprefix_.find("_"),1,"");
  if(branchprefix_.find("_") != std::string::npos)
    branchprefix_.replace(branchprefix_.find("_"),1,"");

  produces<std::vector<int> > (branchprefix_+"idx" ).setBranchAlias(aliasprefix_+"_idx" );
  produces<std::vector<float> > (branchprefix_+"dr" ).setBranchAlias(aliasprefix_+"_dr" );
  produces<std::vector<int> > (branchprefix_+"n" ).setBranchAlias(aliasprefix_+"_n" );

  // input tags
  pt1InputTag = iConfig.getParameter<edm::InputTag>("pt1InputTag");
  pt2InputTag = iConfig.getParameter<edm::InputTag>("pt2InputTag");
  eta1InputTag = iConfig.getParameter<edm::InputTag>("eta1InputTag");
  eta2InputTag = iConfig.getParameter<edm::InputTag>("eta2InputTag");
  phi1InputTag = iConfig.getParameter<edm::InputTag>("phi1InputTag");
  phi2InputTag = iConfig.getParameter<edm::InputTag>("phi2InputTag");

  useDr_ = iConfig.getParameter<bool>("useDr");
  if (useDr_){
    drParConst_ = iConfig.getParameter<double>("drParConst");
    drParPt1In_ = iConfig.getParameter<double>("drParPt1In");
    drParPt2In_ = iConfig.getParameter<double>("drParPt2In");
  } else {
    phiParConst_ = iConfig.getParameter<double>("phiParConst");
    phiParPt1In_ = iConfig.getParameter<double>("phiParPt1In");
    phiParPt2In_ = iConfig.getParameter<double>("phiParPt2In");
    
    etaParConst_ = iConfig.getParameter<double>("etaParConst");
    etaParPt1In_ = iConfig.getParameter<double>("etaParPt1In");
    etaParPt2In_ = iConfig.getParameter<double>("etaParPt2In");
  }
}



void IdxByDRMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<int> > matches_idx                        (new vector<int>  );
  auto_ptr<vector<float> > matches_dr                        (new vector<float>  );
  auto_ptr<vector<int> > matches_n                        (new vector<int>  );

  typedef vector<float> Fs;
  typedef Handle<Fs> HFs;
  HFs aH;
  ev.getByLabel(pt1InputTag, aH);   const Fs& pt1s(*aH.product());
  ev.getByLabel(pt2InputTag, aH);   const Fs& pt2s(*aH.product());
  ev.getByLabel(eta1InputTag, aH);  const Fs& eta1s(*aH.product());
  ev.getByLabel(eta2InputTag, aH);  const Fs& eta2s(*aH.product());
  ev.getByLabel(phi1InputTag, aH);  const Fs& phi1s(*aH.product());
  ev.getByLabel(phi2InputTag, aH);  const Fs& phi2s(*aH.product());

  int n1s = pt1s.size();
  int n2s = pt2s.size();
  for (int i1=0; i1< n1s;++i1){
    
    float pt1 = pt1s[i1];
    float eta1 = eta1s[i1];
    float phi1 = phi1s[i1];

    float minDr = 999;
    int nmatch = 0;
    int idx = -1;
    for (int i2=0; i2< n2s;++i2){
      float pt2 = pt2s[i2];
      float eta2 = eta2s[i2];
      float phi2 = phi2s[i2];
      
      float dEtaCut = useDr_ ? 0 : sqrt(etaParConst_*etaParConst_ + etaParPt1In_/pt1*etaParPt1In_/pt1 +etaParPt2In_/pt2*etaParPt2In_/pt2);
      float dPhiCut = useDr_ ? 0 : sqrt(phiParConst_*phiParConst_ + phiParPt1In_/pt1*phiParPt1In_/pt1 +phiParPt2In_/pt2*phiParPt2In_/pt2);
      float dR2Cut = useDr_ ? drParConst_*drParConst_ + drParPt1In_/pt1*drParPt1In_/pt1 +drParPt2In_/pt2*drParPt2In_/pt2 : 0;
      float dPhi = std::abs(phi1-phi2); constexpr float twopi = 2.*M_PI;
      while (dPhi>M_PI) dPhi -= twopi;
      dPhi = std::abs(dPhi);
      float dEta = std::abs(eta1-eta2);

      float dR2 = deltaR2(eta1, phi1, eta2, phi2);
      if (dR2 < minDr){
	minDr = dR2;

	if ( (useDr_ && dR2 < dR2Cut)
	     || ((!useDr_) && dPhi < dPhiCut && dEta < dEtaCut)){
	  idx = i2;
	  nmatch++;
	}
      }
    }
    matches_idx->push_back(idx);
    matches_dr->push_back(sqrt(minDr));//don't forget it's a dR2 above
    matches_n->push_back(nmatch);
  }

  ev.put(matches_idx, branchprefix_+"idx");
  ev.put(matches_dr, branchprefix_+"dr");
  ev.put(matches_n, branchprefix_+"n");

}

DEFINE_FWK_MODULE(IdxByDRMaker);

