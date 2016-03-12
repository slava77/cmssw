import FWCore.ParameterSet.Config as cms

from Configuration.TotemStandardSequences.prodRPinelasticDefault_cfg import *

process.setName_("prodRPinelasticBeta90Energy6500GeV")


# General config
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
  input = cms.untracked.int32(100)
)

# Specify the output filename
#exec 'process.' + str(process.outpath) + '.fileName = cms.untracked.string("file:prodRPinelasticBeta90Energy6500GeV.root")'

# particle generator paramteres
process.load("IOMC.FlatProtonLogKsiLogTGun.Beta90Energy6500GeV_cfi")

# G4 geometry
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append('Geometry/TotemRPData/data/RP_Beta_90/RP_Dist_Beam_Cent.xml')

#process.g4SimHits.Physics.BeamProtTransportSetup = process.BeamProtTransportSetup

#process.p1 = cms.Path(process.generator*process.SmearingGenerator*process.g4SimHits)
process.p1 = cms.Path(process.generator)

# Configure the output module (save the result in a file)
process.o1 = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *'),
    fileName = cms.untracked.string('file:TDTestElastic.root'))
process.outpath = cms.EndPath(process.o1)
