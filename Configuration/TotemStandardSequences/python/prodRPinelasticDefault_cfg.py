import FWCore.ParameterSet.Config as cms

process = cms.Process("prodRPinelasticBetaXXXEnergyYYYTeV")

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Configure the output module (save the result in a file)
process.o1 = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('file:prodRPinelasticBetaXXXEnergyYYYTeV.root')
)

# Configure if you want to detail or simple log information.
# LoggerMax -- detail log info output including: errors.log, warnings.log, infos.log, debugs.log
# LoggerMin -- simple log info output to the standard output (e.g. screen)
process.load("Configuration.TotemCommon.LoggerMax_cfi")


################## STEP 1 - process.generator
process.source = cms.Source("EmptySource")

# Use random number generator service
process.load("Configuration.TotemCommon.RandomNumbers_cfi")

################## STEP 2 process.SmearingGenerator

# Smearing
process.load("IOMC.SmearingGenerator.SmearingGenerator_cfi")

################## STEP 3 process.g4SimHits
# Magnetic Field, by default we have 3.8T
process.load("Configuration.StandardSequences.MagneticField_cff")

# G4 simulation & proton transport
process.load("Configuration.TotemCommon.g4SimHits_cfi")
process.g4SimHits.Generator.HepMCProductLabel = 'generator'

process.outpath = cms.EndPath(process.o1)
