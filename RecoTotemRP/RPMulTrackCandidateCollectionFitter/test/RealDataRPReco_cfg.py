import FWCore.ParameterSet.Config as cms

process = cms.Process("rpReco")

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

process.load("TotemRawData.Readers.RawDataSource_cfi")
process.source.verbosity = 1
process.source.printProgressFrequency = 0
process.source.skipCorruptedEvents = False
process.source.fileNames.append("/afs/cern.ch/exp/totem/scratch/oljemark/run_3374.000.vmea")

# raw to digi conversion
process.load("TotemCondFormats.DAQInformation.DAQInformationSourceXML_cfi")
process.DAQInformationSourceXML.xmlFileName = cms.string("/afs/cern.ch/exp/totem/scratch/jkaspar/software/offline/311/user/mapping/RP_all_new.xml")
#process.DAQInformationSourceXML.xmlFileName = cms.string("/home/oljemark/FMonitor/WS/totemsw/online/monitor/xml_monitor/RP_all_new.xml")

process.load("TotemRawData.RawToDigi.RPDataDigiProducer_cfi")
process.RPDataDigiProducer.verbosity = 10

# clusterization
process.load("RecoTotemRP.RPClusterSigmaService.ClusterSigmaServiceConf_cfi")
process.load("RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi")

# reco hit production
process.load("RecoTotemRP.RPRecoHitProducer.RPRecoHitProdConf_cfi")

# geometry
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/TotemRPData/data/2010_09_21_vsym2/RP_Dist_Beam_Cent.xml")
process.TotemRPGeometryESModule = cms.ESProducer("TotemRPGeometryESModule")

# alignment
process.load("Geometry.TotemRPGeometryBuilder.TotemRPIncludeAlignments_cfi")
process.TotemRPIncludeAlignments.RealFiles = cms.vstring(
  "/afs/cern.ch/exp/totem/scratch/data/RP/2010_09_21/alignment/version5/tb_all_rot/45_220.xml",
  "/afs/cern.ch/exp/totem/scratch/data/RP/2010_09_21/alignment/version5/tb_all_rot/56_220.xml"
)

# track search/pattern recognition
process.load("RecoTotemRP.RPMulCandidateTrackFinder.RPMulTrackCandFindConf_cfi")
process.RPMulTrackCandFind.Verbosity = 1
process.RPMulTrackCandFind.Output = cms.int32(1)

# track fitting
process.load("RecoTotemRP.RPMulTrackCandidateCollectionFitter.RPMulTrackCandCollFitter_cfi")
process.RPMulTrackCandCollFit.Verbosity = 1

# produces a "run summary"
# process.BasicInformation = cms.EDAnalyzer("BasicInformation",
#    outputFile = cms.string("file:RealDataRPRecoInfo.root")
# )

process.p = cms.Path(
   process.RPDataDigiProducer *
   process.RPClustProd * 
   process.RPHecoHitProd * 
   process.RPMulTrackCandFind *
   process.RPMulTrackCandCollFit
#   process.BasicInformation
)

# store desired results
process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("file:RealDataRPRecoOutput3.root"),
    outputCommands = cms.untracked.vstring(
        'drop TotemRawEvent_*_*_*', 
        'keep RPDigClusteredmDetSetVector_*_*_*',
        'keep RPFittedTrackCollection_*_*_*',
        'keep RPRecoHitedmDetSetVector_*_*_*',
        'keep RPRecognizedPatternsCollection_*_*_*',
        'keep RPStripDigiedmDetSetVector_*_*_*',
        'keep RPTrackCandidateCollection_*_*_*'
    )
)

process.outpath = cms.EndPath(process.output)


