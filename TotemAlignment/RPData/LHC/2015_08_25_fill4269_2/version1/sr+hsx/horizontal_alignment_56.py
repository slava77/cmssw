import FWCore.ParameterSet.Config as cms

process = cms.Process("ModifySingularModes")

# minimum of logs
process.load("Configuration.TotemCommon.LoggerMin_cfi")

# empty source
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)

# geometry
#process.load("Configuration.TotemCommon.geometryRP_cfi")
process.load("Configuration.TotemCommon.geometryRP_real_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append("Geometry/TotemRPData/data/2015_08_25_fill4269_1/RP_Dist_Beam_Cent.xml")
process.TotemRPGeometryESModule = cms.ESProducer("TotemRPGeometryESModule")

# include alignments, if any
process.load("TotemAlignment.RPDataFormats.TotemRPIncludeAlignments_cfi")
process.TotemRPIncludeAlignments.RealFiles = cms.vstring()

sector = "56"

sign = +1.
if sector == "45":
  sign = -1.

# worker
process.load("TotemAlignment.RPTrackBased.ModifySingularModes_cfi")
process.ModifySingularModes.inputFile = '/afs/cern.ch/work/j/jkaspar/software/offline/704/src/TotemAlignment/RPData/LHC/2015_08_25_fill4269_1/sr/56.xml'
process.ModifySingularModes.outputFile = '/afs/cern.ch/work/j/jkaspar/software/offline/704/src/TotemAlignment/RPData/LHC/2015_08_25_fill4269_1/sr+hsx/56.xml'

process.TotemRPIncludeAlignments.RealFiles = cms.vstring('/afs/cern.ch/work/j/jkaspar/software/offline/704/src/TotemAlignment/RPData/LHC/2015_08_25_fill4269_1/sr/56.xml')

process.ModifySingularModes.z1 = 214628*sign # near vert
process.ModifySingularModes.z2 = 220000*sign # far vert

process.ModifySingularModes.de_x1 = .483
process.ModifySingularModes.de_x2 = -.491

process.ModifySingularModes.de_y1 = 0
process.ModifySingularModes.de_y2 = 0

process.p = cms.Path(process.ModifySingularModes)
