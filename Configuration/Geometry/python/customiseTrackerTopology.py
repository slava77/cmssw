
import FWCore.ParameterSet.Config as cms

def addRun1MCTopologyFromFile(process):
    process.GlobalTag.toGet = cms.VPSet(cms.PSet(record = cms.string('PTrackerParametersRcd'),
                                                 tag = cms.string('TK_Parameters_Test02'),
                                                 connect = cms.untracked.string("sqlite_file:/afs/cern.ch/work/s/slava77/reltest/CMSSW_7_4_X_2015-02-04-1400-sign516/external/slc6_amd64_gcc491/data/CondTools/Geometry/data/run1_mc.db")
                                                 )
                                        )
    return process

def addRun2MCTopologyFromFile(process):
    process.GlobalTag.toGet = cms.VPSet(cms.PSet(record = cms.string('PTrackerParametersRcd'),
                                                 tag = cms.string('TK_Parameters_Test02'),
                                                 connect = cms.untracked.string("sqlite_file:/afs/cern.ch/work/s/slava77/reltest/CMSSW_7_4_X_2015-02-04-1400-sign516/external/slc6_amd64_gcc491/data/CondTools/Geometry/data/run2_mc.db")
                                                 )
                                        )
    return process
