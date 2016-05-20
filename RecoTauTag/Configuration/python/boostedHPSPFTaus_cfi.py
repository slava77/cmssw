import FWCore.ParameterSet.Config as cms

def addBoostedTaus(process):
    from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
    from PhysicsTools.PatAlgos.tools.helpers import massSearchReplaceAnyInputTag

    #most modules are needed: use load
    process.load("RecoTauTag.Configuration.boostedHPSPFTaus_cff")

    #do selective load here and clearnup after
    import RecoTauTag.Configuration.RecoPFTauTag_cff as _RecoPFTauTag
    import PhysicsTools.PatAlgos.patSequences_cff as _patSequences
    process.PATTauSequence = cms.Sequence(_RecoPFTauTag.PFTau+_patSequences.makePatTaus+_patSequences.selectedPatTaus)
    process.PATTauSequenceBoosted = cloneProcessingSnippet(process,process.PATTauSequence, "Boosted")
    del process.PATTauSequence

    process.recoTauAK4PFJets08RegionBoosted.src = cms.InputTag('boostedTauSeeds')
    process.recoTauAK4PFJets08RegionBoosted.pfCandSrc = cms.InputTag('particleFlow')
    process.recoTauAK4PFJets08RegionBoosted.pfCandAssocMapSrc = cms.InputTag('boostedTauSeeds', 'pfCandAssocMapForIsolation')
    process.ak4PFJetsLegacyHPSPiZerosBoosted.jetSrc = cms.InputTag('boostedTauSeeds')
    process.ak4PFJetsRecoTauChargedHadronsBoosted.jetSrc = cms.InputTag('boostedTauSeeds')
    process.ak4PFJetsRecoTauChargedHadronsBoosted.builders[1].dRcone = cms.double(0.3)
    process.ak4PFJetsRecoTauChargedHadronsBoosted.builders[1].dRconeLimitedToJetArea = cms.bool(True)
    process.combinatoricRecoTausBoosted.jetSrc = cms.InputTag('boostedTauSeeds')
    process.combinatoricRecoTausBoosted.modifiers.remove(process.combinatoricRecoTausBoosted.modifiers[3])
    #process.combinatoricRecoTausBoosted.builders[0].pfCandSrc = cms.InputTag('pfNoPileUpForBoostedTaus')
    process.combinatoricRecoTausBoosted.builders[0].pfCandSrc = cms.InputTag('particleFlow')
    massSearchReplaceAnyInputTag(process.PATTauSequenceBoosted,cms.InputTag("ak4PFJets"),cms.InputTag("boostedTauSeeds"))  
    process.slimmedTausBoosted = process.slimmedTaus.clone(src = cms.InputTag("selectedPatTausBoosted"))
    
    return process
