import FWCore.ParameterSet.Config as cms

process = cms.Process("ProdRPClust")

process.MessageLogger = cms.Service("MessageLogger",
    destinations = cms.untracked.vstring('warnings', 
        'errors', 
        'infos', 
        'debugs'),
    categories = cms.untracked.vstring('ForwardSim', 
        'TotemRP'),
    debugModules = cms.untracked.vstring('*'),
    infos = cms.untracked.PSet(
        threshold = cms.untracked.string('INFO')
    ),
    warnings = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    errors = cms.untracked.PSet(
        threshold = cms.untracked.string('ERROR')
    ),
    debugs = cms.untracked.PSet(
        threshold = cms.untracked.string('DEBUG'),
        INFO = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        DEBUG = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        ForwardSim = cms.untracked.PSet(
            limit = cms.untracked.int32(1000000)
        ),
        TotemRP = cms.untracked.PSet(
            limit = cms.untracked.int32(1000000)
        )
    )
)

process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    moduleSeeds = cms.PSet(
        RPSiDetDigitizer = cms.untracked.uint32(137137),
        SimG4Object = cms.untracked.uint32(9876),
        sourceSeed = cms.untracked.uint32(98765),
        VtxSmeared = cms.untracked.uint32(12340567)
    ),
    sourceSeed = cms.untracked.uint32(13579097)
)

process.load("SimGeneral.HepPDTESSource.pdt_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(30)
)

process.source = cms.Source("FlatRandomEGunSource",
    PGunParameters = cms.untracked.PSet(
        PartID = cms.untracked.vint32(2212),
        MinEta = cms.untracked.double(9.8933),  # for t=0.01 .. 0.5
        MinPhi = cms.untracked.double(-3.14159265359),
        MaxEta = cms.untracked.double(11.8494),
        MaxPhi = cms.untracked.double(3.14159265359),
        MinE = cms.untracked.double(6999.9),
        MaxE = cms.untracked.double(7000.1)
    ),
    Verbosity = cms.untracked.int32(0)  # set to 1 (or greater)  for printouts
)

process.VtxSmeared = cms.EDFilter("GaussEvtVtxGenerator",
    MeanX = cms.double(0.0),
    MeanY = cms.double(0.0),
    MeanZ = cms.double(0.0),
    SigmaY = cms.double(0.0015),
    SigmaX = cms.double(0.0015),
    SigmaZ = cms.double(5.3)    # in cm (as in COBRA/OSCAR)
)

# Geometry
process.load("SimG4CMS.TotemRP.test.RPGeometryXML_cfi")

# Magnetic Field
### Full field map, static configuration for each field value
# process.load("Configuration.StandardSequences.MagneticField_20T_cff")
# process.load("Configuration.StandardSequences.MagneticField_30T_cff")
# process.load("Configuration.StandardSequences.MagneticField_35T_cff")
process.load("Configuration.StandardSequences.MagneticField_38T_cff")
# process.load("Configuration.StandardSequences.MagneticField_40T_cff") 

process.o1 = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('digitized_event.root')
)

