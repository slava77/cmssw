import FWCore.ParameterSet.Config as cms

def makeRecoJetCollection(process, 
                          pfCandCollection,
                          coneSize,
                          useCHSAlgo,
                          postfix):

    jetColName="ak"+str(int(coneSize*10))+"PFJets"
    internalPfCandColl=pfCandCollection
    if useCHSAlgo:
        setattr( process, "tmpPFCandCollPtr", 
                 cms.EDProducer("PFCandidateFwdPtrProducer",
                                src = cms.InputTag(pfCandCollection) ) 
                 )
        process.load("CommonTools.ParticleFlow.pfNoPileUpJME_cff")
        from PhysicsTools.PatAlgos.tools.helpers import cloneProcessingSnippet
        cloneProcessingSnippet(process, getattr(process,"pfNoPileUpJMESequence"), postfix )
        getattr(process, "pfPileUpJME"+postfix).PFCandidates = cms.InputTag("tmpPFCandCollPtr")
        pfCHS = getattr(process, "pfNoPileUpJME").clone( bottomCollection = cms.InputTag("tmpPFCandCollPtr") )
        
        if not hasattr(process, "pfCHS"+postfix):
            setattr(process,"pfCHS"+postfix,pfCHS)
            internalPfCandColl = "pfCHS"+postfix
        jetColName+="CHS"
        

    from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
    setattr(process, jetColName+postfix, getattr(process,"ak4PFJets").clone(
            src = cms.InputTag(internalPfCandColl),
            rParam=cms.double(coneSize),
            doAreaFastjet = True)
            )
    


def backupUncorrectedJetCollection(process, jetCollection, tag):

    if not hasattr(process,jetCollection): 
        #means that we go only to the patjet level
        #and we have to go up to slimmed Jets before 
        #reducing the collection
        
        #dummy name fix for central collection
        inputName="selectedPatJets"+tag
        inputColl="patJets"+tag+"Backup"
        #if tag=="AK4": 
        #    inputName="selectedPatJets"
        #inputColl="patJetsBackup"

        setattr(process, "selectedPatJets"+tag+"BackupTmp",
                getattr(process, inputName ).clone(
                src = cms.InputTag(inputColl)
                ) )
        setattr(process, "slimmedJets"+tag+"BackupTmp",
                getattr(process, "slimmedJets").clone( #+tag
                 packedPFCandidates = cms.InputTag("packedPFCandidates"), #arg! #MM FIXME
                 src = cms.InputTag("selectedPatJets"+tag+"BackupTmp")
                ) )
        jetCollection="slimmedJets"+tag+"BackupTmp"

    setattr(process,"slimmedJets"+tag+"Backup",
            cms.EDProducer("JetCollectionReducer",
                           jetCollection=cms.InputTag(jetCollection),
                           triggeringCollections=cms.VInputTag(
                              cms.InputTag("cloneGlobalMuonTagger","bad"),
                              cms.InputTag("badGlobalMuonTagger","bad") )
                           ) 
            )
    

def backupJets(process, pfCandidateCollection="particleFlow", puppiCandidateCollection="puppiBackupTmp"):
    
    backupUncorrectedJetCollection(process, "slimmedJetsBackupTmp", "")
    backupUncorrectedJetCollection(process, "slimmedJetsPuppiBackupTmp", "Puppi")
    
    makeRecoJetCollection(process, 
                          pfCandCollection=pfCandidateCollection,
                          coneSize=0.8,
                          useCHSAlgo=True,
                          postfix="BackupTmp")
    makeRecoJetCollection(process, 
                          pfCandCollection=puppiCandidateCollection, #not sure it exists
                          coneSize=0.8,
                          useCHSAlgo=False,
                          postfix="BackupTmp")

    from PhysicsTools.PatAlgos.slimming.applySubstructure_cff import applySubstructure
    applySubstructure(process, "BackupTmp")

    setattr(process,"slimmedJetsAK8Backup",
            cms.EDProducer("JetCollectionReducer",
                           jetCollection=cms.InputTag("slimmedJetsAK8BackupTmp"),
                           triggeringCollections=cms.VInputTag(
                cms.InputTag("cloneGlobalMuonTagger","bad"),
                cms.InputTag("badGlobalMuonTagger","bad") )
                           ) 
            )
    
    setattr(process,"slimmedJetsAK8PFCHSSoftDropPackedSubJetsBackup",
            cms.EDProducer("JetCollectionReducer",
               jetCollection=cms.InputTag("slimmedJetsAK8PFCHSSoftDropPackedBackupTmp:SubJets"),
               triggeringCollections=cms.VInputTag(
                cms.InputTag("cloneGlobalMuonTagger","bad"),
                cms.InputTag("badGlobalMuonTagger","bad") )
                           ) 
            )

    setattr(process,"slimmedJetsAK8PFPuppiSoftDropPackedSubJetsBackup",
            cms.EDProducer("JetCollectionReducer",
               jetCollection=cms.InputTag("slimmedJetsAK8PFPuppiSoftDropPackedBackupTmp:SubJets"),
               triggeringCollections=cms.VInputTag(
                cms.InputTag("cloneGlobalMuonTagger","bad"),
                cms.InputTag("badGlobalMuonTagger","bad") )
                           ) 
            )
