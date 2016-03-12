#ifndef RecoTotemRP_RPRecoDataFormats_RP2DHitDebug_h
#define RecoTotemRP_RPRecoDataFormats_RP2DHitDebug_h

#include "TMath.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"


class RP2DHitDebug : public RP2DHit
{
  public:
    RP2DHitDebug() : RP2DHit() {}
    RP2DHitDebug(const RP2DHit & hit) : RP2DHit(hit) {}
    inline double DeltaX() {return d_x_;}
    inline double DeltaY() {return d_y_;}
    inline double PullX() {return pull_x_;}
    inline double PullY() {return pull_y_;}
    
    inline void DeltaX(double val) {d_x_ = val;}
    inline void DeltaY(double val) {d_y_ = val;}
    inline void PullX(double val) {pull_x_ = val;}
    inline void PullY(double val) {pull_y_ = val;}
  private:
    double d_x_;
    double d_y_;
    double pull_x_;
    double pull_y_;
};

#endif
