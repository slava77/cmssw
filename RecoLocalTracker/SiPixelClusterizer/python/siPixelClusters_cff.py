import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Modifier_run3_common_cff import run3_common
from Configuration.ProcessModifiers.gpu_cff import gpu

# conditions used *only* by the modules running on GPU
from CalibTracker.SiPixelESProducers.siPixelROCsStatusAndMappingWrapperESProducer_cfi import siPixelROCsStatusAndMappingWrapperESProducer
from CalibTracker.SiPixelESProducers.siPixelGainCalibrationForHLTGPU_cfi import siPixelGainCalibrationForHLTGPU

# SwitchProducer wrapping the legacy pixel cluster producer or an alias for the pixel clusters information converted from SoA
from RecoLocalTracker.SiPixelClusterizer.SiPixelClusterizer_cfi import siPixelClusters

siPixelClustersTask = cms.Task(
    # SwitchProducer wrapping the legacy pixel cluster producer or an alias for the pixel clusters information converted from SoA
    siPixelClusters
)

# reconstruct the pixel digis and clusters on the gpu
from RecoLocalTracker.SiPixelClusterizer.siPixelRawToClusterCUDA_cfi import siPixelRawToClusterCUDA as _siPixelRawToClusterCUDA
siPixelClustersCUDA = _siPixelRawToClusterCUDA.clone()

run3_common.toModify(siPixelClustersCUDA,
                     # use the pixel channel calibrations scheme for Run 3
                     isRun2 = False,
                     clusterThreshold_layer1 = 4000)

# convert the pixel digis (except errors) and clusters to the legacy format
from RecoLocalTracker.SiPixelClusterizer.siPixelDigisClustersFromSoAPhase1_cfi import siPixelDigisClustersFromSoAPhase1 as _siPixelDigisClustersFromSoAPhase1
from RecoLocalTracker.SiPixelClusterizer.siPixelDigisClustersFromSoAPhase2_cfi import siPixelDigisClustersFromSoAPhase2 as _siPixelDigisClustersFromSoAPhase2

siPixelDigisClusters = _siPixelDigisClustersFromSoAPhase1.clone()

run3_common.toModify(siPixelDigisClusters,
                     clusterThreshold_layer1 = 4000)

from Configuration.Eras.Modifier_phase2_tracker_cff import phase2_tracker

gpu.toReplaceWith(siPixelClustersTask, cms.Task(
    # conditions used *only* by the modules running on GPU
    siPixelROCsStatusAndMappingWrapperESProducer,
    siPixelGainCalibrationForHLTGPU,
    # reconstruct the pixel digis and clusters on the gpu
    siPixelClustersCUDA,
    # convert the pixel digis (except errors) and clusters to the legacy format
    siPixelDigisClusters,
    # SwitchProducer wrapping the legacy pixel cluster producer or an alias for the pixel clusters information converted from SoA
    siPixelClustersTask.copy()
))

from RecoLocalTracker.SiPixelClusterizer.siPixelPhase2DigiToClusterCUDA_cfi import siPixelPhase2DigiToClusterCUDA as _siPixelPhase2DigiToClusterCUDA
# for phase2 no pixel raw2digi is available at the moment
# so we skip the raw2digi step and run on pixel digis copied to gpu

phase2_tracker.toReplaceWith(siPixelClustersCUDA,_siPixelPhase2DigiToClusterCUDA.clone())

from EventFilter.SiPixelRawToDigi.siPixelDigisSoAFromCUDA_cfi import siPixelDigisSoAFromCUDA as _siPixelDigisSoAFromCUDA
siPixelDigisPhase2SoA = _siPixelDigisSoAFromCUDA.clone(
    src = "siPixelClustersCUDA"
)

phase2_tracker.toReplaceWith(siPixelDigisClusters, _siPixelDigisClustersFromSoAPhase2.clone(
                        clusterThreshold_layer1 = 4000,
                        clusterThreshold_otherLayers = 4000,
                        src = "siPixelDigisPhase2SoA",
                        #produceDigis = False
                        ))

(gpu & phase2_tracker).toReplaceWith(siPixelClustersTask, cms.Task(
                            # reconstruct the pixel clusters on the gpu from copied digis
                            siPixelClustersCUDA,
                            # copy from gpu to cpu
                            siPixelDigisPhase2SoA,
                            # convert the pixel digis (except errors) and clusters to the legacy format
                            siPixelDigisClusters,
                            # SwitchProducer wrapping the legacy pixel cluster producer or an alias for the pixel clusters information converted from SoA
                            siPixelClusters))
