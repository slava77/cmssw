import FWCore.ParameterSet.Config as cms

RPClustProd = cms.EDProducer("RPClusterizer",
    Verbosity = cms.int32(0),
    DigiLabel = cms.InputTag("RPSiDetDigitizer")
)


