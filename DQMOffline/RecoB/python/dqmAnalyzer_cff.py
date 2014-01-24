import FWCore.ParameterSet.Config as cms

from DQMOffline.RecoB.bTagAnalysisData_cfi import *
calobTagAnalysis = bTagAnalysis.clone()
bTagPlots = cms.Sequence(calobTagAnalysis)
calobTagAnalysis.finalizePlots = False
calobTagAnalysis.finalizeOnly = False


#Jet collection
JetCut=cms.string("neutralHadronEnergyFraction < 0.99 && neutralEmEnergyFraction < 0.99 && nConstituents > 1 && chargedHadronEnergyFraction > 0.0 && chargedMultiplicity > 0.0 && chargedEmEnergyFraction < 0.99")

from JetMETCorrections.Configuration.JetCorrectionServices_cff import ak4PFL2L3,ak4PFL2Relative,ak4PFL3Absolute
newAk5PFL2L3 = ak4PFL2L3.clone()

from JetMETCorrections.Configuration.DefaultJEC_cff import ak4PFJetsL2L3
ak4PFJetsJEC = ak4PFJetsL2L3.clone(
    correctors = ['newAk5PFL2L3']
    )

PFJetsFilter = cms.EDFilter("PFJetSelector",
                            src = cms.InputTag("ak4PFJetsJEC"),
                            cut = JetCut,
                            filter = cms.bool(False)
                            )

jetID = cms.InputTag("PFJetsFilter")

#JTA
from RecoJets.JetAssociationProducers.ak4JTA_cff import *
pfAk5JetTracksAssociatorAtVertex = ak4JetTracksAssociatorAtVertex.clone(jets = jetID)

#btag sequence
from RecoBTag.Configuration.RecoBTag_cff import *

pfImpactParameterTagInfos = impactParameterTagInfos.clone(jetTracks = cms.InputTag("pfAk5JetTracksAssociatorAtVertex"))
pfSecondaryVertexTagInfos = secondaryVertexTagInfos.clone(trackIPTagInfos = "pfImpactParameterTagInfos")

#JEC for CHS
from JetMETCorrections.Configuration.JetCorrectionServices_cff import ak5PFCHSL1Fastjet, ak5PFCHSL2Relative, ak5PFCHSL3Absolute, ak5PFCHSResidual, ak5PFCHSL1FastL2L3, ak5PFCHSL1FastL2L3Residual
newak5PFCHSL1Fastjet = ak5PFCHSL1Fastjet.clone(algorithm = 'AK5PFchs')
newak5PFCHSL2Relative = ak5PFCHSL2Relative.clone(algorithm = 'AK5PFchs')
newak5PFCHSL3Absolute = ak5PFCHSL3Absolute.clone(algorithm = 'AK5PFchs')
newak5PFCHSResidual = ak5PFCHSResidual.clone(algorithm = 'AK5PFchs')

newak5PFCHSL1FastL2L3 = ak5PFCHSL1FastL2L3.clone(correctors = cms.vstring('newak5PFCHSL1Fastjet','newak5PFCHSL2Relative','newak5PFCHSL3Absolute'))
newak5PFCHSL1FastL2L3Residual = ak5PFCHSL1FastL2L3Residual.clone(correctors = cms.vstring('newak5PFCHSL1Fastjet','newak5PFCHSL2Relative','newak5PFCHSL3Absolute','newak5PFCHSResidual'))

pfSimpleSecondaryVertexHighEffBJetTags = simpleSecondaryVertexHighEffBJetTags.clone(tagInfos = cms.VInputTag(cms.InputTag("pfSecondaryVertexTagInfos")))
pfSimpleSecondaryVertexHighPurBJetTags = simpleSecondaryVertexHighPurBJetTags.clone(tagInfos = cms.VInputTag(cms.InputTag("pfSecondaryVertexTagInfos")))

pfGhostTrackVertexTagInfos = pfSecondaryVertexTagInfos.clone()
pfGhostTrackVertexTagInfos.vertexReco = ghostTrackVertexRecoBlock.vertexReco
pfGhostTrackVertexTagInfos.vertexCuts.multiplicityMin = 1
pfGhostTrackBJetTags = ghostTrackBJetTags.clone(
    tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfos"),
                             cms.InputTag("pfGhostTrackVertexTagInfos"))
    )

