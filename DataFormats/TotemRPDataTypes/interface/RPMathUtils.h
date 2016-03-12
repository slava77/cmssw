#ifndef DataFormats_TotemRPDataTypes_RPMathUtils_h
#define DataFormats_TotemRPDataTypes_RPMathUtils_h

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "TVector.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include <iostream>
#include <ostream>


namespace tot_rp
{
  inline HepGeom::Point3D<double> convert( const TVector3 & v ) {
    return HepGeom::Point3D<double>(v.X(),v.Y(),v.Z()) ;
  }
  
  inline TVector3 convert( const HepGeom::Point3D<double> & v ) {
    return TVector3(v.x(),v.y(),v.z()) ;
  }
  
  inline CLHEP::Hep3Vector convert3vector( const TVector3 & v ) {
    return CLHEP::Hep3Vector(v.X(),v.Y(),v.Z()) ;
  }
  
  inline TVector3 convert3vector( const CLHEP::Hep3Vector & v ) {
    return TVector3(v.x(),v.y(),v.z()) ;
  }
  
  inline CLHEP::HepLorentzVector convert( const TLorentzVector & v ) {
    return CLHEP::HepLorentzVector(v.X(),v.Y(),v.Z(),v.T()) ;
  }
  
  inline TLorentzVector convert( const CLHEP::HepLorentzVector & v ) {
    return TLorentzVector(v.x(),v.y(),v.z(),v.t()) ;
  }
  
  void Print(std::ostream &o, const TMatrixD &mt);
  void Print(std::ostream &o, const TVectorD &vt);
}

#endif
