import FWCore.ParameterSet.Config as cms

process = cms.Process("rpReconstruction")

run = "5606"

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)


# raw data input
process.load('TotemRawData.Readers.RawDataSource_cfi')
process.source.verbosity = 0
process.source.printProgressFrequency = 1
for i in range(28):
	if i not in [7,11,23]:
		process.source.fileNames.append("/castor/cern.ch/totem/LHCRawData/2011/Physics/run_"+run+".%03d.vmea" % i)

# DAQ mapping
process.load('TotemCondFormats/DAQInformation/DAQMappingSourceXML_cfi')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_220.xml')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_147.xml')

# raw to digi conversion
process.load('TotemRawData.RawToDigi.Raw2DigiProducer_cfi')
process.Raw2DigiProducer.verbosity = 0

# clusterization
process.load("RecoTotemRP.RPClusterSigmaService.ClusterSigmaServiceConf_cfi")
process.load("RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi")

# reco hit production
process.load("RecoTotemRP.RPRecoHitProducer.RPRecoHitProdConf_cfi")

# geometry
process.load("Configuration.TotemCommon.geometryRP_real_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/TotemRPData/data/2011_05_18/RP_Dist_Beam_Cent.xml")

# track search/pattern recognition
process.load("RecoTotemRP.RPNonParallelTrackCandidateFinder.RPNonParallelTrackCandidateFinder_cfi")
process.NonParallelTrackFinder.verbosity = 0
process.NonParallelTrackFinder.maxHitsPerPlaneToSearch = 4

# track fitting
process.load("RecoTotemRP.RPTrackCandidateCollectionFitter.RPSingleTrackCandCollFitted_cfi")
process.RPSingleTrackCandCollFit.Verbosity = 0

# trigger bits a CC simulation
process.TriggerBits = cms.EDProducer("RPTriggerBitsProducer",
    verbose = cms.bool(False)
)
process.load('L1TriggerTotem.CoincidenceChip.RPCoincidenceProducer_cfi')

# optics
process.load("Configuration/TotemOpticsConfiguration/OpticsConfig_3500GeV_1p5_120urad_thin_cfi")

# high-level RP reconstruction
process.load("RecoTotemRP.RPInelasticReconstruction.Rec_3500GeV_beta_1p5_120urad_220_2Arm_cfi")
process.RP2202ArmReconst.BeamProtTransportSetup = process.BeamProtTransportSetup
process.RP2202ArmReconst.ExpectedRPResolution = 0.020 #mm
process.RP2202ArmReconst.Verbosity = 0

process.load("RecoTotemRP.RPInelasticReconstruction.Rec_3500GeV_beta_1p5_120urad_220_cfi")
process.RP220Reconst.BeamProtTransportSetup = process.BeamProtTransportSetup
process.RP220Reconst.ExpectedRPResolution = 0.020 #mm
process.RP220Reconst.Verbosity = 0
process.RP220Reconst.ElasticScatteringReconstruction = False


# Ntuplizer
process.load("TotemAnalysis.TotemNtuplizer.TotemNtuplizer_cfi")
process.TotemNtuplizer.outputFileName = "ntuple_rp_"+run+".root"

process.p = cms.Path(
    process.Raw2DigiProducer
    * process.RPClustProd 
    * process.RPHecoHitProd 
    * process.NonParallelTrackFinder
    * process.RPSingleTrackCandCollFit
    * process.TriggerBits 
    * process.RPCC 
    * process.RP220Reconst
    * process.RP2202ArmReconst
    * process.TotemNtuplizer
)

