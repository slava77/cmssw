import FWCore.ParameterSet.Config as cms

from Phase2MuTriggerAnalysis.GMT2TTIntegration.candMaker_cfi import candMaker

l1muCandMaker = candMaker.clone(aliasPrefix = "l1mus", candInputTag = "l1extraParticles")
l1muExtendedCandMaker = candMaker.clone(aliasPrefix = "l1muexts", candInputTag = "l1extraMuExtended")
l1muExtendedCSCCandMaker = candMaker.clone(aliasPrefix = "l1muextcscs", candInputTag = "l1extraMuExtended:csc")


l1muMakers = cms.Sequence(l1muCandMaker + l1muExtendedCandMaker + l1muExtendedCSCCandMaker)
