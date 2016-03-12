#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include <ostream>

std::ostream & operator<<(std::ostream &o, const RP2DHit &hit)
{
  o<<"pos=("<<hit.X()<<","<<hit.Y()<<","<<hit.Z()<<")"<<std::endl;
  o<<"\tsigma x="<<hit.Sx()<<" sigma y="<<hit.Sy()<<std::endl;
  o<<"\tvariance x="<<hit.Vx()<<" sigma y="<<hit.Vy()<<std::endl;
  return o;
}
