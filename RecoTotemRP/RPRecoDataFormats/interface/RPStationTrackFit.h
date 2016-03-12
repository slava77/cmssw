/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *  Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPStationTrackFit_h
#define RecoTotemRP_RPRecoDataFormats_RPStationTrackFit_h

#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"

#include "TMatrixD.h"

#include <cmath>

/**
 *\brief A result of a track fit through a set of RPs (= station).
 * 
 * Track is described as
 *   x(z) = ax * (z - z0) + bx, y(z) likewise
 **/
class RPStationTrackFit
{
  public:
    /// track parameters
    double ax, ay, bx, by;
    double z0;

    /// covariance matrix for the vector (ax, ay, bx, by)
    TMatrixD covarianceMatrix;

    /// fit uncertainty of the ax parameter
    inline double ax_unc()
    {
      return sqrt(covarianceMatrix(0, 0));
    }

    inline double ay_unc()
    {
      return sqrt(covarianceMatrix(1, 1));
    }

    inline double bx_unc()
    {
      return sqrt(covarianceMatrix(2, 2));
    }

    inline double by_unc()
    {
      return sqrt(covarianceMatrix(3, 3));
    }

    /// fit parameters
    double chiSq;
    double ndf;

    /// fit valid?
    bool valid;

    /// list of contributing hits
    std::vector<RPRecoHit> hits;

    RPStationTrackFit();
};

#endif

