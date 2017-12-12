import FWCore.ParameterSet.Config as cms

# Configuration parameters for MAHI
mahiParameters = cms.PSet(

    ts4Thresh        = cms.double(0.0),
    chiSqSwitch      = cms.double(10.0),
    activeBXs        = cms.vint32(-1, 0, 1),
    nMaxItersMin      = cms.int32(500),
    nMaxItersNNLS     = cms.int32(500),
    deltaChiSqThresh  = cms.double(1e-3),
    nnlsThresh        = cms.double(1e-11)
)
