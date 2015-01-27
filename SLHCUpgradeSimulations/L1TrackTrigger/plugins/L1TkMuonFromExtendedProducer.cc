// -*- C++ -*-
//
// input: L1 TkTracks and  L1MuonParticleExtended (standalone with component details)
// match the two and produce a collection of L1TkMuonParticle
// eventually, this should be made modular and allow to swap out different algorithms

#include "DataFormats/L1TrackTrigger/interface/L1TkMuonParticle.h"
#include "DataFormats/L1TrackTrigger/interface/L1TkMuonParticleFwd.h"

// for L1Tracks:
#include "SimDataFormats/SLHC/interface/StackedTrackerTypes.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "TMath.h"

// system include files
#include <memory>
#include <string>

#include <FWCore/MessageLogger/interface/MessageLogger.h>
#include <iostream>
#include <vector>

using namespace l1extra;

//
// class declaration
//

class L1TkMuonFromExtendedProducer : public edm::EDProducer {
public:
  
  typedef TTTrack< Ref_PixelDigi_ >                     L1TkTrackType;
  typedef std::vector< L1TkTrackType >        L1TkTrackCollectionType;
  
  struct PropState { //something simple, imagine it's hardware emulation
    PropState() : 
      pt(-99),  eta(-99), phi(-99),
      sigmaPt(-99),  sigmaEta(-99), sigmaPhi(-99),
      valid(false) {}
    float pt;
    float eta;
    float phi;
    float sigmaPt;
    float sigmaEta;
    float sigmaPhi;
    bool valid;

  };

  explicit L1TkMuonFromExtendedProducer(const edm::ParameterSet&);
  ~L1TkMuonFromExtendedProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
private:

  virtual void produce(edm::Event&, const edm::EventSetup&);

  PropState propagateToGMT(const L1TkTrackType& l1tk) const;

  //configuration (preserving structure of L1TkMuonFromExtendedProducer
  edm::InputTag L1MuonsInputTag_;
  edm::InputTag L1TrackInputTag_;	 
  
  float ETAMIN_;
  float ETAMAX_;
  float ZMAX_;             // |z_track| < ZMAX in cm
  float CHI2MAX_;
  float PTMINTRA_;
  //  float DRmax_;
  int nStubsmin_ ;         // minimum number of stubs   
  //  bool closest_ ;
  bool correctGMTPropForTkZ_;

  bool use5ParameterFit_;
};

// maximum Muons number
const int MuonMax = 20;

struct L1MuonCand {
  float eta;
  float phi;
  float pt;
  float feta;
  float bx;
  float sigmaEta;
  float sigmaPhi;
  unsigned int quality;
  int  idx;
  bool gmtCand;
  bool dtCand;
  bool cscCand;
  bool rpcCand;
  int  itk;
  int  Num;
};  

// maximum Tracks number
const int TrackMax = 300;

struct L1TkMuonCand {
  float pt;
  int   nPars;
  int   q;
  float z;
  float chi2;
  float eta;
  float phi;
  int   nstubs;
  int   idx;
  float matchPt;
  float matchEta;
  float matchPhi;
  float matchSigmaEta;
  float matchSigmaPhi;
};  
  
//
// constructors  and destructor
//
L1TkMuonFromExtendedProducer::L1TkMuonFromExtendedProducer(const edm::ParameterSet& iConfig)
{
  L1MuonsInputTag_ = iConfig.getParameter<edm::InputTag>("L1MuonsInputTag");
  L1TrackInputTag_ = iConfig.getParameter<edm::InputTag>("L1TrackInputTag");

  ETAMIN_ = (float)iConfig.getParameter<double>("ETAMIN");
  ETAMAX_ = (float)iConfig.getParameter<double>("ETAMAX");
  ZMAX_ = (float)iConfig.getParameter<double>("ZMAX");
  CHI2MAX_ = (float)iConfig.getParameter<double>("CHI2MAX");
  PTMINTRA_ = (float)iConfig.getParameter<double>("PTMINTRA");
  //   DRmax_ = (float)iConfig.getParameter<double>("DRmax");
  nStubsmin_ = iConfig.getParameter<int>("nStubsmin");
  //   closest_ = iConfig.getParameter<bool>("closest");

  correctGMTPropForTkZ_ = iConfig.getParameter<bool>("correctGMTPropForTkZ");
  use5ParameterFit_     = iConfig.getParameter<bool>("use5ParameterFit");
  produces<L1TkMuonParticleCollection>();
}

