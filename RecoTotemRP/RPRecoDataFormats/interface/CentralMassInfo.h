#ifndef RecoTotemRP_RPRecoDataFormats_interface_CentralMassInfo_h
#define RecoTotemRP_RPRecoDataFormats_interface_CentralMassInfo_h

#include "TMath.h"
#include <iostream>

struct CentralMassInfo
{
  CentralMassInfo() {clear();}
  double px, py, pz;
  double pt;
  double e;
  double m;
  double min_rap, max_rap;
  double rap;
  bool empty;
  
  void clear()
  {
    empty = true;
    px = 0.;
    py = 0.; 
    pz = 0.;
    pt = 0.;
    e = 0.;
    m = 0.;
    min_rap  = 0.;
    max_rap = 0.;
    rap = 0.;
  }
  
  void AddParticle(double pxr, double pyr, double pzr, double er)
  {
    e += er; 
    px += pxr;
    py += pyr;
    pz += pzr;
    pt = sqrt(px*px + py*py);
    m = sqrt(e*e - px*px - py*py - pz*pz);
    
    double rapidity = 0.5 * TMath::Log( (er + pzr)/(er - pzr) );
    
    if(empty || rapidity > max_rap)
    {
      max_rap = rapidity;
    }
    if(empty || rapidity < min_rap)
    {
      min_rap = rapidity;
    }
    
    rap = 0.5 * TMath::Log( (e + pz)/(e - pz) );
    
    empty = false;
    
//    std::cout<<"CentralMassInfo: Particle added: "<<"("<<pxr<<","<<pyr<<","<<pzr<<","<<er<<")"<<std::endl;
//    std::cout<<"m="<<m<<" min_rap="<<min_rap<<"  max_rap="<<max_rap<<"  rap="<<rap<<std::endl;
  }
};  


#endif