import FWCore.ParameterSet.Config as cms

process = cms.Process("rpReco")

#Fredrik Oljemark, private repository+multitrack code, code as of Beginning Jan 2011

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(10)
)


process.load('TotemRawData.Readers.RawDataSource_cfi')
process.source.verbosity = 1
process.source.printProgressFrequency = 0
process.source.skipCorruptedEvents = False
process.source.fileNames.append('/afs/cern.ch/exp/totem/scratch/oljemark/run_3374.000.vmea')

# raw to digi conversion
process.load('TotemCondFormats.DAQInformation.DAQInformationSourceXML_cfi')
process.DAQInformationSourceXML.xmlFileName = '/afs/cern.ch/exp/totem/scratch/jkaspar/software/offline/311/user/mapping/RP_all_new.xml'

process.load('TotemRawData.RawToDigi.RPDataDigiProducer_cfi')
process.RPDataDigiProducer.verbosity = 10

# clusterization
process.load("RecoTotemRP.RPClusterSigmaService.ClusterSigmaServiceConf_cfi")
process.load("RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi")

# reco hit production
process.load("RecoTotemRP.RPRecoHitProducer.RPRecoHitProdConf_cfi")

# geometry
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/TotemRPData/data/2010_10_08/RP_Dist_Beam_Cent.xml")
process.TotemRPGeometryESModule = cms.ESProducer("TotemRPGeometryESModule")

#add alignment here

# track search/pattern recognition
#process.load("RecoTotemRP.RPNonParallelTrackCandidateFinder.RPNonParallelTrackCandidateFinder_cfi")
#process.NonParallelTrackFinder.verbosity = 0
#process.NonParallelTrackFinder.maxHitsPerPlaneToSearch = 4

process.load("RecoTotemRP.RPSingleCandidateTrackFinder.RPSingleTrackCandFindConf_cfi")
process.RPSinglTrackCandFind.Verbosity = 0


process.load("RecoTotemRP.RPMulCandidateTrackFinder.RPMulTrackCandFindConf_cfi")
process.RPMulTrackCandFind.Verbosity = 0
process.RPMulTrackCandFind.Output = cms.int32(1)
process.RPMulTrackCandFind.PlotFile = cms.string('r3374_multiVsSColl_trkFindStats.root')


# track fitting
#process.load("RecoTotemRP.RPTrackCandidateCollectionFitter.RPSingleTrackCandCollFitted_cfi")
#process.RPSingleTrackCandCollFit.Verbosity = 0
process.load("RecoTotemRP.RPMulTrackCandidateCollectionFitter.RPMulTrackCandCollFitter_cfi")
process.RPMulTrackCandCollFit.Verbosity = 0


process.load("RecoTotemRP.RPTrackCandidateCollectionFitter.RPSingleTrackCandCollFitted_cfi")
process.RPSingleTrackCandCollFit.Verbosity = 0

#process.load("RecoTotemRP.RPSingleTrackCandidateCollectionFitter.RPMulTrackCandCollFitter_cfi")
#process.RPMulTrackCandCollFit.Verbosity = 0


# produces a "file summary"
#process.load("TotemAnalysis.RealData.BasicInformation_cfi")
#process.BasicInformation.outputFile = '$info_file'
#process.BasicInformation.luminosityFile = '$luminosity_file'

process.p = cms.Path(
    process.RPDataDigiProducer *
    process.RPClustProd * 
    process.RPHecoHitProd * 
    process.RPMulTrackCandFind *
    process.RPMulTrackCandCollFit *
    process.RPSinglTrackCandFind *
    process.RPSingleTrackCandCollFit
    
#    process.BasicInformation
)


# store desired results
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("r3374_multiVsSColl_trkFindStats.root"),
    outputCommands = cms.untracked.vstring(
        'drop TotemRawEvent_*_*_*', 
        'keep RPDigClusteredmDetSetVector_*_*_*',
        'keep RPMulFittedTrackCollection_*_*_*',
        'keep RPFittedTrackCollection_*_*_*',
        'keep *_RPSingleTrackCandCollFit_*_*',
        'keep RPRecoHitedmDetSetVector_*_*_*',
        'keep RPRecognizedPatternsCollection_*_*_*',
        'keep RPStripDigiedmDetSetVector_*_*_*',
        'keep RPMulTrackCandidateCollection_*_*_*',
        'keep RPTrackCandidateCollection_*_*_*'
    )
)

process.outpath = cms.EndPath(process.output)
