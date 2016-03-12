import FWCore.ParameterSet.Config as cms

from RecoTotemRP.RPClusterSigmaService.ClusterSigmaServiceConf_cfi import *
RPRecoHitProd = cms.EDProducer("RPRecoHitProducer",
    Verbosity = cms.int32(1),
    ClusterLabel = cms.InputTag("RPClustProd")
)


