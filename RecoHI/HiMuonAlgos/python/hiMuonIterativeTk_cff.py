import FWCore.ParameterSet.Config as cms

from RecoHI.HiMuonAlgos.HiRegitMuonInitialStep_cff import *
from RecoHI.HiMuonAlgos.HiRegitMuonLowPtTripletStep_cff import *
from RecoHI.HiMuonAlgos.HiRegitMuonPixelPairStep_cff import *
from RecoHI.HiMuonAlgos.HiRegitMuonDetachedTripletStep_cff import *
from RecoHI.HiMuonAlgos.HiRegitMuonMixedTripletStep_cff import *
from RecoHI.HiMuonAlgos.HiRegitMuonPixelLessStep_cff import *
from RecoHI.HiMuonAlgos.HiRegitMuonTobTecStep_cff import *

import RecoTracker.FinalTrackSelectors.trackListMerger_cfi
hiGeneralAndRegitMuTracks = RecoTracker.FinalTrackSelectors.trackListMerger_cfi.trackListMerger.clone(
    TrackProducers = (#cms.InputTag('hiGlobalPrimTracks'),
                      cms.InputTag('hiRegitMuInitialStepTracks'),
                      cms.InputTag('hiRegitMuLowPtTripletStepTracks'),
                      cms.InputTag('hiRegitMuPixelPairStepTracks'),
                      cms.InputTag('hiRegitMuDetachedTripletStepTracks'),
                      cms.InputTag('hiRegitMuMixedTripletStepTracks'),
                      cms.InputTag('hiRegitMuPixelLessStepTracks'),
                      cms.InputTag('hiRegitMuTobTecStepTracks')
                      ),
    selectedTrackQuals = cms.VInputTag(#cms.InputTag("hiInitialStepSelector","hiInitialStepLoose"),
                                       cms.InputTag("hiRegitMuInitialStepSelector","hiRegitMuInitialStepLoose"),
                                       cms.InputTag("hiRegitMuLowPtTripletStepSelector","hiRegitMuLowPtTripletStepLoose"),
                                       cms.InputTag("hiRegitMuPixelPairStepSelector","hiRegitMuPixelPairStep"),
                                       cms.InputTag("hiRegitMuDetachedTripletStepSelector","hiRegitMuDetachedTripletStep"),
                                       cms.InputTag("hiRegitMuMixedTripletStepSelector","hiRegitMuMixedTripletStep"),
                                       cms.InputTag("hiRegitMuPixelLessStepSelector","hiRegitMuPixelLessStep"),
                                       cms.InputTag("hiRegitMuTobTecStepSelector","hiRegitMuTobTecStep")
                                       ),
    hasSelector=cms.vint32(1,1,1,1,1,1,1),
    setsToMerge = cms.VPSet( cms.PSet( tLists=cms.vint32(0,1,2,3,4,5,6), pQual=cms.bool(True))),
    copyExtras = True,
    makeReKeyedSeeds = cms.untracked.bool(False)
    )

hiRegitMuTracking = cms.Sequence(hiRegitMuonInitialStep
                                 *hiRegitMuonDetachedTripletStep
                                 *hiRegitMuonLowPtTripletStep
                                 *hiRegitMuonPixelPairStep
                                 *hiRegitMuonMixedTripletStep
                                 *hiRegitMuonPixelLessStep
                                 *hiRegitMuonTobTecStep
                                 # *hiGeneralAndRegitMuTracks
                                 )

# Emilien
# Seed generator
from RecoMuon.MuonSeedGenerator.standAloneMuonSeeds_cff import *

# Stand alone muon track producer
from RecoMuon.StandAloneMuonProducer.standAloneMuons_cff import *

hiRegitMuTrackingAndSta = cms.Sequence(standAloneMuonSeeds
      *standAloneMuons
      *hiRegitMuTracking)

