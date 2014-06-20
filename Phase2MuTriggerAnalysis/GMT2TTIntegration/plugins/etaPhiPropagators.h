#ifndef etaPhiPropagators_h
#define etaPhiPropagators_h

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <cmath>
#include <utility>


class etaPhiProp {
 public:
  etaPhiProp() {}

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

  virtual PropState propagate(float pt, float eta, float phi, float q, float vz) const = 0;

};

class etaPhiToStation2Run1TF : public etaPhiProp {
 public:
  etaPhiToStation2Run1TF() {}
  PropState propagate(float pt, float eta, float phi, float q, float vz) const {
    PropState result;
    float resEta = eta;
    float etaProp = std::abs(eta);
    if  (etaProp< 1.1) etaProp = 1.1;
    float resPhi = phi - 1.464*q*cosh(1.7)/cosh(etaProp)/pt - M_PI/144.;
    if (resPhi > M_PI) resPhi -= 2.*M_PI;
    if (resPhi < -M_PI) resPhi += 2.*M_PI;

    result.valid = true;
    result.pt = pt;
    result.eta = resEta;
    result.phi = resPhi;
    return result;
  }
};

#endif
