import FWCore.ParameterSet.Config as cms

RP220Reconst = cms.EDProducer("RPPrimaryVertexInelasticReconstruction_RunI",

    RPFittedTrackCollectionLabel = cms.InputTag("RPSingleTrackCandCollFit"),

    StripAlignmentResolutionDegradation = cms.double(1.7),

    HepMCProductLabel = cms.string('generator'),

    ConstrainPrimaryVertex = cms.bool(True),
    ElasticScatteringReconstruction = cms.bool(False),

    Station150MandatoryInReconstruction = cms.bool(False),
    Station220MandatoryInReconstruction = cms.bool(True),
    AnyStationInReconstruction = cms.bool(False),
    ExternalPrimaryVertex = cms.bool(False),

    PrimaryVertexXSigma = cms.double(0.03), # mm
    PrimaryVertexYSigma = cms.double(0.03), # mm
    PrimaryVertexZSigma = cms.double(0.03), # mm

    ParameterizationFileName220Right = cms.string('Geometry/TotemRPOptics/data/parametrization_6500GeV_90_reco.root'),
    ParameterizationFileName220Left = cms.string('Geometry/TotemRPOptics/data/parametrization_6500GeV_90_reco.root'),
    ParameterizationFileName150Right = cms.string('Geometry/TotemRPOptics/data/parametrization_6500GeV_90_reco.root'),
    ParameterizationFileName150Left = cms.string('Geometry/TotemRPOptics/data/parametrization_6500GeV_90_reco.root'),

    ParameterizationNamePrefix220Right = cms.string('ip5_to_station_220'),
    ParameterizationNamePrefix220Left = cms.string('ip5_to_station_220'),
    ParameterizationNamePrefix150Right = cms.string('ip5_to_station_150'),
    ParameterizationNamePrefix150Left = cms.string('ip5_to_station_150'),

    RightBeamPostfix = cms.string('lhcb1'),
    LeftBeamPostfix = cms.string('lhcb2'),

    ComputeFullVarianceMatrix = cms.bool(True),

    RPMultipleScatteringSigma = cms.double(5.7e-07), # rad

    ReconstructionPrecisionX = cms.double(0.001),    # mm
    ReconstructionPrecisionY = cms.double(0.001),    # mm

    ReconstructionPrecisionThetaX = cms.double(1e-06),  # rad
    ReconstructionPrecisionThetaY = cms.double(2e-07),  # rad
    ReconstructionPrecisionKsi = cms.double(0.001), # -1 .. 0

    InitMinX = cms.double(-0.6),
    InitMinY = cms.double(-0.6),

    InitMinThetaX = cms.double(-0.00045),
    InitMinThetaY = cms.double(-0.00045),
    InitMinKsi = cms.double(-0.3),

    InitMaxX = cms.double(0.6),
    InitMaxY = cms.double(0.6),

    InitMaxThetaX = cms.double(0.00045),
    InitMaxThetaY = cms.double(0.00045),
    InitMaxKsi = cms.double(0.0),

    RandomSearchProbability = cms.double(0.3),

    InitIterationsNumber = cms.int32(20),
    MaxChiSqNDFOfConvergedProton = cms.double(15.0),
    MaxChiSqOfConvergedInitialisation = cms.double(100.0),
    MaxAllowedReconstructedXi = cms.double(0.05),
    OutOfXiRangePenaltyFactor = cms.double(10000.0),
    InverseParamRandSeed = cms.int32(14142135),

    BeamProtTransportSetup = cms.PSet(),
    ExpectedRPResolution = cms.double(0.016) # mm
)


