import FWCore.ParameterSet.Config as cms

## filter to study the performance of the CSC system when at random wiregroups, half-strips or TMBs fail
cscGoodTriggerDigis = cms.EDProducer("CSCGoodTriggerDigisProducer",
    CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi"),
    CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi"),
)

