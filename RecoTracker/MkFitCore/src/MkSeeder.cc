#include "RecoTracker/MkFitCore/interface/MkSeeder.h"
#include "RecoTracker/MkFitCore/interface/SeedChain.h"
#include "RecoTracker/MkFitCore/interface/SensorGapMap.h"
#include "RecoTracker/MkFitCore/interface/TrackerInfo.h"
#include "RecoTracker/MkFitCore/src/SeedChainFinder.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <unordered_map>
#include <stdexcept>

namespace mkfit {

  MkSeeder::MkSeeder() {
    m_finder[0] = std::make_unique<SeedChainFinder>();
    m_finder[1] = std::make_unique<SeedChainFinder>();
  }

  MkSeeder::~MkSeeder() = default;

  void MkSeeder::setup(SeedChain &plus, SeedChain &minus) {
    m_finder[0]->setup(plus);
    m_finder[1]->setup(minus);
  }

  void MkSeeder::configure(const SeederConfig &cfg, const TrackerInfo &ti) {
    m_cfg = cfg;
    // the patterns, each followed by its mirror if it has +z discs; the mirror keeps the windows
    std::vector<SeederConfig::Pattern> pats;
    for (const SeederConfig::Pattern &p : cfg.patterns) {
      pats.push_back(p);
      SeederConfig::Pattern m = p;
      bool has = false;
      for (int &l : m.layers)
        if (l >= 16 && l <= 27)
          l += 22, has = true;
      if (has)
        pats.push_back(m);
    }
    m_patterns.clear();
    m_pat_index.clear();
    for (const SeederConfig::Pattern &p : pats) {
      m_pat_index.insert({p.layers, (int)m_patterns.size()});
      m_patterns.push_back(p.layers);
    }
    // the layers: the patterns', then every one the chain can visit; OT1-P and OT2-P for the OT2 check
    for (const SeederConfig::Pattern &p : pats)
      for (int l : p.layers)
        m_hits.add_layer(l, ti[l], ti[l].is_barrel() ? 2.0 : 1.0);
    std::set<int> have;
    for (const SeederConfig::Pattern &p : pats)
      for (int l : p.layers)
        have.insert(l);
    for (int l : {0, 1, 2, 3})
      have.insert(l);
    for (int l = 16; l <= 27; ++l)
      have.insert(l), have.insert(l + 22);
    for (int l : have)
      m_hits.add_layer(l, ti[l], ti[l].is_barrel() ? 2.0 : 1.0);
    if (cfg.fk_ot2 > 0)
      for (int l : {4, 6})
        m_hits.add_layer(l, ti[l], 2.0);
    m_hits.set_with_double(false);  // the batched finder is float
    // the envelopes and the gap map
    m_env = std::make_unique<SeedLayerEnvelopes>();
    m_env->setup(ti, have);
    m_env->delta = cfg.crossing_margin;
    m_gap.reset();
    if (cfg.gap_map_margin >= 0) {
      m_gap = std::make_unique<SensorGapMap>();
      m_gap->build(ti, cfg.gap_map_margin);
    }
    // the two chains: the parameters, the options, the window tables
    SeedingParams P;
    P.pt_min = cfg.pt_min, P.d0_max = cfg.d0_max, P.zv = cfg.zv, P.marg_b = cfg.marg_b;
    P.phi_c = cfg.phi_c, P.q_c = cfg.q_c, P.phi_d = cfg.phi_d, P.q_d = cfg.q_d;
    const double ws = cfg.win_scale;
    auto params_of = [&](const SeederConfig::Pattern &p) {
      SeedingParams Q = P;
      if (p.win[0] >= 0)
        Q.phi_c = p.win[0], Q.q_c = p.win[1], Q.phi_d = p.win[2], Q.q_d = p.win[3];
      Q.b_phi_d = p.bwin[0], Q.b_q_d = p.bwin[1];
      Q.s_ref = p.sref;
      for (const SeederConfig::EtaWin &e : p.eta_win) {
        SeedingParams::EtaWin w{e.lo, e.hi, e.aphi, e.bphi, e.aq, e.bq};
        w.aphi *= ws, w.bphi *= ws, w.aq *= ws, w.bq *= ws;
        Q.add_eta_win(w);
      }
      Q.phi_c *= ws, Q.q_c *= ws, Q.phi_d *= ws, Q.q_d *= ws;
      Q.b_phi_d *= ws, Q.b_q_d *= ws;
      return Q;
    };
    for (int sd = 0; sd < 2; ++sd) {
      m_chain[sd] = std::make_unique<SeedChain>();
      SeedChain &C = *m_chain[sd];
      C.P = P;
      C.P.phi_c *= ws, C.P.q_c *= ws, C.P.phi_d *= ws, C.P.q_d *= ws;
      C.max_holes = cfg.max_holes;
      C.hole_always = cfg.hole_always;
      C.max_holes_ot = cfg.max_holes_ot;
      C.known_only = !cfg.any_combination;
      C.start_holes = cfg.start_holes;
      C.lead_only = cfg.lead_only;
      C.inner_ot_only = cfg.inner_ot_only;
      C.start_gap = cfg.start_gap;
      C.gap_map = m_gap.get();
      C.setup(*m_env, sd == 0 ? 1 : -1, have);
    }
    for (const SeederConfig::Pattern &p : pats) {
      const SeedingParams Q = params_of(p);
      for (auto &chain : m_chain) {
        SeedChain &C = *chain;
        auto &wc = C.win_c[{p.layers[0], p.layers[1], p.layers[2]}];
        wc.first = std::max(wc.first, (float)Q.phi_c), wc.second = std::max(wc.second, (float)Q.q_c);
        C.win_d[p.layers] = {(float)Q.phi_d,
                             (float)Q.b_phi_d,
                             (float)Q.q_d,
                             (float)Q.b_q_d,
                             Q.s_ref,
                             std::vector<SeedingParams::EtaWin>(Q.eta_win, Q.eta_win + Q.n_eta_win),
                             (float)Q.q_c};
      }
    }
    // the finders, and their fake cuts
    setup(*m_chain[0], *m_chain[1]);
    for (auto &f : m_finder) {
      SeedChainFinder &B = *f;
      B.d_mode = cfg.d_mode;
      B.fk_score = cfg.fk_score, B.fk_shape = cfg.fk_shape, B.fk_ot2 = cfg.fk_ot2;
      B.fk_score_fwd = cfg.fk_score_fwd, B.fk_cot_fwd = std::sinh(cfg.fk_eta_fwd);
      B.trip_on = cfg.triplet_seeds, B.trip_lo = cfg.trip_cot_lo, B.trip_hi = cfg.trip_cot_hi;
      B.ot2_aphi = cfg.ot2_win[0], B.ot2_bphi = cfg.ot2_win[1], B.ot2_aq = cfg.ot2_win[2], B.ot2_bq = cfg.ot2_win[3];
      B.ot2_phimin = cfg.ot2_phimin;
      for (const SeederConfig::ShapeWin &w : cfg.shape_win) {
        if (w.layer < 0 || w.layer > 3 || w.bin_width <= 0 || w.lo.empty() || w.lo.size() != w.hi.size())
          throw std::invalid_argument("MkSeeder::configure: bad shape_win for layer " + std::to_string(w.layer));
        SeedChainFinder::ShapeTab &T = B.shape_[w.layer];
        T.inv_bw = 1.0f / w.bin_width;
        T.lo = w.lo, T.hi = w.hi;
      }
    }
  }

