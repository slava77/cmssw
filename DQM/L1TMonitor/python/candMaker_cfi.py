import FWCore.ParameterSet.Config as cms

candMaker = cms.EDProducer(
    "L1MuTkCandMaker",
    aliasPrefix = cms.untracked.string("cands"),
    candInputTag = cms.InputTag("")
    )
