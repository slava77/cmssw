
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

float minV=1000.;
float maxV=-1000.;

// maximum Muons number
const int MuonMax = 20;

// maximum Tracks number
const int TrackMax = 400;

// maximum L1 Tracks Candidates number
const int TkMuonMax = 20;

class L1TkMuonFromExtendedProducer : public edm::EDProducer {
public:
  
  typedef TTTrack< Ref_PixelDigi_ >                     L1TkTrackType;
  typedef std::vector< L1TkTrackType >        L1TkTrackCollectionType;
  
  struct PropState { //something simple, imagine it's hardware emulation
    PropState() : 
       pt(-99), eta(-99), phi(-99), sigmaPt(-99), sigmaEta(-99), sigmaPhi(-99), valid(false) {}
    float pt;
    float eta;
    float phi;
    float sigmaPt;
    float sigmaEta;
    float sigmaPhi;
    bool valid;
  };

  struct L1Muon {
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

  struct L1Track {
    float pt;
    int   nPars;
    int   q;
    float z;
    float chi2;
    float eta;
    float phi;
    float dxy;
    int   nstubs;
    int   idx;
    bool  valid;
    float propPt;
    float propEta;
    float propPhi;
    float propSigmaEta;
    float propSigmaPhi;
  };  

  struct L1TrackProp {
    float pt;
    float p;
    float eta;
    float phi;
    float q;
    float z;
  };  

  struct L1Cand {
    float pt;
    int   nPars;
    float q;
    float z;
    float eta;
    float phi;
    float dxy;
    int   l1TrackIndex;
    int   l1MuonIndex;
    int   quality;
  };

  L1Muon MuCn[MuonMax];
  L1Track L1Tk[TrackMax];
  L1TrackProp L1TkPr[TrackMax];
  L1Cand L1TkCn[TkMuonMax];

  explicit L1TkMuonFromExtendedProducer(const edm::ParameterSet&);
  ~L1TkMuonFromExtendedProducer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:

  virtual void produce(edm::Event&, const edm::EventSetup&);

  int loadL1Muons(edm::Event&, L1Muon*);
  int loadL1Tracks(edm::Event&, L1Track*, L1TrackProp*);

  int computeTkMuCandidates(const int, L1Muon*, const int, L1Track*, L1Cand*);
  void loadTkMuCandidatesToEvent(edm::Event&, std::auto_ptr<L1TkMuonParticleCollection>&, const int, L1Cand*);