  void MkSeeder::seed(const SeedHitSource &src, const BeamSpot &bs, std::vector<SeederQuad> &out, SeedCounters &cnt) {
    out.clear();
    fill(src, bs);
    std::vector<std::pair<std::array<int, 4>, SeedQuad>> cq;
    std::vector<float> sc, fk;
    find(cq, cnt, &sc, &fk);
    // by pattern, in the configured order; a combination no pattern lists (only with any_combination) after
    // them, in the order first found in this event; within a pattern as found
    const int np = m_patterns.size();
    std::map<std::array<int, 4>, int> extra;
    std::vector<int> pi(cq.size());
    for (size_t i = 0; i < cq.size(); ++i) {
      auto it = m_pat_index.find(cq[i].first);
      if (it != m_pat_index.end())
        pi[i] = it->second;
      else
        pi[i] = np + extra.insert({cq[i].first, (int)extra.size()}).first->second;
    }
    std::vector<int> order(cq.size());
    for (size_t i = 0; i < cq.size(); ++i)
      order[i] = i;
    std::stable_sort(order.begin(), order.end(), [&](int a, int b) { return pi[a] < pi[b]; });
    std::vector<std::array<int, 4>> ll(cq.size());
    std::vector<SeedQuad> qq(cq.size());
    std::vector<float> ss(cq.size());
    for (size_t i = 0; i < cq.size(); ++i)
      ll[i] = cq[order[i]].first, qq[i] = cq[order[i]].second, ss[i] = sc[order[i]];
    std::vector<char> keep(cq.size(), 1);
    std::vector<int> amb(cq.size(), 0);
    // triplet seeds (layers[3] < 0) are cleaned after the quads: clean() works on quads only
    std::vector<int> iq, it;
    for (size_t i = 0; i < cq.size(); ++i)
      if (ll[i][3] >= 0)
        iq.push_back(i);
      else if (m_cfg.trip_max_score <= 0.f || ss[i] <= m_cfg.trip_max_score)
        it.push_back(i);
      else
        keep[i] = 0;
    if (m_cfg.dedup > 0) {
      if (iq.size() == cq.size())
        clean(src, ll, qq, ss, m_cfg.dedup, keep, &amb);
      else {
        std::vector<std::array<int, 4>> lq(iq.size());
        std::vector<SeedQuad> hq(iq.size());
        std::vector<float> sq(iq.size());
        for (size_t j = 0; j < iq.size(); ++j)
          lq[j] = ll[iq[j]], hq[j] = qq[iq[j]], sq[j] = ss[iq[j]];
        std::vector<char> kq;
        std::vector<int> aq;
        clean(src, lq, hq, sq, m_cfg.dedup, kq, &aq);
        for (size_t j = 0; j < iq.size(); ++j)
          keep[iq[j]] = kq[j], amb[iq[j]] = aq[j];
      }
    }
    if (!it.empty()) {
      // a triplet seed is dropped if >= 2 of its hits are used by a kept quad or a better kept triplet
      std::unordered_map<uint64_t, int> used;
      auto hkey = [](int l, unsigned int h) { return (uint64_t(uint32_t(l)) << 32) | h; };
      for (int i : iq)
        if (keep[i])
          for (int k = 0; k < 4; ++k)
            used[hkey(ll[i][k], qq[i][k])] = 1;
      std::stable_sort(it.begin(), it.end(), [&](int a, int b) { return ss[a] < ss[b]; });
      for (int i : it) {
        int ns = 0;
        for (int k = 0; k < 3; ++k)
          ns += used.count(hkey(ll[i][k], qq[i][k]));
        if (ns >= 2) {
          keep[i] = 0;
          continue;
        }
        for (int k = 0; k < 3; ++k)
          used[hkey(ll[i][k], qq[i][k])] = 1;
      }
    }
    if (m_cfg.dedup_helix)
      dedup_helix(src, ll, qq, ss, keep);
    for (size_t i = 0; i < cq.size(); ++i)
      if (keep[i])
        out.push_back({ll[i], qq[i], ss[i], fk[order[i]], amb[i]});
  }

