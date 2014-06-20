import FWCore.ParameterSet.Config as cms

ttMaker = cms.EDProducer("FlatTTMaker",
                         aliasPrefix = cms.untracked.string("tts"),
                         ttInputTag = cms.InputTag("TTTracksFromPixelDigis:Level1TTTracks")
                         )
