#ifndef RecoTracker_LST_LSTModulesDev_h
#define RecoTracker_LST_LSTModulesDev_h

#include "HeterogeneousCore/AlpakaInterface/interface/CopyToDevice.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include <SDL/Module.h>

namespace cms::alpakatools {
  template <>
  struct CopyToDevice<SDL::modulesBuffer<alpaka_common::DevHost>> {
    template <typename TQueue>
    static auto copyAsync(TQueue& queue, SDL::modulesBuffer<alpaka_common::DevHost> const& srcData) {
      return SDL::modulesBuffer<alpaka::Dev<TQueue>>(queue, srcData);
    }
  };
} //namespace cms::alpakatools
#endif //RecoTracker_LST_LSTModulesDev_h
