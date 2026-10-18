#include <alpaka/alpaka.hpp>

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "MkSeederDevice.h"
#include "MkSeederKernels.h"

#ifndef MKSEEDER_QUEUE_BLOCKS
#define MKSEEDER_QUEUE_BLOCKS 256
#endif

namespace ALPAKA_ACCELERATOR_NAMESPACE::mkseeder {

  using namespace cms::alpakatools;

  void runSide(Queue &queue, mkseederdev::View const &view, mkseederdev::Queues const &queues, std::vector<int> const &layOf) {
    constexpr uint32_t kThreads = 128;
    // the queue kernels stride over a count only the device knows: a fixed grid
    constexpr uint32_t kQueueBlocks = std::is_same_v<Device, alpaka::DevCpu> ? 1 : MKSEEDER_QUEUE_BLOCKS;
    if (view.C.nwork > 0) {
      const auto wd = make_workdiv<Acc1D>(divide_up_by((uint32_t)view.C.nwork, kThreads), kThreads);
      alpaka::exec<Acc1D>(queue, wd, KernelStart{}, view, queues);
    }
    const auto wq = make_workdiv<Acc1D>(kQueueBlocks, kThreads);
    for (int p = 0; p < view.C.n; ++p) {
      if (layOf[p] < 0)
        continue;
      alpaka::exec<Acc1D>(queue, wq, KernelStageC{}, view, queues, p);
      alpaka::exec<Acc1D>(queue, wq, KernelStageCMissCount{}, view, queues, p);
      alpaka::exec<Acc1D>(queue, make_workdiv<Acc1D>(1, 1), KernelReserve{}, view, queues);
      alpaka::exec<Acc1D>(queue, wq, KernelStageCMissFill{}, view, queues, p);
      alpaka::exec<Acc1D>(queue, wq, KernelStageD{}, view, queues, p);
      alpaka::exec<Acc1D>(queue, make_workdiv<Acc1D>(4, kThreads), KernelRelease{}, view, queues, p);
    }
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::mkseeder
