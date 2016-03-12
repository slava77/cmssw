import FWCore.ParameterSet.Config as cms

MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('warnings', 
        'errors', 
        'infos', 
        'debugs'),
    categories = cms.untracked.vstring('ForwardSim', 
        'TotemRP'),
    debugModules = cms.untracked.vstring('*'),
    errors = cms.untracked.PSet(
        threshold = cms.untracked.string('ERROR')
    ),
    warnings = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    infos = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO')
    ),    
    debugs = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG'),
        INFO = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        DEBUG = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        TotemRP = cms.untracked.PSet(
            limit = cms.untracked.int32(1000000)
        ),
        ForwardSim = cms.untracked.PSet(
            limit = cms.untracked.int32(1000000)
        )
    )
)


