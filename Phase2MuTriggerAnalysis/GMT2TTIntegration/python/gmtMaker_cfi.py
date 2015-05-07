import FWCore.ParameterSet.Config as cms

gmtMaker = cms.EDProducer(
    "GMTMaker",
    aliasPrefix = cms.untracked.string("gmts"),
    gmtInputTag = cms.InputTag("simGmtDigis")
    )
