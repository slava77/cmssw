#ifndef RecoTracker_MkFitCore_interface_Hit_h
#define RecoTracker_MkFitCore_interface_Hit_h

#include <cmath>
#include <unordered_set>

#include "RecoTracker/MkFitCore/interface/Config.h"
#include "RecoTracker/MkFitCore/interface/MatrixSTypes.h"
#include <atomic>
#include <array>

namespace mkfit {

  template <typename T>
  inline T sqr(T x) {
    return x * x;
  }
  template <typename T>
  inline T cube(T x) {
    return x * x * x;
  }

  inline float squashPhiGeneral(float phi) {
    return phi - floor(0.5 * Config::InvPI * (phi + Config::PI)) * Config::TwoPI;
  }

  inline float squashPhiMinimal(float phi) {
    return phi >= Config::PI ? phi - Config::TwoPI : (phi < -Config::PI ? phi + Config::TwoPI : phi);
  }

  // moved from config to here
  inline int getEtaBin(float eta) {
    if (std::isfinite(eta) == 0)
      return -1;

    //first and last bin have extra width
    if (eta < (Config::lEtaBin - Config::fEtaDet))
      return 0;
    if (eta > (Config::fEtaDet - Config::lEtaBin))
      return Config::nEtaBin - 1;

    //now we can treat all bins as if they had same size
    return int((eta + Config::fEtaDet - Config::lEtaBin / 2.0f) / Config::lEtaBin);
  }

  inline int getEtaBinExtendedEdge(float eta) {
    if (std::isfinite(eta) == 0)
      return -1;
    //in this case we are out of bounds
    if (std::abs(eta) > Config::fEtaDet + Config::lEtaPart)
      return -1;

    //first and last bin have extra width
    if (eta < (Config::lEtaBin - Config::fEtaDet))
      return 0;
    if (eta > (Config::fEtaDet - Config::lEtaBin))
      return Config::nEtaBin - 1;

    //now we can treat all bins as if they had same size
    return int((eta + Config::fEtaDet - Config::lEtaBin / 2.0f) / Config::lEtaBin);
  }

  inline float getRad2(float x, float y) { return x * x + y * y; }

  inline float getInvRad2(float x, float y) { return 1.0f / (x * x + y * y); }

  inline float getPhi(float x, float y) { return std::atan2(y, x); }

  inline float getTheta(float r, float z) { return std::atan2(r, z); }

  inline float getEta(float r, float z) { return -1.0f * std::log(std::tan(getTheta(r, z) / 2.0f)); }

  inline float getEta(float theta) { return -1.0f * std::log(std::tan(theta / 2.0f)); }

  inline float getEta(float x, float y, float z) {
    const float theta = std::atan2(std::sqrt(x * x + y * y), z);
    return -1.0f * std::log(std::tan(theta / 2.0f));
  }

  inline float getHypot(float x, float y) { return std::sqrt(x * x + y * y); }

  inline float getRadErr2(float x, float y, float exx, float eyy, float exy) {
    return (x * x * exx + y * y * eyy + 2.0f * x * y * exy) / getRad2(x, y);
  }

  inline float getInvRadErr2(float x, float y, float exx, float eyy, float exy) {
    return (x * x * exx + y * y * eyy + 2.0f * x * y * exy) / cube(getRad2(x, y));
  }

  inline float getPhiErr2(float x, float y, float exx, float eyy, float exy) {
    const float rad2 = getRad2(x, y);
    //  const float dphidx = -y/rad2;
    //  const float dphidy =  x/rad2;
    //  return dphidx*dphidx*exx + dphidy*dphidy*eyy + 2*dphidx*dphidy*exy;
    return (y * y * exx + x * x * eyy - 2.0f * x * y * exy) / (rad2 * rad2);
  }

  inline float getThetaErr2(
      float x, float y, float z, float exx, float eyy, float ezz, float exy, float exz, float eyz) {
    const float rad2 = getRad2(x, y);
    const float rad = std::sqrt(rad2);
    const float hypot2 = rad2 + z * z;
    const float dthetadx = x * z / (rad * hypot2);
    const float dthetady = y * z / (rad * hypot2);
    const float dthetadz = -rad / hypot2;
    return dthetadx * dthetadx * exx + dthetady * dthetady * eyy + dthetadz * dthetadz * ezz +
           2.0f * dthetadx * dthetady * exy + 2.0f * dthetadx * dthetadz * exz + 2.0f * dthetady * dthetadz * eyz;
  }

