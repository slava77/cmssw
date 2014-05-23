#ifndef L1TkTrigger_L1MuonParticle_h
#define L1TkTrigger_L1MuonParticle_h

// -*- C++ -*-
//
// Package:     L1Trigger
// Class  :     L1TkEmParticle
// 

#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/Ptr.h"

#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"

#include "DataFormats/L1TrackTrigger/interface/L1TkEmParticle.h"

#include "SimDataFormats/SLHC/interface/StackedTrackerTypes.h"

#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleExtended.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleExtendedFwd.h"

// PL :include PierLuigi's class
// #include "....."

namespace l1extra {
  
  class L1TkMuonParticle : public reco::LeafCandidate
  {     
    
  public:
    
    typedef L1TkTrack_PixelDigi_                          L1TkTrackType;
    typedef std::vector< L1TkTrackType >                  L1TkTrackCollectionType;
    
    L1TkMuonParticle() : TrkIsol_(-999.), quality_(999) {} 
    
    L1TkMuonParticle( const LorentzVector& p4,
		      // const edm::Ref< XXXCollection >& muRef,     // reference to PL's object
		      const edm::Ref< L1MuonParticleCollection >& muRef,
		      const edm::Ptr< L1TkTrackType >& trkPtr,
		      float tkisol = -999. );
    
    L1TkMuonParticle(const reco::LeafCandidate& cand) : reco::LeafCandidate(cand), TrkIsol_(-999.), quality_(999) {}

    virtual ~L1TkMuonParticle() {}
    
    // ---------- const member functions ---------------------
    
    const edm::Ptr< L1TkTrackType >& getTrkPtr() const
      { return trkPtr_ ; }
    
    // PL :
    // const edm::Ref< XXXCollection >& getMuRef() const
    // { return muRef_ ; }
    
    const L1MuonParticleRef&  getMuRef() const
      { return muRef_ ; }

    const L1MuonParticleExtendedRef&  getMuExtendedRef() const
      { return muExtendedRef_ ; }
    
    float getTrkzVtx() const { return TrkzVtx_ ; }
    float getTrkIsol() const { return TrkIsol_ ; }
    
    
    // ---------- member functions ---------------------------
    
    void setTrkPtr(const edm::Ptr< L1TkTrackType >& p) {trkPtr_ = p;}
    void setMuRef(const L1MuonParticleRef& r){muRef_ = r;}
    void setMuExtendedRef(const L1MuonParticleExtendedRef& r){muExtendedRef_ = r;}

    void setTrkzVtx(float TrkzVtx)  { TrkzVtx_ = TrkzVtx ; }
    void setTrkIsol(float TrkIsol)  { TrkIsol_ = TrkIsol ; }

    void setQuality(unsigned int q){ quality_ = q;}

    int bx() const;
    
    unsigned int quality()  const {return quality_;}
    
    //void setDeltaR(float dr) { DeltaR_ = dr ; }
    
  private:
    
    // PL
    // edm::Ref< XXXCollection > muRef_ ;
    L1MuonParticleRef muRef_ ;
    L1MuonParticleExtendedRef muExtendedRef_ ;
    
    edm::Ptr< L1TkTrackType > trkPtr_ ;
    
    float TrkIsol_;
    float TrkzVtx_ ;
    
    unsigned int quality_;
    //float DeltaR_ ;	// temporary
    
  };
}

#endif


