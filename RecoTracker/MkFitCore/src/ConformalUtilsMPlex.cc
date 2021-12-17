#include "ConformalUtilsMPlex.h"
#include "RecoTracker/MkFitCore/interface/Track.h"
#include "RecoTracker/MkFitCore/interface/Hit.h"

//#define DEBUG
#include "Debug.h"

namespace mkfit {

  inline void CFMap(const MPlexHH& A, const MPlexHV& B, MPlexHV& C) {
    using idx_t = Matriplex::idx_t;

    // C = A * B, C is 3x1, A is 3x3 , B is 3x1

    typedef float T;
    typedef float Tv;
    const idx_t N = NN;

    const T* a = A.fArray;
    ASSUME_ALIGNED(a, 64);
    const Tv* b = B.fArray;
    ASSUME_ALIGNED(b, 64);
    Tv* c = C.fArray;
    ASSUME_ALIGNED(c, 64);

#include "CFMatrix33Vector3.ah"
  }

  //M. Hansroul, H. Jeremie and D. Savard, NIM A 270 (1988) 498
  //http://www.sciencedirect.com/science/article/pii/016890028890722X

  void conformalFitMPlex(bool fitting,
                         MPlexQI seedID,
                         MPlexLS& outErr,
                         MPlexLV& outPar,
                         const MPlexHV& msPar0,
                         const MPlexHV& msPar1,
                         const MPlexHV& msPar2) {
    bool debug(false);

    using idx_t = Matriplex::idx_t;
    const idx_t N = NN;

    // Store positions in mplex vectors... could consider storing in a 3x3 matrix, too
    MPlexHV x, y, z, r2;
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      x.At(n, 0, 0) = msPar0.ConstAt(n, 0, 0);
      x.At(n, 1, 0) = msPar1.ConstAt(n, 0, 0);
      x.At(n, 2, 0) = msPar2.ConstAt(n, 0, 0);

      y.At(n, 0, 0) = msPar0.ConstAt(n, 1, 0);
      y.At(n, 1, 0) = msPar1.ConstAt(n, 1, 0);
      y.At(n, 2, 0) = msPar2.ConstAt(n, 1, 0);

      z.At(n, 0, 0) = msPar0.ConstAt(n, 2, 0);
      z.At(n, 1, 0) = msPar1.ConstAt(n, 2, 0);
      z.At(n, 2, 0) = msPar2.ConstAt(n, 2, 0);

      for (int i = 0; i < 3; ++i) {
        r2.At(n, i, 0) = getRad2(x.ConstAt(n, i, 0), y.ConstAt(n, i, 0));
      }
    }

    // Start setting the output parameters
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      outPar.At(n, 0, 0) = x.ConstAt(n, 0, 0);
      outPar.At(n, 1, 0) = y.ConstAt(n, 0, 0);
      outPar.At(n, 2, 0) = z.ConstAt(n, 0, 0);
    }

    // Use r-phi smearing to set initial error estimation for positions
    // trackStates already initialized to identity for seeding ... don't store off-diag 0's, zero's for fitting set outside CF
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      const float varPhi = Config::varXY / r2.ConstAt(n, 0, 0);
      const float invvarR2 = Config::varR / r2.ConstAt(n, 0, 0);

      outErr.At(n, 0, 0) =
          x.ConstAt(n, 0, 0) * x.ConstAt(n, 0, 0) * invvarR2 + y.ConstAt(n, 0, 0) * y.ConstAt(n, 0, 0) * varPhi;
      outErr.At(n, 0, 1) = x.ConstAt(n, 0, 0) * y.ConstAt(n, 0, 0) * (invvarR2 - varPhi);

      outErr.At(n, 1, 0) = outErr.ConstAt(n, 0, 1);
      outErr.At(n, 1, 1) =
          y.ConstAt(n, 0, 0) * y.ConstAt(n, 0, 0) * invvarR2 + x.ConstAt(n, 0, 0) * x.ConstAt(n, 0, 0) * varPhi;

      outErr.At(n, 2, 2) = Config::varZ;
    }

    MPlexQF initPhi;
    MPlexQI xtou;  // bool to determine "split space", i.e. map x to u or v
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      initPhi.At(n, 0, 0) = std::abs(getPhi(x.ConstAt(n, 0, 0), y.ConstAt(n, 0, 0)));
      xtou.At(n, 0, 0) =
          ((initPhi.ConstAt(n, 0, 0) < Config::PIOver4 || initPhi.ConstAt(n, 0, 0) > Config::PI3Over4) ? 1 : 0);
    }

    MPlexHV u, v;
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      if (xtou.At(n, 0, 0))  // x mapped to u
      {
        for (int i = 0; i < 3; ++i) {
          u.At(n, i, 0) = x.ConstAt(n, i, 0) / r2.ConstAt(n, i, 0);
          v.At(n, i, 0) = y.ConstAt(n, i, 0) / r2.ConstAt(n, i, 0);
        }
      } else  // x mapped to v
      {
        for (int i = 0; i < 3; ++i) {
          u.At(n, i, 0) = y.ConstAt(n, i, 0) / r2.ConstAt(n, i, 0);
          v.At(n, i, 0) = x.ConstAt(n, i, 0) / r2.ConstAt(n, i, 0);
        }
      }
    }

    MPlexHH A;
    //#pragma omp simd // triggers an internal compiler error with icc 18.0.2!
    for (int n = 0; n < N; ++n) {
      for (int i = 0; i < 3; ++i) {
        A.At(n, i, 0) = 1.0f;
        A.At(n, i, 1) = -u.ConstAt(n, i, 0);
        A.At(n, i, 2) = -u.ConstAt(n, i, 0) * u.ConstAt(n, i, 0);
      }
    }
    Matriplex::InvertCramer(A);
    MPlexHV C;
    CFMap(A, v, C);

    MPlexQF a, b;
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      b.At(n, 0, 0) = 1.0f / (2.0f * C.ConstAt(n, 0, 0));
      a.At(n, 0, 0) = b.ConstAt(n, 0, 0) * C.ConstAt(n, 1, 0);
    }

    // constant used throughtout
    const float k = (Config::sol * Config::Bfield) / 100.0f;

    MPlexQF vrx, vry, pT, px, py, pz;
