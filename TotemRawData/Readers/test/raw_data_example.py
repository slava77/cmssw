import FWCore.ParameterSet.Config as cms

process = cms.Process("rpReconstruction")

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(2)
)

process.load('TotemRawData.Readers.RawDataSource_cfi')
process.source.verbosity = 10
process.source.printProgressFrequency = 1
process.source.fileNames.append('/castor/cern.ch/totem/LHCRawData/2015/Physics/run_EVB-wn10_9298.068.vmeb')
process.source.fileNames.append('/castor/cern.ch/totem/LHCRawData/2015/Physics/run_9487_EVB11_2.005.srs')

# raw to digi conversion
process.load('TotemCondFormats.DAQInformation.DAQMappingSourceXML_cfi')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_220.xml')

process.load('TotemRawData.RawToDigi.Raw2DigiProducer_cfi')
process.Raw2DigiProducer.verbosity = 0

process.p = cms.Path(
    process.Raw2DigiProducer
)
