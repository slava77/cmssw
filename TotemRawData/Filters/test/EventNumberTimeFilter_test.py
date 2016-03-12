import FWCore.ParameterSet.Config as cms

process = cms.Process("rpReconstruction")

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

process.load('TotemRawData.Readers.RawDataSource_cfi')
process.source.verbosity = 0
process.source.printProgressFrequency = 0
process.source.fileNames.append('/castor/cern.ch/totem/LHCRawData/2011/Physics/run_5606.000.vmea')

process.load('TotemRawData.Filters.EventNumberTimeFilter_cfi')
process.EventNumberTimeFilter.eventNumber.active = True
process.EventNumberTimeFilter.eventNumber.min = 0
process.EventNumberTimeFilter.eventNumber.max = 10

# raw to digi conversion
process.load('TotemCondFormats.DAQInformation.DAQMappingSourceXML_cfi')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_220.xml')

process.load('TotemRawData.RawToDigi.Raw2DigiProducer_cfi')
process.Raw2DigiProducer.verbosity = 0

process.p = cms.Path(
    process.EventNumberTimeFilter
    * process.Raw2DigiProducer
)
