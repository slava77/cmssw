#ifndef LSTESData_H
#define LSTESData_H

#ifdef LST_IS_CMSSW_PACKAGE
#include "RecoTracker/LSTCore/interface/alpaka/Constants.h"
#else
#include "Constants.h"
#endif

#include "HeterogeneousCore/AlpakaInterface/interface/CopyToDevice.h"

#include <filesystem>
#include <memory>

namespace SDL {

  struct pixelMap;

  template <typename>
  struct modulesBuffer;

  template <typename>
  class EndcapGeometry;

  template <typename TDev>
  struct LSTESData {
    uint16_t nModules;
    uint16_t nLowerModules;
    unsigned int nPixels;
    std::shared_ptr<const modulesBuffer<TDev>> modulesBuffers;
    std::shared_ptr<const EndcapGeometry<TDev>> endcapGeometry;
    std::shared_ptr<const pixelMap> pixelMapping;

    LSTESData(uint16_t const& nModulesIn,
              uint16_t const& nLowerModulesIn,
              unsigned int const& nPixelsIn,
              std::shared_ptr<const modulesBuffer<TDev>> const& modulesBuffersIn,
              std::shared_ptr<const EndcapGeometry<TDev>> const& endcapGeometryIn,
              std::shared_ptr<const pixelMap> const& pixelMappingIn)
        : nModules(nModulesIn),
          nLowerModules(nLowerModulesIn),
          nPixels(nPixelsIn),
          modulesBuffers(modulesBuffersIn),
          endcapGeometry(endcapGeometryIn),
          pixelMapping(pixelMappingIn) {}
  };

  std::unique_ptr<LSTESData<SDL::DevHost>> loadAndFillESHost();

}  // namespace SDL

namespace cms::alpakatools {
  template <>
  struct CopyToDevice<SDL::LSTESData<SDL::DevHost>> {
    template <typename TQueue>
    static SDL::LSTESData<alpaka::Dev<TQueue>> copyAsync(TQueue& queue, SDL::LSTESData<SDL::DevHost> const& srcData);
  };
}  // namespace cms::alpakatools

#endif
