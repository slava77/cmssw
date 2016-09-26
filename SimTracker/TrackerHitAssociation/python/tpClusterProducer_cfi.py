import FWCore.ParameterSet.Config as cms

from SimTracker.TrackerHitAssociation.tpClusterProducerDefault_cfi import *

tpClusterProducer = tpClusterProducerDefault.clone()

from Configuration.StandardSequences.Eras import eras
eras.phase2_tracker.toModify( 
    tpClusterProducer,
    pixelSimLinkSrc = cms.InputTag("simSiPixelDigis", "Pixel"),
    phase2OTSimLinkSrc = cms.InputTag("simSiPixelDigis","Tracker")
)
