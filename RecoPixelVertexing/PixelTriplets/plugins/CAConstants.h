#ifndef RecoPixelVertexing_PixelTriplets_plugins_CAConstants_h
#define RecoPixelVertexing_PixelTriplets_plugins_CAConstants_h

#include <cstdint>

#include <cuda_runtime.h>

#include "CUDADataFormats/SiPixelCluster/interface/gpuClusteringConstants.h"
#include "HeterogeneousCore/CUDAUtilities/interface/GPUSimpleVector.h"
#include "HeterogeneousCore/CUDAUtilities/interface/GPUVecArray.h"
#include "HeterogeneousCore/CUDAUtilities/interface/HistoContainer.h"

// #define ONLY_PHICUT

namespace CAConstants {

  // constants
#ifndef ONLY_PHICUT
#ifdef GPU_SMALL_EVENTS
  constexpr uint32_t maxNumberOfTuples() { return 3 * 1024; }
#else
  constexpr uint32_t maxNumberOfTuples() { return 24 * 1024; }
#endif
#else
  constexpr uint32_t maxNumberOfTuples() { return 48 * 1024; }
#endif
  constexpr uint32_t maxNumberOfQuadruplets() { return maxNumberOfTuples(); }
#ifndef ONLY_PHICUT
#ifndef GPU_SMALL_EVENTS
  constexpr uint32_t maxNumberOfDoublets() { return 448 * 1024; }
  constexpr uint32_t maxCellsPerHit() { return 128; }
#else
  constexpr uint32_t maxNumberOfDoublets() { return 128 * 1024; }
  constexpr uint32_t maxCellsPerHit() { return 128 / 2; }
#endif
#else
  constexpr uint32_t maxNumberOfDoublets() { return 2 * 1024 * 1024; }
  constexpr uint32_t maxCellsPerHit() { return 8 * 128; }
#endif
  constexpr uint32_t maxNumOfActiveDoublets() { return maxNumberOfDoublets() / 4; }

  constexpr uint32_t maxNumberOfLayerPairs() { return 20; }
  constexpr uint32_t maxNumberOfLayers() { return 10; }
  constexpr uint32_t maxTuples() { return maxNumberOfTuples(); }

  // types
  using hindex_type = uint16_t;  // FIXME from siPixelRecHitsHeterogeneousProduct
  using tindex_type = uint16_t;  //  for tuples

#ifndef ONLY_PHICUT
  using CellNeighbors = GPU::VecArray<uint32_t, 36>;
  using CellTracks = GPU::VecArray<tindex_type, 42>;
#else
  using CellNeighbors = GPU::VecArray<uint32_t, 64>;
  using CellTracks = GPU::VecArray<tindex_type, 64>;
#endif

  using CellNeighborsVector = GPU::SimpleVector<CellNeighbors>;
  using CellTracksVector = GPU::SimpleVector<CellTracks>;

  using OuterHitOfCell = GPU::VecArray<uint32_t, maxCellsPerHit()>;
  using TuplesContainer = OneToManyAssoc<hindex_type, maxTuples(), 5 * maxTuples()>;
  using HitToTuple =
      OneToManyAssoc<tindex_type, pixelGPUConstants::maxNumberOfHits, 4 * maxTuples()>;  // 3.5 should be enough
  using TupleMultiplicity = OneToManyAssoc<tindex_type, 8, maxTuples()>;

}  // namespace CAConstants

#endif  // RecoPixelVertexing_PixelTriplets_plugins_CAConstants_h
