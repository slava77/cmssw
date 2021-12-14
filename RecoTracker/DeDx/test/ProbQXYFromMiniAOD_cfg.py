import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
process = cms.Process('RECO2',Run2_2018)

options = VarParsing.VarParsing('analysis')
options.register('globalTag',
                 "auto:phase1_2018_realistic", # default value
                 VarParsing.VarParsing.multiplicity.singleton, # singleton or list
                 VarParsing.VarParsing.varType.string, # string, int, or float
                 "input file name")
options.parseArguments()

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Configuration.StandardSequences.GeometryDB_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
#process.GlobalTag.DumpStat = cms.untracked.bool(True)
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.threshold = "DEBUG"
process.MessageLogger.debugModules = ["*"]   
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles),
                            secondaryFileNames = cms.untracked.vstring()
                            )

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

process.options = cms.untracked.PSet()
process.TFileService = cms.Service('TFileService',
                                 fileName = cms.string('HSCPAna.root'),
                                 )

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, options.globalTag, '')

process.probQXYAna  = cms.EDAnalyzer('ProbQXYAna',
   packedCandidates = cms.InputTag("packedPFCandidates"), 
   tracks           = cms.InputTag("isolatedTracks"),
   dedx             = cms.InputTag("isolatedTracks")
)

# Path and EndPath definitions
process.probQXYAna_step = cms.EndPath(process.probQXYAna)

# Schedule definition
process.schedule = cms.Schedule(process.probQXYAna_step)
