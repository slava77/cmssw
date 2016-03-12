import FWCore.ParameterSet.Config as cms

RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    g4SimHits = cms.PSet(initialSeed = cms.untracked.uint32(9876)),
    SimG4Object = cms.PSet(initialSeed =cms.untracked.uint32(9876)),
    RPSiDetDigitizer = cms.PSet(initialSeed =cms.untracked.uint32(137137)),
    sourceSeed = cms.PSet(initialSeed =cms.untracked.uint32(98765)),
    generator = cms.PSet(initialSeed = cms.untracked.uint32(98766)),
    SmearingGenerator = cms.PSet(initialSeed =cms.untracked.uint32(3849)),
    T2Digis = cms.PSet(initialSeed =cms.untracked.uint32(98765)),
    T2MCl = cms.PSet(initialSeed =cms.untracked.uint32(24141)),
    RPFastStationSimulation = cms.PSet(initialSeed =cms.untracked.uint32(12)),
    RPFastFullSimulation = cms.PSet(initialSeed =cms.untracked.uint32(13)),
    mix = cms.PSet(initialSeed = cms.untracked.uint32(24141)),
    LHCTransport = cms.PSet(initialSeed = cms.untracked.uint32(24143), engineName = cms.untracked.string('TRandom3')
  )

)


