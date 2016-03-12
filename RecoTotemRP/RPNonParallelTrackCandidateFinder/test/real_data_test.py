#todo fix this config to run with 8.0.X

import FWCore.ParameterSet.Config as cms

process = cms.Process("NonParallelTrackFinderTest")


process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
#    input = cms.untracked.int32(100)
)


# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")
#process.load("Configuration.TotemCommon.LoggerMax_cfi")


process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",    
    moduleSeeds = cms.PSet(                                               
    ),
    sourceSeed = cms.untracked.uint32(173205)                                               
)


process.load("Configuration/TotemOpticsConfiguration/OpticsConfig_3500GeV_1p5_120urad_thin_cfi")


process.TotemRPIncludeAlignments = cms.ESProducer("TotemRPIncludeAlignments",
    RealFiles = cms.vstring('TotemAlignment/RPData/LHC/2011_05_18/sr+hsx/45_220.xml', 
        'TotemAlignment/RPData/LHC/2011_05_18/sr+hsx/56_220.xml'),
    MisalignedFiles = cms.vstring(),  MeasuredFiles = cms.vstring()
)


process.load('TotemRawData.Readers.RawDataSource_cfi')
process.source.verbosity = 1
process.source.printProgressFrequency = 0

process.source.fileNames = cms.untracked.vstring()
process.source.fileNames.append('/castor/cern.ch/totem/LHCRawData/2011/Physics/run_5601.000.vmea')


# raw to digi conversion
process.load('TotemCondFormats/DAQInformation/DAQMappingSourceXML_cfi')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_220.xml')
process.DAQMappingSourceXML.mappingFileNames.append('TotemCondFormats/DAQInformation/data/rp_147.xml')


process.load('TotemRawData.RawToDigi.Raw2DigiProducer_cfi')

# clusterization
process.load("RecoTotemRP.RPClusterSigmaService.ClusterSigmaServiceConf_cfi")
process.load("RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi")

# reco hit production
process.load("RecoTotemRP.RPRecoHitProducer.RPRecoHitProdConf_cfi")


# geometry
process.load("Configuration/TotemCommon/geometryGlobal_real_cfi")
toberemoved = []
for xmlfile in process.XMLIdealGeometryESSource.geomXMLFiles:
    if xmlfile.endswith("RP_Dist_Beam_Cent.xml"):
       toberemoved.append(xmlfile)
for xmlfile in toberemoved:
    process.XMLIdealGeometryESSource.geomXMLFiles.remove(xmlfile)
process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/TotemRPData/data/2011_05_18/RP_Dist_Beam_Cent.xml")
process.TotemRPGeometryESModule = cms.ESProducer("TotemRPGeometryESModule")

# track search/pattern recognition
process.load("RecoTotemRP.RPNonParallelTrackCandidateFinder.RPNonParallelTrackCandidateFinder_cfi")
process.NonParallelTrackFinder.verbosity = 0
process.NonParallelTrackFinder.maxHitsPerPlaneToSearch = 4

# track fitting
process.load("RecoTotemRP.RPTrackCandidateCollectionFitter.RPSingleTrackCandCollFitted_cfi")
process.RPSingleTrackCandCollFit.Verbosity = 0

process.load("RecoTotemRP.RPInelasticReconstruction.Rec_3500GeV_beta_1p5_120urad_220_2Arm_cfi")
process.RP2202ArmReconst.BeamProtTransportSetup = process.BeamProtTransportSetup
process.RP2202ArmReconst.ExpectedRPResolution = 0.020 #mm
process.RP2202ArmReconst.Verbosity = 0

process.load("RecoTotemRP.RPInelasticReconstruction.Rec_3500GeV_beta_1p5_120urad_220_cfi")
process.RP220Reconst.BeamProtTransportSetup = process.BeamProtTransportSetup
process.RP220Reconst.ExpectedRPResolution = 0.020 #mm
process.RP220Reconst.Verbosity = 0
process.RP220Reconst.ElasticScatteringReconstruction = False

process.TriggerBits = cms.EDProducer("RPTriggerBitsProducer",
    verbose = cms.bool(False)
)

process.load('L1TriggerTotem.CoincidenceChip.RPCoincidenceProducer_cfi')
#process.load("TotemRawData.RawToDigi.RPDataCCProducer_cfi")


process.load("RecoTotemRP.RPMulCandidateTrackFinder.RPMulTrackCandFindConf_cfi")
process.load("RecoTotemRP.RPMulTrackCandidateCollectionFitter.RPMulTrackCandCollFitter_cfi")

process.p = cms.Path(
      process.Raw2DigiProducer 
    * process.TriggerBits 
    * process.RPCC 
    * process.RPClustProd 
    * process.RPRecoHitProd
    * process.NonParallelTrackFinder 
    * process.RPSingleTrackCandCollFit
    * process.RP220Reconst
    * process.RP2202ArmReconst
    * process.RPMulTrackCandFind
    * process.RPMulTrackCandCollFit
)

# store desired results
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("file:output.root"),
    outputCommands = cms.untracked.vstring(
        'keep *'
    )
)

process.outpath = cms.EndPath(process.output)