L1TkMuonFromExtendedProducer::~L1TkMuonFromExtendedProducer() {
}

// ------------ method called to produce the data  ------------
void
L1TkMuonFromExtendedProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  L1MuonCand MuCn[MuonMax];
  L1TkMuonCand L1Tk[TrackMax];

  using namespace edm;
  using namespace std;
  
  std::auto_ptr<L1TkMuonParticleCollection> tkMuons(new L1TkMuonParticleCollection);
  
  edm::Handle<L1MuonParticleExtendedCollection> l1musH;
  iEvent.getByLabel(L1MuonsInputTag_, l1musH);
  const L1MuonParticleExtendedCollection& l1mus = *l1musH.product(); 
  
  edm::Handle<L1TkTrackCollectionType> l1tksH;
  iEvent.getByLabel(L1TrackInputTag_, l1tksH);
  const L1TkTrackCollectionType& l1tks = *l1tksH.product();

  LogDebug("L1TkMuonFromExtendedProducer") << " l1mus.size= " << l1mus.size() << " l1tks.size= " << l1tks.size();

  L1TkMuonParticleCollection l1tkmuCands;
  l1tkmuCands.reserve(l1mus.size()*4); //can do more if really needed

  // MuCn filling

  int imu = -1;
  int MuonNum = -1;
  int TrackNum = -1;
  for (auto l1mu : l1mus){
    imu++;

    if( imu<MuonMax ) {
      MuonNum = imu + 1;
      MuCn[imu].eta = l1mu.eta();
      MuCn[imu].eta = l1mu.eta();
      MuCn[imu].phi = l1mu.phi();
      MuCn[imu].pt = l1mu.pt();
      MuCn[imu].bx = l1mu.bx();
      MuCn[imu].quality = l1mu.quality();
      MuCn[imu].sigmaEta = l1mu.sigmaEta();
      MuCn[imu].sigmaPhi = l1mu.sigmaPhi();
      MuCn[imu].feta = fabs( l1mu.eta());
  
      const auto& gmtCand = l1mu.gmtMuonCand();
      const auto& dtCand  = l1mu.dtCand();
      const auto& cscCand = l1mu.cscCand();
      const auto& rpcCand = l1mu.rpcCand();
  
      MuCn[imu].gmtCand = gmtCand.empty();
      MuCn[imu].dtCand = dtCand.empty();
      MuCn[imu].cscCand = cscCand.empty();
      MuCn[imu].rpcCand = rpcCand.empty();
  
      LogDebug("L1TkMuonFromExtendedProducer") << " imu= " << imu
        << " eta= " << MuCn[imu].eta 
        << " phi= " << MuCn[imu].phi 
        << " bx= " << MuCn[imu].bx
        << " quality= " << MuCn[imu].quality
        << " sigmaEta= " << MuCn[imu].sigmaEta
        << " sigmaPhi= " << MuCn[imu].sigmaPhi 
        << " gmtCand " << MuCn[imu].gmtCand 
        << " dtCand " << MuCn[imu].dtCand 
        << " cscCand " << MuCn[imu].cscCand 
        << " rpcCand " << MuCn[imu].rpcCand;
    } 
    else { 
      LogWarning("L1TkMuonFromExtendedProducer") << " Maximum Muon number too small " << imu;
    };

    // trCn filling
  
    PropState matchProp;
    float drmin = 999;
    TrackNum = -1;
    int il1tk = -1;
    for (auto l1tk : l1tks ){
      il1tk++;
  
      if( il1tk<TrackMax ) {

        unsigned int nPars = 4;
        if (use5ParameterFit_) nPars = 5;
        L1Tk[il1tk].nPars = nPars;
        L1Tk[il1tk].pt = l1tk.getMomentum(nPars).perp();
        if (L1Tk[il1tk].pt < PTMINTRA_) continue;
  
        L1Tk[il1tk].z  = l1tk.getPOCA(nPars).z();
        if (fabs(L1Tk[il1tk].z) > ZMAX_) continue;
  
        L1Tk[il1tk].chi2 = l1tk.getChi2(nPars);
        if (L1Tk[il1tk].chi2 > CHI2MAX_) continue;
        
        L1Tk[il1tk].nstubs = l1tk.getStubRefs().size();
        if ( L1Tk[il1tk].nstubs < nStubsmin_) continue;
  
        L1Tk[il1tk].eta = l1tk.getMomentum(nPars).eta();
        L1Tk[il1tk].phi = l1tk.getMomentum(nPars).phi();
  
        float dr2 = deltaR2(MuCn[imu].eta, MuCn[imu].phi, L1Tk[il1tk].eta, L1Tk[il1tk].phi);
        if (dr2 > 0.3) continue;
  
        PropState pstate = propagateToGMT(l1tk);
        if (!pstate.valid) continue;
  
        float dr2prop = deltaR2(MuCn[imu].eta, MuCn[imu].phi, pstate.eta, pstate.phi);

        if (dr2prop < drmin){
          TrackNum = il1tk + 1;

          drmin = dr2prop;
          L1Tk[il1tk].idx = il1tk;
          MuCn[imu].itk = il1tk;

          L1Tk[il1tk].matchPt = pstate.pt; 
          L1Tk[il1tk].matchEta = pstate.eta;
          L1Tk[il1tk].matchPhi = pstate.phi;
          L1Tk[il1tk].matchSigmaEta = pstate.sigmaEta;
          L1Tk[il1tk].matchSigmaPhi = pstate.sigmaPhi;

        }
      } 
      else {
        LogWarning("L1TkMuonFromExtendedProducer") << " Maximum Tracks number too small " << TrackNum;
      }
    }//over l1tks

    MuCn[imu].Num = TrackNum;
    LogWarning("L1TkMuonFromExtendedProducer") << " imu " << imu << " itk " << MuCn[imu].itk << " TrackNum " << TrackNum;
  }//over l1mus

  LogWarning("L1TkMuonFromExtendedProducer") << " MuonNum " << MuonNum;

  // **** Calculation for Muons ****

  for (int imu=0; imu<MuonNum; imu++) {

    L1MuonParticleExtendedRef l1muRef(l1musH, imu);

    if (MuCn[imu].feta < ETAMIN_) continue;
    if (MuCn[imu].feta > ETAMAX_) continue;

    // can skip quality cuts at the moment, keep bx=0 req
    if (MuCn[imu].bx != 0) continue;

    if (!MuCn[imu].gmtCand){
      //some selections here
      //currently the input can be a merge from different track finders
      //so, the GMT cand may be missing
    }

    if (!MuCn[imu].dtCand){
      // something can be called from here
    }

    if (!MuCn[imu].cscCand){
      //apply something specific here
    }

    if (!MuCn[imu].rpcCand){
      //apply something specific here
    }

    LogWarning("L1TkMuonFromExtendedProducer") << "\n imu= " << imu << " itk " << MuCn[imu].itk << " TrackNum= " << MuCn[imu].Num;

    // **** Calculation for Tracks ****
    for ( int itk=0; itk<0 /*MuCn[imu].Num*/; itk++ ){

      edm::LogWarning("L1TkMuonFromExtendedProducer") << " itk " << itk << " idx " << L1Tk[itk].idx;

      if (MuCn[imu].itk == itk && L1Tk[itk].idx >= 0){
       
        float etaCut = 3.*sqrt(MuCn[imu].sigmaEta*MuCn[imu].sigmaEta + L1Tk[itk].matchSigmaEta*L1Tk[itk].matchSigmaEta);
        float phiCut = 4.*sqrt(MuCn[imu].sigmaPhi*MuCn[imu].sigmaPhi + L1Tk[itk].matchSigmaPhi*L1Tk[itk].matchSigmaPhi);
  
        float dEta = std::abs(L1Tk[itk].matchEta - MuCn[imu].eta);
        float dPhi = std::abs(deltaPhi(L1Tk[itk].matchPhi, MuCn[imu].phi));
  
        LogDebug("L1TkMuonFromExtendedProducer") 
          << " match details: prop Pt "<<L1Tk[itk].matchPt<<" Eta "<<L1Tk[itk].matchEta
          <<" Phi "<<L1Tk[itk].matchPhi<<" mutk "<<MuCn[imu].pt<<" "<<MuCn[imu].eta<<" "<<MuCn[imu].phi
          <<" dEta "<<dEta<<" dPhi "<<dPhi<<" cut " <<etaCut<<" "<<phiCut;
  
        // Filter for Cuts 
        if (dEta < etaCut && dPhi < phiCut){
  
          // ****
          Ptr< L1TkTrackType > l1tkPtr(l1tksH, L1Tk[itk].idx);
          const L1TkTrackType& matchTk = l1tks[L1Tk[itk].idx];
   
          auto p3 = matchTk.getMomentum(L1Tk[itk].nPars);
          float p4e = sqrt(0.105658369*0.105658369 + p3.mag2() );
   
          math::XYZTLorentzVector l1tkp4(p3.x(), p3.y(), p3.z(), p4e);
   
          auto tkv3=matchTk.getPOCA(L1Tk[itk].nPars);
          math::XYZPoint v3(tkv3.x(), tkv3.y(), tkv3.z());
   
          L1Tk[itk].q = matchTk.getRInv(L1Tk[itk].nPars)>0? 1: -1;
   
          L1TkMuonParticle l1tkmu(reco::LeafCandidate(L1Tk[itk].q, l1tkp4, v3, -13*L1Tk[itk].q ));
   
          l1tkmu.setTrkPtr(l1tkPtr);
          l1tkmu.setMuExtendedRef(l1muRef);
          l1tkmu.setQuality(MuCn[imu].quality);
          float trkisol = -999;
          l1tkmu.setTrkIsol(trkisol);
   
          // EP: add the zvtx information
          l1tkmu.setTrkzVtx( (float)tkv3.z() );
 
          tkMuons->push_back(l1tkmu);
 
	}// over Cuts
      }
   
    }// over itk
  }//over l1mus
  
  iEvent.put( tkMuons );
  
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
L1TkMuonFromExtendedProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}


