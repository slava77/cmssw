import FWCore.ParameterSet.Config as cms

EventNumberTimeFilter = cms.EDFilter("EventNumberTimeFilter",
    # select source of raw-event
    rawEventLabel = cms.InputTag("source"),

    # filter on CMSSW event number (starting at 1)
    eventNumber = cms.PSet(
        active = cms.bool(False),
        min = cms.uint32(0),
        max = cms.uint32(0),
    ),

    # filter on event number from raw data file (starting at 0)
    rawEventNumber = cms.PSet(
        active = cms.bool(False),
        min = cms.uint32(0),
        max = cms.uint32(0),
    ),

    # filter on timestamp
    # value = number of seconds from 00:00:00 1/1/1970 UTC,
    # can be obtained by date program, e.g.
    #   date -d "Jun 25 20:02" +%s
    timestamp = cms.PSet(
        active = cms.bool(False),
        min = cms.uint32(0),
        max = cms.uint32(0),
    )
)
