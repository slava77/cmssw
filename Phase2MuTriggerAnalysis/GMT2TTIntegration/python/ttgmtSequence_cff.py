import FWCore.ParameterSet.Config as cms

from Phase2MuTriggerAnalysis.GMT2TTIntegration.ttMaker_cfi import *

ttgmtSequence = cms.Sequence(ttMaker)
