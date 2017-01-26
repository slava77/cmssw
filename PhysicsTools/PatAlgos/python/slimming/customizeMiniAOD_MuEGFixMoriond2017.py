import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.helpers import MassSearchReplaceAnyInputTagVisitor, listDependencyChain, massSearchReplaceAnyInputTag, cloneProcessingSnippet

def addBadMuonFilters(process):
    process.load("RecoMET.METFilters.badGlobalMuonTaggersAOD_cff")
    process.Flag_noBadMuons = cms.Path(process.noBadGlobalMuons)

def cleanPFCandidates(process, badMuons, verbose=False):
    process.load("CommonTools.ParticleFlow.muonsCleaned_cfi")
    process.patMuons.userData.userInts.src = [ cms.InputTag("muonsCleaned:oldPF") ]

    process.load("CommonTools.ParticleFlow.pfCandidatesBadMuonsCleaned_cfi")
    process.muonsCleaned.badmuons = badMuons[:]
    replaceMuons = MassSearchReplaceAnyInputTagVisitor("muons", "muonsCleaned", verbose=verbose)
    needOriginalMuons = [ process.muonsCleaned ] + [ getattr(process,l.moduleLabel) for l in badMuons ]
    replacePFCandidates = MassSearchReplaceAnyInputTagVisitor("particleFlow", "pfCandidatesBadMuonsCleaned", verbose=verbose)
    for everywhere in [ process.producers, process.filters, process.analyzers, process.psets, process.vpsets ]:
        for name,obj in everywhere.iteritems():
            if obj not in needOriginalMuons:
                replaceMuons.doIt(obj, name)
            if obj != process.pfCandidatesBadMuonsCleaned: 
                replacePFCandidates.doIt(obj, name)
            
    process.load("CommonTools.ParticleFlow.pfEGammaToCandidateRemapper_cfi")
    process.pfEGammaToCandidateRemapper.pf2pf = cms.InputTag("pfCandidatesBadMuonsCleaned")
    process.reducedEgamma.gsfElectronsPFValMap = cms.InputTag("pfEGammaToCandidateRemapper","electrons")
    process.reducedEgamma.photonsPFValMap      = cms.InputTag("pfEGammaToCandidateRemapper","photons")


def addKeepStatement(process, oldKeep, newKeeps, verbose=False):
    for name,out in process.outputModules.iteritems():
        if out.type_() == 'PoolOutputModule' and hasattr(out, "outputCommands"):
            if oldKeep in out.outputCommands:
                out.outputCommands += newKeeps
            if verbose:
                print "Adding the following keep statements to output module %s: " % name
                for k in newKeeps: print "\t'%s'," % k

def addDiscardedPFCandidates(process, inputCollection, verbose=False):
    process.primaryVertexAssociationDiscardedCandidates = process.primaryVertexAssociation.clone(
        particles = inputCollection,
        )
    process.packedPFCandidatesDiscarded = process.packedPFCandidates.clone(
        inputCollection = inputCollection,
        PuppiNoLepSrc = cms.InputTag(""),
        PuppiSrc = cms.InputTag(""),
        secondaryVerticesForWhiteList = cms.VInputTag(),
        vertexAssociator = cms.InputTag("primaryVertexAssociationDiscardedCandidates","original")
        )
    addKeepStatement(process, "keep patPackedCandidates_packedPFCandidates_*_*",
                             ["keep patPackedCandidates_packedPFCandidatesDiscarded_*_*"],
                              verbose=verbose)
    # Now make the mixed map for rekeying
    from PhysicsTools.PatAlgos.slimming.packedPFCandidateRefMixer_cfi import packedPFCandidateRefMixer
    process.oldPFCandToPackedOrDiscarded = packedPFCandidateRefMixer.clone(
        pf2pf = cms.InputTag(inputCollection.moduleLabel),
        pf2packed = cms.VInputTag(cms.InputTag("packedPFCandidates"), cms.InputTag("packedPFCandidatesDiscarded"))
    )

