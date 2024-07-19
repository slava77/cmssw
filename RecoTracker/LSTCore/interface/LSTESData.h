#ifndef RecoTracker_LSTCore_interface_LSTESData_h
#define RecoTracker_LSTCore_interface_LSTESData_h

#include "RecoTracker/LSTCore/interface/Constants.h"
#include "RecoTracker/LSTCore/interface/EndcapGeometryBuffer.h"
#include "RecoTracker/LSTCore/interface/Module.h"
#include "RecoTracker/LSTCore/interface/PixelMap.h"

#include "HeterogeneousCore/AlpakaInterface/interface/CopyToDevice.h"

#include <filesystem>
#include <memory>

namespace SDL {

  template <typename TDev>
  struct LSTESData {
    uint16_t nModules;
    uint16_t nLowerModules;
    unsigned int nPixels;
    unsigned int nEndCapMap;
    std::shared_ptr<const ModulesBuffer<TDev>> modulesBuffers;
    std::shared_ptr<const EndcapGeometryBuffer<TDev>> endcapGeometryBuffers;
    std::shared_ptr<const pixelMap> pixelMapping;

    LSTESData(uint16_t const& nModulesIn,
              uint16_t const& nLowerModulesIn,
              unsigned int const& nPixelsIn,
              unsigned int const& nEndCapMapIn,
              std::shared_ptr<const ModulesBuffer<TDev>> const& modulesBuffersIn,
              std::shared_ptr<const EndcapGeometryBuffer<TDev>> const& endcapGeometryBuffersIn,
              std::shared_ptr<const pixelMap> const& pixelMappingIn)
        : nModules(nModulesIn),
          nLowerModules(nLowerModulesIn),
          nPixels(nPixelsIn),
          nEndCapMap(nEndCapMapIn),
          modulesBuffers(modulesBuffersIn),
          endcapGeometryBuffers(endcapGeometryBuffersIn),
          pixelMapping(pixelMappingIn) {}
  };

  std::unique_ptr<LSTESData<alpaka_common::DevHost>> loadAndFillESHost();

}  // namespace SDL

namespace cms::alpakatools {
  template <>
  struct CopyToDevice<SDL::LSTESData<alpaka_common::DevHost>> {
    template <typename TQueue>
    static SDL::LSTESData<alpaka::Dev<TQueue>> copyAsync(TQueue& queue,
                                                         SDL::LSTESData<alpaka_common::DevHost> const& srcData) {
      auto deviceModulesBuffers = std::make_shared<SDL::ModulesBuffer<alpaka::Dev<TQueue>>>(
          alpaka::getDev(queue), srcData.nModules, srcData.nPixels);
      deviceModulesBuffers->copyFromSrc(queue, *srcData.modulesBuffers);
      auto deviceEndcapGeometryBuffers =
          std::make_shared<SDL::EndcapGeometryBuffer<alpaka::Dev<TQueue>>>(alpaka::getDev(queue), srcData.nEndCapMap);
      deviceEndcapGeometryBuffers->copyFromSrc(queue, *srcData.endcapGeometryBuffers);

      return SDL::LSTESData<alpaka::Dev<TQueue>>(srcData.nModules,
                                                 srcData.nLowerModules,
                                                 srcData.nPixels,
                                                 srcData.nEndCapMap,
                                                 deviceModulesBuffers,
                                                 deviceEndcapGeometryBuffers,
                                                 srcData.pixelMapping);
    }
  };
}  // namespace cms::alpakatools

#endif