process.SimG4Object = cms.EDProducer("OscarProducer",
    
    NonBeamEvent = cms.bool(False),

    G4EventManagerVerbosity = cms.untracked.int32(0),
    G4StackManagerVerbosity = cms.untracked.int32(0),
    G4TrackingManagerVerbosity = cms.untracked.int32(0),

    UseMagneticField = cms.bool(True),
    OverrideUserStackingAction = cms.bool(True),
    StoreRndmSeeds = cms.bool(False),
    RestoreRndmSeeds = cms.bool(False),

    # not sure if it should be (un)tracked ?
    PhysicsTablesDirectory = cms.string('PhysicsTables'),
    StorePhysicsTables = cms.bool(False),
    RestorePhysicsTables = cms.bool(False),

    MagneticField = cms.PSet(
        delta = cms.double(1.0),
        UseLocalMagFieldManager = cms.bool(False),
        ConfGlobalMFM = cms.PSet(
            Volume = cms.string('OCMS'),
            OCMS = cms.PSet(
                Type = cms.string('CMSIMField'),
                Stepper = cms.string('G4ClassicalRK4'),
                G4ClassicalRK4 = cms.PSet(
                    MinStep = cms.double(0.1),              # in mm
                    DeltaChord = cms.double(0.001),         # in mm
                    DeltaOneStep = cms.double(0.001),       # in mm
                    DeltaIntersection = cms.double(0.0001), # in mm
                    DeltaIntersectionAndOneStep = cms.untracked.double(-1.0),   # in mm
                    MaximumLoopCounts = cms.untracked.double(1000.0),   # in mm
                    MinimumEpsilonStep = cms.untracked.double(1e-05),   # in mm
                    MaximumEpsilonStep = cms.untracked.double(0.01)     # in mm
                )
            )
        )
    ),

   Physics = cms.PSet(
        type = cms.string('SimG4Core/Physics/TotemRPPhysicsList'),
        DummyEMPhysics = cms.bool(False),
        CutsPerRegion = cms.bool(True),
        DefaultCutValue = cms.double(10000.0),
        G4BremsstrahlungThreshold = cms.double(0.5),    # cut in GeV
        Verbosity = cms.untracked.int32(0),

        GFlashEmin = cms.double(1.0),
        GFlashEmax = cms.double(1000000.0),
        GFlashEToKill = cms.double(0.1),
        
        BeamProtTransportSetup = cms.PSet(
            ModelRootFile = cms.string('Geometry/TotemRPOptics/data/parametrization_90_transp.root'),
            Model_IP_150_R_Name = cms.string('ip5_to_beg_150_station_lhcb1'),
            Model_IP_150_L_Name = cms.string('ip5_to_beg_150_station_lhcb1'),
            Model_IP_150_220_R_Name = cms.string('end_150_station_to_beg_220_station_lhcb1'),
            Model_IP_150_220_L_Name = cms.string('end_150_station_to_beg_220_station_lhcb1'),
            
            # in m, should be consistent with geometry xml definitions
            Model_IP_150_R_Zmin = cms.double(0.0),
            Model_IP_150_R_Zmax = cms.double(148.176),
            Model_IP_150_L_Zmin = cms.double(0.0),
            Model_IP_150_L_Zmax = cms.double(-148.176),
            Model_150_220_R_Zmin = cms.double(151.084),
            Model_150_220_R_Zmax = cms.double(214.02),
            Model_150_220_L_Zmin = cms.double(-151.084),
            Model_150_220_L_Zmax = cms.double(-214.02),

            Verbosity = cms.bool(False)
        )
    ),

    Generator = cms.PSet(
        HepMCProductLabel = cms.string('VtxSmeared'),
        ApplyPtCuts = cms.bool(False),
        ApplyEtaCuts = cms.bool(False),
        ApplyPhiCuts = cms.bool(False),
        MinPhiCut = cms.double(-3.14159265359),     # in radians
        MaxPhiCut = cms.double(3.14159265359),      # according to CMS conventions
        MinEtaCut = cms.double(-9.0),
        MaxEtaCut = cms.double(9.0),
        MinPtCut = cms.double(40.0),
        MaxPtCut = cms.double(999999.0),
        DecLenCut = cms.double(10.0),               # the minimum decay length in cm (!) for mother tracking
        Verbosity = cms.untracked.int32(0)
    ),

    RunAction = cms.PSet(
        StopFile = cms.string('StopRun')
    ),

    EventAction = cms.PSet(
        debug = cms.untracked.bool(False),
        StopFile = cms.string('StopRun'),
        CollapsePrimaryVertices = cms.bool(False)
    ),

    StackingAction = cms.PSet(
        SavePrimaryDecayProductsAndConversions = cms.untracked.bool(False)
    ),

    TrackingAction = cms.PSet(
        DetailedTiming = cms.untracked.bool(False),
        G4TrackManagerVerbosity = cms.untracked.int32(0)
    ),
    
    SteppingAction = cms.PSet(
        CriticalDensity = cms.double(1e-25),
        Verbosity = cms.untracked.int32(0),
        CriticalEnergyForVacuum = cms.double(2.0),
        KillBeamPipe = cms.bool(False)
    ),
    
    Totem_RP_SD = cms.PSet(
        Verbosity = cms.int32(0)
    ),

    Watchers = cms.VPSet(cms.PSet(
        type = cms.string('TotemRP'),
        TotemRP = cms.PSet(
            Names = cms.vstring('TotemHitsRP'),
            FileNameOLD = cms.string('TotemTestRP_Hits_Old.root'),
            RPDebugFileName = cms.string('TotemDebugRP.root'),
            FileName = cms.string('TotemTestRP_Hits.root'),
            Verbosity = cms.bool(False)
        )
    ))
)

# RP Strip digitization
process.load("SimTotem.RPDigiProducer.RPSiDetConf_cfi")

process.mix = cms.EDFilter("MixingModule",
    bunchspace = cms.int32(25),
    Label = cms.string('')
)

process.load("RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi")

process.p1 = cms.Path(process.VtxSmeared*process.SimG4Object*process.mix*process.RPSiDetDigitizer*process.RPClustProd)

process.outpath = cms.EndPath(process.o1)

process.Timing = cms.Service("Timing")

process.Tracer = cms.Service("Tracer")



