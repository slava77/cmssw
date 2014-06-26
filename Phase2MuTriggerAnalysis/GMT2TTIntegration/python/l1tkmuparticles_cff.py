import FWCore.ParameterSet.Config as cms

from Phase2MuTriggerAnalysis.GMT2TTIntegration.candMaker_cfi import candMaker

l1tkmugmtsCandMaker = candMaker.clone(aliasPrefix = "l1tkmugmts", candInputTag = "l1TkMuonsExt")
l1tkmucsctfsCandMaker = candMaker.clone(aliasPrefix = "l1tkmucsctfs", candInputTag = "l1TkMuonsExtCSC")
l1tkmusCandMaker = candMaker.clone(aliasPrefix = "l1tkmus", candInputTag = "L1TkMuonsMerge")


l1tkmuMakers = cms.Sequence(l1tkmugmtsCandMaker + l1tkmucsctfsCandMaker + l1tkmusCandMaker)
