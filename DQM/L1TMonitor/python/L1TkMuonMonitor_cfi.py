import FWCore.ParameterSet.Config as cms

L1TkMuonMonitor = cms.EDAnalyzer("L1TkMuonMonitor",
    disableROOToutput = cms.untracked.bool(True),
    verbose = cms.untracked.bool(False),
    gmtSource = cms.InputTag("gtDigis"),
    DQMStore = cms.untracked.bool(True),
    candInputTag = cms.InputTag("L1TkMuonsMerge")
)