  inline float getEtaErr2(float x, float y, float z, float exx, float eyy, float ezz, float exy, float exz, float eyz) {
    const float rad2 = getRad2(x, y);
    const float detadx = -x / (rad2 * std::sqrt(1 + rad2 / (z * z)));
    const float detady = -y / (rad2 * std::sqrt(1 + rad2 / (z * z)));
    const float detadz = 1.0f / (z * std::sqrt(1 + rad2 / (z * z)));
    return detadx * detadx * exx + detady * detady * eyy + detadz * detadz * ezz + 2.0f * detadx * detady * exy +
           2.0f * detadx * detadz * exz + 2.0f * detady * detadz * eyz;
  }

  inline float getPxPxErr2(float ipt, float phi, float vipt, float vphi) {  // ipt = 1/pT, v = variance
    const float iipt2 = 1.0f / (ipt * ipt);                                 //iipt = 1/(1/pT) = pT
    const float cosP = std::cos(phi);
    const float sinP = std::sin(phi);
    return iipt2 * (iipt2 * cosP * cosP * vipt + sinP * sinP * vphi);
  }

  inline float getPyPyErr2(float ipt, float phi, float vipt, float vphi) {  // ipt = 1/pT, v = variance
    const float iipt2 = 1.0f / (ipt * ipt);                                 //iipt = 1/(1/pT) = pT
    const float cosP = std::cos(phi);
    const float sinP = std::sin(phi);
    return iipt2 * (iipt2 * sinP * sinP * vipt + cosP * cosP * vphi);
  }

  inline float getPzPzErr2(float ipt, float theta, float vipt, float vtheta) {  // ipt = 1/pT, v = variance
    const float iipt2 = 1.0f / (ipt * ipt);                                     //iipt = 1/(1/pT) = pT
    const float cotT = 1.0f / std::tan(theta);
    const float cscT = 1.0f / std::sin(theta);
    return iipt2 * (iipt2 * cotT * cotT * vipt + cscT * cscT * cscT * cscT * vtheta);
  }

  struct MCHitInfo {
    MCHitInfo() {}
    MCHitInfo(int track, int layer, int ithlayerhit, int mcHitID)
        : mcTrackID_(track), layer_(layer), ithLayerHit_(ithlayerhit), mcHitID_(mcHitID) {}

    int mcTrackID_;
    int layer_;
    int ithLayerHit_;
    int mcHitID_;

    int mcTrackID() const { return mcTrackID_; }
    int layer() const { return layer_; }
    int mcHitID() const { return mcHitID_; }
    static void reset();
  };
  typedef std::vector<MCHitInfo> MCHitInfoVec;

  struct MeasurementState {
  public:
    MeasurementState() {}
    MeasurementState(const SVector3& p, const SVector6& e) : pos_(p), err_(e) {}
    MeasurementState(const SVector3& p, const SMatrixSym33& e) : pos_(p) {
      for (int i = 0; i < 6; ++i)
        err_[i] = e.Array()[i];
    }
    const SVector3& parameters() const { return pos_; }
    SMatrixSym33 errors() const {
      SMatrixSym33 result;
      for (int i = 0; i < 6; ++i)
        result.Array()[i] = err_[i];
      return result;
    }
    SVector3 pos_;
    SVector6 err_;
  };

  class Hit {
  public:
    Hit() : mcHitID_(-1) {}

    Hit(const SVector3& position, const SMatrixSym33& error, int mcHitID = -1)
        : state_(position, error), mcHitID_(mcHitID) {}

    const SVector3& position() const { return state_.parameters(); }
    const SVector3& parameters() const { return state_.parameters(); }
    const SMatrixSym33 error() const { return state_.errors(); }

    const float* posArray() const { return state_.pos_.Array(); }
    const float* errArray() const { return state_.err_.Array(); }

    // Non-const versions needed for CopyOut of Matriplex.
    SVector3& parameters_nc() { return state_.pos_; }
    SVector6& error_nc() { return state_.err_; }

    float r() const {
      return sqrtf(state_.parameters().At(0) * state_.parameters().At(0) +
                   state_.parameters().At(1) * state_.parameters().At(1));
    }
    float x() const { return state_.parameters().At(0); }
    float y() const { return state_.parameters().At(1); }
    float z() const { return state_.parameters().At(2); }
    float exx() const { return state_.errors().At(0, 0); }
    float eyy() const { return state_.errors().At(1, 1); }
    float ezz() const { return state_.errors().At(2, 2); }
    float phi() const { return getPhi(state_.parameters().At(0), state_.parameters().At(1)); }
    float eta() const {
      return getEta(state_.parameters().At(0), state_.parameters().At(1), state_.parameters().At(2));
    }
    float ephi() const { return getPhiErr2(x(), y(), exx(), eyy(), state_.errors().At(0, 1)); }
    float eeta() const {
      return getEtaErr2(x(),
                        y(),
                        z(),
                        exx(),
                        eyy(),
                        ezz(),
                        state_.errors().At(0, 1),
                        state_.errors().At(0, 2),
                        state_.errors().At(1, 2));
    }

