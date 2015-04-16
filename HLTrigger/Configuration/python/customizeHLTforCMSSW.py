import FWCore.ParameterSet.Config as cms

# Updating the config (PR #8356)
def customiseFor8356(process):

    MTRBPSet = cms.PSet(
        Rescale_eta = cms.double( 3.0 ),
        Rescale_phi = cms.double( 3.0 ),
        Rescale_Dz = cms.double( 3.0 ),
        EtaR_UpperLimit_Par1 = cms.double( 0.25 ),
        EtaR_UpperLimit_Par2 = cms.double( 0.15 ),
        PhiR_UpperLimit_Par1 = cms.double( 0.6 ),
        PhiR_UpperLimit_Par2 = cms.double( 0.2 ),
        UseVertex = cms.bool( False ),
        Pt_fixed = cms.bool( False ),
        Z_fixed = cms.bool( True ),
        Phi_fixed = cms.bool( False ),
        Eta_fixed = cms.bool( False ),
        Pt_min = cms.double( 1.5 ),
        Phi_min = cms.double( 0.1 ),
        Eta_min = cms.double( 0.1 ),
        DeltaZ = cms.double( 15.9 ),
        DeltaR = cms.double( 0.2 ),
        DeltaEta = cms.double( 0.2 ),
        DeltaPhi = cms.double( 0.2 ),
        maxRegions = cms.int32( 1 ),
        precise = cms.bool( True ),
        OnDemand = cms.int32( -1 ),
        MeasurementTrackerName = cms.InputTag( "hltESPMeasurementTracker" ),
        beamSpot = cms.InputTag( "hltOnlineBeamSpot" ),
        vertexCollection = cms.InputTag( "pixelVertices" ),
        input = cms.InputTag( 'hltL2Muons','UpdatedAtVtx' )
    )

    listL3trackingModule = ['hltL3MuonsOIState','hltL3MuonsOIHit','hltL3MuonsIOHit','hltL3NoFiltersNoVtxMuonsOIState','hltL3NoFiltersNoVtxMuonsOIHit','hltL3NoFiltersNoVtxMuonsIOHit']
    for l3module in listL3trackingModule:
        if hasattr(process, l3module):
            if hasattr(getattr(process, l3module), 'GlbRefitterParameters'):
                setattr(getattr(process, l3module).GlbRefitterParameters, 'RefitFlag', cms.bool(True))
            if hasattr(getattr(process, l3module), 'L3TrajBuilderParameters'):
                if hasattr(getattr(process, l3module).L3TrajBuilderParameters, 'MuonTrackingRegionBuilder'):
                    setattr(getattr(process, l3module).L3TrajBuilderParameters, 'MuonTrackingRegionBuilder', MTRBPSet)

    listL3seedingModule = ['hltL3TrajSeedIOHit','hltL3NoFiltersNoVtxTrajSeedIOHit']
    for l3seeding in listL3seedingModule:
        if hasattr(process, l3seeding):
            if hasattr(getattr(process, l3seeding), 'MuonTrackingRegionBuilder'):
		setattr(getattr(process, l3seeding), 'MuonTrackingRegionBuilder', MTRBPSet)

    return process




# Simplified TrackerTopologyEP config (PR #7966)
def customiseFor7966(process):
    if hasattr(process, 'trackerTopology'):
        params = process.trackerTopology.parameterNames_()
        for param in params:
            delattr(process.trackerTopology, param)
        setattr(process.trackerTopology, 'appendToDataLabel', cms.string(""))
    return process


# Removal of 'upgradeGeometry' from TrackerDigiGeometryESModule (PR #7794)
def customiseFor7794(process):
    if hasattr(process, 'TrackerDigiGeometryESModule'):
        if hasattr(process.TrackerDigiGeometryESModule, 'trackerGeometryConstants'):
            if hasattr(process.TrackerDigiGeometryESModule.trackerGeometryConstants, 'upgradeGeometry'):
                delattr(process.TrackerDigiGeometryESModule.trackerGeometryConstants, 'upgradeGeometry')
    return process


# CMSSW version specific customizations
def customiseHLTforCMSSW(process,menuType="GRun",fastSim=False):
    import os
    cmsswVersion = os.environ['CMSSW_VERSION']

    if cmsswVersion >= "CMSSW_7_5":
        process = customiseFor7966(process)
        process = customiseFor7794(process)
        process = customiseFor8356(process)


    return process
