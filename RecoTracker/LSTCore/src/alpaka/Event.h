#ifndef RecoTracker_LSTCore_src_alpaka_Event_h
#define RecoTracker_LSTCore_src_alpaka_Event_h

#include "RecoTracker/LSTCore/interface/alpaka/Constants.h"
#include "RecoTracker/LSTCore/interface/Module.h"
#include "RecoTracker/LSTCore/interface/LST.h"

#include "Hit.h"
#include "Segment.h"
#include "Triplet.h"
#include "Kernels.h"
#include "Quintuplet.h"
#include "MiniDoublet.h"
#include "PixelQuintuplet.h"
#include "PixelTriplet.h"
#include "TrackCandidate.h"

#include "HeterogeneousCore/AlpakaInterface/interface/host.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  namespace lst {

    class Event {
    private:
      Queue queue;
      Device devAcc;
      DevHost devHost;
      bool addObjects;

      std::array<unsigned int, 6> n_hits_by_layer_barrel_;
      std::array<unsigned int, 5> n_hits_by_layer_endcap_;
      std::array<unsigned int, 6> n_minidoublets_by_layer_barrel_;
      std::array<unsigned int, 5> n_minidoublets_by_layer_endcap_;
      std::array<unsigned int, 6> n_segments_by_layer_barrel_;
      std::array<unsigned int, 5> n_segments_by_layer_endcap_;
      std::array<unsigned int, 6> n_triplets_by_layer_barrel_;
      std::array<unsigned int, 5> n_triplets_by_layer_endcap_;
      std::array<unsigned int, 6> n_trackCandidates_by_layer_barrel_;
      std::array<unsigned int, 5> n_trackCandidates_by_layer_endcap_;
      std::array<unsigned int, 6> n_quintuplets_by_layer_barrel_;
      std::array<unsigned int, 5> n_quintuplets_by_layer_endcap_;
      unsigned int nTotalSegments_;

      //Device stuff
      ::lst::ObjectRanges* rangesInGPU;
      ::lst::ObjectRangesBuffer<Device>* rangesBuffers;
      ::lst::Hits* hitsInGPU;
      ::lst::HitsBuffer<Device>* hitsBuffers;
      ::lst::MiniDoublets* mdsInGPU;
      ::lst::MiniDoubletsBuffer<Device>* miniDoubletsBuffers;
      ::lst::Segments* segmentsInGPU;
      ::lst::SegmentsBuffer<Device>* segmentsBuffers;
      ::lst::Triplets* tripletsInGPU;
      ::lst::TripletsBuffer<Device>* tripletsBuffers;
      ::lst::Quintuplets* quintupletsInGPU;
      ::lst::QuintupletsBuffer<Device>* quintupletsBuffers;
      ::lst::TrackCandidates* trackCandidatesInGPU;
      ::lst::TrackCandidatesBuffer<Device>* trackCandidatesBuffers;
      ::lst::PixelTriplets* pixelTripletsInGPU;
      ::lst::PixelTripletsBuffer<Device>* pixelTripletsBuffers;
      ::lst::PixelQuintuplets* pixelQuintupletsInGPU;
      ::lst::PixelQuintupletsBuffer<Device>* pixelQuintupletsBuffers;

      //CPU interface stuff
      ::lst::ObjectRangesBuffer<DevHost>* rangesInCPU;
      ::lst::HitsBuffer<DevHost>* hitsInCPU;
      ::lst::MiniDoubletsBuffer<DevHost>* mdsInCPU;
      ::lst::SegmentsBuffer<DevHost>* segmentsInCPU;
      ::lst::TripletsBuffer<DevHost>* tripletsInCPU;
      ::lst::TrackCandidatesBuffer<DevHost>* trackCandidatesInCPU;
      ::lst::ModulesBuffer<DevHost>* modulesInCPU;
      ::lst::QuintupletsBuffer<DevHost>* quintupletsInCPU;
      ::lst::PixelTripletsBuffer<DevHost>* pixelTripletsInCPU;
      ::lst::PixelQuintupletsBuffer<DevHost>* pixelQuintupletsInCPU;

      void initSync(bool verbose);

      int* superbinCPU;
      int8_t* pixelTypeCPU;

      const uint16_t nModules_;
      const uint16_t nLowerModules_;
      const unsigned int nPixels_;
      const unsigned int nEndCapMap_;
      ::lst::ModulesBuffer<Device> const& modulesBuffers_;
      ::lst::PixelMap const& pixelMapping_;
      ::lst::EndcapGeometryBuffer<Device> const& endcapGeometryBuffers_;

    public:
      // Constructor used for CMSSW integration. Uses an external queue.
      Event(bool verbose, Queue const& q, const ::lst::LSTESData<Device>* deviceESData)
          : queue(q),
            devAcc(alpaka::getDev(q)),
            devHost(cms::alpakatools::host()),
            nModules_(deviceESData->nModules),
            nLowerModules_(deviceESData->nLowerModules),
            nPixels_(deviceESData->nPixels),
            nEndCapMap_(deviceESData->nEndCapMap),
            modulesBuffers_(deviceESData->modulesBuffers),
            pixelMapping_(*deviceESData->pixelMapping),
            endcapGeometryBuffers_(deviceESData->endcapGeometryBuffers) {
        initSync(verbose);
      }
      void resetEventSync();  // synchronizes
      void wait() const { alpaka::wait(queue); }

      // Calls the appropriate hit function, then increments the counter
      void addHitToEvent(std::vector<float> const& x,
                         std::vector<float> const& y,
                         std::vector<float> const& z,
                         std::vector<unsigned int> const& detId,
                         std::vector<unsigned int> const& idxInNtuple);
      void addPixelSegmentToEvent(std::vector<unsigned int> const& hitIndices0,
                                  std::vector<unsigned int> const& hitIndices1,
                                  std::vector<unsigned int> const& hitIndices2,
                                  std::vector<unsigned int> const& hitIndices3,
                                  std::vector<float> const& dPhiChange,
                                  std::vector<float> const& ptIn,
                                  std::vector<float> const& ptErr,
                                  std::vector<float> const& px,
                                  std::vector<float> const& py,
                                  std::vector<float> const& pz,
                                  std::vector<float> const& eta,
                                  std::vector<float> const& etaErr,
                                  std::vector<float> const& phi,
                                  std::vector<int> const& charge,
                                  std::vector<unsigned int> const& seedIdx,
                                  std::vector<int> const& superbin,
                                  std::vector<int8_t> const& pixelType,
                                  std::vector<char> const& isQuad);

      void createMiniDoublets();
      void createSegmentsWithModuleMap();
      void createTriplets();
      void createTrackCandidates(bool no_pls_dupclean, bool tc_pls_triplets);
      void createPixelTriplets();
      void createQuintuplets();
      void pixelLineSegmentCleaning(bool no_pls_dupclean);
      void createPixelQuintuplets();

      // functions that map the objects to the appropriate modules
      void addMiniDoubletsToEventExplicit();
      void addSegmentsToEventExplicit();
      void addQuintupletsToEventExplicit();
      void addTripletsToEventExplicit();
      void resetObjectsInModule();

      unsigned int getNumberOfHits();
      unsigned int getNumberOfHitsByLayer(unsigned int layer);
      unsigned int getNumberOfHitsByLayerBarrel(unsigned int layer);
      unsigned int getNumberOfHitsByLayerEndcap(unsigned int layer);

      unsigned int getNumberOfMiniDoublets();
      unsigned int getNumberOfMiniDoubletsByLayer(unsigned int layer);
      unsigned int getNumberOfMiniDoubletsByLayerBarrel(unsigned int layer);
      unsigned int getNumberOfMiniDoubletsByLayerEndcap(unsigned int layer);

      unsigned int getNumberOfSegments();
      unsigned int getNumberOfSegmentsByLayer(unsigned int layer);
      unsigned int getNumberOfSegmentsByLayerBarrel(unsigned int layer);
      unsigned int getNumberOfSegmentsByLayerEndcap(unsigned int layer);

      unsigned int getNumberOfTriplets();
      unsigned int getNumberOfTripletsByLayer(unsigned int layer);
      unsigned int getNumberOfTripletsByLayerBarrel(unsigned int layer);
      unsigned int getNumberOfTripletsByLayerEndcap(unsigned int layer);

      int getNumberOfPixelTriplets();
      int getNumberOfPixelQuintuplets();

      unsigned int getNumberOfQuintuplets();
      unsigned int getNumberOfQuintupletsByLayer(unsigned int layer);
      unsigned int getNumberOfQuintupletsByLayerBarrel(unsigned int layer);
      unsigned int getNumberOfQuintupletsByLayerEndcap(unsigned int layer);

      int getNumberOfTrackCandidates();
      int getNumberOfPT5TrackCandidates();
      int getNumberOfPT3TrackCandidates();
      int getNumberOfPLSTrackCandidates();
      int getNumberOfPixelTrackCandidates();
      int getNumberOfT5TrackCandidates();

      // sync adds alpaka::wait at the end of filling a buffer during lazy fill
      // (has no effect on repeated calls)
      // set to false may allow faster operation with concurrent calls of get*
      // HANDLE WITH CARE
      ::lst::HitsBuffer<DevHost>* getHits(bool sync = true);
      ::lst::HitsBuffer<DevHost>* getHitsInCMSSW(bool sync = true);
      ::lst::ObjectRangesBuffer<DevHost>* getRanges(bool sync = true);
      ::lst::MiniDoubletsBuffer<DevHost>* getMiniDoublets(bool sync = true);
      ::lst::SegmentsBuffer<DevHost>* getSegments(bool sync = true);
      ::lst::TripletsBuffer<DevHost>* getTriplets(bool sync = true);
      ::lst::QuintupletsBuffer<DevHost>* getQuintuplets(bool sync = true);
      ::lst::PixelTripletsBuffer<DevHost>* getPixelTriplets(bool sync = true);
      ::lst::PixelQuintupletsBuffer<DevHost>* getPixelQuintuplets(bool sync = true);
      ::lst::TrackCandidatesBuffer<DevHost>* getTrackCandidates(bool sync = true);
      ::lst::TrackCandidatesBuffer<DevHost>* getTrackCandidatesInCMSSW(bool sync = true);
      ::lst::ModulesBuffer<DevHost>* getModules(bool isFull = false, bool sync = true);
    };

  }  // namespace lst

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE
#endif
