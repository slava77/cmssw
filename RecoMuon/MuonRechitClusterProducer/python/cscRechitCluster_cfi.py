import FWCore.ParameterSet.Config as cms

ca4cscRechitClusters = cms.EDProducer(
        "CSCRechitClusterProducer",
        rParam = cms.double(0.4),
        nRechitMin = cms.int32(50)
)

ca2cscRechitClusters = cms.EDProducer(
        "CSCRechitClusterProducer",
        rParam = cms.double(0.2),
        nRechitMin = cms.int32(50)
)
ca3cscRechitClusters = cms.EDProducer(
        "CSCRechitClusterProducer",
        rParam = cms.double(0.3),
        nRechitMin = cms.int32(50)
)