  PropState propagateToGMT(const L1TkTrackType& l1tk) const;
  float Pt2Bin(float Pt);
  float Phi2Bin(float Phi);
  float Eta2Bin(float Eta);
  float Z2Bin(float Eta);

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

// ------------ Muon and Tracks converter  ------------
int
L1TkMuonFromExtendedProducer::loadL1Muons(edm::Event& iEvent, L1Muon* MuCn)
{
  using namespace edm;
  using namespace std;

  edm::Handle<L1MuonParticleExtendedCollection> l1musH;
  iEvent.getByLabel(L1MuonsInputTag_, l1musH);
  const L1MuonParticleExtendedCollection& l1mus = *l1musH.product(); 
  
  LogDebug("L1TkMuonFromExtendedProducer") << " l1mus.size= " << l1mus.size();

 /* L1 Muons diapasons:
  *          min         max
  * Eta      -2.425      2.425   
  * Phi      -3.14159    3.14159
  * pt        2          140
  * sigmaEta  0.0144337  0.0288675
  * sigmaPhi  0          0.0125959
  */

  // Muons filling

  int imu = -1;
  int nL1Muons = -1;
  for (auto l1mu : l1mus){
    imu++;

    if( imu<MuonMax ) {
      nL1Muons = imu + 1;

      MuCn[imu].eta = l1mu.eta();
      MuCn[imu].phi = l1mu.phi();
      MuCn[imu].pt = l1mu.pt();
      MuCn[imu].sigmaEta = l1mu.sigmaEta();
      MuCn[imu].sigmaPhi = l1mu.sigmaPhi();

      MuCn[imu].quality = l1mu.quality();
      MuCn[imu].bx = l1mu.bx();
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
        << " pt= "  << MuCn[imu].pt
        << " sigmaEta= " << MuCn[imu].sigmaEta
        << " sigmaPhi= " << MuCn[imu].sigmaPhi 
        << " bx= "  << MuCn[imu].bx
        << " quality= "  << MuCn[imu].quality
        << " gmtCand " << MuCn[imu].gmtCand 
        << " dtCand "  << MuCn[imu].dtCand 
        << " cscCand " << MuCn[imu].cscCand 
        << " rpcCand " << MuCn[imu].rpcCand;
    } 
    else { 
      LogWarning("L1TkMuonFromExtendedProducer") << " Maximum MuonMax number too small " << imu;
    };
  }
  LogDebug("L1TkMuonFromExtendedProducer") << " ::loadL1Muons nL1Muons " << nL1Muons;
  return nL1Muons;
}

// ------------ Tracks converter  ------------
int
L1TkMuonFromExtendedProducer::loadL1Tracks(edm::Event& iEvent, L1Track* L1Tk, L1TrackProp* L1TkPr)
{
  using namespace edm;
  using namespace std;

  edm::Handle<L1TkTrackCollectionType> l1tksH;
  iEvent.getByLabel(L1TrackInputTag_, l1tksH);
  const L1TkTrackCollectionType& l1tks = *l1tksH.product();

  int nL1Tracks = -1;
  int il1tk = -1;
  for (auto l1tk : l1tks ){
    il1tk++;
  
    if( il1tk<TrackMax ) {

      nL1Tracks = il1tk + 1;

      unsigned int nPars = 4;
      if (use5ParameterFit_) nPars = 5;
      L1Tk[il1tk].nPars = nPars;

      L1Tk[il1tk].pt = l1tk.getMomentum(nPars).perp();
      L1Tk[il1tk].z  = l1tk.getPOCA(nPars).z();
      L1Tk[il1tk].chi2 = l1tk.getChi2(nPars);
      L1Tk[il1tk].nstubs = l1tk.getStubRefs().size();
      L1Tk[il1tk].eta = l1tk.getMomentum(nPars).eta();
      L1Tk[il1tk].phi = l1tk.getMomentum(nPars).phi();

      // ****
      const L1TkTrackType& matchTk = l1tks[il1tk];
      auto tkv3=matchTk.getPOCA(L1Tk[il1tk].nPars);
      L1Tk[il1tk].dxy =  tkv3.x()*sin(L1Tk[il1tk].phi) -  tkv3.y()*cos(L1Tk[il1tk].phi);
      L1Tk[il1tk].idx = il1tk;

      auto p3 = l1tk.getMomentum();
      L1TkPr[il1tk].pt   = p3.perp();
      L1TkPr[il1tk].p    = p3.mag();
      L1TkPr[il1tk].eta  = p3.eta();
      L1TkPr[il1tk].phi  = p3.phi() ;
      L1TkPr[il1tk].q    = l1tk.getRInv()>0? 1.:-1.;
      L1TkPr[il1tk].z    = l1tk.getPOCA().z();

      /*
      PropState pstate = propagateToGMT(l1tk);
      if (!pstate.valid) {

        L1Tk[il1tk].valid = true;
        L1Tk[il1tk].propPt  = pstate.pt; 
        L1Tk[il1tk].propEta = pstate.eta;
        L1Tk[il1tk].propPhi = pstate.phi;
        L1Tk[il1tk].propSigmaEta = pstate.sigmaEta;
        L1Tk[il1tk].propSigmaPhi = pstate.sigmaPhi;
  
        LogDebug("L1TkMuonFromExtendedProducer") << " L1Tk[" << il1tk << "].prop "
  	    << " Pt " << L1Tk[il1tk].propPt << " eta " << L1Tk[il1tk].propEta << " phi " << L1Tk[il1tk].propPhi 
            << " SigmaEta " << L1Tk[il1tk].propSigmaEta << " SigmaPhi " << L1Tk[il1tk].propSigmaPhi;
        } else { 
        L1Tk[il1tk].valid = false;
      };
      */
    } 
    else {
      LogWarning("L1TkMuonFromExtendedProducer") << " Maximum TrackMaxn number too small " << nL1Tracks;
    }
  }//over l1tks
  LogDebug("L1TkMuonFromExtendedProducer") << " ::loadL1Muons nL1Tracks " << nL1Tracks;
  return nL1Tracks;
}

// ------------ method called to produce the data  ------------
void
L1TkMuonFromExtendedProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;

