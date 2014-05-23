import FWCore.ParameterSet.Config as cms

from SLHCUpgradeSimulations.L1TrackTriggerObjects.l1TkMuonsExt_cfi import l1TkMuonsExt
l1TkMuonsExtCSC = l1TkMuonsExt.clone(
    L1MuonsInputTag = cms.InputTag("l1extraMuExtended", "csc"),
    ETAMIN = cms.double(1.1)
    )


l1TkMuonsExtSequence = cms.Sequence (l1TkMuonsExt * l1TkMuonsExtCSC)