pfCombinedSecondaryVertexBJetTags = combinedSecondaryVertexBJetTags.clone(
    tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfos"),
                             cms.InputTag("pfSecondaryVertexTagInfos"))
    )
pfCombinedSecondaryVertexMVABJetTags = combinedSecondaryVertexMVABJetTags.clone(
    tagInfos = cms.VInputTag(cms.InputTag("pfImpactParameterTagInfos"),
                             cms.InputTag("pfSecondaryVertexTagInfos"))
    )

pfSoftPFMuonsTagInfos = softPFMuonsTagInfos.clone(jets = jetID)
pfSoftPFElectronsTagInfos = softPFElectronsTagInfos.clone(jets = jetID)
pfSoftPFMuonBJetTags = softPFMuonBJetTags.clone(tagInfos = cms.VInputTag(cms.InputTag("pfSoftPFMuonsTagInfos")))
pfSoftPFElectronBJetTags = softPFElectronBJetTags.clone(tagInfos = cms.VInputTag(cms.InputTag("pfSoftPFElectronsTagInfos")))

pfbtagging = cms.Sequence(
    pfImpactParameterTagInfos *
    ( pfTrackCountingHighEffBJetTags +
      pfTrackCountingHighPurBJetTags +
      pfJetProbabilityBJetTags +
      pfJetBProbabilityBJetTags +

      pfSecondaryVertexTagInfos *
      ( pfSimpleSecondaryVertexHighEffBJetTags +
        pfSimpleSecondaryVertexHighPurBJetTags +
        pfCombinedSecondaryVertexBJetTags +
        pfCombinedSecondaryVertexMVABJetTags
        ) +
      pfGhostTrackVertexTagInfos *
      pfGhostTrackBJetTags
      ) +

    #softPFLeptonsTagInfos*
    pfSoftPFMuonsTagInfos*
    pfSoftPFElectronsTagInfos*
    pfSoftPFElectronBJetTags*
    pfSoftPFMuonBJetTags
)


#preSeq
prebTagSequence = cms.Sequence(ak4PFJetsJEC*PFJetsFilter*pfAk5JetTracksAssociatorAtVertex*pfbtagging)

# Module execution for data
#from DQMOffline.RecoB.bTagAnalysisData_cfi import *
pfbTagAnalysis = bTagAnalysis.clone(
    tagConfig = cms.VPSet(
       cms.PSet(
           bTagTrackIPAnalysisBlock,
           type = cms.string('TrackIP'),
           label = cms.InputTag("pfImpactParameterTagInfos"),
           folder = cms.string("IPTag")
           ),
       cms.PSet(
           bTagCombinedSVAnalysisBlock,
           ipTagInfos = cms.InputTag("pfImpactParameterTagInfos"),
           type = cms.string('GenericMVA'),
           svTagInfos = cms.InputTag("pfSecondaryVertexTagInfos"),
           label = cms.InputTag("combinedSecondaryVertex"),
           folder = cms.string("CSVTag")
           ),
       cms.PSet(
           bTagTrackCountingAnalysisBlock,
           label = cms.InputTag("pfTrackCountingHighEffBJetTags"),
           folder = cms.string("TCHE")
           ),
       cms.PSet(
           bTagTrackCountingAnalysisBlock,
           label = cms.InputTag("pfTrackCountingHighPurBJetTags"),
           folder = cms.string("TCHP")
           ),
       cms.PSet(
           bTagProbabilityAnalysisBlock,
           label = cms.InputTag("pfJetProbabilityBJetTags"),
           folder = cms.string("JP")
           ),
       cms.PSet(
           bTagBProbabilityAnalysisBlock,
           label = cms.InputTag("pfJetBProbabilityBJetTags"),
           folder = cms.string("JBP")
           ),
       cms.PSet(
           bTagSimpleSVAnalysisBlock,
           label = cms.InputTag("pfSimpleSecondaryVertexHighEffBJetTags"),
           folder = cms.string("SSVHE")
           ),
       cms.PSet(
           bTagSimpleSVAnalysisBlock,
           label = cms.InputTag("pfSimpleSecondaryVertexHighPurBJetTags"),
           folder = cms.string("SSVHP")
           ),
       cms.PSet(
           bTagGenericAnalysisBlock,
           label = cms.InputTag("pfCombinedSecondaryVertexBJetTags"),
           folder = cms.string("CSV")
           ),
       cms.PSet(
           bTagGenericAnalysisBlock,
           label = cms.InputTag("pfCombinedSecondaryVertexMVABJetTags"),
           folder = cms.string("CSVMVA")
           ),
       cms.PSet(
           bTagGenericAnalysisBlock,
           label = cms.InputTag("pfGhostTrackBJetTags"),
           folder = cms.string("GhTrk")
           ),
       cms.PSet(
           bTagSoftLeptonAnalysisBlock,
           label = cms.InputTag("pfSoftPFMuonBJetTags"),
           folder = cms.string("SMT")
           ),
       cms.PSet(
           bTagSoftLeptonAnalysisBlock,
           label = cms.InputTag("pfSoftPFElectronBJetTags"),
           folder = cms.string("SET")
           ),
       ),
    )
