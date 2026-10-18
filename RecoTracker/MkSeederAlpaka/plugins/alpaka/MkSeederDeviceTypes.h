#ifndef RecoTracker_MkSeederAlpaka_plugins_alpaka_MkSeederDeviceTypes_h
#define RecoTracker_MkSeederAlpaka_plugins_alpaka_MkSeederDeviceTypes_h

// The mkFit seeder's chain finder (mkfit::SeedChainFinder) as flat, trivially copyable data: the event's binned
// layers and one z side's chain, built on the host from a configured mkfit::MkSeeder and copied to the device in
// one buffer (MkSeederBlob). Host and device code; no alpaka, no mkFit types.

#include <cstdint>

namespace mkseederdev {

  // One mkFit layer of the event (mkfit::SeedLayerOfHits): its hits in bin order at hit0 .. hit0 + n of the hit
  // arrays, its bin starts at start0 (nq * kNPhi + 1 entries), the axes and the extents.
  struct Layer {
    static constexpr int kNPhi = 2048;  // SeedLayerOfHits::c_phi_nbits = 11
    int ok, disc, nq, q_last;
    float phi_rmin, phi_fac;         // axis_pow2_u1: bin = floor((phi - rmin) * fac) & (kNPhi - 1)
    float q_rmin, q_fac, q_lbhp;     // axis: bin = q <= rmin ? 0 : q >= lbhp ? q_last : floor((q - rmin) * fac)
    double qbar_lo, qbar_hi, q_lo, q_hi;
    float bsx, bsy, bsz, bsdxdz, bsdydz;  // the beam spot of the fill
    uint32_t hit0, start0, n;
  };

  // one chain position's crossing envelope (SeedChainFinder::EnvF)
  struct Env {
    int ok, disc, pix;
    float pos, plo, phi, lo_in, hi_in, lo_out, hi_out;
  };

  // one window table (SeedChainFinder::ParF) and its |eta| slices
  struct Par {
    float phi_c, q_c, aphi, bphi, aq, bq, sref;
    int e0, ne;
  };
  struct EtaWin {
    float lo, hi, aphi, bphi, aq, bq;
  };

  // a candidate on a queue (mkfit::SeedCand): hits in the layers' bin order, chain positions (pos[2] = 0: a
  // doublet), the stage c score; its r-z line (a-b for a doublet, a-c for a triplet) is recomputed from the hits
  struct Cand {
    uint32_t k[3];
    uint8_t pos[3], holes, st, inner;
    float sc;
  };

  // a found quad (layers[3] = -1: a triplet seed), hits as the layers' external (orig) indices
  struct Quad {
    int16_t layers[4];
    uint32_t hits[4];
    float score, fake;
  };

  // per start pair
  struct Start {
    int pa, pb, gap_ok, gap_lid;  // gap_lid: the one skipped barrel layer with a map, or -1
    float clo, chi;               // the usable cot range; clo > chi: none
    int h0, nh;                   // hole positions at holes[h0 .. h0 + nh)
    uint32_t a0;                  // the first work item of this start (prefix of the A layer sizes)
  };
  struct HolePos {
    int q, between;  // chain position; 1 if between a and b
  };

  // the gap map of one barrel layer (SensorGapMap, export_layer) and the event's ray table (prepare())
  struct GapLayer {
    static constexpr int kNPhiMap = 256, kNPhiRay = 2048;
    int ok;
    float r_mid, zg0, margin;
    int nzg;
    uint32_t zgap0;                // zgap bytes
    uint32_t lad0, nlad;           // ladders: lad floats at 7 * (lad0 + i), lz0 / lnz / lbits per ladder
    uint32_t start0, lidx0;        // ladders by phi bin
    uint32_t ray0;                 // the ray table: r1, r2 (floats) and edge (bytes) at ray0 .. + kNPhiRay
  };

  // the constants of one side's finder
  struct ChainConst {
    int n, side, nstarts, nwork;
    int max_holes, max_holes_ot, hs, lead_only, inner_ot_only, known_only, hole_always, gap_map;
    int fk_on, fk_shape, trip_on;
    float fks, fk_score_fwd, fk_cot_fwd, trip_lo, trip_hi;
    float d0_max, pt_min;  // the finder's (float)
    // stage b, as start_doublets() computes them
    float inv2R, d0, marg, zlo, zhi;
    // b_fetch (double), the SeedingParams in float
    float P_pt_min, P_d0_max, P_zv, P_marg_b;
  };

  // shape band of the barrel layers 0-3 (mkFit ids): lo / hi per |cot| bin at off .. off + nb
  struct Shape {
    int ok, off, nb;
    float inv_bw;
  };

  // The device view of one side: pointers into the blob, set on the host with the device base address.
  struct View {
    ChainConst C;
    const Layer *layers;   // by mkFit layer id, nLayers
    int nLayers;
    const int *lay_of;     // per chain position: mkFit layer id with hits, or -1
    const int *order;      // per chain position: mkFit layer id
    const Env *env;
    const Start *starts;
    const HolePos *holes;
    const int *idx_c, *idx_d;
    const Par *par;
    const EtaWin *etaw;
    Shape shape[4];
    const int *shape_lo, *shape_hi;
    const GapLayer *gap;   // by mkFit layer id (barrel 0-3)
    const float *gap_lad, *gap_lz0, *gap_ray;
    const int *gap_lnz, *gap_lbits, *gap_start, *gap_lidx;
    const unsigned long long *gap_bits;
    const unsigned char *gap_zgap, *gap_edge;
    // the hits
    const float *phi, *z, *r, *invr, *x, *y;
    const uint32_t *orig;
    const int *span;
    const uint32_t *bin_start;
  };

  // The queues of one side: Q2 (doublets) and Q3 (triplets) per chain position, each a list of fixed-size chunks
  // from one pool. A queue is consumed once, at its position, after every producer for it has run.
  struct Queues {
    static constexpr uint32_t kChunk = 256;
    Cand *pool;
    uint32_t poolChunks, maxChunksPerQueue;
    uint32_t *poolNext;   // chunks handed out fresh (= the peak of live chunks: a freed chunk is taken first)
    int *freeList;        // chunks of consumed queues (KernelRelease), taken by push() first
    uint32_t *freeCount;
    uint32_t *count;      // per queue (2 * n): candidates pushed
    int *chunkTab;        // per queue: maxChunksPerQueue chunk ids, -1: not yet
    uint32_t *overflow;   // candidates lost to a full pool or table
    Quad *out;
    uint32_t outCap;
    uint32_t *outCount;
  };

}  // namespace mkseederdev

#endif
