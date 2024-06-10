import FWCore.ParameterSet.Config as cms

from RecoTracker.LST.lstProducer_cfi import lstProducer

# not scheduled task to get the framework to hide the producer
from RecoTracker.LST.lstModulesDevESProducer_cfi import lstModulesDevESProducer
dummyLSTModulesDevESProducerTask = cms.Task(lstModulesDevESProducer)
