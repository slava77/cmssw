import FWCore.ParameterSet.Config as cms

RPMulTrackRoadSearchedCandCollFit = cms.EDProducer("RPMulTrackCandidateCollectionFitter",
    Verbosity = cms.int32(0),
    readReconstructedPatterns = cms.bool(True),
    reconstructedPatternsInstance = cms.string('RPSinglTrackCandFind'),
    RPMulTrackCandidateCollectionLabel = cms.InputTag("RPMulTrackCandFind")
#    reconstructedPatternsInstance = cms.string('NonParallelTrackFinder')
)
