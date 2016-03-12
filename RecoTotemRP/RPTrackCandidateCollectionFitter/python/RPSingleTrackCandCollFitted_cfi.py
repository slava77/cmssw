import FWCore.ParameterSet.Config as cms

RPSingleTrackCandCollFit = cms.EDProducer("RPTrackCandidateCollectionFitter",
    Verbosity = cms.int32(0),

    # the name of the track-candidate producer module
    #   if empty, the fitter takes the ONLY track-candidate record in the event
    #   use `RPSinglTrackCandFind' for parallel finder
    #   use `NonParallelTrackFinder' for non-parallel finder
    RPTrackCandCollProducer = cms.string(''),
    RPTrackCandidateCollectionLabel = cms.InputTag("RPSinglTrackCandFind")
)
