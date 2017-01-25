import FWCore.ParameterSet.Config as cms

from PhysicsTools.PatAlgos.tools.helpers import MassSearchReplaceAnyInputTagVisitor

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

def customizeAll(process):
    process.load("RecoEgamma.EgammaTools.egammaGainSwitchFix_cff")

    addBadMuonFilters(process)    

    loadJetMETBTag(process)

    cleanPFCandidates(process, 
                      [ cms.InputTag("badGlobalMuonTagger","bad"), cms.InputTag("cloneGlobalMuonTagger","bad") ],
                      verbose=True) # FIXME set to False when no longer needed

    process.patMuons.embedCaloMETMuonCorrs = False # FIXME

    return process
