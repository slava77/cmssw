import FWCore.ParameterSet.Config as cms

from Phase2MuTriggerAnalysis.GMT2TTIntegration.ttMaker_cfi import *
from Phase2MuTriggerAnalysis.GMT2TTIntegration.simtMaker_cfi import *
from Phase2MuTriggerAnalysis.GMT2TTIntegration.gmtMaker_cfi import *
from Phase2MuTriggerAnalysis.GMT2TTIntegration.csctfMaker_cfi import *

from Phase2MuTriggerAnalysis.GMT2TTIntegration.l1muparticles_cff import *
from Phase2MuTriggerAnalysis.GMT2TTIntegration.l1tkmuparticles_cff import *

from Phase2MuTriggerAnalysis.GMT2TTIntegration.idxByDRMakers_cff import *



ttgmtSequence = cms.Sequence(ttMaker + simtMaker + gmtMaker + csctfMaker + l1muMakers + l1tkmuMakers + idxByDRMakers )
