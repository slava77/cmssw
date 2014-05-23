// -*- C++ -*-
//
// Package:     L1Trigger
// Class  :     L1TkEmParticle
// 

#include "DataFormats/L1TrackTrigger/interface/L1TkMuonParticle.h"


using namespace l1extra ;


L1TkMuonParticle::L1TkMuonParticle( const LorentzVector& p4,
         // const edm::Ref< XXXCollection >& muRef,  // PL
	 const edm::Ref< L1MuonParticleCollection > &muRef,
         const edm::Ptr< L1TkTrackType >& trkPtr,
	 float tkisol )
   : LeafCandidate( ( char ) 0, p4 ),
     muRef_ ( muRef ) ,		// PL uncomment
     trkPtr_ ( trkPtr ) ,
     TrkIsol_ ( tkisol ), quality_(999)

{

 if ( trkPtr_.isNonnull() ) {
	float z = getTrkPtr() -> getVertex().z(); 
	setTrkzVtx( z );
 }
}

int L1TkMuonParticle::bx() const {
 int dummy = -999;
	// PL.  In case Pierluigi's objects have a bx

 if ( muRef_.isNonnull() ) {
   return (getMuRef() -> bx()) ;
 }
 else if (muExtendedRef_.isNonnull()) {
   return getMuExtendedRef()->bx();
 
 }

        return dummy;
}







