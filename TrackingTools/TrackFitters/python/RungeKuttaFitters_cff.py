import FWCore.ParameterSet.Config as cms

import TrackingTools.TrackFitters.KFTrajectoryFitter_cfi
RKTrajectoryFitter = TrackingTools.TrackFitters.KFTrajectoryFitter_cfi.KFTrajectoryFitter.clone(
    ComponentName = 'RKFitter',
    Propagator    = 'RungeKuttaTrackerPropagator'
)
RKTrajectoryFitterNoOutliers = RKTrajectoryFitter.clone(
    ComponentName = 'RKFitterNoOutliers',
    maxEstimate = 100,
    firstHitScale = 0.01
)

import TrackingTools.TrackFitters.KFTrajectorySmoother_cfi
RKTrajectorySmoother = TrackingTools.TrackFitters.KFTrajectorySmoother_cfi.KFTrajectorySmoother.clone(
    ComponentName = 'RKSmoother',
    Propagator    = 'RungeKuttaTrackerPropagator'
)

import TrackingTools.TrackFitters.KFFittingSmoother_cfi
RKFittingSmoother = TrackingTools.TrackFitters.KFFittingSmoother_cfi.KFFittingSmoother.clone(
    ComponentName = 'RKFittingSmoother',
    Fitter        = 'RKFitter',
    Smoother      = 'RKSmoother',
    MinNumberOfHits = 3
)

KFFittingSmootherWithOutliersRejectionAndRK = RKFittingSmoother.clone(
    ComponentName   = 'KFFittingSmootherWithOutliersRejectionAndRK',
    EstimateCut     = 20.0,
    MinNumberOfHits = 3
)
KFFittingNoOutliersSmootherWithOutliersRejectionAndRK = KFFittingSmootherWithOutliersRejectionAndRK.clone(
    ComponentName = "KFFittingNoOutliersSmootherWithOutliersRejectionAndRK",
    Fitter = "RKFitterNoOutliers"
)

import TrackingTools.TrackFitters.FlexibleKFFittingSmoother_cfi as _mod
FlexibleKFFittingNoOutliersSmoother = _mod.FlexibleKFFittingSmoother.clone(
    ComponentName = "FlexibleKFFittingNoOutliersSmoother",
    standardFitter = "KFFittingNoOutliersSmootherWithOutliersRejectionAndRK"
)