#pragma omp simd
    for (int n = 0; n < N; ++n) {
      vrx.At(n, 0, 0) =
          (xtou.ConstAt(n, 0, 0) ? x.ConstAt(n, 0, 0) - a.ConstAt(n, 0, 0) : x.ConstAt(n, 0, 0) - b.ConstAt(n, 0, 0));
      vry.At(n, 0, 0) =
          (xtou.ConstAt(n, 0, 0) ? y.ConstAt(n, 0, 0) - b.ConstAt(n, 0, 0) : y.ConstAt(n, 0, 0) - a.ConstAt(n, 0, 0));
      pT.At(n, 0, 0) = k * hipo(vrx.ConstAt(n, 0, 0), vry.ConstAt(n, 0, 0));
      px.At(n, 0, 0) = std::copysign(k * vry.ConstAt(n, 0, 0), x.ConstAt(n, 2, 0) - x.ConstAt(n, 0, 0));
      py.At(n, 0, 0) = std::copysign(k * vrx.ConstAt(n, 0, 0), y.ConstAt(n, 2, 0) - y.ConstAt(n, 0, 0));
      pz.At(n, 0, 0) = (pT.ConstAt(n, 0, 0) * (z.ConstAt(n, 2, 0) - z.ConstAt(n, 0, 0))) /
                       hipo((x.ConstAt(n, 2, 0) - x.ConstAt(n, 0, 0)), (y.ConstAt(n, 2, 0) - y.ConstAt(n, 0, 0)));
    }

#pragma omp simd
    for (int n = 0; n < N; ++n) {
      outPar.At(n, 3, 0) = 1.0f / pT.ConstAt(n, 0, 0);
      outPar.At(n, 4, 0) = getPhi(px.ConstAt(n, 0, 0), py.ConstAt(n, 0, 0));
      outPar.At(n, 5, 0) = getTheta(pT.ConstAt(n, 0, 0), pz.ConstAt(n, 0, 0));
#ifdef INWARDFIT  // arctan is odd, so pz -> -pz means theta -> -theta
      if (fitting)
        outPar.At(n, 5, 0) *= -1.0f;
#endif
    }

#pragma omp simd
    for (int n = 0; n < N; ++n) {
      outErr.At(n, 3, 3) =
          (fitting ? Config::ptinverr049 * Config::ptinverr049 : Config::ptinverr012 * Config::ptinverr012);
      outErr.At(n, 4, 4) = (fitting ? Config::phierr049 * Config::phierr049 : Config::phierr012 * Config::phierr012);
      outErr.At(n, 5, 5) =
          (fitting ? Config::thetaerr049 * Config::thetaerr049 : Config::thetaerr012 * Config::thetaerr012);
    }

    if (debug) {
      for (int n = 0; n < N; ++n) {
        dprintf("afterCF seedID: %1u \n", seedID.ConstAt(n, 0, 0));
        // do a dumb copy out
        TrackState updatedState;
        for (int i = 0; i < 6; i++) {
          updatedState.parameters[i] = outPar.ConstAt(n, i, 0);
          for (int j = 0; j < 6; j++) {
            updatedState.errors[i][j] = outErr.ConstAt(n, i, j);
          }
        }

        dcall(print("CCS", updatedState));
        updatedState.convertFromCCSToCartesian();
        dcall(print("Pol", updatedState));
        dprint("--------------------------------");
      }
    }
  }

}  // end namespace mkfit
