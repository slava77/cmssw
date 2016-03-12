import FWCore.ParameterSet.Config as cms

process = cms.Process("rpReconstruction")

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(100)
)

process.source = cms.Source("RawDataMappingSource",
    verbosity = cms.untracked.uint32(1),
    runNumber = cms.untracked.uint32(9985),
    mappingFileName = cms.untracked.string("/afs/cern.ch/work/j/jkaspar/software/offline/704/user/2015_data_correction/9985/mapping_3"),
    listFileName = cms.untracked.string("/afs/cern.ch/work/j/jkaspar/software/offline/704/user/2015_data_correction/9985/input_files")
)

# raw to digi conversion
process.load('TotemCondFormats.DAQInformation.DAQMappingSourceXML_cfi')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_220.xml')

process.load('TotemRawData.RawToDigi.Raw2DigiProducer_cfi')
process.Raw2DigiProducer.verbosity = 0

process.p = cms.Path(
    process.Raw2DigiProducer
)
