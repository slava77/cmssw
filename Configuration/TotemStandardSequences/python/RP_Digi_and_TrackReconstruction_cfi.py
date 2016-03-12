import FWCore.ParameterSet.Config as cms

# Use random number generator service
from Configuration.TotemCommon.RandomNumbers_cfi import *

# Use particle table
from SimGeneral.HepPDTESSource.pdt_cfi import *

# No pile up for the mixing module
# from SimGeneral.MixingModule.mixNoPU_cfi import *
# No pile up for the mixing module
from Configuration.TotemCommon.mixNoPU_cfi import *

# RP Strip digitization
from SimTotem.RPDigiProducer.RPSiDetConf_cfi import *
# RPSiDetDigitizer.RPVerbosity = 1

from RecoTotemRP.RPClusterizer.RPClusterizationConf_cfi import *
# RPClustProd.Verbosity = 1

from RecoTotemRP.RPRecoHitProducer.RPRecoHitProdConf_cfi import *
# RPRecoHitProd.Verbosity = 1

from RecoTotemRP.RPSingleCandidateTrackFinder.RPSingleTrackCandFindConf_cfi import *
# RPSinglTrackCandFind.Verbosity = 1

from RecoTotemRP.RPTrackCandidateCollectionFitter.RPSingleTrackCandCollFitted_cfi import *
# RPSingleTrackCandCollFit.Verbosity = 1

