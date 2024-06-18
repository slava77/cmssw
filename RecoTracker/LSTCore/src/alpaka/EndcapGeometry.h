#ifndef EndcapGeometry_h
#define EndcapGeometry_h

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef LST_IS_CMSSW_PACKAGE
#include "RecoTracker/LSTCore/interface/alpaka/Constants.h"
#else
#include "Constants.h"
#endif

#include "HeterogeneousCore/AlpakaInterface/interface/host.h"

namespace SDL {

  template <typename TDev>
  class EndcapGeometry {
  private:
    std::map<unsigned int, float> dxdy_slope_;     // dx/dy slope
    std::map<unsigned int, float> centroid_phis_;  // centroid phi

    // Friend all other instantiations of this template
    template <typename OtherTDev>
    friend class EndcapGeometry;

  public:
    Buf<TDev, unsigned int> geoMapDetId_buf;
    Buf<TDev, float> geoMapPhi_buf;

    unsigned int nEndCapMap;

    EndcapGeometry(TDev const& devAccIn);
    template <typename TQueue>
    EndcapGeometry(TQueue& queue, EndcapGeometry<DevHost> const& endcapGeometrySrc);
    ~EndcapGeometry() = default;

    void load(std::string);
    void fillGeoMapArraysExplicitHost();
    float getdxdy_slope(unsigned int detid) const;
  };
}  // namespace SDL

#endif
