#ifndef RecoPixelVertexing_PixelTriplets_plugins_CAHitNtupletGeneratorOnGPU_h
#define RecoPixelVertexing_PixelTriplets_plugins_CAHitNtupletGeneratorOnGPU_h

#include <cuda_runtime.h>
#include "CUDADataFormats/TrackingRecHit/interface/TrackingRecHit2DCUDA.h"
#include "CUDADataFormats/Track/interface/PixelTrackHeterogeneous.h"


#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "HeterogeneousCore/CUDAServices/interface/CUDAService.h"
#include "HeterogeneousCore/CUDAUtilities/interface/GPUSimpleVector.h"

#include "CAHitNtupletGeneratorKernels.h"
#include "HelixFitOnGPU.h"

// FIXME  (split header???)
#include "GPUCACell.h"

namespace edm {
  class Event;
  class EventSetup;
  class ParameterSetDescription;
}  // namespace edm

class CAHitNtupletGeneratorOnGPU {
public:
  using HitsOnGPU = TrackingRecHit2DSOAView;
  using HitsOnCPU = TrackingRecHit2DCUDA;
  using hindex_type = TrackingRecHit2DSOAView::hindex_type;

  using Quality = pixelTrack::Quality;
  using OutputSoA = pixelTrack::TrackSoA;
  using HitContainer = pixelTrack::HitContainer;
  using Tuple = HitContainer;

public:
  CAHitNtupletGeneratorOnGPU(const edm::ParameterSet& cfg, edm::ConsumesCollector&& iC)
      : CAHitNtupletGeneratorOnGPU(cfg, iC) {}
  CAHitNtupletGeneratorOnGPU(const edm::ParameterSet& cfg, edm::ConsumesCollector& iC);

  ~CAHitNtupletGeneratorOnGPU();

  static void fillDescriptions(edm::ParameterSetDescription& desc);
  static const char* fillDescriptionsLabel() { return "caHitNtupletOnGPU"; }

  PixelTrackHeterogeneous makeTuplesAsync(TrackingRecHit2DCUDA const& hits_d,
                                float bfield,
                                cuda::stream_t<>& stream) const;
 
private:

  void buildDoublets(HitsOnCPU const& hh, cuda::stream_t<>& stream) const;

  void hitNtuplets(HitsOnCPU const& hh,
                   const edm::EventSetup& es,
                   bool useRiemannFit,
                   cuda::stream_t<>& cudaStream);

  void cleanup(cudaStream_t stream);

  void launchKernels(HitsOnCPU const& hh, bool useRiemannFit, cuda::stream_t<>& cudaStream) const;


  CAHitNtupletGeneratorKernels::Params m_params;

  using Counters = CAHitNtupletGeneratorKernels::Counters;
  CAHitNtupletGeneratorKernels::Counters * m_counters = nullptr;

};

#endif  // RecoPixelVertexing_PixelTriplets_plugins_CAHitNtupletGeneratorOnGPU_h
