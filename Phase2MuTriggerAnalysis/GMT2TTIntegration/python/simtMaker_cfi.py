import FWCore.ParameterSet.Config as cms

simtMaker = cms.EDProducer(
    "SimTVMaker",
    aliasPrefix = cms.untracked.string("sims"),
    simtInputTag = cms.InputTag("g4SimHits"),
    simvInputTag = cms.InputTag("g4SimHits"),
    saveOnlyMuons = cms.bool(True)
    )
