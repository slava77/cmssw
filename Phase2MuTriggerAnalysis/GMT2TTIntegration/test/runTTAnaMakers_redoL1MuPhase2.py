import FWCore.ParameterSet.Config as cms


process = cms.Process("TTGMT")

process.source = cms.Source(
    "PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    skipEvents = cms.untracked.uint32(0),
    fileNames  = cms.untracked.vstring(
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_1_1_aJu.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_2_2_iuW.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_3_1_TRT.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_4_2_iOj.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_5_2_jA5.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_6_2_haC.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_7_2_4Ya.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_8_2_4aM.root",
"/store/user/slava77/SingleMuPlusFlatPt0p2To150/TTI2023Upg14D-PU140bx25-ILT_SLHC14/6a6577f18a9b70d924bea399af3ff625/2023TTIUpg14D_9_2_3dX.root"
    ),
#    fileNames  = cms.untracked.vstring('/store/mc/TTI2023Upg14D/SingleMuMinusFlatPt0p2To150/GEN-SIM-DIGI-RAW/PU140bx25_PH2_1K_FB_V3-v2/00000/FEF0177C-E8E5-E311-9756-00304867906C.root'),
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

process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.Digi_cff")
process.load('Configuration.StandardSequences.L1Reco_cff')

process.load("L1Trigger.Configuration.SimL1Emulator_cff")
process.load("SLHCUpgradeSimulations.L1TrackTrigger.L1TkMuonSequence_cfi")
process.pMuons = cms.Path(process.simCscTriggerPrimitiveDigis + process.simCsctfTrackDigis + process.simCsctfDigis + process.simGmtDigis
                          + process.l1extraParticles + process.L1TkMuons )
# DT reco forces me to keep pixelDigis, too much to store, will drop it
process.pMuons.remove(process.L1TkMuonsDTSequence)

# if DT Tk Mu is running
#process.es_prefer_dt = cms.ESPrefer("DTConfigTrivialProducer","L1DTConfig")
process.es_prefer_dt = cms.ESPrefer("DTConfigDBProducer","L1DTConfigFromDB")

process.AdaptorConfig = cms.Service (
    "AdaptorConfig",
    stats = cms.untracked.bool(True),
    enable = cms.untracked.bool(True),
    cacheHint = cms.untracked.string("lazy-download"),
    readHint = cms.untracked.string("auto-detect")
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.out = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string('tt_ntuple_redoL1MuPhase2.root'),
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
from SLHCUpgradeSimulations.Configuration.muonCustoms import customise_csc_L1Emulator
process = customise_csc_L1Extra_allsim(process)

from SLHCUpgradeSimulations.Configuration.gemCustoms import customise2023 as customise_gem2023
from SLHCUpgradeSimulations.Configuration.rpcCustoms import customise as customise_rpc
from SLHCUpgradeSimulations.Configuration.combinedCustoms import cust_2023TTI as cust_2023TTI
process = cust_2023TTI(process)
#the one above is "too smart" need to modify manually

#process=customise_gem2023(process)
from SLHCUpgradeSimulations.Configuration.gemCustoms import customise_L1Emulator2019 as customise_L1Emulator2019_gem
from SLHCUpgradeSimulations.Configuration.gemCustoms import customise_L1Emulator2023 as customise_L1Emulator2023_gem
process = customise_L1Emulator2019_gem(process, 'pt0')
process = customise_L1Emulator2023_gem(process, 'pt0')

#process=customise_rpc(process)
from SLHCUpgradeSimulations.Configuration.rpcCustoms import customise_L1Emulator as customise_L1Emulator_rpc
process = customise_L1Emulator_rpc(process)
