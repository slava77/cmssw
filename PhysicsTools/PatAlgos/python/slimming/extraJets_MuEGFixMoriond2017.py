import FWCore.ParameterSet.Config as cms

def makeRecoJetCollection(process, 
                          pfCandCollection,
                          coneSize,
                          useCHSAlgo,
                          postfix):

    jetColName="ak"+str(int(coneSize*10))+"PFJets"
    internalPfCandColl=pfCandCollection
    if useCHSAlgo:
        setattr( process, "tmpPFCandCollPtr"+postfix, 
                 cms.EDProducer("PFCandidateFwdPtrProducer",
                                src = cms.InputTag(pfCandCollection) ) 
                 )
        process.load("CommonTools.ParticleFlow.pfNoPileUpJME_cff")
        from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
        cloneProcessingSnippet(process, getattr(process,"pfNoPileUpJMESequence"), postfix )
        getattr(process, "pfPileUpJME"+postfix).PFCandidates = cms.InputTag("tmpPFCandCollPtr"+postfix)
        getattr(process, "pfNoPileUpJME"+postfix).bottomCollection = cms.InputTag("tmpPFCandCollPtr"+postfix) 
        
        internalPfCandColl = "pfNoPileUpJME"+postfix
        jetColName+="CHS"
 
    setattr(process, jetColName+postfix, getattr(process,"ak4PFJets").clone(
            src = cms.InputTag(internalPfCandColl),
            rParam=cms.double(coneSize),
            doAreaFastjet = True)
            )
    


def backupUncorrectedJetCollection(process, jetCollection, tag):

    setattr(process,"slimmedJets"+tag+"Backup",
            cms.EDProducer("JetCollectionReducer",
                           jetCollection=cms.InputTag(jetCollection),
                           triggeringCollections=cms.VInputTag(
                              cms.InputTag("cloneGlobalMuonTagger","bad"),
                              cms.InputTag("badGlobalMuonTagger","bad") )
                           ) 
            )
    

def backupJets(process):
    
    from PhysicsTools.PatAlgos.slimming.applySubstructure_cff import applySubstructure
    applySubstructure(process, "BackupTmp")
    
    backupUncorrectedJetCollection(process, "slimmedJetsBackupTmp", "")
    backupUncorrectedJetCollection(process, "slimmedJetsPuppiBackupTmp", "Puppi")
    backupUncorrectedJetCollection(process, "slimmedJetsAK8BackupTmp","AK8")
    backupUncorrectedJetCollection(process, "slimmedJetsAK8PFCHSSoftDropPackedBackupTmp", "AK8PFCHSSoftDropPackedSubJets")
    backupUncorrectedJetCollection(process, "slimmedJetsAK8PFPuppiSoftDropPackedBackupTmp", "AK8PFPuppiSoftDropPackedSubJets")
    
