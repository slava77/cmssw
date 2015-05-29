import FWCore.ParameterSet.Config as cms

l1MuTk = cms.EDAnalyzer("L1MUTK",
    disableROOToutput = cms.untracked.bool(True),
    verbose = cms.untracked.bool(False),
    gmtSource = cms.InputTag("gtDigis"),
    DQMStore = cms.untracked.bool(True)
)


