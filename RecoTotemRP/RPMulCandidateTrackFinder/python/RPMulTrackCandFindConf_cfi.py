import FWCore.ParameterSet.Config as cms

RPMulTrackCandFind = cms.EDProducer("RPMulCandidateTrackFinder",
    Verbosity = cms.int32(1),
    RoadSize = cms.double(0.2),
    MinHitsPerRP = cms.uint32(5),    # the minimal number of U+V hits per Roman Pot to perform a meaningful track finding
    MinHitsPerRoad = cms.uint32(4),  # the minimal number of U/V hits to build a valid U/V road
    MinDetsPerRoad = cms.uint32(3),  # the minimal number of U/V detectors triggered to build a valid U/V road (combined with MinHitsPerRoad)
    MaxHitsPerDetector = cms.uint32(10),  # the allowable maximun number of U/V hits per detector to filter out the shower case

    PlotFile = cms.string('trackFind_statistics.root'),  # additional statistic record of various plots about the candidate track finder process
    RPList = cms.vuint32(120, 121, 122, 123, 124, 125, 20, 21, 22, 23, 24, 25),  # Roman Pots of interest
    Output = cms.int32(0),                # produce the PlotFile or not
    ProduceHitsNumTree = cms.int32(1),    # produce U/V hits number per detector for each Roman Pot trees or not
    ProduceRPHitsPlot = cms.int32(1),     # produce U-V hits number and distribution for each Roman Pot histograms or not
    ProduceRPRoadsPlot = cms.int32(1),    # produce U-V roads number for each Roman Pot histograms or not
    ProduceRPTracksPlot = cms.int32(1),   # produce (UV) candidate track for each Roman Pot histogram or not

    RPRecoHitDetSetLabel = cms.InputTag("RPRecoHitProd")
)
