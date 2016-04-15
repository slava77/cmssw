import FWCore.ParameterSet.Config as cms

# Muon Digitization (CSC, DT, RPC electronics responce)
# CSC digitizer
#
from SimMuon.CSCDigitizer.muonCSCDigis_cfi import *
from CalibMuon.CSCCalibration.CSCChannelMapper_cfi import *
from CalibMuon.CSCCalibration.CSCIndexer_cfi import *
# DT digitizer
#
from SimMuon.DTDigitizer.muondtdigi_cfi import *
# RPC digitizer
# 
from SimMuon.RPCDigitizer.muonrpcdigi_cfi import *
muonDigi = cms.Sequence(simMuonCSCDigis+simMuonDTDigis+simMuonRPCDigis)

def _modifySimMuonForRun3( theProcess ):
    theProcess.load("SimMuon.GEMDigitizer.muonGEMDigi_cff")
    theProcess.muonDigi += theProcess.muonGEMDigi

def _modifySimMuonForPhase2( theProcess ):
    theProcess.load("SimMuon.GEMDigitizer.muonME0DigisPreReco_cfi")
    theProcess.muonDigi += theProcess.simMuonME0Digis

from Configuration.StandardSequences.Eras import eras
modifyConfigurationStandardSequencesSimMuonRun3_ = eras.run3_GEM.makeProcessModifier( _modifySimMuonForRun3 )
modifyConfigurationStandardSequencesSimMuonPhase2_ = eras.phase2_muon.makeProcessModifier( _modifySimMuonForPhase2 )
