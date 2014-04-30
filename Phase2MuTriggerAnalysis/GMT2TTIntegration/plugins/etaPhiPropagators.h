#ifndef etaPhiPropagators_h
#define etaPhiPropagators_h

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <cmath>
#include <utility>


class etaPhiProp {
 public:
  etaPhiProp() {}
  virtual std::pair<float,float> propagate(float pt, float eta, float phi, float q, float vz) const = 0;
};

class etaPhiToStation2Run1TF : public etaPhiProp {
 public:
  etaPhiToStation2Run1TF() {}
  std::pair<float,float> propagate(float pt, float eta, float phi, float q, float vz) const {
    float resEta = eta;
    float etaProp = std::abs(eta);
    if  (etaProp< 1.1) etaProp = 1.1;
    float resPhi = phi - 1.464*q*cosh(1.7)/cosh(etaProp)/pt - M_PI/144.;
    if (resPhi > M_PI) resPhi -= 2.*M_PI;
    if (resPhi < -M_PI) resPhi += 2.*M_PI;

    return std::make_pair(resEta, resPhi);    
  }
};

#endif