  void MkSeeder::fill(const std::vector<HitVec> &layer_hits, const BeamSpot &bs) { m_hits.fill(layer_hits, bs); }

  void MkSeeder::fill(const SeedHitSource &src, const BeamSpot &bs) { m_hits.fill(src, bs); }

  void MkSeeder::find(std::vector<std::pair<std::array<int, 4>, SeedQuad>> &out,
                      SeedCounters &cnt,
                      std::vector<float> *scores,
                      std::vector<float> *fake_scores) {
    SeedChainFinder::run_both(*m_finder[0], *m_finder[1], m_hits.layer_map(), out, cnt, scores, fake_scores);
  }

  void MkSeeder::clean(const std::vector<HitVec> &layer_hits,
                       const std::vector<std::array<int, 4>> &layers,
                       const std::vector<SeedQuad> &quads,
                       const std::vector<float> &scores,
                       int min_shared,
                       std::vector<char> &keep,
                       std::vector<int> *n_dropped) {
    clean(seed_hit_source(layer_hits), layers, quads, scores, min_shared, keep, n_dropped);
  }

  void MkSeeder::clean(const SeedHitSource &src,
                       const std::vector<std::array<int, 4>> &layers,
                       const std::vector<SeedQuad> &quads,
                       const std::vector<float> &scores,
                       int min_shared,
                       std::vector<char> &keep,
                       std::vector<int> *n_dropped) {
    const int nc = quads.size();
    keep.assign(nc, 1);
    if (n_dropped)
      n_dropped->assign(nc, 0);
    // the order: tier first (a pattern with an outer-tracker layer after every pure-pixel one: its
    // windows are several times wider, so its scores are not comparable), then the score, packed in
    // one 32-bit key for the (stable) radix sort: the tier in the top 3 bits, then the score's float
    // bits without their 3 lowest (the score is >= 0, so its bits sort as the value; relative 1e-6)
    std::vector<unsigned int> &key = m_cl_key, &rank = m_cl_rank;
    key.resize(nc);
    for (int i = 0; i < nc; ++i) {
      int t = 0;
      for (int l : layers[i])
        t += !SeedLayerEnvelopes::is_pix(l);
      const float sf = scores[i];
      unsigned int sb;
      __builtin_memcpy(&sb, &sf, 4);
      if (m_cfg.dedup_inner) {
        // the first layer's rank along the chain: barrel 0-3, +z discs 16-27, -z discs 38-49 (mirrored)
        const int l0 = layers[i][0];
        const unsigned int rk = (unsigned int)std::min(31, l0 < 16 ? l0 : (l0 < 38 ? l0 - 12 : l0 - 34));
        key[i] = (unsigned int)t << 29 | rk << 24 | sb >> 8;
      } else
        key[i] = (unsigned int)t << 29 | sb >> 3;
    }
    m_cl_sort.sort(key, rank);
    // each quad's hits as sorted global indices: the layer's offset + the hit's position within the layer.
    // Which quads are kept does not depend on the numbering, but which kept quad a dropped one is charged
    // to (n_dropped) does, through ties; so a layer given by index (SeedLayerHits::idx) is numbered by
    // position too, as if it had a HitVec of its own. m_cl_pos: the position of each external index.
    const int nl = src.size();
    std::vector<unsigned int> off(nl + 1, 0);
    for (int l = 0; l < nl; ++l)
      off[l + 1] = off[l] + src[l].n;
    std::vector<const unsigned int *> pos_of(nl, nullptr);
    {
      std::vector<std::pair<const HitVec *, size_t>> base;  // per shared HitVec: its start in m_cl_pos
      size_t total = 0;
      for (int l = 0; l < nl; ++l)
        if (src[l].idx) {
          bool found = false;
          for (const auto &b : base)
            found |= b.first == src[l].hits;
          if (!found)
            base.push_back({src[l].hits, total}), total += src[l].hits->size();
        }
      m_cl_pos.resize(total);
      for (int l = 0; l < nl; ++l)
        if (src[l].idx) {
          size_t b0 = 0;
          for (const auto &b : base)
            if (b.first == src[l].hits)
              b0 = b.second;
          unsigned int *p = m_cl_pos.data() + b0;
          for (unsigned int k = 0; k < src[l].n; ++k)
            p[src[l].idx[k]] = k;
          pos_of[l] = p;
        }
    }
    std::vector<std::array<unsigned int, 4>> &gh = m_cl_gh;
    gh.resize(nc);
    for (int i = 0; i < nc; ++i) {
      const auto &ll = layers[i];
      for (int k = 0; k < 4; ++k)
        gh[i][k] = off[ll[k]] + (pos_of[ll[k]] ? pos_of[ll[k]][quads[i][k]] : quads[i][k]);
      std::sort(gh[i].begin(), gh[i].end());
    }
    std::vector<CleanHL> &hl = m_cl_hl;
    if (hl.size() < off[nl])
      hl.resize(off[nl]);
    std::vector<std::pair<int, int>> &link = m_cl_link;
    link.clear();
    // a kept quad sharing >= N of the 4 hits holds at least one of any 5 - N of them: walk the
    // 5 - N shortest lists only, and count the shared hits of each quad found there directly
    const int nwalk = std::clamp(5 - min_shared, 1, 4);
    for (int ik = 0; ik < nc; ++ik) {
      // the order is by score, so every access below is random: prefetch the hit lists 8 quads ahead
      // and their hit indices 16 ahead (the loop is latency-bound, ~135 ns per quad without)
      if (ik + 16 < nc)
        __builtin_prefetch(&gh[rank[ik + 16]]);
      if (ik + 8 < nc)
        for (unsigned int gk : gh[rank[ik + 8]])
          __builtin_prefetch(&hl[gk]);
      const int i = rank[ik];
      const auto &g = gh[i];
      // the positions by list length: rank each (ties by position), no branches
      const int ln[4] = {hl[g[0]].len, hl[g[1]].len, hl[g[2]].len, hl[g[3]].len};
      int pos[4];
      for (int a = 0; a < 4; ++a) {
        int r = 0;
        for (int b = 0; b < 4; ++b)
          r += ln[b] < ln[a] || (ln[b] == ln[a] && b < a);
        pos[r] = a;
      }
      bool drop = false;
      for (int w = 0; w < nwalk && !drop; ++w)
        for (int e = hl[g[pos[w]]].head; e >= 0; e = link[e].second) {
          const auto &h = gh[link[e].first];
          // shared hits: all 16 pairs, no branches (the hits of one quad are distinct)
          int ns = 0;
          for (int a = 0; a < 4; ++a)
            for (int b = 0; b < 4; ++b)
              ns += g[a] == h[b];
          if (ns >= min_shared) {
            drop = true;
            if (n_dropped)
              ++(*n_dropped)[link[e].first];
            break;
          }
        }
      if (drop) {
        keep[i] = 0;
        continue;
      }
      for (int k = 0; k < 4; ++k) {
        CleanHL &h = hl[g[k]];
        link.push_back({i, h.head});
        h.head = link.size() - 1;
        ++h.len;
      }
    }
    // back to empty, for the next event
    for (const auto &e : link)
      for (unsigned int gk : gh[e.first])
        hl[gk] = CleanHL();
  }


