import FWCore.ParameterSet.Config as cms

TotemNtuplizer = cms.EDAnalyzer("TotemNtuplizer",
    outputFileName = cms.untracked.string('totem_ntuple.root'),
    verbosity = cms.untracked.uint32(0),

    includeDigi = cms.bool(False),
    includePatterns = cms.bool(False),

    ModulLabelSimu = cms.string('Raw2DigiProducer'),

    ProductLabelSimu = cms.string('rpCCOutput'),
    RPFittedTrackCollectionLabel = cms.InputTag("RPSingleTrackCandCollFit"),
    RPMulFittedTrackCollectionLabel = cms.InputTag("RPMulTrackCandCollFit"),
    RPStripDigiSetLabel = cms.InputTag("RPSiDetDigitizer"),
    RawEventLabel = cms.InputTag("RPSiDetDigitizer"),
    RPDigClusterLabel = cms.InputTag("RPClustProd"),
    RPReconstructedProtonCollectionLabel = cms.InputTag("ElasticReconstruction"),
    RPReconstructedProtonPairCollectionLabel = cms.InputTag("ElasticReconstruction"),

    RoadLabel = cms.string('NewRoadFinder'),


#    primaryProtons = cms.bool(True),
#    primaryJets = cms.bool(True),
#    primaryJetsInstance = cms.string("CentralMCJetReco"),
#    primaryJetsLabel = cms.string("kt6algorithm")
)
