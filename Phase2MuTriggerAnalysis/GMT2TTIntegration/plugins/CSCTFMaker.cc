#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1CSCTrackFinder/interface/L1CSCTrackCollection.h"

using namespace std;
using namespace edm;

class CSCTFMaker : public edm::EDProducer {
public:
  explicit CSCTFMaker (const edm::ParameterSet&);
  ~CSCTFMaker() {};

private:
  virtual void produce(edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  edm::InputTag gmtROInputTag_;
  edm::InputTag csctfInputTag_;
  std::string aliasprefix_;
};


CSCTFMaker::CSCTFMaker(const edm::ParameterSet& iConfig) {
  aliasprefix_ = iConfig.getUntrackedParameter<std::string>("aliasPrefix", "csctfs");
  std::string branchprefix = aliasprefix_;
  if(branchprefix.find("_") != std::string::npos)
    branchprefix.replace(branchprefix.find("_"),1,"");

  produces<std::vector<float> > (branchprefix+"pt" ).setBranchAlias(aliasprefix_+"_pt" );
  produces<std::vector<float> > (branchprefix+"eta" ).setBranchAlias(aliasprefix_+"_eta" );
  produces<std::vector<float> > (branchprefix+"phi" ).setBranchAlias(aliasprefix_+"_phi" );
  produces<std::vector<int> > (branchprefix+"q" ).setBranchAlias(aliasprefix_+"_q" );
  produces<std::vector<int> > (branchprefix+"quality" ).setBranchAlias(aliasprefix_+"_quality" );
  produces<std::vector<int> > (branchprefix+"bx" ).setBranchAlias(aliasprefix_+"_bx" );

  produces<std::vector<int> >(branchprefix+"nlcts" ).setBranchAlias(aliasprefix_+"_nlcts" );

  produces<std::vector<int> >(branchprefix+"ringS1" ).setBranchAlias(aliasprefix_+"_ringS1" );
  produces<std::vector<int> >(branchprefix+"chamberS1" ).setBranchAlias(aliasprefix_+"_chamberS1" );
  produces<std::vector<int> >(branchprefix+"wgS1" ).setBranchAlias(aliasprefix_+"_wgS1" );
  produces<std::vector<int> >(branchprefix+"hsS1" ).setBranchAlias(aliasprefix_+"_hsS1" );
  produces<std::vector<int> >(branchprefix+"clctPatternS1" ).setBranchAlias(aliasprefix_+"_clctPatternS1" );
  produces<std::vector<int> >(branchprefix+"clctBendS1" ).setBranchAlias(aliasprefix_+"_clctBendS1" );
  produces<std::vector<float> >(branchprefix+"gemDPhiS1" ).setBranchAlias(aliasprefix_+"_gemDPhiS1" );
  produces<std::vector<int> >(branchprefix+"bxS1" ).setBranchAlias(aliasprefix_+"_bxS1" );

  produces<std::vector<int> >(branchprefix+"ringS2" ).setBranchAlias(aliasprefix_+"_ringS2" );
  produces<std::vector<int> >(branchprefix+"chamberS2" ).setBranchAlias(aliasprefix_+"_chamberS2" );
  produces<std::vector<int> >(branchprefix+"wgS2" ).setBranchAlias(aliasprefix_+"_wgS2" );
  produces<std::vector<int> >(branchprefix+"hsS2" ).setBranchAlias(aliasprefix_+"_hsS2" );
  produces<std::vector<int> >(branchprefix+"clctPatternS2" ).setBranchAlias(aliasprefix_+"_clctPatternS2" );
  produces<std::vector<int> >(branchprefix+"clctBendS2" ).setBranchAlias(aliasprefix_+"_clctBendS2" );
  produces<std::vector<float> >(branchprefix+"gemDPhiS2" ).setBranchAlias(aliasprefix_+"_gemDPhiS2" );
  produces<std::vector<int> >(branchprefix+"bxS2" ).setBranchAlias(aliasprefix_+"_bxS2" );

  produces<std::vector<int> >(branchprefix+"ringS3" ).setBranchAlias(aliasprefix_+"_ringS3" );
  produces<std::vector<int> >(branchprefix+"chamberS3" ).setBranchAlias(aliasprefix_+"_chamberS3" );
  produces<std::vector<int> >(branchprefix+"wgS3" ).setBranchAlias(aliasprefix_+"_wgS3" );
  produces<std::vector<int> >(branchprefix+"hsS3" ).setBranchAlias(aliasprefix_+"_hsS3" );
  produces<std::vector<int> >(branchprefix+"clctPatternS3" ).setBranchAlias(aliasprefix_+"_clctPatternS3" );
  produces<std::vector<int> >(branchprefix+"clctBendS3" ).setBranchAlias(aliasprefix_+"_clctBendS3" );
  produces<std::vector<float> >(branchprefix+"gemDPhiS3" ).setBranchAlias(aliasprefix_+"_gemDPhiS3" );
  produces<std::vector<int> >(branchprefix+"bxS3" ).setBranchAlias(aliasprefix_+"_bxS3" );

  produces<std::vector<int> >(branchprefix+"ringS4" ).setBranchAlias(aliasprefix_+"_ringS4" );
  produces<std::vector<int> >(branchprefix+"chamberS4" ).setBranchAlias(aliasprefix_+"_chamberS4" );
  produces<std::vector<int> >(branchprefix+"wgS4" ).setBranchAlias(aliasprefix_+"_wgS4" );
  produces<std::vector<int> >(branchprefix+"hsS4" ).setBranchAlias(aliasprefix_+"_hsS4" );
  produces<std::vector<int> >(branchprefix+"clctPatternS4" ).setBranchAlias(aliasprefix_+"_clctPatternS4" );
  produces<std::vector<int> >(branchprefix+"clctBendS4" ).setBranchAlias(aliasprefix_+"_clctBendS4" );
  produces<std::vector<float> >(branchprefix+"gemDPhiS4" ).setBranchAlias(aliasprefix_+"_gemDPhiS4" );
  produces<std::vector<int> >(branchprefix+"bxS4" ).setBranchAlias(aliasprefix_+"_bxS4" );

  // input tags
  gmtROInputTag_ = iConfig.getParameter<edm::InputTag>("gmtROInputTag");
  csctfInputTag_ = iConfig.getParameter<edm::InputTag>("csctfInputTag");
}


void CSCTFMaker::produce(edm::Event& ev, const edm::EventSetup& es){
  auto_ptr<vector<float> > csctfs_pt                        (new vector<float>  );
  auto_ptr<vector<float> > csctfs_eta                        (new vector<float>  );
  auto_ptr<vector<float> > csctfs_phi                        (new vector<float>  );
  auto_ptr<vector<int> >           csctfs_q  (new vector<int>);
  auto_ptr<vector<int> >           csctfs_quality  (new vector<int>);
  auto_ptr<vector<int> >           csctfs_bx  (new vector<int>);

  auto_ptr<vector<int> >           csctfs_nlcts  (new vector<int>);

  auto_ptr<vector<int> >csctfs_ringS1 (new vector<int> );
  auto_ptr<vector<int> >csctfs_chamberS1 (new vector<int> );
  auto_ptr<vector<int> >csctfs_wgS1 (new vector<int> );
  auto_ptr<vector<int> >csctfs_hsS1 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctPatternS1 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctBendS1 (new vector<int> );
  auto_ptr<vector<float> >csctfs_gemDPhiS1 (new vector<float> );
  auto_ptr<vector<int> >csctfs_bxS1 (new vector<int> );

  auto_ptr<vector<int> >csctfs_ringS2 (new vector<int> );
  auto_ptr<vector<int> >csctfs_chamberS2 (new vector<int> );
  auto_ptr<vector<int> >csctfs_wgS2 (new vector<int> );
  auto_ptr<vector<int> >csctfs_hsS2 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctPatternS2 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctBendS2 (new vector<int> );
  auto_ptr<vector<float> >csctfs_gemDPhiS2 (new vector<float> );
  auto_ptr<vector<int> >csctfs_bxS2 (new vector<int> );

  auto_ptr<vector<int> >csctfs_ringS3 (new vector<int> );
  auto_ptr<vector<int> >csctfs_chamberS3 (new vector<int> );
  auto_ptr<vector<int> >csctfs_wgS3 (new vector<int> );
  auto_ptr<vector<int> >csctfs_hsS3 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctPatternS3 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctBendS3 (new vector<int> );
  auto_ptr<vector<float> >csctfs_gemDPhiS3 (new vector<float> );
  auto_ptr<vector<int> >csctfs_bxS3 (new vector<int> );

  auto_ptr<vector<int> >csctfs_ringS4 (new vector<int> );
  auto_ptr<vector<int> >csctfs_chamberS4 (new vector<int> );
  auto_ptr<vector<int> >csctfs_wgS4 (new vector<int> );
  auto_ptr<vector<int> >csctfs_hsS4 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctPatternS4 (new vector<int> );
  auto_ptr<vector<int> >csctfs_clctBendS4 (new vector<int> );
  auto_ptr<vector<float> >csctfs_gemDPhiS4 (new vector<float> );
  auto_ptr<vector<int> >csctfs_bxS4 (new vector<int> );


  Handle<L1MuGMTReadoutCollection> aH;
  ev.getByLabel(gmtROInputTag_, aH);
  const L1MuGMTReadoutCollection& gmtROs(*aH.product());

  Handle<L1CSCTrackCollection> tftH;
  ev.getByLabel(csctfInputTag_, tftH);
  const L1CSCTrackCollection& tfts(*tftH.product());

  for (auto gmtRO : gmtROs.getRecords()){
    for (auto const csctfReg : gmtRO.getCSCCands() ){
      if (csctfReg.empty()) continue;
      csctfs_pt->push_back(csctfReg.ptValue());
      csctfs_eta->push_back(csctfReg.etaValue());
      csctfs_phi->push_back(csctfReg.phiValue());
      csctfs_q->push_back(csctfReg.chargeValid() ? csctfReg.chargeValue() : 0);
      csctfs_quality->push_back(csctfReg.quality());
      csctfs_bx->push_back(csctfReg.bx());      

      csctfs_nlcts->push_back(0);
      int& nlcts = csctfs_nlcts->back();

      csctfs_ringS1->push_back(0);
      int& ringS1 = csctfs_ringS1->back();
      csctfs_chamberS1->push_back(0);
      int& chamberS1 = csctfs_chamberS1->back();
      csctfs_wgS1->push_back(0);
      int& wgS1 = csctfs_wgS1->back();
      csctfs_hsS1->push_back(0);
      int& hsS1 = csctfs_hsS1->back();
      csctfs_clctPatternS1->push_back(0);
      int& clctPatternS1 = csctfs_clctPatternS1->back();
      csctfs_clctBendS1->push_back(0);
      int& clctBendS1 = csctfs_clctBendS1->back();
      csctfs_gemDPhiS1->push_back(0);
      float& gemDPhiS1 = csctfs_gemDPhiS1->back();
      csctfs_bxS1->push_back(0);
      int& bxS1 = csctfs_bxS1->back();

      csctfs_ringS2->push_back(0);
      int& ringS2 = csctfs_ringS2->back();
      csctfs_chamberS2->push_back(0);
      int& chamberS2 = csctfs_chamberS2->back();
      csctfs_wgS2->push_back(0);
      int& wgS2 = csctfs_wgS2->back();
      csctfs_hsS2->push_back(0);
      int& hsS2 = csctfs_hsS2->back();
      csctfs_clctPatternS2->push_back(0);
      int& clctPatternS2 = csctfs_clctPatternS2->back();
      csctfs_clctBendS2->push_back(0);
      int& clctBendS2 = csctfs_clctBendS2->back();
      csctfs_gemDPhiS2->push_back(0);
      float& gemDPhiS2 = csctfs_gemDPhiS2->back();
      csctfs_bxS2->push_back(0);
      int& bxS2 = csctfs_bxS2->back();

      csctfs_ringS3->push_back(0);
      int& ringS3 = csctfs_ringS3->back();
      csctfs_chamberS3->push_back(0);
      int& chamberS3 = csctfs_chamberS3->back();
      csctfs_wgS3->push_back(0);
      int& wgS3 = csctfs_wgS3->back();
      csctfs_hsS3->push_back(0);
      int& hsS3 = csctfs_hsS3->back();
      csctfs_clctPatternS3->push_back(0);
      int& clctPatternS3 = csctfs_clctPatternS3->back();
      csctfs_clctBendS3->push_back(0);
      int& clctBendS3 = csctfs_clctBendS3->back();
      csctfs_gemDPhiS3->push_back(0);
      float& gemDPhiS3 = csctfs_gemDPhiS3->back();
      csctfs_bxS3->push_back(0);
      int& bxS3 = csctfs_bxS3->back();

      csctfs_ringS4->push_back(0);
      int& ringS4 = csctfs_ringS4->back();
      csctfs_chamberS4->push_back(0);
      int& chamberS4 = csctfs_chamberS4->back();
      csctfs_wgS4->push_back(0);
      int& wgS4 = csctfs_wgS4->back();
      csctfs_hsS4->push_back(0);
      int& hsS4 = csctfs_hsS4->back();
      csctfs_clctPatternS4->push_back(0);
      int& clctPatternS4 = csctfs_clctPatternS4->back();
      csctfs_clctBendS4->push_back(0);
      int& clctBendS4 = csctfs_clctBendS4->back();
      csctfs_gemDPhiS4->push_back(0);
      float& gemDPhiS4 = csctfs_gemDPhiS4->back();
      csctfs_bxS4->push_back(0);
      int& bxS4 = csctfs_bxS4->back();

      //      edm::LogWarning("MYDEBUG")<<"Searching parent  for pt "<<csctfs_pt->back()<<" eta "<<csctfs_eta->back()<<" phi "<<csctfs_phi->back();
      unsigned int roWord = csctfReg.getDataWord();
      for (auto csctfFull : tfts){
	auto cscL1Tk = csctfFull.first;
	unsigned int fWord = cscL1Tk.getDataWord();
	if (cscL1Tk.endcap() == 2) fWord |= (1<< (L1MuRegionalCand::ETA_START+L1MuRegionalCand::ETA_LENGTH-1));
	unsigned int fQuality = 0;
	unsigned int fPt = 0;
	cscL1Tk.decodeRank(cscL1Tk.rank(), fPt, fQuality);
	fWord |= (fQuality<< L1MuRegionalCand::QUAL_START);
	fWord |= (fPt << L1MuRegionalCand::PT_START);
	//from L1Trigger/CSCTrackFinder/src/CSCTFMuonSorter.cc
	unsigned int fgPhi = cscL1Tk.localPhi() + (cscL1Tk.sector() -1)*24 + 6;
	if(fgPhi > 143) fgPhi -= 143; 
	fWord |= (fgPhi <<  L1MuRegionalCand::PHI_START);
	//	edm::LogWarning("MYDEBUG")<<"Checking or vs other "<<roWord<<" "<<fWord;
	if (fWord == roWord){
	  //	  edm::LogWarning("MYDEBUG")<<"Found csctfFull";
	  nlcts = 0;
	  for (auto anLCT : csctfFull.second){
	    auto anID =  anLCT.first;
	    auto aDigi = anLCT.second.first;
	    int aStation = anID.station();
	    int aRing = anID.ring();
	    int aChamber = anID.chamber();
	    int aWG = 0;
	    int aHS = 0;
	    int aCLCTPattern = -9;
	    int aBend = -9;
	    float aGemDPhi = -9.;
	    int aBX = -9;
	    bool hadLCT = false;
	    int countLCTs = 0;
	    for (; aDigi != anLCT.second.second; ++aDigi){
	      //do we really get many here?
	      if (countLCTs>0) edm::LogWarning("ManyLCTs")<<"got a new LCT in st "<<aStation<<" r "<<aRing <<"  overriting previous: WG "<<aWG<< " HS "<<aHS;
	      aWG = aDigi->getKeyWG();
	      aHS = aDigi->getStrip();
	      aCLCTPattern = aDigi->getCLCTPattern();
	      aBend = aDigi->getBend();
	      aGemDPhi = aDigi->getGEMDPhi();
	      aBX = aDigi->getBX();
	      hadLCT = true;
	      countLCTs++;
	    }
	    if (hadLCT) nlcts++;
	    if (aStation ==1){
	      ringS1 = aRing;
	      chamberS1 = aChamber;
	      wgS1 = aWG;
	      hsS1 = aHS;
	      clctPatternS1 = aCLCTPattern;
	      clctBendS1 = aBend;
	      gemDPhiS1 = aGemDPhi;
	      bxS1 = aBX;
	    } else if (aStation == 2){
	      ringS2 = aRing;
	      chamberS2 = aChamber;
	      wgS2 = aWG;
	      hsS2 = aHS;
	      clctPatternS2 = aCLCTPattern;
	      clctBendS2 = aBend;
	      gemDPhiS2 = aGemDPhi;
	      bxS2 = aBX;
	    } else if (aStation == 3){
	      ringS3 = aRing;
	      chamberS3 = aChamber;
	      wgS3 = aWG;
	      hsS3 = aHS;
	      clctPatternS3 = aCLCTPattern;
	      clctBendS3 = aBend;
	      gemDPhiS3 = aGemDPhi;
	      bxS3 = aBX;
	    } else if (aStation == 4){
	      ringS4 = aRing;
	      chamberS4 = aChamber;
	      wgS4 = aWG;
	      hsS4 = aHS;
	      clctPatternS4 = aCLCTPattern;
	      clctBendS4 = aBend;
	      gemDPhiS4 = aGemDPhi;
	      bxS4 = aBX;
	    } else {
	      edm::LogError("BADSTATION")<<aStation;
	    }
	  }
	}
      }
    }
  }

  ev.put(csctfs_pt, aliasprefix_+"pt");
  ev.put(csctfs_eta, aliasprefix_+"eta");
  ev.put(csctfs_phi, aliasprefix_+"phi");
  ev.put(csctfs_q , aliasprefix_+"q");
  ev.put(csctfs_quality , aliasprefix_+"quality");
  ev.put(csctfs_bx , aliasprefix_+"bx");

  ev.put(csctfs_nlcts , aliasprefix_+"nlcts" );

  ev.put(csctfs_ringS1 , aliasprefix_+"ringS1" );
  ev.put(csctfs_chamberS1 , aliasprefix_+"chamberS1" );
  ev.put(csctfs_wgS1 , aliasprefix_+"wgS1" );
  ev.put(csctfs_hsS1 , aliasprefix_+"hsS1" );
  ev.put(csctfs_clctPatternS1 , aliasprefix_+"clctPatternS1" );
  ev.put(csctfs_clctBendS1 , aliasprefix_+"clctBendS1" );
  ev.put(csctfs_gemDPhiS1 , aliasprefix_+"gemDPhiS1" );
  ev.put(csctfs_bxS1 , aliasprefix_+"bxS1" );

  ev.put(csctfs_ringS2 , aliasprefix_+"ringS2" );
  ev.put(csctfs_chamberS2 , aliasprefix_+"chamberS2" );
  ev.put(csctfs_wgS2 , aliasprefix_+"wgS2" );
  ev.put(csctfs_hsS2 , aliasprefix_+"hsS2" );
  ev.put(csctfs_clctPatternS2 , aliasprefix_+"clctPatternS2" );
  ev.put(csctfs_clctBendS2 , aliasprefix_+"clctBendS2" );
  ev.put(csctfs_gemDPhiS2 , aliasprefix_+"gemDPhiS2" );
  ev.put(csctfs_bxS2 , aliasprefix_+"bxS2" );

  ev.put(csctfs_ringS3 , aliasprefix_+"ringS3" );
  ev.put(csctfs_chamberS3 , aliasprefix_+"chamberS3" );
  ev.put(csctfs_wgS3 , aliasprefix_+"wgS3" );
  ev.put(csctfs_hsS3 , aliasprefix_+"hsS3" );
  ev.put(csctfs_clctPatternS3 , aliasprefix_+"clctPatternS3" );
  ev.put(csctfs_clctBendS3 , aliasprefix_+"clctBendS3" );
  ev.put(csctfs_gemDPhiS3 , aliasprefix_+"gemDPhiS3" );
  ev.put(csctfs_bxS3 , aliasprefix_+"bxS3" );

  ev.put(csctfs_ringS4 , aliasprefix_+"ringS4" );
  ev.put(csctfs_chamberS4 , aliasprefix_+"chamberS4" );
  ev.put(csctfs_wgS4 , aliasprefix_+"wgS4" );
  ev.put(csctfs_hsS4 , aliasprefix_+"hsS4" );
  ev.put(csctfs_clctPatternS4 , aliasprefix_+"clctPatternS4" );
  ev.put(csctfs_clctBendS4 , aliasprefix_+"clctBendS4" );
  ev.put(csctfs_gemDPhiS4 , aliasprefix_+"gemDPhiS4" );
  ev.put(csctfs_bxS4 , aliasprefix_+"bxS4" );

}

DEFINE_FWK_MODULE(CSCTFMaker);

