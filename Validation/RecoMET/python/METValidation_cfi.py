import FWCore.ParameterSet.Config as cms

# File: CaloMET.cfi
# Author: B. Scurlock & R. Remington
# Date: 03.04.2008
#
# Fill validation histograms for MET
metAnalyzer = cms.EDAnalyzer(
    "METTester",
    OutputFile = cms.untracked.string(''),
    InputMETLabel = cms.InputTag("caloMet"),
    METType = cms.untracked.string("calo")
    )

#metHOAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metHO")
#    )
#
#metNoHFAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metNoHF")
#    )
#
#metNoHFHOAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metNoHFHO")
#    )
#
#metOptAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metOpt")
#    )
#
#metOptHOAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metOptHO")
#    )
#
#metOptNoHFAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metOptNoHF")
#    )
#
#metOptNoHFHOAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("metOptNoHFHO")
#    )

pfMetAnalyzer = cms.EDAnalyzer(
   "METTester",
   OutputFile = cms.untracked.string(''),
   InputMETLabel = cms.InputTag("pfMet"),
    METType = cms.untracked.string("pf")
   ) 


#genMptTrueAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("genMptTrue"),
#    )

genMetTrueAnalyzer = cms.EDAnalyzer(
    "METTester",
    OutputFile = cms.untracked.string(''),
    InputMETLabel = cms.InputTag("genMetTrue"),
    METType = cms.untracked.string("gen")
    )

#genMetCaloAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("genMetCalo")
#    )
#
#genMptCaloAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("genMptCalo")
#    )
#
#
#genMetCaloAndNonPromptAnalyzer = cms.EDAnalyzer(
#    "METTester",
#    OutputFile = cms.untracked.string(''),
#    InputMETLabel = cms.InputTag("genMetCaloAndNonPrompt")
#    )
pfType0CorrectedMetAnalyzer = cms.EDAnalyzer(
   "METTester",
   OutputFile = cms.untracked.string(''),
   InputMETLabel = cms.InputTag("pfType0CorrectedMet"),
    METType = cms.untracked.string("pf")
   )
pfType1CorrectedMetAnalyzer = cms.EDAnalyzer(
   "METTester",
   OutputFile = cms.untracked.string(''),
   InputMETLabel = cms.InputTag("pfType1CorrectedMet"),
    METType = cms.untracked.string("pf")
   )
pfType01CorrectedMetAnalyzer = cms.EDAnalyzer(
   "METTester",
   OutputFile = cms.untracked.string(''),
   InputMETLabel = cms.InputTag("pfType01CorrectedMet"),
    METType = cms.untracked.string("pf")
   )


