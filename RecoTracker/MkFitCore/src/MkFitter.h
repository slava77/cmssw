#ifndef RecoTracker_MkFitCore_src_MkFitter_h
#define RecoTracker_MkFitCore_src_MkFitter_h

#include "MkBase.h"

#include "RecoTracker/MkFitCore/interface/HitStructures.h"

namespace mkfit {

  class CandCloner;

  const int MPlexHitIdxMax = 16;
  using MPlexHitIdx = Matriplex::Matriplex<int, MPlexHitIdxMax, 1, NN>;
  using MPlexQHoT = Matriplex::Matriplex<HitOnTrack, 1, 1, NN>;

  class MkFitter : public MkBase {
  public:
    MPlexQF Chi2;

    MPlexHS msErr[Config::nMaxTrkHits];
    MPlexHV msPar[Config::nMaxTrkHits];

    MPlexQI Label;    //this is the seed index in global seed vector (for MC truth match)
    MPlexQI SeedIdx;  //this is the seed index in local thread (for bookkeeping at thread level)
    MPlexQI CandIdx;  //this is the candidate index for the given seed (for bookkeeping of clone engine)

    MPlexQHoT HoTArr[Config::nMaxTrkHits];

    // Hold hit indices to explore at current layer.
    MPlexQI XHitSize;
    MPlexHitIdx XHitArr;

    int Nhits;

  public:
    MkFitter() : Nhits(0) {}

    // Copy-in timing tests.
    MPlexLS& GetErr0() { return Err[0]; }
    MPlexLV& GetPar0() { return Par[0]; }

    void CheckAlignment();

    void PrintPt(int idx);

    void SetNhits(int newnhits) { Nhits = std::min(newnhits, Config::nMaxTrkHits - 1); }

    int countValidHits(int itrack, int end_hit) const;
    int countInvalidHits(int itrack, int end_hit) const;
    int countValidHits(int itrack) const { return countValidHits(itrack, Nhits); }
    int countInvalidHits(int itrack) const { return countInvalidHits(itrack, Nhits); }

    void InputTracksAndHits(const std::vector<Track>& tracks, const std::vector<HitVec>& layerHits, int beg, int end);
    void InputTracksAndHits(const std::vector<Track>& tracks,
                            const std::vector<LayerOfHits>& layerHits,
                            int beg,
                            int end);
    void SlurpInTracksAndHits(const std::vector<Track>& tracks, const std::vector<HitVec>& layerHits, int beg, int end);
    void InputTracksAndHitIdx(const std::vector<Track>& tracks, int beg, int end, bool inputProp);
    void InputTracksAndHitIdx(const std::vector<std::vector<Track> >& tracks,
                              const std::vector<std::pair<int, int> >& idxs,
                              int beg,
                              int end,
                              bool inputProp);
    void InputSeedsTracksAndHits(const std::vector<Track>& seeds,
                                 const std::vector<Track>& tracks,
                                 const std::vector<HitVec>& layerHits,
                                 int beg,
                                 int end);

    void InputTracksForFit(const std::vector<Track>& tracks, int beg, int end);
    void FitTracksWithInterSlurp(const std::vector<HitVec>& layersohits, int N_proc);

    void OutputTracks(std::vector<Track>& tracks, int beg, int end, int iCP) const;

    void OutputFittedTracks(std::vector<Track>& tracks, int beg, int end) const {
      return OutputTracks(tracks, beg, end, iC);
    }

    void OutputPropagatedTracks(std::vector<Track>& tracks, int beg, int end) const {
      return OutputTracks(tracks, beg, end, iP);
    }

    void OutputFittedTracksAndHitIdx(std::vector<Track>& tracks, int beg, int end, bool outputProp) const;
  };

}  // end namespace mkfit

#endif
