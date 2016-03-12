import FWCore.ParameterSet.Config as cms

BeamOpticsParamsESSource = cms.ESSource("BeamOpticsParamsESSource",
    BeamEnergy = cms.double(6500.0), # Gev
    ProtonMass = cms.double(0.938272029), # Gev
    LightSpeed = cms.double(300000000.0),
    NormalizedEmittanceX = cms.double(2.75e-06),
    NormalizedEmittanceY = cms.double(2.75e-06),
    BetaStarX = cms.double(90.0), # m
    BetaStarY = cms.double(90.0), # m
    CrossingAngleX = cms.double(50e-6),
    CrossingAngleY = cms.double(0.0),
    BeamDisplacementX = cms.double(0.0), # m
    BeamDisplacementY = cms.double(0.0), # m
    BeamDisplacementZ = cms.double(0.0), # m
    BunchSizeZ = cms.double(0.07), # m
    MeanXi = cms.double(0.0), # energy smearing
    SigmaXi = cms.double(0.0001)
)

ProtonTransportFunctionsESSource = cms.ESProducer("ProtonTransportFunctionsESSource",
    opticsFile = cms.string(''), # automatic
    maySymmetrize = cms.bool(True), # this optic is assymmetric
    verbosity = cms.untracked.uint32(1)
)

BeamProtTransportSetup = cms.PSet(
    Verbosity = cms.bool(False),
    ModelRootFile = cms.string('Geometry/TotemRPOptics/data/parametrization_6500GeV_90p0_50urad_transp.root'),
    Model_IP_150_R_Name = cms.string('ip5_to_beg_150_station_lhcb1'),
    Model_IP_150_L_Name = cms.string('ip5_to_beg_150_station_lhcb1'),

    # in m, should be consistent with geometry xml definitions
    Model_IP_150_R_Zmin = cms.double(0.0),
    Model_IP_150_R_Zmax = cms.double(202.769),
    Model_IP_150_L_Zmax = cms.double(-202.769),
    Model_IP_150_L_Zmin = cms.double(0.0),
)

