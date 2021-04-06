import FWCore.ParameterSet.Config as cms
from RecoTracker.FinalTrackSelectors.TrackCollectionMerger_cfi import *
from RecoTracker.FinalTrackSelectors.trackAlgoPriorityOrder_cfi import trackAlgoPriorityOrder

import RecoTracker.FinalTrackSelectors.trackListMerger_cfi
displacedRegionalStepInputTracks =  TrackCollectionMerger.clone()
displacedRegionalStepInputTracks.trackProducers = ['earlyGeneralTracks',
                                             'muonSeededTracksInOut',
                                             'muonSeededTracksOutIn'
                                     ]
displacedRegionalStepInputTracks.inputClassifiers =["earlyGeneralTracks",
                                              "muonSeededTracksInOutClassifier",
                                              "muonSeededTracksOutInClassifier"
                                      ]
from Configuration.Eras.Modifier_trackingLowPU_cff import trackingLowPU
trackingLowPU.toModify(displacedRegionalStepInputTracks,
    trackProducers = [
        'earlyGeneralTracks',
        'muonSeededTracksInOut',
        'muonSeededTracksOutIn'
    ],
    inputClassifiers = [
        "earlyGeneralTracks",
        "muonSeededTracksInOutClassifier",
        "muonSeededTracksOutInClassifier"
    ]
)
from Configuration.Eras.Modifier_trackingPhase1_cff import trackingPhase1
_forPhase1 = dict(
    trackProducers = [
        'earlyGeneralTracks',
        'muonSeededTracksInOut',
        'muonSeededTracksOutIn'
    ],
    inputClassifiers = [
        "earlyGeneralTracks",
        "muonSeededTracksInOutClassifier",
        "muonSeededTracksOutInClassifier"
    ],
)
trackingPhase1.toModify(displacedRegionalStepInputTracks, **_forPhase1)
