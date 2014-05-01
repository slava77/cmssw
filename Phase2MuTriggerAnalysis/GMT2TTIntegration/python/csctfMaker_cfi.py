import FWCore.ParameterSet.Config as cms

csctfMaker = cms.EDProducer(
    "CSCTFMaker",
    aliasPrefix = cms.untracked.string("csctfs"),
    gmtROInputTag = cms.InputTag("simGmtDigis"),
    csctfInputTag = cms.InputTag("simCsctfTrackDigis")
    )
