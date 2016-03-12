import FWCore.ParameterSet.Config as cms

RPMulTrackCandCollFit = cms.EDProducer("RPMulTrackCandidateCollectionFitter",
    Verbosity = cms.int32(0),

    RPMulTrackCandidateCollectionLabel = cms.InputTag("RPMulTrackCandFind")
)
