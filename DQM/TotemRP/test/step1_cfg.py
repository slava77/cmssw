import FWCore.ParameterSet.Config as cms

process = cms.Process('RECODQM')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EDMtoMEAtRunEnd_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

# load DQM
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")

# TOTEM RP geometry
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/TotemRPData/data/RP_Garage/RP_Dist_Beam_Cent.xml")

# TOTEM RP plot module
process.TotemRPDQMSource = cms.EDAnalyzer("TotemRPDQMSource",
    tagStripDigi = cms.InputTag("Raw2DigiProducer", "rpDataOutput"),
	tagDigiCluster = cms.InputTag("RPClustProd"),
	tagRecoHit = cms.InputTag("RPRecoHitProd"),
	tagPatternColl = cms.InputTag("NonParallelTrackFinder"),
	tagTrackColl = cms.InputTag("RPSingleTrackCandCollFit"),
	tagTrackCandColl = cms.InputTag("NonParallelTrackFinder"),
	tagMultiTrackColl = cms.InputTag(""),

    buildCorrelationPlots = cms.untracked.bool(False),
    correlationPlotsLimit = cms.untracked.uint32(50),
	correlationPlotsFilter = cms.untracked.string("default=0,1")
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(200)
)

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/j/jkaspar/software/offline/800_pre5/user/reco_test/reco_test.root')
)

process.DQMoutput = cms.OutputModule("DQMRootOutputModule",
  fileName = cms.untracked.string("OUT_step1.root")
)

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:mc', '')  #for MC

# Path and EndPath definitions
process.dqm_offline_step = cms.Path(process.TotemRPDQMSource)
process.dqm_output_step = cms.EndPath(process.DQMoutput)

# Schedule definition
process.schedule = cms.Schedule(
    process.dqm_offline_step,
    process.dqm_output_step
)
