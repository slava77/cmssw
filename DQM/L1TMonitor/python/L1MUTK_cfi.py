import FWCore.ParameterSet.Config as cms

l1MuTk = cms.EDAnalyzer("L1MUTK",
    disableROOToutput = cms.untracked.bool(True),
    verbose = cms.untracked.bool(False),
    gmtSource = cms.InputTag("gtDigis"),
<<<<<<< HEAD
    DQMStore = cms.untracked.bool(True),
    candInputTag = cms.InputTag("L1TkMuonsMerge")
)
=======
    DQMStore = cms.untracked.bool(True)
)


>>>>>>> f1500e5... add interface/L1MUTK.h python/L1MUTK_cfi.py python/candMaker_cfi.py src/L1MUTK.cc src/L1MuTkCandMaker.cc
