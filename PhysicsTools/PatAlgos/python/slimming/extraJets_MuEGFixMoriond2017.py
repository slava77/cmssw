import FWCore.ParameterSet.Config as cms

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



def backupCHSJets(process):
    backupUncorrectedJetCollection(process, "slimmedJetsUnClean", "AK4")
    #backupUncorrectedJetCollection(process, "slimmedJetsAK8UnClean", "AK8")


def backupPuppiJets(process):
    print "blou"
