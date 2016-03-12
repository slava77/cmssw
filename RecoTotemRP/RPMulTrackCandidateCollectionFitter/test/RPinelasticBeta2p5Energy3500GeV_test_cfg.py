import FWCore.ParameterSet.Config as cms

process = cms.Process("prodRPinelasticBeta2p5Energy3500GeV")					##### 3.5 / 2.5

# Specify the maximum events to simulate
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# Configure the output module (save the result in a file)
process.o1 = cms.OutputModule("PoolOutputModule",
    outputCommands = cms.untracked.vstring('keep *', 'drop *_*_*TrackerHits*_*', 'drop *_*_*Muon*_*', 'drop *_*_*Ecal*_*', 'drop *_*_*Hcal*_*', 'drop *_*_*Calo*_*', 'drop *_*_*Castor*_*', 'drop *_*_*FP420SI_*', 'drop *_*_*ZDCHITS_*', 'drop *_*_*BSCHits_*', 'drop *_*_*ChamberHits_*', 'drop *_*_*FibreHits_*', 'drop *_*_*WedgeHits_*'),
    fileName = cms.untracked.string('file:prodRPinelasticBeta2p5Energy3500GeV.root')		##### 3.5 / 2.5
)

# Configure if you want to detail or simple log information.
# LoggerMax -- detail log info output including: errors.log, warnings.log, infos.log, debugs.log
# LoggerMin -- simple log info output to the standard output (e.g. screen)
process.load("Configuration.TotemCommon.LoggerMin_cfi")


################## STEP 1 - process.generator

# Use random number generator service
process.load("Configuration.TotemCommon.RandomNumbers_cfi")

# Monte Carlo gun 
process.load("IOMC.FlatProtonLogKsiLogTGunMul.Beta2p5Energy3500GeV_cfi")			##### 3.5 / 2.5
process.source.ProtonNumberPerArm = cms.untracked.int32(3)


################## STEP 2 process.SmearingGenerator

# declare optics parameters
process.load("Configuration.TotemOpticsConfiguration.OpticsConfig_3500GeV_2p5_cfi")		##### 3.5 / 2.5

# Smearing
process.load("IOMC.SmearingGenerator.SmearingGenerator_cfi")
process.SmearingGenerator.originalLabel = 'original'
process.SmearingGenerator.modifyLabel = 'source'
# process.SmearingGenerator.verbosity = cms.untracked.uint32(6)


################## STEP 3 process.g4SimHits

# G4 Geometry - beta* specific
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append('Geometry/TotemRPData/data/RP_3500_Beta_2.5_220/RP_Dist_Beam_Cent.xml')		##### 3.5 / 2.5

# Magnetic Field, by default we have 3.8T
process.load("Configuration.StandardSequences.MagneticField_cff")

# G4 simulation & proton transport
process.load("Configuration.TotemCommon.g4SimHits_cfi")
process.g4SimHits.Physics.BeamProtTransportSetup = process.BeamProtTransportSetup
process.g4SimHits.Generator.HepMCProductLabel = 'source'

################## STEP 4 
#### There are the following stages in this step
# process.mix*
# process.RPSiDetDigitizer*
# process.RPClustProd*
# process.RPHecoHitProd*
# process.RPSinglTrackCandFind*
# process.RPSingleTrackCandCollFit

# process.load("Configuration.TotemStandardSequences/RP_Digi_and_TrackReconstruction_cfi") 

# Use particle table
process.load("SimGeneral.HepPDTESSource.pdt_cfi")

# No pile up for the mixing module
process.load("SimGeneral.MixingModule.mixNoPU_cfi")

# RP Strip digitization
process.load("SimTotem.RPDigiProducer.RPSiDetConf_cfi")
# process.RPSiDetDigitizer.RPVerbosity = 1

process.load("RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi")
# process.RPClustProd.Verbosity = 1

process.load("RecoTotemRP.RPRecoHitProducer.RPRecoHitProdConf_cfi")
# process.RPHecoHitProd.Verbosity = 1

process.load("RecoTotemRP.RPMulCandidateTrackFinder.RPMulTrackCandFindConf_cfi")
# process.RPMulTrackCandFind.Verbosity = 1
process.RPMulTrackCandFind.Output = cms.int32(1)

process.load("RecoTotemRP.RPMulTrackCandidateCollectionFitter.RPMulTrackCandCollFitter_cfi")
process.RPMulTrackCandCollFit.Verbosity = 1


process.p1 = cms.Path(process.SmearingGenerator*process.g4SimHits*process.mix*process.RPSiDetDigitizer*process.RPClustProd*process.RPHecoHitProd*process.RPMulTrackCandFind*process.RPMulTrackCandCollFit)

# process.p1 = cms.Path(process.SmearingGenerator*process.g4SimHits*process.mix*process.RPSiDetDigitizer*process.RPClustProd*process.RPHecoHitProd*process.RPSinglTrackCandFind*process.RPSingleTrackCandCollFit)
process.outpath = cms.EndPath(process.o1)

