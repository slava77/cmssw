#ifndef HeterogeneousCore_AlpakaInterface_interface_geomFunctions_h
#define HeterogeneousCore_AlpakaInterface_interface_geomFunctions_h

namespace cms::alpakatools {

  template <typename TAcc>
  ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE float eta(TAcc const& acc, const float x, const float y, const float z) {
    float r3 = alpaka::math::sqrt(acc, x * x + y * y + z * z);
    float rt = alpaka::math::sqrt(acc, x * x + y * y);
    float eta = ((z > 0) - (z < 0)) * alpaka::math::acosh(acc, r3 / rt);
    return eta;
  };

  template <typename TAcc>
  ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE float phi_mpi_pi(TAcc const& acc, const float x) {
    if (alpaka::math::abs(acc, x) <= float(M_PI))
      return x;

    constexpr float o2pi = 1.f / (2.f * float(M_PI));
    float n = alpaka::math::round(acc, x * o2pi);
    return x - n * float(2.f * float(M_PI));
  };

  template <typename TAcc>
  ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE float phi(TAcc const& acc, const float x, const float y) {
    return phi_mpi_pi(acc, float(M_PI) + alpaka::math::atan2(acc, -y, -x));
  };

  template <typename TAcc>
  ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE float deltaPhi(TAcc const& acc, const float x1, const float y1, const float x2, const float y2) {
    float phi1 = phi(acc, x1, y1);
    float phi2 = phi(acc, x2, y2);
    return phi_mpi_pi(acc, (phi2 - phi1));
  };

  template <typename TAcc>
  ALPAKA_FN_HOST_ACC ALPAKA_FN_INLINE float deltaPhiChange(TAcc const& acc, const float x1, const float y1, const float x2, const float y2) {
    return deltaPhi(acc, x1, y1, x2 - x1, y2 - y1);
  };

  ALPAKA_FN_ACC ALPAKA_FN_INLINE float calculate_dPhi(const float phi1, const float phi2) {
    // Calculate dPhi
    float dPhi = phi1 - phi2;

    // Normalize dPhi to be between -pi and pi
    if (dPhi > float(M_PI)) {
      dPhi -= 2 * float(M_PI);
    } else if (dPhi < -float(M_PI)) {
      dPhi += 2 * float(M_PI);
    }

    return dPhi;
  };

}  // namespace cms::alpakatools

#endif  // HeterogeneousCore_AlpakaInterface_interface_geomFunctions_h
