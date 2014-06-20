import FWCore.ParameterSet.Config as cms


process = cms.Process("TTGMT")

process.source = cms.Source(
    "PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    skipEvents = cms.untracked.uint32(0),
#    fileNames  = cms.untracked.vstring('file:/eos/uscms/store/user/slava77/SingleMuPlusFlatPt0p2To100/TTv5PU140/7744c92071593b44380945a41dbb1ee2/ttreco_1_1_C1w.root'),
    fileNames  = cms.untracked.vstring('/store/mc/TTI2023Upg14D/SingleMuMinusFlatPt0p2To150/GEN-SIM-DIGI-RAW/PU140bx25_PH2_1K_FB_V3-v2/00000/FEF0177C-E8E5-E311-9756-00304867906C.root'),
    )


# ---- Global Tag :
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'PH2_1K_FB_V3::All', '')


process.load('Configuration.Geometry.GeometryExtended2023TTIReco_cff')
process.load('Geometry.TrackerGeometryBuilder.StackedTrackerGeometry_cfi')
process.load('Configuration.Geometry.GeometryExtended2023TTI_cff')

process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')

#make extras, use simDigis for all
process.load('Configuration.StandardSequences.L1Reco_cff')


# ---------------------------------------------------------------------------
#
# ---  Produces various L1TkMuon collections - as well as "extended" L1Muons
#

process.load('Configuration.StandardSequences.L1Reco_cff')

process.load("SLHCUpgradeSimulations.L1TrackTrigger.L1TkMuonSequence_cfi")
process.pMuons = cms.Path( process.l1extraParticles + process.L1TkMuons )


process.AdaptorConfig = cms.Service (
    "AdaptorConfig",
    stats = cms.untracked.bool(True),
    enable = cms.untracked.bool(True),
    cacheHint = cms.untracked.string("lazy-download"),
    readHint = cms.untracked.string("auto-detect")
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.out = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('tt_ntuple.root'),
    dropMetaData = cms.untracked.string("NONE")
    )

process.out.outputCommands = cms.untracked.vstring( 'drop *' )
process.out.outputCommands.extend(cms.untracked.vstring('keep *_*Maker*_*_TTGMT*'))

process.load("Phase2MuTriggerAnalysis.GMT2TTIntegration.ttgmtSequence_cff")
process.ttgmt_path = cms.Path( process.ttgmtSequence )
process.outpath = cms.EndPath(process.out)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

from SLHCUpgradeSimulations.Configuration.muonCustoms import customise_csc_L1Extra_allsim
process = customise_csc_L1Extra_allsim(process)
