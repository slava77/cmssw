import FWCore.ParameterSet.Config as cms

process = cms.Process("GeometryTree")

# logs
process.load("FWCore.MessageService.MessageLogger_cfi")

# geometry
process.load("Configuration.TotemCommon.geometryRP_cfi")
process.XMLIdealGeometryESSource.geomXMLFiles.append('Geometry/TotemRPData/data/RP_Garage/RP_Dist_Beam_Cent.xml')

# (no) events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
process.source = cms.Source("EmptySource")

# teh analyzer
process.tree = cms.EDAnalyzer('GeometryTree')

process.p = cms.Path(process.tree)
