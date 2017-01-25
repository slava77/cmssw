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
    setattr(process, jetColName, getattr(process,"ak4PFJets").clone(
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
        inputName="selectedPatJets"+tag #+"UnClean"
        inputColl="patJets"+tag+"UnClean"
        if tag=="AK4": 
            inputName="selectedPatJets"
            inputColl="patJetsUnClean"

        setattr(process, "selectedPatJets"+tag+"UnClean",
                getattr(process, inputName ).clone(
                src = cms.InputTag(inputColl)
                ) )
        setattr(process, "slimmedJets"+tag+"UnClean",
                getattr(process, "slimmedJets").clone( #+tag
                 packedPFCandidates = cms.InputTag("packedPFCandidates"), #arg!
                 src = cms.InputTag("selectedPatJets"+tag+"UnClean")
                ) )

    setattr(process,"uncorSlimmedJets"+tag+"Backup",
            cms.EDProducer("JetCollectionReducer",
                           jetCollection=cms.InputTag(jetCollection), #"slimmedJets"
                           triggeringCollections=cms.VInputTag(
                              cms.InputTag("cloneGlobalMuonTagger","bad"),
                              cms.InputTag("badGlobalMuonTagger","bad") )
                           ) 
            )
            
    process.MINIAODoutput.outputCommands.append("keep *_uncorSlimmedJets"+tag+"Backup_*_*")


def makeExtraPuppiJetCollection(process):
    print "blou"



def backupJets(process):
    
    backupUncorrectedJetCollection(process, "slimmedJetsUnClean", "AK4")
    #backupUncorrectedJetCollection(process, "slimmedJetsAK8UnClean", "AK8")
    makeRecoJetCollection(process, 
                          pfCandCollection="particleFlow",
                          coneSize=0.8,
                          useCHSAlgo=True,
                          postfix="UnClean")
    makeRecoJetCollection(process, 
                          pfCandCollection="puppiUnClean", #not sure it exists
                          coneSize=0.8,
                          useCHSAlgo=False,
                          postfix="UnClean")

    from PhysicsTools.PatAlgos.slimming.applySubstructure_cff import applySubstructure
    applySubstructure(process, "UnClean")


