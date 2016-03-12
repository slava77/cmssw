import FWCore.ParameterSet.Config as cms

source = cms.Source("RawDataSource",
    # if non-zero, prints a file summary in the beginning
    verbosity = cms.untracked.uint32(1),
    
    skipCorruptedEvents = cms.bool(True),

    # event number will be printed every 'printProgressFrequency' events,
    # nothing printed if 0
    printProgressFrequency = cms.untracked.uint32(0),

    # the list of files to be processed
    fileNames = cms.untracked.vstring(),

    # whether run numbers shall be extracted from input file names
    setRunNumberFromFileName = cms.bool(True)
)