L1TkMuonFromExtendedProducer::PropState L1TkMuonFromExtendedProducer::propagateToGMT(const L1TkMuonFromExtendedProducer::L1TkTrackType& tk) const {

  auto p3 = tk.getMomentum();
  float tk_pt = p3.perp();
  float tk_p = p3.mag();
  float tk_eta = p3.eta();
  float tk_aeta = std::abs(tk_eta);
  float tk_phi = p3.phi();
  float tk_q = tk.getRInv()>0? 1.: -1.;
  float tk_z  = tk.getPOCA().z();
  if (!correctGMTPropForTkZ_) tk_z = 0;

  L1TkMuonFromExtendedProducer::PropState dest;
  if (tk_p<3.5 ) return dest;
  if (tk_aeta <1.1 && tk_pt < 3.5) return dest;
  if (tk_aeta > 2.5) return dest;

  //0th order:
  dest.valid = true;

  float dzCorrPhi = 1.;
  float deta = 0;
  float etaProp = tk_aeta;

  if (tk_aeta < 1.1){
    etaProp = 1.1;
    deta = tk_z/550./cosh(tk_aeta);
  } else {
    float delta = tk_z/850.; //roughly scales as distance to 2nd station
    if (tk_eta > 0) delta *=-1;
    dzCorrPhi = 1. + delta;

    float zOzs = tk_z/850.;
    if (tk_eta > 0) deta = zOzs/(1. - zOzs);
    else deta = zOzs/(1.+zOzs);
    deta = deta*tanh(tk_eta);
  }
  float resPhi = tk_phi - 1.464*tk_q*cosh(1.7)/cosh(etaProp)/tk_pt*dzCorrPhi - M_PI/144.;
  if (resPhi > M_PI) resPhi -= 2.*M_PI;
  if (resPhi < -M_PI) resPhi += 2.*M_PI;

  dest.eta = tk_eta + deta;
  dest.phi = resPhi;
  dest.pt = tk_pt; //not corrected for eloss

  dest.sigmaEta = 0.100/tk_pt; //multiple scattering term
  dest.sigmaPhi = 0.106/tk_pt; //need a better estimate for these
  return dest;
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1TkMuonFromExtendedProducer);