def loadJetMETBTag(process):
    import RecoJets.Configuration.RecoPFJets_cff
    process.ak4PFJetsCHS = RecoJets.Configuration.RecoPFJets_cff.ak4PFJetsCHS.clone()
    process.ak8PFJetsCHS = RecoJets.Configuration.RecoPFJets_cff.ak8PFJetsCHS.clone()
    process.load("RecoMET.METProducers.PFMET_cfi")
    process.load("RecoBTag.ImpactParameter.impactParameter_cff")
    process.load("RecoBTag.SecondaryVertex.secondaryVertex_cff")
    process.load("RecoBTag.SoftLepton.softLepton_cff")
    process.load("RecoBTag.Combined.combinedMVA_cff")
    process.load("RecoBTag.CTagging.cTagging_cff")
    process.load("RecoVertex.AdaptiveVertexFinder.inclusiveVertexing_cff")

def customizeAll(process, verbose=False):

    # MM first duplicate uncorrected sequences needed by JetMET
    cloneProcessingSnippet(process, getattr(process,"makePatPhotons"),"UnClean")
    cloneProcessingSnippet(process, getattr(process,"makePatElectrons"),"UnClean")
    
    process.load("RecoEgamma.EgammaTools.egammaGainSwitchFix_cff")

    loadJetMETBTag(process)

    process.originalAK4JetSequence = listDependencyChain(process, process.slimmedJets, ('particleFlow', 'muons'))
    backupAK4JetSequence = cloneProcessingSnippet(process, process.originalAK4JetSequence, "BackupTmp")
    process.originalAK4PuppiJetSequence = listDependencyChain(process, process.slimmedJetsPuppi, ('particleFlow', 'muons'))
    backupAK4PuppiJetSequence = cloneProcessingSnippet(process, process.originalAK4PuppiJetSequence, "BackupTmp")
    process.originalAK8JetSequence = listDependencyChain(process, process.slimmedJetsAK8, ('particleFlow', 'muons'))
    backupAK8JetSequence = cloneProcessingSnippet(process, process.originalAK8JetSequence, "Backup")

    addBadMuonFilters(process)    

    # clean the muons and PF candidates, and make *everything* point to the new candidates
    cleanPFCandidates(process, 
                      [ cms.InputTag("badGlobalMuonTagger","bad"), cms.InputTag("cloneGlobalMuonTagger","bad") ],
                      verbose=verbose) 

    addDiscardedPFCandidates(process, cms.InputTag("pfCandidatesBadMuonsCleaned","discarded"), verbose=verbose)

    # now make the backup sequences point to the right place
    massSearchReplaceAnyInputTag(backupAK4JetSequence, "pfCandidatesBadMuonsCleaned", "particleFlow")
    massSearchReplaceAnyInputTag(backupAK4JetSequence, "muonsCleaned", "muons")
    massSearchReplaceAnyInputTag(backupAK4PuppiJetSequence, "pfCandidatesBadMuonsCleaned", "particleFlow")
    massSearchReplaceAnyInputTag(backupAK4PuppiJetSequence, "muonsCleaned", "muons")
    massSearchReplaceAnyInputTag(backupAK8JetSequence, "pfCandidatesBadMuonsCleaned", "particleFlow")
    massSearchReplaceAnyInputTag(backupAK8JetSequence, "muonsCleaned", "muons")
    # fix a few names
    process.patJetsAK8Backup.userData.userFloats.labelPostfixesToStrip = cms.vstring("Backup",)
    process.patJetsAK8PuppiBackup.userData.userFloats.labelPostfixesToStrip = cms.vstring("Backup",)

    # for these we can keep the daughters
    process.slimmedJetsBackupTmp.mixedDaughters = True
    process.slimmedJetsBackupTmp.packedPFCandidates = cms.InputTag("oldPFCandToPackedOrDiscarded")
    process.slimmedJetsAK8PFCHSSoftDropSubjetsBackup.mixedDaughters = True
    process.slimmedJetsAK8PFCHSSoftDropSubjetsBackup.packedPFCandidates = cms.InputTag("oldPFCandToPackedOrDiscarded")
    # for these we can't
    process.slimmedJetsPuppiBackupTmp.dropDaughters = '1'
    process.slimmedJetsAK8PFPuppiSoftDropSubjetsBackup.dropDaughters = '1'
    # for these we do even if we wouldn't have done in the standard case, since we couldn't for the subjets
    process.packedPatJetsAK8Backup.fixDaughters = False
    process.slimmedJetsAK8Backup.rekeyDaughters = '1'
    process.slimmedJetsAK8Backup.mixedDaughters = True
    process.slimmedJetsAK8Backup.packedPFCandidates = cms.InputTag("oldPFCandToPackedOrDiscarded")
    
    process.patMuons.embedCaloMETMuonCorrs = False # FIXME
    ##extra METs and MET corrections ===============================================================
    from PhysicsTools.PatAlgos.slimming.extraSlimmedMETs_MuEGFixMoriond2017 import addExtraMETCollections,addExtraPuppiMETCorrections
    
    addExtraMETCollections(process,
                           unCleanPFCandidateCollection="particleFlow",
                           cleanElectronCollection="slimmedElectrons",
                           cleanPhotonCollection="slimmedPhotons",
                           unCleanElectronCollection="patElectronsUnClean",
                           unCleanPhotonCollection="patPhotonsUnClean")
    addExtraPuppiMETCorrections(process,
                                cleanPFCandidateCollection="particleFlow",
                                unCleanPFCandidateCollection="pfCandidatesBadMuonsCleaned",
                                cleanElectronCollection="slimmedElectrons",
                                cleanPhotonCollection="slimmedPhotons",
                                unCleanElectronCollection="patElectronsUnClean",
                                unCleanPhotonCollection="patPhotonsUnClean")

    addKeepStatement(process,
                     "keep *_slimmedMETs_*_*",
                     ["keep *_slimmedMETsUncorrected_*_*",
                      "keep *_slimmedMETsEGClean_*_*",
                      "keep *_slimmedMETsMuEGClean_*_*"],
                     verbose=verbose)
    addKeepStatement(process,
                     "keep *_slimmedMETsPuppi_*_*",
                     ["keep *_puppiMETEGCor_*_*",
                      "keep *_puppiMETMuCor_*_*"],
                     verbose=verbose)


    #jets ====
    from PhysicsTools.PatAlgos.slimming.extraJets_MuEGFixMoriond2017 import backupJets
    backupJets(process)
    
    addKeepStatement(process,
                     "keep *_slimmedJets_*_*",
                     ["keep *_slimmedJetsBackup_*_*"],
                     verbose=verbose)
    addKeepStatement(process, "keep *_slimmedJetsPuppi_*_*",
                     ["keep *_slimmedJetsPuppiBackup_*_*"],
                     verbose=verbose)
    addKeepStatement(process,
                     "keep *_slimmedJetsAK8_*_*",
                     ["keep *_slimmedJetsAK8Backup_*_*"],
                     verbose=verbose)
    addKeepStatement(process,"keep *_slimmedJetsAK8PFCHSSoftDropPacked_SubJets_*",
                     ["keep *_slimmedJetsAK8PFCHSSoftDropPackedBackup_SubJets_*"],
                     verbose=verbose)
    addKeepStatement(process,"keep *_slimmedJetsAK8PFPuppiSoftDropPacked_SubJets_*",
                     ["keep *_slimmedJetsAK8PFPuppiSoftDropPackedBackup_SubJets_*"],
                     verbose=verbose)

    #redo the miniAOD data customization for new JEC modules created during the backup process
    from PhysicsTools.PatAlgos.slimming.miniAOD_tools import miniAOD_customizeData
    miniAOD_customizeData(process)

    return process
