import FWCore.ParameterSet.Config as cms

EventNumberArrayFilter = cms.EDFilter("EventNumberArrayFilter",
    # select event numbers
    selectedEventNumbers = cms.vuint32()
)
