import FWCore.ParameterSet.Config as cms
import RecoTracker.IterativeTracking.iterativeTkConfig as _cfg
from RecoTracker.IterativeTracking.PixelLessStep_cff import *
from HIPixelTripletSeeds_cff import *
from HIPixel3PrimTracks_cfi import *

##########################################################################
# Large impact parameter tracking using TIB/TID/TEC stereo layer seeding #
##########################################################################

hiPixelLessStepClusters = cms.EDProducer("HITrackClusterRemover",
     clusterLessSolution = cms.bool(True),
     trajectories = cms.InputTag("hiMixedTripletStepTracks"),
     overrideTrkQuals = cms.InputTag('hiMixedTripletStepSelector','hiMixedTripletStep'),
     TrackQuality = cms.string('highPurity'),
     minNumberOfLayersWithMeasBeforeFiltering = cms.int32(0),
     pixelClusters = cms.InputTag("siPixelClusters"),
     stripClusters = cms.InputTag("siStripClusters"),
     Common = cms.PSet(
         maxChi2 = cms.double(9.0),
     ),
     Strip = cms.PSet(
        #Yen-Jie's mod to preserve merged clusters
        maxSize = cms.uint32(2),
        maxChi2 = cms.double(9.0)
     )
)

# SEEDING LAYERS
pixelLessStepSeedLayers.TIB.skipClusters   = cms.InputTag('hiPixelLessStepClusters')
pixelLessStepSeedLayers.MTIB.skipClusters   = cms.InputTag('hiPixelLessStepClusters')
pixelLessStepSeedLayers.TID.skipClusters   = cms.InputTag('hiPixelLessStepClusters')
pixelLessStepSeedLayers.MTID.skipClusters   = cms.InputTag('hiPixelLessStepClusters')
pixelLessStepSeedLayers.TEC.skipClusters   = cms.InputTag('hiPixelLessStepClusters')
pixelLessStepSeedLayers.MTEC.skipClusters   = cms.InputTag('hiPixelLessStepClusters')

# TrackingRegion
from RecoTracker.TkTrackingRegions.globalTrackingRegionWithVertices_cfi import globalTrackingRegionWithVertices as _globalTrackingRegionWithVertices
hiPixelLessStepTrackingRegions = _globalTrackingRegionWithVertices.clone(RegionPSet=dict(
     precise = True,
     useMultipleScattering = False,
     beamSpot = "offlineBeamSpot",
     useFoundVertices = True,
     useFakeVertices       = False,
     VertexCollection = "hiSelectedPixelVertex",
     useFixedError = True,
     fixedError = 3.0,#12.0
     ptMin = 0.7, #0.4
     originRadius = 1.0,
     originRScaling4BigEvts = cms.bool(True),
     halfLengthScaling4BigEvts = cms.bool(False),
     ptMinScaling4BigEvts = cms.bool(True),
     minOriginR = 0,
     minHalfLength = 0,
     maxPtMin = 1.0,#0.7
     scalingStartNPix = 20000,
     scalingEndNPix = 35000     
))

# seeding
pixelLessStepHitDoublets.clusterCheck = ""
pixelLessStepHitDoublets.trackingRegions = "hiPixelLessStepTrackingRegions"

# QUALITY CUTS DURING TRACK BUILDING
from RecoTracker.IterativeTracking.PixelLessStep_cff import _pixelLessStepTrajectoryFilterBase
_pixelLessStepTrajectoryFilterBase.minimumNumberOfHits = 5
_pixelLessStepTrajectoryFilterBase.minPt = 0.7

# TRACK BUILDING

# MAKING OF TRACK CANDIDATES
pixelLessStepTrackCandidates.clustersToSkip = cms.InputTag('hiPixelLessStepClusters')

# TRACK FITTING
hiPixelLessStepTracks = pixelLessStepTracks.clone()

# Final selection
import RecoHI.HiTracking.hiMultiTrackSelector_cfi
hiPixelLessStepSelector = RecoHI.HiTracking.hiMultiTrackSelector_cfi.hiMultiTrackSelector.clone(
    src='hiPixelLessStepTracks',
    useAnyMVA = cms.bool(False),
    GBRForestLabel = cms.string('HIMVASelectorIter12'),
    GBRForestVars = cms.vstring(['chi2perdofperlayer', 'nhits', 'nlayers', 'eta']),
    trackSelectors= cms.VPSet(
    RecoHI.HiTracking.hiMultiTrackSelector_cfi.hiLooseMTS.clone(
    name = 'hiPixelLessStepLoose',
    applyAdaptedPVCuts = cms.bool(False),
    useMVA = cms.bool(False),
    ), #end of pset
    RecoHI.HiTracking.hiMultiTrackSelector_cfi.hiTightMTS.clone(
    name = 'hiPixelLessStepTight',
    preFilterName = 'hiPixelLessStepLoose',
    applyAdaptedPVCuts = cms.bool(False),
    useMVA = cms.bool(False),
    minMVA = cms.double(-0.2)
    ),
    RecoHI.HiTracking.hiMultiTrackSelector_cfi.hiHighpurityMTS.clone(
    name = 'hiPixelLessStep',
    preFilterName = 'hiPixelLessStepTight',
    applyAdaptedPVCuts = cms.bool(False),
    useMVA = cms.bool(False),
    minMVA = cms.double(-0.09)
    ),
    ) #end of vpset
    ) #end of clone

import RecoTracker.FinalTrackSelectors.trackListMerger_cfi
hiPixelLessStepQual = RecoTracker.FinalTrackSelectors.trackListMerger_cfi.trackListMerger.clone(
    TrackProducers=cms.VInputTag(cms.InputTag('hiPixelLessStepTracks')),
    hasSelector=cms.vint32(1),
    selectedTrackQuals = cms.VInputTag(cms.InputTag("hiPixelLessStepSelector","hiPixelLessStep")),
    copyExtras = True,
    makeReKeyedSeeds = cms.untracked.bool(False),
    )

hiPixelLessStep = cms.Sequence(hiPixelLessStepClusters*
                             pixelLessStepSeedLayers*
                             hiPixelLessStepTrackingRegions*
                             pixelLessStepHitDoublets*
                             pixelLessStepHitTriplets*
                             pixelLessStepSeeds*
                             pixelLessStepTrackCandidates*
                             hiPixelLessStepTracks*
                             hiPixelLessStepSelector*
                             hiPixelLessStepQual
                             )