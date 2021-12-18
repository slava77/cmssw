#ifndef RecoTracker_MkFitCore_src_Matrix_h
#define RecoTracker_MkFitCore_src_Matrix_h

#include "RecoTracker/MkFitCore/interface/Config.h"
#include "RecoTracker/MkFitCore/interface/MatrixSTypes.h"

// This should go elsewhere, eventually.

#ifdef __cpp_lib_clamp
using std::clamp;
#else
template <class T, class Compare>
inline constexpr const T clamp(const T v, const T lo, const T hi, Compare comp) {
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template <class T>
inline constexpr const T clamp(const T v, const T lo, const T hi) {
  return clamp(v, lo, hi, std::less<T>());
}
#endif

#include <sys/time.h>

namespace mkfit {

  inline double dtime() {
    double tseconds = 0.0;
    struct timeval mytime;
    gettimeofday(&mytime, (struct timezone*)nullptr);
    tseconds = (double)(mytime.tv_sec + mytime.tv_usec * 1.0e-6);
    return (tseconds);
  }

  inline float hipo(float x, float y) { return std::sqrt(x * x + y * y); }

  inline float hipo_sqr(float x, float y) { return x * x + y * y; }

  inline void sincos4(const float x, float& sin, float& cos) {
    // Had this writen with explicit division by factorial.
    // The *whole* fitting test ran like 2.5% slower on MIC, sigh.

    const float x2 = x * x;
    cos = 1.f - 0.5f * x2 + 0.04166667f * x2 * x2;
    sin = x - 0.16666667f * x * x2;
  }
}  // end namespace mkfit

//==============================================================================

// Matriplex dimensions and typedefs

#ifdef __INTEL_COMPILER
#define ASSUME_ALIGNED(a, b) __assume_aligned(a, b)
#else
#define ASSUME_ALIGNED(a, b) a = static_cast<decltype(a)>(__builtin_assume_aligned(a, b))
#endif

#include "Matriplex/MatriplexSym.h"

namespace mkfit {

  constexpr Matriplex::idx_t NN = MPT_SIZE;  // "Length" of MPlex.

  constexpr Matriplex::idx_t LL = 6;  // Dimension of large/long  MPlex entities
  constexpr Matriplex::idx_t HH = 3;  // Dimension of small/short MPlex entities

  typedef Matriplex::Matriplex<float, LL, LL, NN> MPlexLL;
  typedef Matriplex::Matriplex<float, LL, 1, NN> MPlexLV;
  typedef Matriplex::MatriplexSym<float, LL, NN> MPlexLS;

  typedef Matriplex::Matriplex<float, HH, HH, NN> MPlexHH;
  typedef Matriplex::Matriplex<float, HH, 1, NN> MPlexHV;
  typedef Matriplex::MatriplexSym<float, HH, NN> MPlexHS;

  typedef Matriplex::Matriplex<float, 2, 2, NN> MPlex22;
  typedef Matriplex::Matriplex<float, 2, 1, NN> MPlex2V;
  typedef Matriplex::MatriplexSym<float, 2, NN> MPlex2S;

  typedef Matriplex::Matriplex<float, LL, HH, NN> MPlexLH;
  typedef Matriplex::Matriplex<float, HH, LL, NN> MPlexHL;

  typedef Matriplex::Matriplex<float, LL, 2, NN> MPlexL2;

  typedef Matriplex::Matriplex<float, 1, 1, NN> MPlexQF;
  typedef Matriplex::Matriplex<int, 1, 1, NN> MPlexQI;
  typedef Matriplex::Matriplex<unsigned int, 1, 1, NN> MPlexQUI;

  typedef Matriplex::Matriplex<bool, 1, 1, NN> MPlexQB;

}  // end namespace mkfit

//==============================================================================

#include <random>

namespace mkfit {
  extern std::default_random_engine g_gen;
  extern std::normal_distribution<float> g_gaus;
  extern std::uniform_real_distribution<float> g_unif;
}  // end namespace mkfit

#endif
