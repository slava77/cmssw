import FWCore.ParameterSet.Config as cms

RPMulTrackNonParallelCandCollFit = cms.EDProducer("RPMulTrackCandidateCollectionFitter",
    Verbosity = cms.int32(0),
    readReconstructedPatterns = cms.bool(True),
#    reconstructedPatternsInstance = cms.string('RPSinglTrackCandFind')
    reconstructedPatternsInstance = cms.string('NonParallelTrackFinder'),
    RPMulTrackCandidateCollectionLabel = cms.InputTag("NonParallelTrackFinder")

)
