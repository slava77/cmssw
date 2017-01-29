import FWCore.ParameterSet.Config as cms

gsFixedRefinedBarrelSuperClusters = cms.EDProducer("EGRefinedSCFixer",
                                                   fixedSC=cms.InputTag("particleFlowSuperClusterECALGSFixed","particleFlowSuperClusterECALBarrel"),
                                                   orgSC=cms.InputTag("particleFlowSuperClusterECAL","particleFlowSuperClusterECALBarrel",processName=cms.InputTag.skipCurrentProcess()),
                                                   orgRefinedSC=cms.InputTag("particleFlowEGamma",processName=cms.InputTag.skipCurrentProcess()),
                                                   fixedPFClusters=cms.InputTag("particleFlowClusterECALGSFixed"),
                                                   )
