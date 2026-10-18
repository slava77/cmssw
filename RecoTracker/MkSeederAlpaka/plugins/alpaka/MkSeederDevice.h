#ifndef RecoTracker_MkSeederAlpaka_plugins_alpaka_MkSeederDevice_h
#define RecoTracker_MkSeederAlpaka_plugins_alpaka_MkSeederDevice_h

#include <vector>

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

#include "MkSeederDeviceTypes.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::mkseeder {

  // The chain finder of one z side on the queue: stage b, then stage c and d per chain position. layOf: the
  // side's chain positions with a layer (host copy of View::lay_of), to skip the others.
  void runSide(Queue &queue, mkseederdev::View const &view, mkseederdev::Queues const &queues, std::vector<int> const &layOf);

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::mkseeder

#endif
