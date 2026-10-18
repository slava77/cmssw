#include <algorithm>
#include <chrono>
#include <cstring>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <tuple>
#include <vector>

#include <alpaka/alpaka.hpp>

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/Event.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EventSetup.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/SynchronizingEDProducer.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoTracker/MkFit/interface/MkFitGeometry.h"
#include "RecoTracker/MkFit/interface/MkFitHitWrapper.h"
#include "RecoTracker/MkFit/interface/MkFitSeedWrapper.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"

#include "RecoTracker/MkFitCore/interface/MkSeeder.h"
#include "RecoTracker/MkFitCore/interface/SeedChain.h"
#include "RecoTracker/MkFitCore/interface/SeederConfig.h"
#include "RecoTracker/MkFitCore/interface/SensorGapMap.h"
#include "RecoTracker/MkFitCore/interface/Track.h"
#include "RecoTracker/MkFitCore/interface/TrackerInfo.h"
#include "RecoTracker/MkFitCore/interface/SeedChainFinder.h"

#include "MkSeederDevice.h"
#include "MkSeederDeviceTypes.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace mkseederdev;

  namespace {
    // the host image of the device buffer: arrays at 64-byte aligned offsets
    struct Blob {
      std::vector<std::byte> data;
      template <typename T>
      size_t add(const T *p, size_t n) {
        const size_t off = (data.size() + 63) & ~size_t(63);
        data.resize(off + n * sizeof(T) + 64);
        if (n)
          std::memcpy(data.data() + off, p, n * sizeof(T));
        return off;
      }
      template <typename T>
      size_t add(const std::vector<T> &v) {
        return add(v.data(), v.size());
      }
    };
    using Key = std::tuple<int, int, int, int, unsigned, unsigned, unsigned, unsigned>;
    Key key_of(const std::array<int, 4> &l, const mkfit::SeedQuad &h) {
      return {l[0], l[1], l[2], l[3], h[0], h[1], h[2], l[3] < 0 ? 0u : h[3]};
    }
  }  // namespace

  // The mkFit seeder (MkFitSeederProducer) with its chain finder on the device: the hits are binned on the host
  // (mkfit::MkSeeder::fill), the doublets, triplets and quads are found by the kernels of MkSeederKernels.h,
  // and the cleaning and the seed fit run on the host as in the CPU module (mkfit::MkSeeder::finish,
  // mkfit::seeder_make_seeds). Same inputs, same output (MkFitSeedWrapper): a drop-in for MkFitSeederProducer.
  class MkSeederAlpaka : public stream::SynchronizingEDProducer<> {
  public:
    explicit MkSeederAlpaka(edm::ParameterSet const &iConfig)
        : SynchronizingEDProducer(iConfig),
          pixelHitsToken_{consumes(iConfig.getParameter<edm::InputTag>("pixelHits"))},
          stripHitsToken_{consumes(iConfig.getParameter<edm::InputTag>("stripHits"))},
          pixelLayerIndexToken_{consumes(iConfig.getParameter<edm::InputTag>("pixelHits"))},
          stripLayerIndexToken_{consumes(iConfig.getParameter<edm::InputTag>("stripHits"))},
          beamSpotToken_{consumes(iConfig.getParameter<edm::InputTag>("beamSpot"))},
          mkFitGeomToken_{esConsumes()},
          seederConfigToken_{esConsumes(iConfig.getParameter<edm::ESInputTag>("config"))},
          putToken_{produces()},
          algo_{reco::TrackBase::algoByName(iConfig.getParameter<std::string>("algorithm"))},
          maxNSeeds_{iConfig.getParameter<unsigned int>("maxNSeeds")},
          minCandidates_{iConfig.getParameter<unsigned int>("minCandidates")},
          minQuads_{iConfig.getParameter<unsigned int>("minQuads")},
          validate_{iConfig.getParameter<bool>("validateWithCPU")},
          printStats_{iConfig.getParameter<bool>("printStats")} {}

    static void fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
      edm::ParameterSetDescription desc;
      desc.add("pixelHits", edm::InputTag{"mkFitSiPixelHits"});
      desc.add("stripHits", edm::InputTag{"mkFitSiPhase2Hits"});
      desc.add("beamSpot", edm::InputTag{"offlineBeamSpot"});
      desc.add<edm::ESInputTag>("config", edm::ESInputTag{"", "mkFitSeederConfig"});
      desc.add<std::string>("algorithm", "initialStep");
      desc.add("maxNSeeds", 500000U);
      desc.add("minCandidates", 262144U)
          ->setComment("The smallest queue pool (live doublets and triplets of one z side); it is sized to 1.25 x the "
                       "largest use seen in the stream, and an event that overflows it is rerun with twice the size");
      desc.add("minQuads", 131072U)->setComment("The same for the quads and triplet seeds before the cleaning");
      desc.add("validateWithCPU", false)->setComment("Also run the CPU chain finder and compare the quads");
      desc.add("printStats", false)->setComment("Print the device buffer sizes and uses per event");
      descriptions.addWithDefaultLabel(desc);
    }

    void acquire(device::Event const &iEvent, device::EventSetup const &iSetup) override;
    void produce(device::Event &iEvent, device::EventSetup const &iSetup) override;

  private:
    void configure(const mkfit::SeederConfig &cfg, const mkfit::TrackerInfo &ti);
    // the side's chain into the blob; offsets of its arrays in offs
    void flatten(int side, Blob &blob, std::map<std::string, size_t> &offs, ChainConst &cc, std::vector<int> &layOf);

    const edm::EDGetTokenT<MkFitHitWrapper> pixelHitsToken_;
    const edm::EDGetTokenT<MkFitHitWrapper> stripHitsToken_;
    const edm::EDGetTokenT<std::vector<int>> pixelLayerIndexToken_;
    const edm::EDGetTokenT<std::vector<int>> stripLayerIndexToken_;
    const edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
    const edm::ESGetToken<MkFitGeometry, TrackerRecoGeometryRecord> mkFitGeomToken_;
    const edm::ESGetToken<mkfit::SeederConfig, TrackerRecoGeometryRecord> seederConfigToken_;
    const edm::EDPutTokenT<MkFitSeedWrapper> putToken_;
    const int algo_;
    const unsigned int maxNSeeds_, minCandidates_, minQuads_;
    const bool validate_, printStats_;

    // per stream
    std::unique_ptr<mkfit::MkSeeder> seeder_;
    const void *configuredFor_[2] = {nullptr, nullptr};
    std::vector<std::vector<unsigned int>> layerHits_;
    mkfit::SeedHitSource src_;
    const mkfit::TrackerInfo *trackerInfo_ = nullptr;
    const mkfit::SeederConfig *seederConfig_ = nullptr;
    // the event's device buffers
    std::optional<cms::alpakatools::device_buffer<Device, std::byte[]>> dBlob_;
    std::optional<cms::alpakatools::device_buffer<Device, Cand[]>> dPool_;
    std::optional<cms::alpakatools::device_buffer<Device, uint32_t[]>> dCounters_;  // count per queue, poolNext,
                                                                                     // overflow, outCount, pool use
    std::optional<cms::alpakatools::device_buffer<Device, int[]>> dChunkTab_;
    std::optional<cms::alpakatools::device_buffer<Device, uint8_t[]>> dFound_;
    std::optional<cms::alpakatools::device_buffer<Device, uint16_t[]>> dMissTgt_;
    std::optional<cms::alpakatools::device_buffer<Device, Quad[]>> dOut_;
    std::optional<cms::alpakatools::host_buffer<uint32_t[]>> hStats_;
    uint32_t nQueuesMax_ = 0;
    // the event's views and chain positions with a layer, for a rerun
    View view_[2];
    std::vector<int> layOf_[2];
    size_t blobBytes_ = 0;
    // the buffer sizes: the largest uses seen in this stream
    uint32_t peakChunks_ = 0, peakQuads_ = 0, poolChunks_ = 0, outCap_ = 0;
    void launch(Queue &queue, uint32_t poolChunks, uint32_t outCap);
    // host time per part (printStats): fill, blob, enqueue, copy back, finish, fit
    using Clock = std::chrono::steady_clock;
    double tPart_[6] = {0, 0, 0, 0, 0, 0};
    long nEvents_ = 0;
    // the CPU finder's quads, with validateWithCPU
    std::vector<std::pair<std::array<int, 4>, mkfit::SeedQuad>> cpuQuads_;
  };

  void MkSeederAlpaka::configure(const mkfit::SeederConfig &cfg, const mkfit::TrackerInfo &ti) {
    seeder_ = std::make_unique<mkfit::MkSeeder>();
    seeder_->configure(cfg, ti);
    const auto &F = seeder_->finder(0);
    if (F.d_mode != 0 || F.fk_ot2 > 0 || F.C->hole_always)
      throw cms::Exception("Configuration") << "MkSeederAlpaka: d_mode 0, fk_ot2 0 and hole_always 0 only";
  }

  void MkSeederAlpaka::flatten(
      int side, Blob &blob, std::map<std::string, size_t> &offs, ChainConst &cc, std::vector<int> &layOf) {
    mkfit::SeedChainFinder &F = seeder_->finder(side);
    const mkfit::SeedChain &Ch = *F.C;
    const auto &L = seeder_->hits().layer_map();
    const int n = F.n;
    cc = ChainConst{};
    cc.n = n, cc.side = Ch.side, cc.nstarts = Ch.starts.size();
    cc.max_holes = Ch.max_holes, cc.max_holes_ot = Ch.max_holes_ot;
    cc.hs = Ch.start_holes < 0 ? Ch.max_holes : Ch.start_holes;
    cc.lead_only = Ch.lead_only, cc.inner_ot_only = Ch.inner_ot_only, cc.known_only = Ch.known_only;
    cc.hole_always = Ch.hole_always, cc.gap_map = Ch.gap_map != nullptr;
    cc.fk_on = F.fk_score > 0 || F.fk_shape, cc.fk_shape = F.fk_shape, cc.trip_on = F.trip_on;
    cc.fks = F.fk_score > 0 ? F.fk_score : 1e30f, cc.fk_score_fwd = F.fk_score_fwd, cc.fk_cot_fwd = F.fk_cot_fwd;
    cc.trip_lo = F.trip_lo, cc.trip_hi = F.trip_hi, cc.d0_max = F.d0_max, cc.pt_min = F.pt_min;
    const mkfit::SeedingParams &P = Ch.P;
    cc.inv2R = 0.003f * 3.8f / (2.0f * P.pt_min), cc.d0 = P.d0_max, cc.marg = P.marg_b;
    cc.P_pt_min = P.pt_min, cc.P_d0_max = P.d0_max, cc.P_zv = P.zv, cc.P_marg_b = P.marg_b;
    std::vector<int> order(Ch.order.begin(), Ch.order.end());
    layOf.assign(n, -1);
    for (int p = 0; p < n; ++p)
      if (L.count(order[p]))
        layOf[p] = order[p];
    {
      const mkfit::SeedLayerOfHits *any = L.empty() ? nullptr : L.begin()->second;
      const float bsz = any ? any->m_bs.z : 0.f;
      cc.zlo = bsz - P.zv, cc.zhi = bsz + P.zv;
    }
    std::vector<Env> env(n);
    for (int p = 0; p < n; ++p) {
      const auto &e = F.env_[p];
      env[p] = {e.ok, e.disc, e.pix, e.pos, e.plo, e.phi, e.lo_in, e.hi_in, e.lo_out, e.hi_out};
    }
    std::vector<Start> starts(cc.nstarts);
    std::vector<HolePos> holes;
    uint32_t a0 = 0;
    for (int si = 0; si < cc.nstarts; ++si) {
      Start &S = starts[si];
      S.pa = Ch.starts[si].first, S.pb = Ch.starts[si].second;
      S.gap_ok = Ch.start_gap_ok[si];
      S.gap_lid = F.gap_lid_.empty() ? -1 : F.gap_lid_[si];
      S.clo = F.start_cot_[si].first, S.chi = F.start_cot_[si].second;
      S.h0 = holes.size(), S.nh = F.hole_pos_[si].size();
      for (const auto &h : F.hole_pos_[si])
        holes.push_back({h.first, h.second ? 1 : 0});
      S.a0 = a0;
      if (layOf[S.pa] >= 0 && layOf[S.pb] >= 0 && S.clo <= S.chi)
        a0 += L.at(order[S.pa])->n();
    }
    cc.nwork = a0;
    std::vector<Par> par;
    for (const auto &q : F.par_)
      par.push_back({q.phi_c, q.q_c, q.aphi, q.bphi, q.aq, q.bq, q.sref, q.e0, q.ne});
    std::vector<EtaWin> etaw;
    for (const auto &e : F.etaw_)
      etaw.push_back({e.lo, e.hi, e.aphi, e.bphi, e.aq, e.bq});
    const std::string s = std::to_string(side);
    offs["order" + s] = blob.add(order);
    offs["layof" + s] = blob.add(layOf);
    offs["env" + s] = blob.add(env);
    offs["starts" + s] = blob.add(starts);
    offs["holes" + s] = blob.add(holes);
    offs["idxc" + s] = blob.add(Ch.idx_c_);
    offs["idxd" + s] = blob.add(Ch.idx_d_);
    offs["par" + s] = blob.add(par);
    offs["etaw" + s] = blob.add(etaw);
  }

  void MkSeederAlpaka::acquire(device::Event const &iEvent, device::EventSetup const &iSetup) {
    const auto &mkFitGeom = iSetup.getData(mkFitGeomToken_);
    if (mkFitGeom.isPhase1())
      throw cms::Exception("Configuration") << "MkSeederAlpaka: the mkFit seeder exists for phase 2 only";
    const auto &seederConfig = iSetup.getData(seederConfigToken_);
    const mkfit::TrackerInfo &ti = mkFitGeom.trackerInfo();
    if (!seeder_ || configuredFor_[0] != &mkFitGeom || configuredFor_[1] != &seederConfig) {
      configure(seederConfig, ti);
      configuredFor_[0] = &mkFitGeom, configuredFor_[1] = &seederConfig;
    }
    trackerInfo_ = &ti, seederConfig_ = &seederConfig;

    // the layers' hits, as MkFitSeederProducer
    const auto &pixelHits = iEvent.get(pixelHitsToken_);
    const auto &stripHits = iEvent.get(stripHitsToken_);
    const auto &pixelLayerIndex = iEvent.get(pixelLayerIndexToken_);
    const auto &stripLayerIndex = iEvent.get(stripLayerIndexToken_);
    const int nLayers = ti.n_layers();
    layerHits_.resize(nLayers);
    for (auto &v : layerHits_)
      v.clear();
    auto addHits = [&](const std::vector<int> &layerIndex, bool pixel) {
      for (unsigned int i = 0, n = layerIndex.size(); i < n; ++i) {
        const int l = layerIndex[i];
        if (l < 0)
          continue;
        if (l >= nLayers || ti[l].is_pixel() != pixel)
          throw cms::Exception("LogicError") << "MkSeederAlpaka: cluster " << i << " has mkFit layer " << l;
        layerHits_[l].push_back(i);
      }
    };
    addHits(pixelLayerIndex, true);
    addHits(stripLayerIndex, false);
    src_.assign(nLayers, {});
    for (int l = 0; l < nLayers; ++l)
      src_[l] = {ti[l].is_pixel() ? &pixelHits.hits() : &stripHits.hits(),
                 layerHits_[l].data(),
                 (unsigned int)layerHits_[l].size()};
    const auto &bs = iEvent.get(beamSpotToken_);
    const mkfit::BeamSpot mbs(bs.x0(), bs.y0(), bs.z0(), bs.sigmaZ(), bs.BeamWidthX(), bs.BeamWidthY(), bs.dxdz(), bs.dydz());
    const auto t0 = Clock::now();
    seeder_->fill(src_, mbs);
    const auto t1 = Clock::now();
    if (validate_) {
      mkfit::SeedCounters cnt;
      cpuQuads_.clear();
      seeder_->find(cpuQuads_, cnt);
    }

    // the blob: layers and hits, gap map, both chains
    Blob blob;
    std::map<std::string, size_t> offs;
    const auto &L = seeder_->hits().layer_map();
    std::vector<Layer> layers(nLayers, Layer{});
    std::vector<float> hphi, hz, hr, hinvr, hx, hy;
    std::vector<uint32_t> horig, hstart;
    std::vector<int> hspan;
    for (const auto &[id, Lp] : L) {
      const mkfit::SeedLayerOfHits &S = *Lp;
      Layer &Y = layers.at(id);
      Y.ok = 1, Y.disc = S.m_disc, Y.nq = S.n_q_bins(), Y.q_last = S.ax_q().m_last_N_bin;
      if (S.n_phi_bins() != (unsigned)Layer::kNPhi)
        throw cms::Exception("LogicError") << "MkSeederAlpaka: phi bins " << S.n_phi_bins();
      Y.phi_rmin = S.ax_phi().m_R_min, Y.phi_fac = S.ax_phi().m_N_fac;
      Y.q_rmin = S.ax_q().m_R_min, Y.q_fac = S.ax_q().m_N_fac, Y.q_lbhp = S.ax_q().m_N_lbhp;
      Y.qbar_lo = S.m_qbar_lo, Y.qbar_hi = S.m_qbar_hi, Y.q_lo = S.m_q_lo, Y.q_hi = S.m_q_hi;
      Y.bsx = S.m_bs.x, Y.bsy = S.m_bs.y, Y.bsz = S.m_bs.z, Y.bsdxdz = S.m_bs.dxdz, Y.bsdydz = S.m_bs.dydz;
      Y.hit0 = hphi.size(), Y.start0 = hstart.size(), Y.n = S.n();
      hphi.insert(hphi.end(), S.m_phi.begin(), S.m_phi.end());
      hz.insert(hz.end(), S.m_z.begin(), S.m_z.end());
      hr.insert(hr.end(), S.m_r.begin(), S.m_r.end());
      hinvr.insert(hinvr.end(), S.m_invr.begin(), S.m_invr.end());
      hx.insert(hx.end(), S.m_x.begin(), S.m_x.end());
      hy.insert(hy.end(), S.m_y.begin(), S.m_y.end());
      horig.insert(horig.end(), S.m_orig.begin(), S.m_orig.end());
      hspan.insert(hspan.end(), S.m_span.begin(), S.m_span.end());
      hstart.insert(hstart.end(), S.bin_starts().begin(), S.bin_starts().end());
    }
    offs["layers"] = blob.add(layers);
    offs["phi"] = blob.add(hphi), offs["z"] = blob.add(hz), offs["r"] = blob.add(hr);
    offs["invr"] = blob.add(hinvr), offs["x"] = blob.add(hx), offs["y"] = blob.add(hy);
    offs["orig"] = blob.add(horig), offs["span"] = blob.add(hspan), offs["start"] = blob.add(hstart);
    // the gap map of the barrel layers, and the event's ray tables
    const mkfit::SensorGapMap *gm = seeder_->finder(0).C->gap_map;
    std::vector<GapLayer> gap(4, GapLayer{});
    std::vector<float> glad, glz0, gray;
    std::vector<int> glnz, glbits, gstart, glidx;
    std::vector<unsigned long long> gbits;
    std::vector<unsigned char> gzgap, gedge;
    if (gm) {
      const mkfit::SeedLayerOfHits *any = L.empty() ? nullptr : L.begin()->second;
      for (int lid = 0; lid < 4; ++lid) {
        if (!gm->has_map(lid) || !any)
          continue;
        GapLayer &G = gap[lid];
        G.ok = 1, G.r_mid = gm->r_mid(lid), G.zg0 = gm->zgap_z0(lid), G.margin = gm->margin();
        G.nzg = gm->zgap(lid).size(), G.zgap0 = gzgap.size();
        gzgap.insert(gzgap.end(), gm->zgap(lid).begin(), gm->zgap(lid).end());
        std::vector<float> lad, z0;
        std::vector<int> nz, boff, st, li;
        std::vector<unsigned long long> bits;
        gm->export_layer(lid, lad, z0, nz, boff, bits, st, li);
        G.lad0 = glz0.size(), G.nlad = z0.size();
        for (auto &b : boff)
          b += gbits.size();
        glad.insert(glad.end(), lad.begin(), lad.end());
        glz0.insert(glz0.end(), z0.begin(), z0.end());
        glnz.insert(glnz.end(), nz.begin(), nz.end());
        glbits.insert(glbits.end(), boff.begin(), boff.end());
        gbits.insert(gbits.end(), bits.begin(), bits.end());
        G.start0 = gstart.size(), G.lidx0 = glidx.size();
        gstart.insert(gstart.end(), st.begin(), st.end());
        glidx.insert(glidx.end(), li.begin(), li.end());
        std::vector<float> r1, r2;
        std::vector<unsigned char> edge;
        gm->ray_table(lid, any->m_bs.x, any->m_bs.y, GapLayer::kNPhiRay, gm->margin() + 0.02f, r1, r2, edge);
        G.ray0 = gray.size();
        gray.insert(gray.end(), r1.begin(), r1.end());
        gray.insert(gray.end(), r2.begin(), r2.end());
        gedge.resize(G.ray0 + 2 * GapLayer::kNPhiRay, 0);
        std::copy(edge.begin(), edge.end(), gedge.begin() + G.ray0);
      }
    }
    offs["gap"] = blob.add(gap);
    offs["glad"] = blob.add(glad), offs["glz0"] = blob.add(glz0), offs["gray"] = blob.add(gray);
    offs["glnz"] = blob.add(glnz), offs["glbits"] = blob.add(glbits), offs["gstart"] = blob.add(gstart);
    offs["glidx"] = blob.add(glidx), offs["gbits"] = blob.add(gbits), offs["gzgap"] = blob.add(gzgap);
    offs["gedge"] = blob.add(gedge);
    // the shape bands (the same for both sides)
    std::vector<int> slo, shi;
    Shape shapes[4];
    for (int l = 0; l < 4; ++l) {
      const auto &T = seeder_->finder(0).shape_[l];
      shapes[l] = {T.ok() ? 1 : 0, (int)slo.size(), (int)T.lo.size(), T.inv_bw};
      slo.insert(slo.end(), T.lo.begin(), T.lo.end());
      shi.insert(shi.end(), T.hi.begin(), T.hi.end());
    }
    offs["slo"] = blob.add(slo), offs["shi"] = blob.add(shi);
    ChainConst cc[2];
    std::vector<int> layOf[2];
    for (int sd = 0; sd < 2; ++sd)
      flatten(sd, blob, offs, cc[sd], layOf[sd]);

    // to the device
    const auto t2 = Clock::now();
    auto &queue = iEvent.queue();
    auto hBlob = cms::alpakatools::make_host_buffer<std::byte[]>(queue, blob.data.size());
    std::memcpy(hBlob.data(), blob.data.data(), blob.data.size());
    dBlob_.emplace(cms::alpakatools::make_device_buffer<std::byte[]>(queue, blob.data.size()));
    alpaka::memcpy(queue, *dBlob_, hBlob);
    std::byte *base = dBlob_->data();
    auto at = [&](const std::string &k) { return base + offs.at(k); };
    View view[2];
    for (int sd = 0; sd < 2; ++sd) {
      const std::string s = std::to_string(sd);
      View &v = view[sd];
      v.C = cc[sd];
      v.layers = reinterpret_cast<const Layer *>(at("layers")), v.nLayers = nLayers;
      v.lay_of = reinterpret_cast<const int *>(at("layof" + s));
      v.order = reinterpret_cast<const int *>(at("order" + s));
      v.env = reinterpret_cast<const Env *>(at("env" + s));
      v.starts = reinterpret_cast<const Start *>(at("starts" + s));
      v.holes = reinterpret_cast<const HolePos *>(at("holes" + s));
      v.idx_c = reinterpret_cast<const int *>(at("idxc" + s));
      v.idx_d = reinterpret_cast<const int *>(at("idxd" + s));
      v.par = reinterpret_cast<const Par *>(at("par" + s));
      v.etaw = reinterpret_cast<const EtaWin *>(at("etaw" + s));
      for (int l = 0; l < 4; ++l)
        v.shape[l] = shapes[l];
      v.shape_lo = reinterpret_cast<const int *>(at("slo")), v.shape_hi = reinterpret_cast<const int *>(at("shi"));
      v.gap = reinterpret_cast<const GapLayer *>(at("gap"));
      v.gap_lad = reinterpret_cast<const float *>(at("glad"));
      v.gap_lz0 = reinterpret_cast<const float *>(at("glz0"));
      v.gap_ray = reinterpret_cast<const float *>(at("gray"));
      v.gap_lnz = reinterpret_cast<const int *>(at("glnz"));
      v.gap_lbits = reinterpret_cast<const int *>(at("glbits"));
      v.gap_start = reinterpret_cast<const int *>(at("gstart"));
      v.gap_lidx = reinterpret_cast<const int *>(at("glidx"));
      v.gap_bits = reinterpret_cast<const unsigned long long *>(at("gbits"));
      v.gap_zgap = reinterpret_cast<const unsigned char *>(at("gzgap"));
      v.gap_edge = reinterpret_cast<const unsigned char *>(at("gedge"));
      v.phi = reinterpret_cast<const float *>(at("phi"));
      v.z = reinterpret_cast<const float *>(at("z"));
      v.r = reinterpret_cast<const float *>(at("r"));
      v.invr = reinterpret_cast<const float *>(at("invr"));
      v.x = reinterpret_cast<const float *>(at("x"));
      v.y = reinterpret_cast<const float *>(at("y"));
      v.orig = reinterpret_cast<const uint32_t *>(at("orig"));
      v.span = reinterpret_cast<const int *>(at("span"));
      v.bin_start = reinterpret_cast<const uint32_t *>(at("start"));
    }

    for (int sd = 0; sd < 2; ++sd)
      view_[sd] = view[sd], layOf_[sd] = layOf[sd];
    nQueuesMax_ = 2 * std::max(cc[0].n, cc[1].n);
    blobBytes_ = blob.data.size();
    const uint32_t poolChunks =
        std::max((minCandidates_ + Queues::kChunk - 1) / Queues::kChunk, peakChunks_ + peakChunks_ / 4);
    launch(queue, poolChunks, std::max(minQuads_, peakQuads_ + peakQuads_ / 4));
    const auto t3 = Clock::now();
    auto ms = [](auto a, auto b) { return std::chrono::duration<double, std::milli>(b - a).count(); };
    tPart_[0] += ms(t0, t1), tPart_[1] += ms(t1, t2), tPart_[2] += ms(t2, t3);
  }

  void MkSeederAlpaka::launch(Queue &queue, uint32_t poolChunks, uint32_t outCap) {
    // the queues, reused by the two sides in turn; a queue can hold the whole pool
    const uint32_t nq = nQueuesMax_, mcpq = poolChunks;
    poolChunks_ = poolChunks, outCap_ = outCap;
    dPool_.emplace(cms::alpakatools::make_device_buffer<Cand[]>(queue, poolChunks * Queues::kChunk));
    dChunkTab_.emplace(cms::alpakatools::make_device_buffer<int[]>(queue, nq * mcpq + poolChunks));
    // stage c's found flags, one per candidate a queue can hold (cleared by KernelStageCMiss as it reads them)
    dFound_.emplace(cms::alpakatools::make_device_buffer<uint8_t[]>(queue, poolChunks * Queues::kChunk));
    alpaka::memset(queue, *dFound_, 0);
    dMissTgt_.emplace(cms::alpakatools::make_device_buffer<uint16_t[]>(queue, poolChunks * Queues::kChunk));
    // counters: [0, nq) count per queue, nq poolNext, nq + 1 freeCount, nq + 2 overflow, nq + 3 outCount,
    // nq + 4/5 pool chunks used per side
    dCounters_.emplace(cms::alpakatools::make_device_buffer<uint32_t[]>(queue, 2 * nq + 6));
    dOut_.emplace(cms::alpakatools::make_device_buffer<Quad[]>(queue, outCap));
    alpaka::memset(queue, *dCounters_, 0);
    uint32_t *cnt = dCounters_->data();
    Queues Q;
    Q.pool = dPool_->data(), Q.poolChunks = poolChunks, Q.maxChunksPerQueue = mcpq;
    Q.poolNext = cnt + nq, Q.freeCount = cnt + nq + 1, Q.count = cnt, Q.overflow = cnt + nq + 2;
    Q.chunkTab = dChunkTab_->data(), Q.freeList = dChunkTab_->data() + nq * mcpq, Q.found = dFound_->data();
    Q.missTgt = dMissTgt_->data(), Q.reserve = cnt + nq + 6;
    Q.out = dOut_->data(), Q.outCap = outCap, Q.outCount = cnt + nq + 3;
    for (int sd = 0; sd < 2; ++sd) {
      if (sd) {
        // the second side starts with empty queues and the whole pool
        auto counts = cms::alpakatools::make_device_view<uint32_t>(alpaka::getDev(queue), cnt, nq + 2);
        alpaka::memset(queue, counts, 0);
      }
      auto tab = cms::alpakatools::make_device_view<int>(alpaka::getDev(queue), dChunkTab_->data(), nq * mcpq);
      alpaka::memset(queue, tab, 0xff);
      mkseeder::runSide(queue, view_[sd], Q, layOf_[sd]);
      auto use = cms::alpakatools::make_device_view<uint32_t>(alpaka::getDev(queue), cnt + nq + 4 + sd, 1);
      auto next = cms::alpakatools::make_device_view<uint32_t>(alpaka::getDev(queue), cnt + nq, 1);
      alpaka::memcpy(queue, use, next);
    }
    hStats_.emplace(cms::alpakatools::make_host_buffer<uint32_t[]>(queue, 2 * nq + 6));
    alpaka::memcpy(queue, *hStats_, *dCounters_);
  }

  void MkSeederAlpaka::produce(device::Event &iEvent, device::EventSetup const &iSetup) {
    auto &queue = iEvent.queue();
    const uint32_t nq = nQueuesMax_;
    const auto p0 = Clock::now();
    // an event that overflowed a buffer runs again with twice its size (and the size the quads needed)
    for (int retry = 0; hStats_->data()[nq + 2] > 0; ++retry) {
      const uint32_t *st = hStats_->data();
      if (retry == 4)
        throw cms::Exception("MkSeederAlpaka") << "buffers still too small after 4 reruns";
      edm::LogInfo("MkSeederAlpaka") << st[nq + 2] << " candidates or quads lost to full buffers (pool " << poolChunks_
                                     << " chunks, quads " << outCap_ << "): rerun";
      const bool poolFull = std::max(st[nq + 4], st[nq + 5]) >= poolChunks_;
      launch(queue, poolFull ? 2 * poolChunks_ : poolChunks_, std::max(outCap_, st[nq + 3] + st[nq + 3] / 8 + 1024));
      alpaka::wait(queue);
    }
    const uint32_t *st = hStats_->data();
    peakChunks_ = std::max({peakChunks_, st[nq + 4], st[nq + 5]});
    peakQuads_ = std::max(peakQuads_, st[nq + 3]);
    const uint32_t nOut = st[nq + 3];
    if (printStats_)
      edm::LogPrint("MkSeederAlpaka") << "MkSeederAlpaka buffers: blob " << blobBytes_ / 1024 << " kB, pool "
                                      << poolChunks_ * Queues::kChunk * sizeof(Cand) / 1024 << " kB ("
                                      << poolChunks_ << " chunks, used " << st[nq + 4] << " / " << st[nq + 5]
                                      << "), chunk table + free list "
                                      << (nq * poolChunks_ + poolChunks_) * sizeof(int) / 1024 << " kB, quads "
                                      << outCap_ * sizeof(Quad) / 1024 << " kB (" << nOut << " used)";
    auto hOut = cms::alpakatools::make_host_buffer<Quad[]>(queue, std::max(nOut, 1u));
    if (nOut) {
      auto dv = cms::alpakatools::make_device_view<Quad>(alpaka::getDev(queue), dOut_->data(), nOut);
      auto hv = cms::alpakatools::make_host_view<Quad>(hOut.data(), nOut);
      alpaka::memcpy(queue, hv, dv);
      alpaka::wait(queue);
    }
    // in a fixed order (the device's is not): by layers, then hits
    std::vector<std::pair<std::array<int, 4>, mkfit::SeedQuad>> cq(nOut);
    std::vector<float> sc(nOut), fk(nOut);
    std::vector<uint32_t> ord(nOut);
    for (uint32_t i = 0; i < nOut; ++i) {
      const Quad &q = hOut.data()[i];
      cq[i] = {{q.layers[0], q.layers[1], q.layers[2], q.layers[3]}, {q.hits[0], q.hits[1], q.hits[2], q.hits[3]}};
      ord[i] = i;
    }
    std::sort(ord.begin(), ord.end(), [&](uint32_t a, uint32_t b) { return cq[a] < cq[b]; });
    std::vector<std::pair<std::array<int, 4>, mkfit::SeedQuad>> cqs(nOut);
    for (uint32_t i = 0; i < nOut; ++i)
      cqs[i] = cq[ord[i]], sc[i] = hOut.data()[ord[i]].score, fk[i] = hOut.data()[ord[i]].fake;

    if (validate_) {
      std::set<Key> dev, cpu;
      for (const auto &q : cqs)
        dev.insert(key_of(q.first, q.second));
      for (const auto &q : cpuQuads_)
        cpu.insert(key_of(q.first, q.second));
      long onlyDev = 0, onlyCpu = 0;
      for (const auto &k : dev)
        onlyDev += !cpu.count(k);
      for (const auto &k : cpu)
        onlyCpu += !dev.count(k);
      edm::LogPrint("MkSeederAlpaka") << "MkSeederAlpaka validation: device " << dev.size() << " (" << cqs.size()
                                      << " with repeats), CPU " << cpu.size() << " (" << cpuQuads_.size()
                                      << "), only device " << onlyDev << ", only CPU " << onlyCpu
                                      << "; pool chunks used " << st[nq + 4] << " / " << st[nq + 5];
    }

    const auto p1 = Clock::now();
    std::vector<mkfit::SeederQuad> quads;
    seeder_->finish(src_, cqs, sc, fk, quads);
    const auto p2 = Clock::now();
    mkfit::TrackVec seeds;
    std::vector<mkfit::SeedQuality> qualityByQuad;
    mkfit::SeedFitCounters fitCounters;
    mkfit::seeder_make_seeds(seederConfig_->fit, quads, src_, *trackerInfo_, algo_, seeds, &qualityByQuad, &fitCounters);
    {
      auto ms = [](auto a, auto b) { return std::chrono::duration<double, std::milli>(b - a).count(); };
      tPart_[3] += ms(p0, p1), tPart_[4] += ms(p1, p2), tPart_[5] += ms(p2, Clock::now());
      if (printStats_ && ++nEvents_ % 10 == 0)
        edm::LogPrint("MkSeederAlpaka") << "MkSeederAlpaka host ms/event over " << nEvents_ << " events: fill "
                                        << tPart_[0] / nEvents_ << ", blob " << tPart_[1] / nEvents_ << ", enqueue "
                                        << tPart_[2] / nEvents_ << ", wait + copy back " << tPart_[3] / nEvents_
                                        << ", cleaning " << tPart_[4] / nEvents_ << ", fit " << tPart_[5] / nEvents_;
    }
    dBlob_.reset(), dPool_.reset(), dChunkTab_.reset(), dCounters_.reset(), dOut_.reset(), dFound_.reset(), dMissTgt_.reset();
    if (seeds.size() > maxNSeeds_) {
      edm::LogError("TooManySeeds") << "Exceeded maximum number of seeds! maxNSeeds=" << maxNSeeds_
                                    << " nSeed=" << seeds.size();
      iEvent.emplace(putToken_, mkfit::TrackVec(), std::vector<mkfit::SeedQuality>());
      return;
    }
    std::vector<mkfit::SeedQuality> quality(seeds.size());
    for (unsigned int i = 0; i < seeds.size(); ++i) {
      quality[i] = qualityByQuad[seeds[i].label()];
      seeds[i].setLabel(i);
    }
    iEvent.emplace(putToken_, std::move(seeds), std::move(quality));
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(MkSeederAlpaka);
