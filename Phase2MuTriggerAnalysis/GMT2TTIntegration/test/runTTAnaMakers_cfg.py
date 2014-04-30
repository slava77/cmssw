import FWCore.ParameterSet.Config as cms


process = cms.Process("TTGMT")

process.source = cms.Source(
    "PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    skipEvents = cms.untracked.uint32(0),
    fileNames  = cms.untracked.vstring('file:/eos/uscms/store/user/slava77/SingleMuPlusFlatPt0p2To100/TTv5PU140/7744c92071593b44380945a41dbb1ee2/ttreco_1_1_C1w.root'),
    )

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