    const MeasurementState& measurementState() const { return state_; }

    int mcHitID() const { return mcHitID_; }
    int layer(const MCHitInfoVec& globalMCHitInfo) const { return globalMCHitInfo[mcHitID_].layer(); }
    int mcTrackID(const MCHitInfoVec& globalMCHitInfo) const { return globalMCHitInfo[mcHitID_].mcTrackID(); }

    struct PackedData {
      union {
        struct {
          unsigned int detid_in_layer : 12;
          unsigned int charge_pcm : 8;  // MIMI see set/get funcs: is this Run2 specific ???
          unsigned int span_rows : 3;
          unsigned int span_cols : 3;
        };
        unsigned int _raw_;
      };

      PackedData() : _raw_(0) {}

      void set_charge_pcm(int cpcm) {
        if (cpcm < 1620)
          charge_pcm = 0;
        else
          charge_pcm = std::min(0xff, ((cpcm - 1620) >> 3) + 1);
      }
      unsigned int get_charge_pcm() const {
        if (charge_pcm == 0)
          return 0;
        else
          return ((charge_pcm - 1) << 3) + 1620;
      }
    };

    unsigned int detIDinLayer() const { return pdata_.detid_in_layer; }
    unsigned int chargePerCM() const { return pdata_.get_charge_pcm(); }
    unsigned int spanRows() const { return pdata_.span_rows + 1; }
    unsigned int spanCols() const { return pdata_.span_cols + 1; }

    static unsigned int minChargePerCM() { return 1620; }
    static unsigned int maxChargePerCM() { return 1620 + (0xfe << 3); }
    static unsigned int maxSpan() { return 8; }

    void setupAsPixel(unsigned int id, int rows, int cols) {
      pdata_.detid_in_layer = id;
      pdata_.charge_pcm = 0xff;
      pdata_.span_rows = std::min(0x7, rows - 1);
      pdata_.span_cols = std::min(0x7, cols - 1);
    }

    void setupAsStrip(unsigned int id, int cpcm, int rows) {
      pdata_.detid_in_layer = id;
      pdata_.set_charge_pcm(cpcm);
      pdata_.span_rows = std::min(0x7, rows - 1);
    }

  private:
    MeasurementState state_;
    int mcHitID_;
    PackedData pdata_;
  };

  typedef std::vector<Hit> HitVec;
  typedef std::map<int, std::map<int, std::vector<int> > > LayIdxIDVecMapMap;
  typedef std::map<int, std::unordered_set<int> > TrkIDLaySetMap;
  typedef std::array<int, 2> PairIdx;
  typedef std::vector<PairIdx> PairIdxVec;
  typedef std::array<int, 3> TripletIdx;
  typedef std::vector<TripletIdx> TripletIdxVec;

  struct HitOnTrack {
    int index : 24;
    int layer : 8;

    HitOnTrack() : index(-1), layer(-1) {}
    HitOnTrack(int i, int l) : index(i), layer(l) {}

    bool operator<(const HitOnTrack o) const {
      if (layer != o.layer)
        return layer < o.layer;
      return index < o.index;
    }
  };

  typedef std::vector<HitOnTrack> HoTVec;

  void print(std::string label, const MeasurementState& s);

  struct DeadRegion {
    float phi1, phi2, q1, q2;
    DeadRegion(float a1, float a2, float b1, float b2) : phi1(a1), phi2(a2), q1(b1), q2(b2) {}
  };
  typedef std::vector<DeadRegion> DeadVec;

  struct BeamSpot {
    float x = 0, y = 0, z = 0;
    float sigmaZ = 5;
    float beamWidthX = 5e-4, beamWidthY = 5e-4;
    float dxdz = 0, dydz = 0;

    BeamSpot() = default;
    BeamSpot(float ix, float iy, float iz, float is, float ibx, float iby, float idxdz, float idydz)
        : x(ix), y(iy), z(iz), sigmaZ(is), beamWidthX(ibx), beamWidthY(iby), dxdz(idxdz), dydz(idydz) {}
  };
}  // end namespace mkfit
#endif