  void MkSeeder::dedup_helix(const SeedHitSource &src,
                             const std::vector<std::array<int, 4>> &ll,
                             const std::vector<SeedQuad> &qq,
                             const std::vector<float> &ss,
                             std::vector<char> &keep) const {
    // the helix of a seed from its first, second and last hit: phi0 (direction at the beam line), cot(theta),
    // signed curvature, z0
    struct HP {
      float phi0, cot, eta, kappa, z0;
    };
    const int nc = ll.size();
    std::vector<HP> hp(nc);
    std::vector<char> ok(nc, 0);
    std::vector<int> ord;
    for (int i = 0; i < nc; ++i) {
      if (!keep[i])
        continue;
      const int nh = ll[i][3] < 0 ? 3 : 4;
      const Hit &a = (*src[ll[i][0]].hits)[qq[i][0]], &b = (*src[ll[i][1]].hits)[qq[i][1]],
                &d = (*src[ll[i][nh - 1]].hits)[qq[i][nh - 1]];
      const float abx = b.x() - a.x(), aby = b.y() - a.y(), bdx = d.x() - b.x(), bdy = d.y() - b.y();
      const float adx = d.x() - a.x(), ady = d.y() - a.y();
      const float lab = std::hypot(abx, aby), lbd = std::hypot(bdx, bdy), lad = std::hypot(adx, ady);
      if (!(lab * lbd * lad > 0.f))
        continue;
      const float k = 2.f * (abx * bdy - aby * bdx) / (lab * lbd * lad);
      // tangent at hit a: the a-b chord turned back by half its turning angle
      const float ta = std::atan2(aby, abx) - 0.5f * k * lab;
      const float ra = std::hypot(a.x(), a.y());
      const float cot = (d.z() - a.z()) / lad;
      hp[i] = {ta - k * ra, cot, std::asinh(cot), k, a.z() - cot * ra};
      ok[i] = 1;
      ord.push_back(i);
    }
    // the cleaning order: the outer-tracker tier, the first layer's rank, then the score
    auto rank = [&](int i) {
      int t = 0;
      for (int l : ll[i])
        t += l >= 0 && !SeedLayerEnvelopes::is_pix(l);
      const int l0 = ll[i][0];
      const int rk = l0 < 16 ? l0 : (l0 < 38 ? l0 - 12 : l0 - 34);
      return t * 64 + rk;
    };
    std::stable_sort(ord.begin(), ord.end(), [&](int x, int y) {
      const int rx = rank(x), ry = rank(y);
      return rx != ry ? rx < ry : ss[x] < ss[y];
    });
    constexpr float kEtaBin = 0.02f, kPhiBin = 0.01f, kPi = 3.14159265f;
    const int nPhi = int(2.f * kPi / kPhiBin) + 1;
    auto key = [&](int ie, int ip) { return (long long)ie * 100000 + ((ip % nPhi) + nPhi) % nPhi; };
    std::unordered_map<long long, std::vector<int>> grid;
    for (int i : ord) {
      const HP &h = hp[i];
      const int ie = int(std::floor(h.eta / kEtaBin)), ip = int(std::floor((h.phi0 + kPi) / kPhiBin));
      bool dup = false;
      for (int de = -1; de <= 1 && !dup; ++de)
        for (int dp = -1; dp <= 1 && !dup; ++dp) {
          auto it = grid.find(key(ie + de, ip + dp));
          if (it == grid.end())
            continue;
          for (int j : it->second) {
            const HP &g = hp[j];
            float dphi = std::abs(h.phi0 - g.phi0);
            dphi = std::min(dphi, 2.f * kPi - dphi);
            if (dphi < 0.01f && std::abs(h.cot - g.cot) < 0.02f * std::abs(g.cot) + 0.01f &&
                std::abs(h.kappa - g.kappa) < 0.1f * std::abs(g.kappa) + 2e-4f && std::abs(h.z0 - g.z0) < 0.3f) {
              dup = true;
              break;
            }
          }
        }
      if (dup)
        keep[i] = 0;
      else
        grid[key(ie, ip)].push_back(i);
    }
  }

}  // namespace mkfit
