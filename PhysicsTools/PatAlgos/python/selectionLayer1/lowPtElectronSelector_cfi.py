import FWCore.ParameterSet.Config as cms

# module to select Electrons
# See https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePhysicsCutParser
# on how to use the cut-string

# By default, no low-pT electrons are stored in MINIAOD
selectedPatLowPtElectrons = cms.EDFilter("PATElectronSelector",
    src = cms.InputTag("patLowPtElectrons"),
    cut = cms.string(""),
)

# Modifier for bParking (order is important)
from Configuration.Eras.Modifier_bParking_cff import bParking
bParking.toModify(selectedPatLowPtElectrons,cut = "")

# Modifier for run2_miniAOD_UL (order is important: overrides bParking)
from Configuration.ProcessModifiers.run2_miniAOD_UL_cff import run2_miniAOD_UL
run2_miniAOD_UL.toModify(selectedPatLowPtElectrons,cut = "pt>1. && electronID('ID')>1.5")

# Modifier for run2_miniAOD_devel (order is important: overrides run2_miniAOD_UL)
from Configuration.Eras.Modifier_run2_miniAOD_devel_cff import run2_miniAOD_devel
run2_miniAOD_devel.toModify(selectedPatLowPtElectrons,cut = "pt>1. && electronID('ID')>-0.25")

# Modifiers for legacy AOD
from Configuration.Eras.Modifier_run2_miniAOD_80XLegacy_cff import run2_miniAOD_80XLegacy
from Configuration.Eras.Modifier_run2_miniAOD_94XFall17_cff import run2_miniAOD_94XFall17
(run2_miniAOD_80XLegacy | run2_miniAOD_94XFall17).toModify(selectedPatLowPtElectrons, 
                                                           cut = "pt>99999."
                                                           )
