import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.helpers import MassSearchReplaceAnyInputTagVisitor, listDependencyChain, massSearchReplaceAnyInputTag, cloneProcessingSnippet

def addBadMuonFilters(process):
    process.load("RecoMET.METFilters.badGlobalMuonTaggersAOD_cff")
    process.Flag_noBadMuons = cms.Path(process.noBadGlobalMuons)

def cleanPFCandidates(process, badMuons, verbose=False):
    process.load("CommonTools.ParticleFlow.muonsCleaned_cfi")
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
                
    #FIXME: particleBasedIsolation, the e/gamma GED map, needs to be fixed with the new PFCands


def addKeepStatement(process, oldKeep, newKeeps, verbose=False):
    for name,out in process.outputModules.iteritems():
        if out.type_() == 'PoolOutputModule' and hasattr(out, "outputCommands"):
            if oldKeep in out.outputCommands:
                out.outputCommands += newKeeps
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
    from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
    cloneProcessingSnippet(process, getattr(process,"makePatPhotons"),"UnClean")
    cloneProcessingSnippet(process, getattr(process,"makePatElectrons"),"UnClean")
    
    process.load("RecoEgamma.EgammaTools.egammaGainSwitchFix_cff")

    loadJetMETBTag(process)

    process.originalAK4JetSequence = listDependencyChain(process, process.slimmedJets, ('particleFlow', 'muons'))
    backupAK4JetSequence = cloneProcessingSnippet(process, process.originalAK4JetSequence, "Backup")

    addBadMuonFilters(process)    

    # clean the muons and PF candidates, and make *everything* point to the new candidates
    cleanPFCandidates(process, 
                      [ cms.InputTag("badGlobalMuonTagger","bad"), cms.InputTag("cloneGlobalMuonTagger","bad") ],
                      verbose=verbose) 

    addDiscardedPFCandidates(process, cms.InputTag("pfCandidatesBadMuonsCleaned","discarded"), verbose=verbose)

    # now make the backup sequences point to the right place
    massSearchReplaceAnyInputTag(backupAK4JetSequence, "pfCandidatesBadMuonsCleaned", "particleFlow")
    massSearchReplaceAnyInputTag(backupAK4JetSequence, "muonsCleaned", "muons")
    addKeepStatement(process, "keep *_slimmedJets_*_*",
                     ["keep *_slimmedJetsBackup_*_*"],
                     verbose=verbose)

    process.patMuons.embedCaloMETMuonCorrs = False # FIXME


    #extra METs and MET corrections ===============================================================
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
    addKeepStatement(process,
                     "keep *_slimmedJetsAK8_*_*",
                     ["keep *_slimmedJetsAK8Backup_*_*"],
                     verbose=verbose)
    addKeepStatement(process,"keep *_slimmedJetsAK8PFCHSSoftDropPacked_SubJets_*",
                     ["keep *_slimmedJetsAK8PFCHSSoftDropPackedSubJetsBackup_*_*"],
                     verbose=verbose)
    addKeepStatement(process,"keep *_slimmedJetsAK8PFPuppiSoftDropPacked_SubJets_*",
                     ["keep *_slimmedJetsAK8PFPuppiSoftDropPackedSubJetsBackup_*_*"],
                     verbose=verbose)

    return process