  std::auto_ptr<L1TkMuonParticleCollection> tkMuons(new L1TkMuonParticleCollection);

    int nL1Muons = L1TkMuonFromExtendedProducer::loadL1Muons(iEvent, MuCn);
    LogDebug("L1TkMuonFromExtendedProducer") << " nL1Muons " << nL1Muons;
  
    int nL1Tracks = L1TkMuonFromExtendedProducer::loadL1Tracks(iEvent, L1Tk, L1TkPr);
    LogDebug("L1TkMuonFromExtendedProducer") << " nL1Tracks " << nL1Tracks;
  
    int nL1TkCand = computeTkMuCandidates(nL1Muons, MuCn, nL1Tracks, L1Tk, L1TkCn);
    if( nL1TkCand+1 > 0 ){
      LogDebug("L1TkMuonFromExtendedProducer") << " nL1TkCand " << nL1TkCand+1;
    }
   
    L1TkMuonFromExtendedProducer::loadTkMuCandidatesToEvent(iEvent, tkMuons, nL1TkCand, L1TkCn);

    iEvent.put( tkMuons );

}


// ------------ Tracks candidate computation ------------
int
L1TkMuonFromExtendedProducer::computeTkMuCandidates(const int nL1Muons, L1Muon* MuCn, const int nL1Tracks, L1Track* L1Tk,
						    L1Cand* L1TkCn)
{
  using namespace edm;
  using namespace std;

  // PropState calculation
  for ( int itk=0; itk<nL1Tracks; itk++ ){

    L1Tk[itk].valid = false;
    L1Tk[itk].propPt  = -99.;
    L1Tk[itk].propEta = -99.;
    L1Tk[itk].propPhi = -99.;
    L1Tk[itk].propSigmaEta = -99.;
    L1Tk[itk].propSigmaPhi = -99.;

    if (L1Tk[itk].idx >= 0){

      if (!correctGMTPropForTkZ_) L1TkPr[itk].z = 0;
      if ( (L1TkPr[itk].p<3.5) || (abs(L1TkPr[itk].eta) <1.1 && L1TkPr[itk].pt < 3.5) || (abs(L1TkPr[itk].eta) > 2.5) ){
        L1Tk[itk].valid = false;
      } 
      else {
        //0th order:
        L1Tk[itk].valid = true;

        float dzCorrPhi = 1.;
        float deta = 0;
      
        L1TkPr[itk].pt = L1TkMuonFromExtendedProducer::Pt2Bin(L1TkPr[itk].pt);
	L1TkPr[itk].phi = L1TkMuonFromExtendedProducer::Phi2Bin(L1TkPr[itk].phi);
	L1TkPr[itk].eta = L1TkMuonFromExtendedProducer::Eta2Bin(L1TkPr[itk].eta);
	L1TkPr[itk].z = L1TkMuonFromExtendedProducer::Z2Bin(L1TkPr[itk].z);

        float etaProp = abs(L1TkPr[itk].eta);

        if (abs(L1TkPr[itk].eta) < 1.1){
          etaProp = 1.1;
          deta = L1TkPr[itk].z/550./cosh(abs(L1TkPr[itk].eta));
        } else {
          float delta = L1TkPr[itk].z/850.; //roughly scales as distance to 2nd station
          if (L1TkPr[itk].eta > 0) delta *=-1;
          dzCorrPhi = 1. + delta;
      
          float zOzs = L1TkPr[itk].z/850.;
          if (L1TkPr[itk].eta > 0) deta = zOzs/(1. - zOzs);
          else deta = zOzs/(1.+zOzs);
          deta = deta*tanh(L1TkPr[itk].eta);
        }

        float resPhi = L1TkPr[itk].phi - 1.464*L1TkPr[itk].q*cosh(1.7)/cosh(etaProp)/L1TkPr[itk].pt*dzCorrPhi - M_PI/144.;
        if (resPhi > M_PI) resPhi -= 2.*M_PI;
        if (resPhi < -M_PI) resPhi += 2.*M_PI;
      
        L1Tk[itk].propPt  = L1TkPr[itk].pt;            //not corrected for eloss
        L1Tk[itk].propEta = L1TkPr[itk].eta + deta;
        L1Tk[itk].propPhi = resPhi;
        L1Tk[itk].propSigmaEta = 0.100/L1TkPr[itk].pt; //multiple scattering term
        L1Tk[itk].propSigmaPhi = 0.106/L1TkPr[itk].pt; //need a better estimate for these
      
        LogDebug("L1TkMuonFromExtendedProducer") << " L1Tk[" << itk << "].prop "
	          << " Pt " << L1Tk[itk].propPt << " eta " << L1Tk[itk].propEta << " phi " << L1Tk[itk].propPhi 
                  << " SigmaEta " << L1Tk[itk].propSigmaEta << " SigmaPhi " << L1Tk[itk].propSigmaPhi;

      }// else "true"  
    }// if idx
  }// over itk

  // **** Calculation for Muons ****
  int nL1TkCand = -1;
  int il1tkcn = -1;

  for (int imu=0; imu<nL1Muons; imu++) {

    if (MuCn[imu].feta < ETAMIN_) continue;
    if (MuCn[imu].feta > ETAMAX_) continue;

    // can skip quality cuts at the moment, keep bx=0 req
    if (MuCn[imu].bx != 0) continue;

    if (!MuCn[imu].gmtCand){
      //some selections here currently the input can be a merge from different track finders so, the GMT cand may be missing
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

    // **** Calculation for Tracks ****
    float dr2 = -1;
    float dr2prop = 999.;
    float drmin = 999.;

    for ( int itk=0; itk<nL1Tracks; itk++ ){
      if (L1Tk[itk].idx >= 0){
       
        L1Tk[itk].pt = L1TkMuonFromExtendedProducer::Pt2Bin(L1Tk[itk].pt);
        L1Tk[itk].phi = L1TkMuonFromExtendedProducer::Phi2Bin(L1Tk[itk].phi);
        L1Tk[itk].eta = L1TkMuonFromExtendedProducer::Eta2Bin(L1Tk[itk].eta);
        L1Tk[itk].z = L1TkMuonFromExtendedProducer::Z2Bin(L1Tk[itk].z);

        if (     L1Tk[itk].pt < PTMINTRA_) continue;
        if (fabs(L1Tk[itk].z) > ZMAX_) continue;
        if (     L1Tk[itk].chi2 > CHI2MAX_) continue;
        if (     L1Tk[itk].nstubs < nStubsmin_) continue;
        // Do we need it !? // if (     L1Tk[itk].valid) continue;
   
        dr2 = deltaR2(MuCn[imu].eta, MuCn[imu].phi, L1Tk[itk].eta, L1Tk[itk].phi);
        if (dr2 > 0.3) continue;

        dr2prop = deltaR2(MuCn[imu].eta, MuCn[imu].phi, L1Tk[itk].propEta, L1Tk[itk].propPhi);
        if (dr2prop < drmin) drmin = dr2prop;
        if (dr2prop < drmin) continue;

        float etaCut = 3.*sqrt(MuCn[imu].sigmaEta*MuCn[imu].sigmaEta + L1Tk[itk].propSigmaEta*L1Tk[itk].propSigmaEta);
        float phiCut = 4.*sqrt(MuCn[imu].sigmaPhi*MuCn[imu].sigmaPhi + L1Tk[itk].propSigmaPhi*L1Tk[itk].propSigmaPhi);
        float dEta = std::abs(L1Tk[itk].propEta - MuCn[imu].eta);
        float dPhi = std::abs(deltaPhi(L1Tk[itk].propPhi, MuCn[imu].phi));
  
        LogDebug("L1TkMuonFromExtendedProducer") 
              << " match details: prop Pt "<<L1Tk[itk].propPt<<" Eta "<<L1Tk[itk].propEta
              << " Phi "<<L1Tk[itk].propPhi<<" mutk "<<MuCn[imu].pt<<" "<<MuCn[imu].eta<<" "<<MuCn[imu].phi
              << " dEta "<< dEta <<" dPhi "<< dPhi <<" cut "<< etaCut <<" "<< phiCut;
  
        // Filter for Cuts 
        if (dEta < etaCut && dPhi < phiCut){
          il1tkcn++;

	  L1TkCn[il1tkcn].nPars = L1Tk[itk].nPars;
          L1TkCn[il1tkcn].pt = L1Tk[itk].pt;
          L1TkCn[il1tkcn].q = L1Tk[itk].q;
          L1TkCn[il1tkcn].z = L1Tk[itk].z;
          L1TkCn[il1tkcn].eta = L1Tk[itk].eta;
          L1TkCn[il1tkcn].phi = L1Tk[itk].phi;
          L1TkCn[il1tkcn].dxy = L1Tk[itk].dxy;
          L1TkCn[il1tkcn].l1MuonIndex = imu;
          L1TkCn[il1tkcn].l1TrackIndex = itk;
          L1TkCn[il1tkcn].quality = MuCn[imu].quality;

	}// over Cuts
      }
   
    }// over itk
  }//over imu

  nL1TkCand = il1tkcn;
  LogDebug("L1TkMuonFromExtendedProducer") << " ::computeTkMuCandidates nL1TkCand " << nL1TkCand;
  return nL1TkCand;
}

float L1TkMuonFromExtendedProducer::Pt2Bin(float Pt){
  using namespace std;

  float MinPt   = 2.;   // GeV
  float MaxPt   = 200.; // GeV

  // 2**6 = 64
  float PtBinStep = (MaxPt - MinPt)/exp2(6);

  // float tmpPt = Pt;
  Pt = max( (double)MinPt, min((double)Pt, (double)MaxPt));
  Pt = (MinPt + (float)( PtBinStep * (int)(Pt/PtBinStep)));

 /*
  cout << "   Pt " << Pt << " PtBinStep " << PtBinStep << " sPt " << tmpPt - Pt << endl;
  */
  return Pt;
}


float L1TkMuonFromExtendedProducer::Phi2Bin(float Phi){
  using namespace std;

  float MinPhi  = -3.14159; // Rad
  float MaxPhi  =  3.14159; // Rad

  // 2**8 = 256
  float PhiBinStep = (MaxPhi - MinPhi)/exp2(8);

  Phi = max( (double)MinPhi, min((double)Phi, (double)MaxPhi));

  //float tmpPhi = Phi;
  Phi = (float)( PhiBinStep * (int)(Phi/PhiBinStep));
  if (Phi < 0.){
    Phi = -(float)( PhiBinStep * (int)(abs(Phi)/PhiBinStep));
  } 

 /*
  cout << "   tmpPhi " << tmpPhi << " Phi " << Phi << " dPhi " << (tmpPhi - Phi) << endl;
  */
  return Phi;
}


float L1TkMuonFromExtendedProducer::Eta2Bin(float Eta){
  using namespace std;

  float MinEta  = -2.65;
  float MaxEta  =  2.65;

  // 2**8 = 256
  float EtaBinStep = (MaxEta - MinEta)/exp2(8);

  Eta = max( (double)MinEta, min((double)Eta, (double)MaxEta));

  //float tmpEta = Eta;
  Eta = (float)( EtaBinStep * (int)(Eta/EtaBinStep));
  if (Eta < 0.){
    Eta = -(float)( EtaBinStep * (int)(abs(Eta)/EtaBinStep));
  } 

 /*
  cout << "   tmpEta " << tmpEta << " Eta " << Eta << " dEta " << (tmpEta - Eta) << endl;
  */
  return Eta;
}

float L1TkMuonFromExtendedProducer::Z2Bin(float Z){
  using namespace std;

  float MinZ  = -17;
  float MaxZ  =  17;

  // 2**4 = 16
  float ZBinStep = (MaxZ - MinZ)/exp2(4);

  Z = max( (double)MinZ, min((double)Z, (double)MaxZ));

  //float tmpZ = Z;
  Z = (float)( ZBinStep * (int)(Z/ZBinStep));
  if (Z < 0.){
    Z = -(float)( ZBinStep * (int)(abs(Z)/ZBinStep));
  } 

 /*
  cout << "   tmpZ " << tmpZ << " Z " << Z << " dZ " << (tmpZ - Z) << endl;
  */
  return Z;
}


L1TkMuonFromExtendedProducer::PropState L1TkMuonFromExtendedProducer::propagateToGMT(const L1TkMuonFromExtendedProducer::L1TkTrackType& tk) 
const {

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


// ------------ Tracks candidate  ------------
void
L1TkMuonFromExtendedProducer::loadTkMuCandidatesToEvent(edm::Event& iEvent, std::auto_ptr<L1TkMuonParticleCollection>& tkMuons,
const int nL1TkCand, L1Cand* L1TkCn)
{
  using namespace edm;
  using namespace std;

  edm::Handle<L1TkTrackCollectionType> l1tksH;
  iEvent.getByLabel(L1TrackInputTag_, l1tksH);
  const L1TkTrackCollectionType& l1tks = *l1tksH.product();

  edm::Handle<L1MuonParticleExtendedCollection> l1musH;
  iEvent.getByLabel(L1MuonsInputTag_, l1musH);
  const L1MuonParticleExtendedCollection& l1mus = *l1musH.product(); 
  
  L1TkMuonParticleCollection l1tkmuCands;
  l1tkmuCands.reserve(l1mus.size()*4); //can do more if really needed
  LogDebug("L1TkMuonFromExtendedProducer") << " l1tks.size= " << l1tks.size();
  
  for ( int il1tkcn=0; il1tkcn<=nL1TkCand; il1tkcn++ ){

    Ptr< L1TkTrackType > l1tkPtr(l1tksH, L1TkCn[il1tkcn].l1TrackIndex);
    const L1TkTrackType& matchTk = l1tks[L1TkCn[il1tkcn].l1TrackIndex];
     
    auto p3 = matchTk.getMomentum(L1TkCn[il1tkcn].nPars);
    float p4e = sqrt(0.105658369*0.105658369 + p3.mag2() );
     
    math::XYZTLorentzVector l1tkp4(p3.x(), p3.y(), p3.z(), p4e);
     
    auto tkv3=matchTk.getPOCA(L1TkCn[il1tkcn].nPars);
    math::XYZPoint v3(tkv3.x(), tkv3.y(), tkv3.z());
     
    L1TkCn[il1tkcn].q = matchTk.getRInv(L1TkCn[il1tkcn].nPars)>0? 1: -1;
  
    L1TkMuonParticle l1tkmu(reco::LeafCandidate(L1TkCn[il1tkcn].q, l1tkp4, v3, -13*L1TkCn[il1tkcn].q ));
  
    l1tkmu.setTrkPtr(l1tkPtr);
  
    L1MuonParticleExtendedRef l1muRef(l1musH, L1TkCn[il1tkcn].l1MuonIndex);
    l1tkmu.setMuExtendedRef(l1muRef);
  
    l1tkmu.setQuality(L1TkCn[il1tkcn].quality);
  
    float trkisol = -999;
    l1tkmu.setTrkIsol(trkisol);
     
    // EP: add the zvtx information
    l1tkmu.setTrkzVtx( (float)tkv3.z() );
  
    tkMuons->push_back(l1tkmu);

  }//over il1tkcn
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

//define this as a plug-in
DEFINE_FWK_MODULE(L1TkMuonFromExtendedProducer);
