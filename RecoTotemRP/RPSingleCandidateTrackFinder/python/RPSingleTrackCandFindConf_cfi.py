import FWCore.ParameterSet.Config as cms

RPSinglTrackCandFind = cms.EDProducer("RPSingleCandidateTrackFinder",
    Verbosity = cms.int32(1),

    # width of bins in u or v histograms
    RoadSize = cms.double(0.2),

    # planes with higher hit-multiplicity than below are omitted
    MaxHitsPerDetector = cms.int32(3),

    # a pattern (= road) is considered valid if it has at least this number of hits (in each projection)
    MinHitsPerCoord = cms.int32(4),

    # if set to true: weight of a hit = 1/N_of_hits_in_the_plane
    # if set to false: weight of a hit = 1
    ReduceWeightsWithMultiplicity = cms.bool(True),
    
    RPRecoHitLabel = cms.InputTag("RPRecoHitProd")
)