pfbTagAnalysis.finalizePlots = False
pfbTagAnalysis.finalizeOnly = False
pfbTagAnalysis.ptRanges = cms.vdouble(0.0)
bTagPlotsDATA = cms.Sequence(pfbTagAnalysis)

######### DATA ############
from DQMOffline.RecoB.bTagAnalysisData_cfi import *
bTagAnalysis.finalizePlots = False
bTagAnalysis.finalizeOnly = False
bTagAnalysis.ptRanges = cms.vdouble(0.0)
#Residual correction will be added inside the c++ code only for data (checking the presence of genParticles collection), not explicit here as this sequence also ran on MC FullSim
bTagAnalysis.doJetID = True
bTagAnalysis.doJEC = True
bTagAnalysis.JECsource = cms.string("newak5PFCHSL1FastL2L3") 
bTagPlotsDATA = cms.Sequence(bTagAnalysis)

########## MC ############
#Matching
from PhysicsTools.JetMCAlgos.CaloJetsMCFlavour_cfi import *
AK5byRef.jets = cms.InputTag("ak5PFJetsCHS")
#Get gen jet collection for real jets
ak5GenJetsForPUid = cms.EDFilter("GenJetSelector",
                                 src = cms.InputTag("ak5GenJets"),
                                 cut = cms.string('pt > 8.'),
                                 filter = cms.bool(False)
                                 )
#do reco gen - reco matching
from PhysicsTools.PatAlgos.mcMatchLayer0.jetMatch_cfi import patJetGenJetMatch
newpatJetGenJetMatch = patJetGenJetMatch.clone(
    src = cms.InputTag("ak5PFJetsCHS"),
    matched = cms.InputTag("ak5GenJetsForPUid"),
    maxDeltaR = cms.double(0.25),
    resolveAmbiguities = cms.bool(True)
)

# Module execution for MC
from Validation.RecoB.bTagAnalysis_cfi import *
bTagValidation.finalizePlots = False
bTagValidation.finalizeOnly = False
bTagValidation.jetMCSrc = 'AK5byValAlgo'
bTagValidation.ptRanges = cms.vdouble(0.0)
bTagValidation.etaRanges = cms.vdouble(0.0)
bTagValidation.doJetID = True
bTagValidation.doJEC = True
bTagValidation.JECsource = cms.string("newak5PFCHSL1FastL2L3")
bTagValidation.doPUid = cms.bool(True)
bTagValidation.genJetsMatched = cms.InputTag("newpatJetGenJetMatch")
#to run on fastsim
prebTagSequenceMC = cms.Sequence(ak5GenJetsForPUid*newpatJetGenJetMatch*myPartons*AK5Flavour)
bTagPlotsMC = cms.Sequence(bTagValidation)

#to run on fullsim in the validation sequence, all histograms produced in the dqmoffline sequence
bTagValidationNoall = bTagValidation.clone(flavPlots="noall")
bTagPlotsMCbcl = cms.Sequence(myPartons*AK5Flavour*bTagValidationNoall)
