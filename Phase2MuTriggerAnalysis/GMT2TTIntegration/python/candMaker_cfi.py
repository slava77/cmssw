import FWCore.ParameterSet.Config as cms

candMaker = cms.EDProducer(
    "LeafCandMaker",
    aliasPrefix = cms.untracked.string("cands"),
    candInputTag = cms.InputTag("")
    )
