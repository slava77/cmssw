/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef RecoTotemRP_RPTrackCandidateFitter_RPTrackCandidateFitter_h
#define RecoTotemRP_RPTrackCandidateFitter_RPTrackCandidateFitter_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "RecoTotemRP/RPDetectorMaskingService/interface/RPDetectorAvailability.h"
#include "Geometry/TotemRPDetTopology/interface/RPTopology.h"
#include "DataFormats/TotemRPDataTypes/interface/RPMathUtils.h"

#include "TVector3.h"
#include "TVector2.h"

#include <ext/hash_map>


struct RPDetCoordinateAlgebraObjs
{
  //u*nom_pitch = (u0*pitch+u_cor)+(pitch/eff_pitch*rot_cor*v)*(x-x0)
  //             eff_u_middle_edge +    eff_v*(x-middle_of_edge_pos)
  //       (eff_u_middle_edge-eff_v*middle_of_edge_pos) + eff_v*x
  TVector3 centre_of_det_global_position_;
  double rec_u_0_;              ///< in mm, position of det. centre projected on readout direction
  TVector2 readout_direction_;  ///< non paralell projection and rot_cor included
  bool available_;              ///< if det should be included in the reconstruction
};



/**
 *\brief Fits a RPTrackCandidate, results in RPFittedTrack.
 **/
class RPTrackCandidateFitter
{
  public:
    RPTrackCandidateFitter(const edm::ParameterSet &conf);

    /// performs the track fit, returns true if successful
    bool FitTrack(const RPTrackCandidate& track_cand, double z_0, RPFittedTrack &fitted_track,
      const TotemRPGeometry &tot_geom);

    /// Resets the reconstruction-data cache.
    void Reset();

#if 0
    TVector2 ComputeXYPointInZDir(const RPRecoHit& hit_0, const RPRecoHit& hit_1,
      const TotemRPGeometry &tot_geom);

    TVector2 ComputeXYPointOfTheGivenLine(const RPRecoHit& hit_0, const RPRecoHit& hit_1,
      double tx, double ty, double z0, const TotemRPGeometry &tot_geom);
#endif

  private:
    /// A cache of reconstruction data. Must be reset every time the geometry chagnges.
    typedef __gnu_cxx::hash_map<unsigned int, RPDetCoordinateAlgebraObjs> DetReconstructionDataMap;
    DetReconstructionDataMap det_data_map_;

    RPDetectorAvailability det_availability_;
    RPTopology rp_topology_;

    /// Returns the reconstruction data for the chosen detector from the cache DetReconstructionDataMap.
    /// If it is not yet in the cache, calls PrepareReconstAlgebraData to make it.
    RPDetCoordinateAlgebraObjs *GetDetAlgebraData(unsigned int det_id, const TotemRPGeometry &tot_rp_geom);

    /// Build the reconstruction data.
    RPDetCoordinateAlgebraObjs PrepareReconstAlgebraData(unsigned int det_id, const TotemRPGeometry &tot_rp_geom);

    /// A matrix multiplication shorthand.
    void MultiplyByDiagonalInPlace(TMatrixD &mt, const TVectorD &diag);
    
};


#endif

