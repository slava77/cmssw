import FWCore.ParameterSet.Config as cms

displacedRegionProducer = cms.EDProducer('DisplacedRegionProducer',
    rParam = cms.double(1.0),

    minRadius = cms.double(-1.0),
    discriminatorCut = cms.double(-1.0),
    genMatch = cms.string("None"),
    input_names = cms.vstring("phi_0", "phi_1"),
    output_names = cms.vstring("model_5/activation_10/Softmax"),
    nThreads = cms.uint32(1),
    singleThreadPool = cms.string("no_threads"),
    graph_path = cms.FileInPath("RecoTracker/DisplacedRegionalTracking/data/FullData_Phi-64-128-256_16-32-64_F-128-64-32_Model.pb"),

    beamSpot = cms.InputTag("offlineBeamSpot"),
    trackClusters = cms.InputTag("generalV0Candidates", "Kshort", "TREEMAKER"),
    genParticles = cms.InputTag("genParticles"),
)
