#ifndef RecoTracker_MkFitCore_src_CandCloner_h
#define RecoTracker_MkFitCore_src_CandCloner_h

#include "MkFinder.h"

#include <vector>

namespace mkfit {

  class IterationParams;
  class EventOfCombCandidates;

  //#define CC_TIME_LAYER
  //#define CC_TIME_ETA

  typedef std::pair<int, int> CandClonerWork_t;

  class CandCloner {
  public:
    // Maximum number of seeds processed in one call to processSeedRange()
    static const int s_max_seed_range = MPT_SIZE;

    CandCloner() { t_cands_for_next_lay.resize(s_max_seed_range); }

    void setup(const IterationParams &ip);
    void release();

    ~CandCloner() {}

    void begin_eta_bin(EventOfCombCandidates *e_o_ccs,
                       std::vector<std::pair<int, int>> *update_list,
                       std::vector<std::vector<TrackCand>> *extra_cands,
                       int start_seed,
                       int n_seeds) {
      mp_event_of_comb_candidates = e_o_ccs;
      mp_kalman_update_list = update_list;
      mp_extra_cands = extra_cands;
      m_start_seed = start_seed;
      m_n_seeds = n_seeds;
      m_hits_to_add.resize(n_seeds);

      // XXX Should resize vectors in m_hits_to_add to whatever makes sense ???
      // for (int i = 0; i < n_seeds; ++i)
      // {
      //   m_hits_to_add[i].reserve(20 * Config::maxCandsPerSeed);
      // }

#ifdef CC_TIME_ETA
      printf("CandCloner::begin_eta_bin\n");
      t_eta = dtime();
#endif
    }

    void begin_layer(int lay) {
      m_layer = lay;

      m_idx_max = 0;
      m_idx_max_prev = 0;

      mp_kalman_update_list->clear();

#ifdef CC_TIME_LAYER
      t_lay = dtime();
#endif
    }

    void begin_iteration() {
      // Do nothing, "secondary" state vars updated when work completed/assigned.
    }

    void add_cand(int idx, const IdxChi2List &cand_info) {
      m_hits_to_add[idx].push_back(cand_info);

      m_idx_max = std::max(m_idx_max, idx);
    }

    int num_cands(int idx) { return m_hits_to_add[idx].size(); }

    void end_iteration() {
      int proc_n = m_idx_max - m_idx_max_prev;

      // printf("CandCloner::end_iteration process %d, max_prev=%d, max=%d\n", proc_n, m_idx_max_prev, m_idx_max);

      if (proc_n >= s_max_seed_range) {
        // Round to multiple of s_max_seed_range.
        doWork((m_idx_max / s_max_seed_range) * s_max_seed_range);
      }
    }

    void end_layer() {
      if (m_n_seeds > m_idx_max_prev) {
        doWork(m_n_seeds);
      }

      for (int i = 0; i < m_n_seeds; ++i) {
        m_hits_to_add[i].clear();
      }

#ifdef CC_TIME_LAYER
      t_lay = dtime() - t_lay;
      printf("CandCloner::end_layer %d -- t_lay=%8.6f\n", m_layer, t_lay);
      printf("                      m_idx_max=%d, m_idx_max_prev=%d, issued work=%d\n",
             m_idx_max,
             m_idx_max_prev,
             m_idx_max + 1 > m_idx_max_prev);
#endif
    }

    void end_eta_bin() {
#ifdef CC_TIME_ETA
      t_eta = dtime() - t_eta;
      printf("CandCloner::end_eta_bin t_eta=%8.6f\n", t_eta);
#endif
    }

    void doWork(int idx) {
      // printf("CandCloner::DoWork assigning work from seed %d to %d\n", m_idx_max_prev, idx);

      int beg = m_idx_max_prev;
      int the_end = idx;

      // printf("CandCloner::DoWork working on beg=%d to the_end=%d\n", beg, the_end);

      while (beg != the_end) {
        int end = std::min(beg + s_max_seed_range, the_end);

        // printf("CandCloner::DoWork processing %4d -> %4d\n", beg, end);

        processSeedRange(beg, end);

        beg = end;
      }

      m_idx_max_prev = idx;
    }

    // ----------------------------------------------------------------

    void processSeedRange(int is_beg, int is_end);

    // ----------------------------------------------------------------

    // eventually, protected or private

    int m_idx_max, m_idx_max_prev;
    std::vector<std::vector<IdxChi2List>> m_hits_to_add;

    const IterationParams *mp_iteration_params = nullptr;
    EventOfCombCandidates *mp_event_of_comb_candidates;
    std::vector<std::pair<int, int>> *mp_kalman_update_list;
    std::vector<std::vector<TrackCand>> *mp_extra_cands;

#if defined(CC_TIME_ETA) or defined(CC_TIME_LAYER)
    double t_eta, t_lay;
#endif

    int m_start_seed, m_n_seeds;
    int m_layer;

  private:
    // Temporaries in processSeedRange(), resized/reserved  in constructor.

    // Size of this one is s_max_seed_range
    std::vector<std::vector<TrackCand>> t_cands_for_next_lay;

  };

}  // end namespace mkfit
#endif
