import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.slimming.slimmedLowPtElectrons_cfi import slimmedLowPtElectrons
from PhysicsTools.PatAlgos.slimming.lowPtGsfLinks_cfi import lowPtGsfLinks
from RecoEgamma.EgammaElectronProducers.lowPtGsfElectronID_cfi import lowPtGsfElectronID

# Task
slimmedLowPtElectronsTask = cms.Task(
    lowPtGsfLinks,
    slimmedLowPtElectrons,
)

# Modifiers for BParking
#from Configuration.Eras.Modifier_bParking_cff import bParking
#
#patLowPtGsfElectronID = lowPtGsfElectronID.clone(
#    electrons=cms.InputTag('patLowPtElectrons'),
#    rho=cms.InputTag('fixedGridRhoFastjetAll'),
#    unbiased=cms.InputTag(''), # instead, use embedded value from userData('unbiased')
#    ModelWeights=cms.vstring(['RecoEgamma/ElectronIdentification/data/LowPtElectrons/LowPtElectrons_ID_2020Sept15.root']),
#    Version=cms.string('V1'),
#)
#slimmedLowPtElectronsTask_ = slimmedLowPtElectronsTask.copy()
#slimmedLowPtElectronsTask_.add(patLowPtGsfElectronID)
#bParking.toReplaceWith(slimmedLowPtElectronsTask,slimmedLowPtElectronsTask_)
#
#slimmedLowPtElectrons_ = slimmedLowPtElectrons.clone()
#slimmedLowPtElectrons_.modifierConfig.modifications.append(
#    cms.PSet(
#        electron_config = cms.PSet(
#            ID = cms.InputTag("patLowPtGsfElectronID"),
#            electronSrc = cms.InputTag("patLowPtElectrons"),
#        ),
#        modifierName = cms.string('EGExtraInfoModifierFromFloatValueMaps'),
#        photon_config = cms.PSet()
#    )
#)
#bParking.toReplaceWith(slimmedLowPtElectrons,slimmedLowPtElectrons_)
