#ifndef RecoTracker_MkSeederAlpaka_plugins_alpaka_MkSeederKernels_h
#define RecoTracker_MkSeederAlpaka_plugins_alpaka_MkSeederKernels_h

// The kernels of the mkFit seeder's chain finder, one z side at a time (see MkSeederDeviceTypes.h):
//   KernelStart   stage b: one work item per (start pair, a hit), the doublets that pass the start tests,
//                 routed onto the Q2 queue of their next crossed position (SeedChainFinder::start_doublets
//                 and flush_start);
//   KernelStageC  stage c at position p: one item per Q2[p] candidate, triplets onto Q3, misses onto Q2;
//   KernelStageD  stage d at position p: one item per Q3[p] candidate, quads and triplet seeds out, misses
//                 onto Q3 (SeedChainFinder::forward).
// Positions run in chain order (a candidate only moves to later positions), so every producer of a queue has
// run before the queue is consumed. The arithmetic follows the CPU finder expression by expression.

#include <alpaka/alpaka.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

#include "MkSeederDeviceTypes.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE::mkseeder {

  using namespace mkseederdev;

  constexpr float kPi = 3.14159265358979323846f;
  constexpr float k2Pi = 6.28318530717959f, kInv2Pi = 0.159154943091895f;
  constexpr double kTwoPiD = 2 * 3.14159265358979323846;
  constexpr float kConstPi = 3.14159265358979323846f;  // mkfit::Const::PI

  // seedchain::wrap
  ALPAKA_FN_HOST_ACC inline float wrapc(float d) { return d - k2Pi * std::floor(d * kInv2Pi + 0.5f); }
  // SeedLayerOfHits::wrap
  ALPAKA_FN_HOST_ACC inline float wrapl(float p) {
    if (p > kConstPi)
      p -= 2 * kConstPi;
    else if (p < -kConstPi)
      p += 2 * kConstPi;
    return p;
  }

  struct IPair {
    uint32_t b, e;
  };

  ALPAKA_FN_HOST_ACC inline IPair phi_range(const Layer &L, float lo, float hi) {
    lo = wrapl(lo), hi = wrapl(hi);
    const uint32_t m = Layer::kNPhi - 1;
    const uint32_t b = ((uint32_t)(uint16_t)std::floor((lo - L.phi_rmin) * L.phi_fac)) & m;
    const uint32_t e = ((uint32_t)(uint16_t)std::floor((hi - L.phi_rmin) * L.phi_fac) + 1u) & m;
    return {b, e};
  }
  ALPAKA_FN_HOST_ACC inline uint32_t q_bin_safe(const Layer &L, float r) {
    return r <= L.q_rmin ? 0u : (r >= L.q_lbhp ? (uint32_t)L.q_last : (uint32_t)(uint16_t)std::floor((r - L.q_rmin) * L.q_fac));
  }
  ALPAKA_FN_HOST_ACC inline IPair q_range(const Layer &L, float lo, float hi) {
    return {q_bin_safe(L, lo), q_bin_safe(L, hi) + 1u};
  }
  ALPAKA_FN_HOST_ACC inline IPair phi_bins(const Layer &L, float c, float w) {
    w = std::min(w, 0.9f * kPi);
    return phi_range(L, c - w, c + w);
  }
  ALPAKA_FN_HOST_ACC inline IPair q_bins(const Layer &L, float lo, float hi) {
    return q_range(L, lo - 1e-4f, hi + 1e-4f);
  }
  ALPAKA_FN_HOST_ACC inline IPair phi_bins_d(const Layer &L, double c, double w) {
    w = std::min(w, 0.9 * kTwoPiD / 2);
    return phi_range(L, (float)(c - w), (float)(c + w));
  }
  ALPAKA_FN_HOST_ACC inline IPair q_bins_d(const Layer &L, double lo, double hi) {
    return q_range(L, (float)(lo - 1e-4), (float)(hi + 1e-4));
  }

  // SeedLayerOfHits::for_each_run: f(begin, end) over the layer's own hit indices
  template <typename F>
  ALPAKA_FN_HOST_ACC inline void for_each_run(const View &v, const Layer &L, IPair p, IPair q, F &&f) {
    const uint32_t np = Layer::kNPhi;
    const uint32_t *st = v.bin_start + L.start0;
    for (uint32_t qi = q.b; qi < q.e; ++qi) {
      if (p.b < p.e || p.e == 0) {
        f(st[qi * np + p.b], st[qi * np + (p.e == 0 ? np : p.e)]);
      } else if (p.b > p.e) {
        f(st[qi * np + p.b], st[qi * np + np]);
        f(st[qi * np + 0], st[qi * np + p.e]);
      }
    }
  }

  // SeedChainFinder::cls / states() for one line
  ALPAKA_FN_HOST_ACC inline int cls(const Env &e, float x1, float x2) {
    const float xl = std::min(x1, x2), xh = std::max(x1, x2);
    return ((xh > e.lo_out) & (xl < e.hi_out)) + ((xl > e.lo_in) & (xh < e.hi_in));
  }
  ALPAKA_FN_HOST_ACC inline int state(const Env &e, float z0, float c) {
    if (!e.disc)
      return cls(e, z0 + c * e.plo, z0 + c * e.phi);
    const float cs = std::abs(c) >= 1e-9f ? c : 1.0f, ic = std::abs(c) >= 1e-9f ? 1.0f / cs : 0.0f;
    const float zr = -z0 * ic;
    const int valid = (ic != 0) & (e.pos * ic + zr > 0);
    return valid * cls(e, e.plo * ic + zr, e.phi * ic + zr);
  }
  // route(): the next crossed position after p (a non-pixel one only with allow), and its state
  ALPAKA_FN_HOST_ACC inline int route(const View &v, int p, float z0, float c, int allow, int &st) {
    for (int q = p + 1; q < v.C.n; ++q) {
      const Env &e = v.env[q];
      if (!e.ok)
        continue;
      const int s = state(e, z0, c);
      if ((s > 0) & (e.pix | allow)) {
        st = s;
        return q;
      }
    }
    st = 0;
    return -1;
  }

  ALPAKA_FN_HOST_ACC inline const Shape *shape_of(const View &v, int p) {
    const int l = v.order[p];
    return v.C.fk_shape && l >= 0 && l < 4 && v.shape[l].ok ? &v.shape[l] : nullptr;
  }
  ALPAKA_FN_HOST_ACC inline int shape_bin(const Shape &s, float acot) {
    return std::min((int)(acot * s.inv_bw), s.nb - 1);
  }
  ALPAKA_FN_HOST_ACC inline int shape_pass(const View &v, const Shape &s, int span, float acot) {
    const int b = shape_bin(s, acot);
    return (span >= v.shape_lo[s.off + b]) & (span <= v.shape_hi[s.off + b]);
  }

  // SensorGapMap::segment_in_gap
  ALPAKA_FN_HOST_ACC inline bool segment_in_gap(const View &v, const GapLayer &Y, const float a[3], const float b[3], float phi) {
    constexpr float kPiM = (float)M_PI;
    int bin = (int)((phi + kPiM) * (GapLayer::kNPhiMap / (2 * kPiM)));
    bin = std::min(std::max(bin, 0), GapLayer::kNPhiMap - 1);
    const float mg = Y.margin;
    const int *start = v.gap_start + Y.start0;
    for (int k = start[bin]; k < start[bin + 1]; ++k) {
      const uint32_t li = Y.lad0 + v.gap_lidx[Y.lidx0 + k];
      const float *q = v.gap_lad + 7 * li;
      const float fa = q[0] * a[0] + q[1] * a[1] - q[2], fb = q[0] * b[0] + q[1] * b[1] - q[2];
      if ((fa > 0) == (fb > 0))
        continue;
      const float s = fa / (fa - fb);
      const float x = a[0] + (b[0] - a[0]) * s, y = a[1] + (b[1] - a[1]) * s, z = a[2] + (b[2] - a[2]) * s;
      if (std::abs(q[3] * x + q[4] * y - q[5]) > q[6] - mg)
        continue;
      const int kz = (int)std::floor((z - v.gap_lz0[li]) * (1.0f / 0.005f));
      const int nz = v.gap_lnz[li];
      if (kz >= 0 && kz < nz && ((v.gap_bits[v.gap_lbits[li] + (kz >> 6)] >> (kz & 63)) & 1ull))
        return false;
    }
    return true;
  }

  ALPAKA_FN_HOST_ACC inline float asin_ox(float x) {
    const float x2 = x * x;
    if (x2 < 0.04f)
      return 1 + x2 * (1.f / 6 + x2 * (3.f / 40 + x2 * (5.f / 112 + x2 * (35.f / 1152 + x2 * (63.f / 2816)))));
    return x >= 1 ? kPi / 2 : std::asin(x) / x;
  }
  ALPAKA_FN_HOST_ACC inline void sinc_cs(float h, float &sc, float &c, float &s) {
    const float h2 = h * h;
    if (h2 < 0.04f) {
      sc = 1 - h2 * (1.f / 6 - h2 * (1.f / 120 - h2 * (1.f / 5040)));
      c = 1 - h2 * (0.5f - h2 * (1.f / 24 - h2 * (1.f / 720 - h2 * (1.f / 40320))));
      s = h * sc;
    } else {
      s = std::sin(h), c = std::cos(h);
      sc = s / h;
    }
  }

  // seedchain::HelixF
  struct HelixF {
    float k = 0, tx = 0, ty = 0, cx = 0, cy = 0, cz = 0, cot = 0;
    float ux = 0, uy = 0, im = 0, c2 = 0, cn = 0;
    bool ok = false;

    ALPAKA_FN_HOST_ACC void make(float ax, float ay, float az, float bx, float by, float cx_, float cy_, float cz_) {
      cx = cx_, cy = cy_, cz = cz_;
      const float vx0 = bx - ax, vy0 = by - ay, vx = cx - bx, vy = cy - by;
      const float lu = std::sqrt(vx0 * vx0 + vy0 * vy0), lv = std::sqrt(vx * vx + vy * vy);
      const float wx = cx - ax, wy = cy - ay, lw = std::sqrt(wx * wx + wy * wy);
      ok = lu >= 1e-6f && lv >= 1e-6f;
      if (!ok)
        return;
      k = 2 * (vx0 * vy - vy0 * vx) / (lu * lv * lw);
      const float sn = std::min(std::max(0.5f * k * lv, -1.0f), 1.0f), cs = std::sqrt(1 - sn * sn);
      const float ex = vx / lv, ey = vy / lv;
      tx = ex * cs - ey * sn;
      ty = ex * sn + ey * cs;
      const float ak = std::abs(k);
      const float s_ab = lu * asin_ox(std::min(1.0f, 0.5f * ak * lu)), s_bc = lv * asin_ox(std::min(1.0f, 0.5f * ak * lv));
      cot = (cz - az) / (s_ab + s_bc);
      const float mx = k * cx - ty, my = k * cy + tx;
      im = 1.0f / std::sqrt(mx * mx + my * my);
      ux = mx * im, uy = my * im;
      c2 = cx * cx + cy * cy;
      cn = -cx * ty + cy * tx;
    }
    ALPAKA_FN_HOST_ACC float pt() const { return std::abs(k) > 1e-12f ? 0.0114f / std::abs(k) : 1e9f; }

    ALPAKA_FN_HOST_ACC bool at_r(float r, float &px, float &py, float &q, float &s3) const {
      const float A = (0.5f * k * (r * r + c2) + cn) * im, g2 = r * r - A * A;
      if (g2 < 0)
        return false;
      const float g = std::sqrt(g2);
      const float p1x = A * ux - g * uy, p1y = A * uy + g * ux, p2x = A * ux + g * uy, p2y = A * uy - g * ux;
      const float d1x = p1x - cx, d1y = p1y - cy, d2x = p2x - cx, d2y = p2y - cy;
      const bool a1 = d1x * tx + d1y * ty > 0, a2 = d2x * tx + d2y * ty > 0;
      const float L1 = d1x * d1x + d1y * d1y, L2 = d2x * d2x + d2y * d2y;
      float L2s;
      if (a1 && (!a2 || L1 <= L2))
        px = p1x, py = p1y, L2s = L1;
      else if (a2)
        px = p2x, py = p2y, L2s = L2;
      else
        return false;
      const float L = std::sqrt(L2s), s = L * asin_ox(std::min(1.0f, 0.5f * std::abs(k) * L));
      q = cz + cot * s;
      s3 = s * std::sqrt(1 + cot * cot);
      return s > 0;
    }
    ALPAKA_FN_HOST_ACC bool at_z(float u, float &px, float &py, float &q, float &s3) const {
      if (std::abs(cot) < 1e-9f)
        return false;
      const float ds = (u - cz) / cot;
      if (ds <= 0)
        return false;
      float sc, ch, sh;
      sinc_cs(0.5f * k * ds, sc, ch, sh);
      const float f = ds * sc;
      px = cx + f * (ch * tx - sh * ty);
      py = cy + f * (sh * tx + ch * ty);
      q = std::sqrt(px * px + py * py);
      s3 = ds * std::sqrt(1 + cot * cot);
      return true;
    }
    ALPAKA_FN_HOST_ACC bool at(bool disc, float u, float &px, float &py, float &q, float &s3) const {
      return disc ? at_z(u, px, py, q, s3) : at_r(u, px, py, q, s3);
    }
  };

  // a candidate's r-z line, as the stage that queued it computed it
  ALPAKA_FN_HOST_ACC inline void line(const View &v, const Cand &c, float &z0, float &cot) {
    const uint32_t ia = v.layers[v.lay_of[c.pos[0]]].hit0 + c.k[0];
    const float za = v.z[ia], ra = v.r[ia];
    if (c.pos[2] == 0) {
      const uint32_t ib = v.layers[v.lay_of[c.pos[1]]].hit0 + c.k[1];
      const float dr = v.r[ib] - ra, dz = v.z[ib] - za;
      cot = dz / (dr > 0.1f ? dr : 1.0f);
    } else {
      const uint32_t ic = v.layers[v.lay_of[c.pos[2]]].hit0 + c.k[2];
      cot = (v.z[ic] - za) / (v.r[ic] - ra);
    }
    z0 = za - cot * ra;
  }

  // ---- the queues
  template <typename TAcc>
  ALPAKA_FN_ACC inline void push(const TAcc &acc, const Queues &Q, int queue, const Cand &c) {
    const uint32_t s = alpaka::atomicAdd(acc, &Q.count[queue], 1u, alpaka::hierarchy::Blocks{});
    const uint32_t ci = s / Queues::kChunk, slot = s % Queues::kChunk;
    if (ci >= Q.maxChunksPerQueue) {
      alpaka::atomicAdd(acc, Q.overflow, 1u, alpaka::hierarchy::Blocks{});
      return;
    }
    int *t = &Q.chunkTab[queue * Q.maxChunksPerQueue + ci];
    int cid;
    if (slot == 0) {
      // a freed chunk if there is one (the free list only grows between the kernels that push), else a fresh one
      cid = -2;
      uint32_t fc = alpaka::atomicAdd(acc, Q.freeCount, 0u, alpaka::hierarchy::Blocks{});
      while (fc > 0) {
        const uint32_t prev = alpaka::atomicCas(acc, Q.freeCount, fc, fc - 1, alpaka::hierarchy::Blocks{});
        if (prev == fc) {
          cid = Q.freeList[fc - 1];
          break;
        }
        fc = prev;
      }
      if (cid < 0) {
        const uint32_t nc = alpaka::atomicAdd(acc, Q.poolNext, 1u, alpaka::hierarchy::Blocks{});
        cid = nc < Q.poolChunks ? (int)nc : -2;
      }
      alpaka::atomicExch(acc, t, cid, alpaka::hierarchy::Blocks{});
    } else {
      // the chunk is being handed out by the push that took its slot 0
      do {
        cid = alpaka::atomicCas(acc, t, -1, -1, alpaka::hierarchy::Blocks{});
      } while (cid == -1);
    }
    if (cid < 0) {
      alpaka::atomicAdd(acc, Q.overflow, 1u, alpaka::hierarchy::Blocks{});
      return;
    }
    Q.pool[(uint32_t)cid * Queues::kChunk + slot] = c;
  }
  // the i-th candidate of a queue; false if it was lost
  ALPAKA_FN_HOST_ACC inline bool fetch(const Queues &Q, int queue, uint32_t i, Cand &c) {
    const uint32_t ci = i / Queues::kChunk;
    if (ci >= Q.maxChunksPerQueue)
      return false;
    const int cid = Q.chunkTab[queue * Q.maxChunksPerQueue + ci];
    if (cid < 0)
      return false;
    c = Q.pool[(uint32_t)cid * Queues::kChunk + i % Queues::kChunk];
    return true;
  }
  template <typename TAcc>
  ALPAKA_FN_ACC inline void emit(const TAcc &acc, const Queues &Q, const Quad &q) {
    const uint32_t o = alpaka::atomicAdd(acc, Q.outCount, 1u, alpaka::hierarchy::Blocks{});
    if (o < Q.outCap)
      Q.out[o] = q;
    else
      alpaka::atomicAdd(acc, Q.overflow, 1u, alpaka::hierarchy::Blocks{});
  }
  // queue ids: Q2[p] = p, Q3[p] = n + p
  ALPAKA_FN_HOST_ACC inline int q2(const View &v, int p) { return p; }
  ALPAKA_FN_HOST_ACC inline int q3(const View &v, int p) { return v.C.n + p; }

  // ---- stage b
  class KernelStart {
  public:
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, View v, Queues Q) const {
      const ChainConst &C = v.C;
      for (uint32_t w : cms::alpakatools::uniform_elements(acc, (uint32_t)C.nwork)) {
        // the start pair of this work item: the last with a0 <= w
        int lo = 0, hi = C.nstarts - 1;
        while (lo < hi) {
          const int mid = (lo + hi + 1) / 2;
          if (v.starts[mid].a0 <= w)
            lo = mid;
          else
            hi = mid - 1;
        }
        const int si = lo;
        const Start &S = v.starts[si];
        const uint32_t ka = w - S.a0;
        const Layer &A = v.layers[v.lay_of[S.pa]], &B = v.layers[v.lay_of[S.pb]];
        const uint32_t ia = A.hit0 + ka;
        // b_fetch (double)
        const float raf = v.r[ia], paf = v.phi[ia], zaf = v.z[ia];
        const double ra_d = raf, pa_d = paf, za_d = zaf;
        const double zlo_d = B.bsz - C.P_zv, zhi_d = B.bsz + C.P_zv;
        auto b_window = [&](double ra, double rb) {
          const double Rmin = C.P_pt_min / (0.003 * 3.8);
          return (rb - ra) / (2 * Rmin) + C.P_d0_max * (1 / ra - 1 / rb);
        };
        const double rbmax = B.disc ? B.q_hi : B.qbar_hi;
        const double w_ab = b_window(ra_d, rbmax) + C.P_marg_b;
        double bq_lo, bq_hi;
        if (!B.disc) {
          double zz[4];
          int nz = 0;
          const double rbs[2] = {B.qbar_lo, B.qbar_hi}, z0s[2] = {zlo_d, zhi_d};
          for (int i = 0; i < 2; ++i)
            for (int j = 0; j < 2; ++j)
              zz[nz++] = za_d + (za_d - z0s[j]) * (rbs[i] - ra_d) / ra_d;
          bq_lo = std::min(std::min(zz[0], zz[1]), std::min(zz[2], zz[3]));
          bq_hi = std::max(std::max(zz[0], zz[1]), std::max(zz[2], zz[3]));
        } else {
          const double s = (B.qbar_lo + B.qbar_hi) > 0 ? 1 : -1;
          const double za = s * za_d, zb0 = std::min(s * B.qbar_lo, s * B.qbar_hi),
                       zb1 = std::max(s * B.qbar_lo, s * B.qbar_hi);
          const double z0min = std::min(s * zlo_d, s * zhi_d), z0max = std::max(s * zlo_d, s * zhi_d);
          if (za - z0min <= 0)
            continue;
          bq_lo = ra_d + ra_d * (zb0 - za) / (za - z0min);
          bq_hi = za - z0max > 0 ? ra_d + ra_d * (zb1 - za) / (za - z0max) : B.q_hi;
        }
        if (bq_hi < B.q_lo || bq_lo > B.q_hi)
          continue;
        IPair fp = phi_bins_d(B, pa_d, w_ab), fq = q_bins_d(B, bq_lo, bq_hi);
        // the q range the start pair's cot range allows on B
        const float ulo = S.clo, uhi = S.chi;
        const bool cot_q = B.disc ? (ulo > 0 || uhi < 0) : (ulo > -1e29f && uhi < 1e29f);
        if (cot_q) {
          float qlo, qhi;
          if (!B.disc) {
            const float d0 = B.qbar_lo - raf, d1 = B.qbar_hi - raf;
            const float e0 = ulo * d0, e1 = ulo * d1, e2 = uhi * d0, e3 = uhi * d1;
            qlo = zaf + std::min(std::min(e0, e1), std::min(e2, e3));
            qhi = zaf + std::max(std::max(e0, e1), std::max(e2, e3));
          } else {
            const float i0 = 1.0f / ulo, i1 = 1.0f / uhi, d0 = B.qbar_lo - zaf, d1 = B.qbar_hi - zaf;
            const float e0 = i0 * d0, e1 = i0 * d1, e2 = i1 * d0, e3 = i1 * d1;
            qlo = raf + std::min(std::min(e0, e1), std::min(e2, e3));
            qhi = raf + std::max(std::max(e0, e1), std::max(e2, e3));
          }
          const IPair nq = q_bins_d(B, qlo - 0.01f, qhi + 0.01f);
          fq.b = std::max(fq.b, nq.b), fq.e = std::min(fq.e, nq.e);
          if (fq.b >= fq.e)
            continue;
          if (B.disc && qhi + 0.01f < B.q_hi)
            fp = phi_bins_d(B, pa_d, b_window(ra_d, qhi + 0.01f) + C.P_marg_b);
        }
        const float inva = 1.0f / raf, side = C.side;
        const int g_lid = S.gap_lid;
        const Shape *shA = shape_of(v, S.pa), *shB = shape_of(v, S.pb);
        const int lead_only = C.lead_only & !S.gap_ok;
        for_each_run(v, B, fp, fq, [&](uint32_t b, uint32_t e) {
          for (uint32_t kb = b; kb < e; ++kb) {
            const uint32_t ib = B.hit0 + kb;
            const float dr = v.r[ib] - raf, dz = v.z[ib] - zaf;
            float dp = v.phi[ib] - paf;
            dp = dp > kPi ? dp - k2Pi : dp;
            dp = dp < -kPi ? dp + k2Pi : dp;
            const float wd = dr * C.inv2R + C.d0 * (inva - v.invr[ib]) + C.marg;
            const float num = zaf * dr - raf * dz;
            const int cut = (dr > 0.1f) & (std::abs(dp) <= wd) & (num >= C.zlo * dr) & (num <= C.zhi * dr);
            const int sd = !((dz * side < 0) | ((dz == 0) & (side < 0)));
            const float cot = dz / (dr > 0.1f ? dr : 1.0f);
            const int in0 = (cot >= S.clo) & (cot <= S.chi);
            if (!(cut & sd & in0))
              continue;
            const float z0 = zaf - cot * raf;
            if (g_lid >= 0) {
              const GapLayer &G = v.gap[g_lid];
              const float g_pfac = GapLayer::kNPhiRay / k2Pi, g_izc = 1.0f / 0.005f;
              float ph = paf + dp * (G.r_mid - raf) / (dr > 0.1f ? dr : 1.0f);
              ph = ph > kPi ? ph - k2Pi : ph;
              ph = ph < -kPi ? ph + k2Pi : ph;
              int pbn = (int)((ph + kPi) * g_pfac);
              pbn = pbn < 0 ? 0 : pbn >= GapLayer::kNPhiRay ? GapLayer::kNPhiRay - 1 : pbn;
              const float *r1 = v.gap_ray + G.ray0, *r2 = r1 + GapLayer::kNPhiRay;
              const unsigned char *zg = v.gap_zgap + G.zgap0;
              const float z1 = zaf + cot * (r1[pbn] - raf), z2 = zaf + cot * (r2[pbn] - raf);
              const int k1 = (int)std::floor((z1 - G.zg0) * g_izc), k2 = (int)std::floor((z2 - G.zg0) * g_izc);
              const int in1g = (k1 < 0) | (k1 >= G.nzg) || zg[k1], in2g = (k2 < 0) | (k2 >= G.nzg) || zg[k2];
              if (!(v.gap_edge[G.ray0 + pbn] | in1g | in2g))
                continue;
            }
            // flush_start: holes before b, the start layers crossed, the gap map, lead-only, the shape
            int holes = 0, bt = 0;
            for (int h = 0; h < S.nh; ++h) {
              const HolePos &H = v.holes[S.h0 + h];
              const int d = state(v.env[H.q], z0, cot) == 2;
              holes += d, bt |= d & H.between;
            }
            const int sa = state(v.env[S.pa], z0, cot), sb = state(v.env[S.pb], z0, cot);
            int gv = 1;
            if (S.gap_ok && C.gap_map && g_lid < 0) {
              const float za = zaf, zb = v.z[ib];
              const float a[3] = {v.x[ia] + A.bsx + A.bsdxdz * (za - A.bsz), v.y[ia] + A.bsy + A.bsdydz * (za - A.bsz), za};
              const float bb[3] = {v.x[ib] + A.bsx + A.bsdxdz * (zb - A.bsz), v.y[ib] + A.bsy + A.bsdydz * (zb - A.bsz), zb};
              for (int h = 0; h < S.nh && gv; ++h) {
                const HolePos &H = v.holes[S.h0 + h];
                const int lid = v.order[H.q];
                if (!H.between || lid < 0 || lid > 3 || !v.gap[lid].ok)
                  continue;
                if (state(v.env[H.q], z0, cot) != 2)
                  continue;
                const GapLayer &G = v.gap[lid];
                const float rm = G.r_mid, rb = v.r[ib];
                const float pm = paf + wrapl(v.phi[ib] - paf) * (rm - raf) / (rb - raf);
                if (!segment_in_gap(v, G, a, bb, wrapl(pm)))
                  gv = 0;
              }
            }
            const int inner = S.gap_ok & bt;
            int shp = !(lead_only & bt);
            if (shA || shB) {
              const float ac = std::abs(cot);
              shp &= (!shA || shape_pass(v, *shA, v.span[ia], ac)) & (!shB || shape_pass(v, *shB, v.span[ib], ac));
            }
            if (!(shp & gv & (holes <= C.hs) & (sa > 0) & (sb > 0)))
              continue;
            int st;
            const int nq = route(v, S.pb, z0, cot, 0, st);
            if (nq < 0 || v.lay_of[nq] < 0)
              continue;
            Cand c;
            c.k[0] = ka, c.k[1] = kb, c.k[2] = 0;
            c.pos[0] = S.pa, c.pos[1] = S.pb, c.pos[2] = 0;
            c.holes = holes, c.inner = inner, c.st = st;
            c.sc = 0;
            push(acc, Q, q2(v, nq), c);
          }
        });
      }
    }
  };

  // ---- stage c at position p
  class KernelStageC {
  public:
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, View v, Queues Q, int p) const {
      const ChainConst &C = v.C;
      const int n = C.n;
      const Layer &T = v.layers[v.lay_of[p]];
      const bool disc = T.disc;
      const float u0 = T.qbar_lo, u1 = T.qbar_hi;
      const Shape *shT = shape_of(v, p);
      const uint32_t nc = std::min(Q.count[q2(v, p)], Q.maxChunksPerQueue * Queues::kChunk);
      for (uint32_t j : cms::alpakatools::uniform_elements(acc, nc)) {
        Cand c;
        if (!fetch(Q, q2(v, p), j, c))
          continue;
        float cz0, ccot;
        line(v, c, cz0, ccot);
        const int ix = v.idx_c[(c.pos[0] * n + c.pos[1]) * n + p];
        bool found = false;
        const float fks_c = C.fk_score_fwd > 0 && std::abs(ccot) >= C.fk_cot_fwd ? C.fk_score_fwd : C.fks;
        if (!C.known_only || ix >= 0) {
          const Par &w = v.par[ix >= 0 ? ix : 0];
          const Layer &La = v.layers[v.lay_of[c.pos[0]]], &Lb = v.layers[v.lay_of[c.pos[1]]];
          const uint32_t ia = La.hit0 + c.k[0], ib = Lb.hit0 + c.k[1];
          const float ua = disc ? v.z[ia] : v.r[ia], ub = disc ? v.z[ib] : v.r[ib];
          const float qa = disc ? v.r[ia] : v.z[ia], qb = disc ? v.r[ib] : v.z[ib];
          const float pa = v.phi[ia], dp = wrapc(v.phi[ib] - pa);
          const float iA = v.invr[ia], iB = v.invr[ib], idu = 1.0f / (ub - ua);
          const float t0_ = (u0 - ua) * idu, t1_ = (u1 - ua) * idu;
          if (std::max(t0_, t1_) > 1) {
            const float cq0 = qa + (qb - qa) * t0_, cq1 = qa + (qb - qa) * t1_;
            const float cp0 = pa + dp * t0_, cp1 = pa + dp * t1_;
            const bool far0 = std::abs(t0_) > std::abs(t1_);
            const float ufar = far0 ? u0 : u1, tfar = far0 ? t0_ : t1_;
            const float rfar = std::max(0.5f, disc ? qa + (qb - qa) * tfar : ufar);
            const float wcphi = C.d0_max * std::abs(1 / rfar - (iA + (iB - iA) * tfar)) + w.phi_c;
            const IPair qc = q_bins(T, std::min(cq0, cq1) - w.q_c, std::max(cq0, cq1) + w.q_c);
            const float dcp = wrapc(cp1 - cp0), cmid = cp0 + 0.5f * dcp, chalf = 0.5f * std::abs(dcp);
            const float qcw = w.q_c, pcw = w.phi_c, dqab = qb - qa, dib = iB - iA, iqcw = 1.0f / qcw;
            int slo = 0, shi = 1 << 30;
            if (shT) {
              const int sb = shape_bin(*shT, std::abs(dqab * idu));
              slo = v.shape_lo[shT->off + sb], shi = v.shape_hi[shT->off + sb];
            }
            for_each_run(v, T, phi_bins(T, cmid, chalf + wcphi + 1e-6f), qc, [&](uint32_t b, uint32_t e) {
              for (uint32_t i = b; i < e; ++i) {
                const uint32_t ih = T.hit0 + i;
                const float hu = disc ? v.z[ih] : v.r[ih], hq = disc ? v.r[ih] : v.z[ih];
                const float t = (hu - ua) * idu;
                const float dq = hq - (qa + dqab * t);
                if (!(t > 1) || !(std::abs(dq) <= qcw))
                  continue;
                const float dph = wrapc(wrapc(v.phi[ih] - pa) - dp * t);
                const float wph = C.d0_max * std::abs(v.invr[ih] - (iA + dib * t)) + pcw;
                if (!(std::abs(dph) <= wph))
                  continue;
                float sc = 0;
                if (C.fk_on) {
                  const float rq = dq * iqcw, rp = dph / wph;
                  sc = rq * rq + rp * rp;
                  if (sc >= fks_c || v.span[ih] < slo || v.span[ih] > shi)
                    continue;
                }
                found = true;
                // the triplet: its a-c line, routed from p
                const float za = v.z[ia], ra = v.r[ia], zc = v.z[ih], rc = v.r[ih];
                const float cot = (zc - za) / (rc - ra), z0 = za - cot * ra;
                int st;
                const int nq = route(v, p, z0, cot, c.holes <= C.max_holes_ot, st);
                if (nq < 0 || v.lay_of[nq] < 0)
                  continue;
                Cand t3 = c;
                t3.k[2] = i, t3.pos[2] = p, t3.sc = sc, t3.st = st;
                push(acc, Q, q3(v, nq), t3);
              }
            });
          }
        }
        if (!found || C.hole_always) {
          const int h = c.holes + (c.st == 2);
          if (h <= C.max_holes) {
            int st;
            const int nq = route(v, p, cz0, ccot, 0, st);
            if (nq >= 0 && v.lay_of[nq] >= 0) {
              Cand m = c;
              m.inner += h > c.holes;
              m.holes = h, m.st = st;
              push(acc, Q, q2(v, nq), m);
            }
          }
        }
      }
    }
  };

  // ---- stage d at position p
  class KernelStageD {
  public:
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, View v, Queues Q, int p) const {
      const ChainConst &C = v.C;
      const int n = C.n;
      const Layer &T = v.layers[v.lay_of[p]];
      const bool disc = T.disc;
      const float u0 = T.qbar_lo, u1 = T.qbar_hi;
      const Shape *shT = shape_of(v, p);
      const float fks = C.fks;
      const uint32_t nc = std::min(Q.count[q3(v, p)], Q.maxChunksPerQueue * Queues::kChunk);
      for (uint32_t j : cms::alpakatools::uniform_elements(acc, nc)) {
        Cand c;
        if (!fetch(Q, q3(v, p), j, c))
          continue;
        float cz0, ccot;
        line(v, c, cz0, ccot);
        const Layer &La = v.layers[v.lay_of[c.pos[0]]], &Lb = v.layers[v.lay_of[c.pos[1]]],
                    &Lc = v.layers[v.lay_of[c.pos[2]]];
        const uint32_t ia = La.hit0 + c.k[0], ib = Lb.hit0 + c.k[1], ic = Lc.hit0 + c.k[2];
        HelixF H;
        H.make(v.x[ia], v.y[ia], v.z[ia], v.x[ib], v.y[ib], v.x[ic], v.y[ic], v.z[ic]);
        const int ix = v.idx_d[((c.pos[0] * n + c.pos[1]) * n + c.pos[2]) * n + p];
        bool found = false;
        const bool inner_ok = !C.inner_ot_only || !c.inner || !v.env[p].pix;
        auto emit3 = [&]() {
          if (!C.trip_on)
            return;
          const float ac = std::abs(ccot);
          if (ac < C.trip_lo || ac >= C.trip_hi)
            return;
          Quad o;
          o.layers[0] = v.order[c.pos[0]], o.layers[1] = v.order[c.pos[1]], o.layers[2] = v.order[c.pos[2]];
          o.layers[3] = -1;
          o.hits[0] = v.orig[ia], o.hits[1] = v.orig[ib], o.hits[2] = v.orig[ic], o.hits[3] = 0;
          o.score = c.sc, o.fake = c.sc;
          emit(acc, Q, o);
        };
        if ((!C.known_only || ix >= 0) && H.ok && inner_ok) {
          const Par &w = v.par[ix >= 0 ? ix : 0];
          float aphi = w.aphi, bphi = w.bphi, aq = w.aq, bq = w.bq;
          if (w.ne > 0) {
            const float ae = std::asinh(std::abs(H.cot));
            for (int i = 0; i < w.ne; ++i) {
              const EtaWin &E = v.etaw[w.e0 + i];
              if (ae >= E.lo && ae < E.hi) {
                aphi = E.aphi, bphi = E.bphi, aq = E.aq, bq = E.bq;
                break;
              }
            }
          }
          float x0, y0, q0, s0 = -1, x1, y1, q1, s1 = -1;
          const bool ok0 = H.at(disc, u0, x0, y0, q0, s0), ok1 = H.at(disc, u1, x1, y1, q1, s1);
          if (ok0 || ok1) {
            if (!ok0)
              x0 = x1, y0 = y1, q0 = q1;
            if (!ok1)
              x1 = x0, y1 = y0, q1 = q0;
            const float p0 = std::atan2(y0, x0), p1 = std::atan2(y1, x1);
            const float pte = H.pt(), ipt = 1.0f / std::max(C.pt_min, pte);
            const float isr = w.sref > 0 ? 1.0f / w.sref : 0.0f;
            const float smax = std::max(s0, s1), lmax = isr > 0 && smax > 0 ? smax * isr : 1.0f;
            const float wpd = aphi + lmax * bphi * ipt, wqd = aq + lmax * bq * ipt;
            const IPair qd = q_bins(T, std::min(q0, q1) - wqd, std::max(q0, q1) + wqd);
            const float dpp = wrapc(p1 - p0), dmid = p0 + 0.5f * dpp, dhalf = 0.5f * std::abs(dpp);
            const float bqi = bq * ipt, bpi = bphi * ipt;
            const float sw = std::sin(wpd), sw2 = sw * sw;
            const float fkd = (C.fk_score_fwd > 0 && std::abs(ccot) >= C.fk_cot_fwd ? C.fk_score_fwd : fks) - c.sc;
            int slo = 0, shi = 1 << 30;
            if (shT) {
              const int sb = shape_bin(*shT, std::abs(ccot));
              slo = v.shape_lo[shT->off + sb], shi = v.shape_hi[shT->off + sb];
            }
            auto take = [&](uint32_t ih, float dq, float wq, float c2, float sn, float s2) {
              const float rq = dq / wq, fk_d = rq * rq + c2 / sn;
              if (C.fk_on) {
                if (fk_d >= fkd || v.span[ih] < slo || v.span[ih] > shi)
                  return;
              }
              found = true;
              Quad o;
              o.layers[0] = v.order[c.pos[0]], o.layers[1] = v.order[c.pos[1]], o.layers[2] = v.order[c.pos[2]];
              o.layers[3] = v.order[p];
              o.hits[0] = v.orig[ia], o.hits[1] = v.orig[ib], o.hits[2] = v.orig[ic], o.hits[3] = v.orig[ih];
              // the cleaning score with the pattern's own windows
              const bool dc = Lc.disc;
              const float ua = dc ? v.z[ia] : v.r[ia], ub = dc ? v.z[ib] : v.r[ib], uc = dc ? v.z[ic] : v.r[ic];
              const float qa = dc ? v.r[ia] : v.z[ia], qb = dc ? v.r[ib] : v.z[ib], qcc = dc ? v.r[ic] : v.z[ic];
              const float rqc = (qcc - (qa + (qb - qa) * (uc - ua) / (ub - ua))) / w.q_c;
              const float wpc = w.aphi + w.bphi * ipt, wqc = w.aq + w.bq * ipt;
              const float dph2 = s2 * (1 + s2 * (1.0f / 3));
              const float sc = rqc * rqc + dph2 / (wpc * wpc) + (dq / wqc) * (dq / wqc);
              o.score = std::isfinite(sc) ? sc : 1e30f;
              o.fake = c.sc + fk_d;
              emit(acc, Q, o);
            };
            // the q pre-filter of the CPU finder's direct mode
            float xm, ym, qm = 0, sm = -1;
            const float um = 0.5f * (u0 + u1), ihh = 2.0f / (u1 - u0);
            const bool pre = ok0 && ok1 && H.at(disc, um, xm, ym, qm, sm);
            const float qa1 = 0.5f * (q1 - q0), qa2 = 0.5f * (q0 + q1) - qm, wq_pre = 1.1f * wqd + 0.002f;
            for_each_run(v, T, phi_bins(T, dmid, dhalf + wpd + 1e-6f), qd, [&](uint32_t b, uint32_t e) {
              for (uint32_t i = b; i < e; ++i) {
                const uint32_t ih = T.hit0 + i;
                const float hu = disc ? v.z[ih] : v.r[ih], hq = disc ? v.r[ih] : v.z[ih];
                if (pre) {
                  const float t = (hu - um) * ihh;
                  if (std::abs(hq - (qm + t * (qa1 + t * qa2))) > wq_pre)
                    continue;
                }
                float px = 0, py = 0, qp = 0, s3 = -1;
                const bool ok = H.at(disc, hu, px, py, qp, s3);
                float wq = wqd, sp2 = sw2;
                if (isr > 0) {
                  const float lv = s3 > 0 ? s3 * isr : 1.0f, wp = aphi + lv * bpi, sp = std::sin(wp);
                  wq = aq + lv * bqi, sp2 = sp * sp;
                }
                const float hxx = v.x[ih], hyy = v.y[ih];
                const float cr = px * hyy - py * hxx, dt = px * hxx + py * hyy;
                const float n2 = (px * px + py * py) * (hxx * hxx + hyy * hyy);
                const float dq = hq - qp, c2 = cr * cr, sn = sp2 * n2;
                if (ok && std::abs(dq) <= wq && dt > 0 && c2 <= sn)
                  take(ih, dq, wq, c2, sn, c2 / n2);
              }
            });
          }
        }
        if (!found || C.hole_always) {
          const int h = c.holes + (c.st == 2);
          if (h <= C.max_holes) {
            int st;
            const int nq = route(v, p, cz0, ccot, h <= C.max_holes_ot, st);
            if (nq < 0 || v.lay_of[nq] < 0) {
              emit3();
            } else {
              Cand m = c;
              m.inner += h > c.holes;
              m.holes = h, m.st = st;
              push(acc, Q, q3(v, nq), m);
            }
          } else if (!found)
            emit3();
        }
      }
    }
  };

  // ---- the chunks of the queues of position p (consumed) back to the free list
  class KernelRelease {
  public:
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, View v, Queues Q, int p) const {
      for (int k = 0; k < 2; ++k) {
        const int queue = k ? q3(v, p) : q2(v, p);
        const uint32_t nch = std::min((Q.count[queue] + Queues::kChunk - 1) / Queues::kChunk, Q.maxChunksPerQueue);
        for (uint32_t i : cms::alpakatools::uniform_elements(acc, nch)) {
          const int cid = Q.chunkTab[queue * Q.maxChunksPerQueue + i];
          if (cid >= 0)
            Q.freeList[alpaka::atomicAdd(acc, Q.freeCount, 1u, alpaka::hierarchy::Blocks{})] = cid;
        }
      }
    }
  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE::mkseeder

#endif
