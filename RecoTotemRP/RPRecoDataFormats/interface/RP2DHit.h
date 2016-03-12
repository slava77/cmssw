#ifndef RecoTotemRP_RPRecoDataFormats_RP2DHit_h
#define RecoTotemRP_RPRecoDataFormats_RP2DHit_h

#include "TMath.h"
#include <ostream>

class RP2DHit
{
  public:
    RP2DHit() {x_=0.0; y_=0.0; vx_=0.0; vy_=0.0; z_=0.0;} 
    RP2DHit(double x, double y, double vx, double vy, double z=0.0) 
          {x_=x; y_=y; vx_=vx; vy_=vy; z_=z;}
    inline double X() const {return x_;}
    inline double Y() const {return y_;}
    inline double Vx() const {return vx_;}
    inline double Vy() const {return vy_;}
    inline double Z() const {return z_;}
    
    inline double Sx() const {return TMath::Sqrt(vx_);}
    inline double Sy() const {return TMath::Sqrt(vy_);}
    
    inline void X(double val) {x_=val;}
    inline void Y(double val) {y_=val;}
    inline void Vx(double val) {vx_=val;}
    inline void Vy(double val) {vy_=val;}
    inline void Z(double val) {z_=val;}
  private:
    double x_, y_, z_;
    double vx_, vy_;  //variance of x & y 
};

std::ostream & operator<<(std::ostream &o, const RP2DHit &hit);

#endif
