#include "RecoTracker/MkFitCore/interface/Config.h"

#include "RecoTracker/MkFitCore/interface/TrackerInfo.h"
#include "RecoTracker/MkFitCore/interface/IterationConfig.h"

namespace mkfit {

  namespace Config {
    TrackerInfo TrkInfo;
    IterationsInfo ItrInfo;

    int nTracks = 10000;
    int nEvents = 20;
    int nItersCMSSW = 0;
    bool loopOverFile = false;

    int nTotalLayers = -1;

    std::string geomPlugin = "CylCowWLids";

    // Multi threading and Clone engine configuration
    int numThreadsFinder = 1;
    int numThreadsEvents = 1;

#if defined(__AVX512F__)
    int numThreadsSimulation = 60;
#else
    int numThreadsSimulation = 12;
#endif

    int finderReportBestOutOfN = 1;

    /*MM: moving out to IterationParams*/
    //int   nlayers_per_seed = 3; // can be overriden from Geom plugin; a very confusing variable :)
    int numSeedsPerTask = 32;

    // number of hits per task for finding seeds
    int numHitsPerTask = 32;

    // material effects
    float RlgridME[Config::nBinsZME][Config::nBinsRME];
    float XigridME[Config::nBinsZME][Config::nBinsRME];

    //float chi2Cut = 15.;
    //float chi2CutOverlap = 5.;
    //float pTCutOverlap = 0.;

    seedOpts seedInput = simSeeds;
    cleanOpts seedCleaning = noCleaning;

    bool finding_requires_propagation_to_hit_pos;
    PropagationFlags finding_inter_layer_pflags;
    PropagationFlags finding_intra_layer_pflags;
    PropagationFlags backward_fit_pflags;
    PropagationFlags forward_fit_pflags;
    PropagationFlags seed_fit_pflags;
    PropagationFlags pca_prop_pflags;

#ifdef CONFIG_PhiQArrays
    bool usePhiQArrays = true;
#endif

    bool useCMSGeom = false;
    bool readCmsswTracks = false;

    bool dumpForPlots = false;
    bool silent = false;

    bool cf_seeding = false;
    bool cf_fitting = false;

    bool quality_val = false;
    bool sim_val_for_cmssw = false;
    bool sim_val = false;
    bool cmssw_val = false;
    bool fit_val = false;
    bool readSimTrackStates = false;
    bool inclusiveShorts = false;
    bool keepHitInfo = false;
    bool tryToSaveSimInfo = false;
    matchOpts cmsswMatchingFW = hitBased;
    matchOpts cmsswMatchingBK = trkParamBased;

    bool removeDuplicates = false;
    bool useHitsForDuplicates = true;
    const float maxdPt = 0.5;
    const float maxdPhi = 0.25;
    const float maxdEta = 0.05;
    const float maxdR = 0.0025;
    const float minFracHitsShared = 0.75;

    const float maxd1pt = 1.8;     //windows for hit
    const float maxdphi = 0.37;    //and/or dr
    const float maxdcth = 0.37;    //comparisons
    const float maxcth_ob = 1.99;  //eta 1.44
    const float maxcth_fw = 6.05;  //eta 2.5

    bool useDeadModules = false;

    bool mtvLikeValidation = false;
    bool mtvRequireSeeds = false;
    int cmsSelMinLayers = 12;
    int nMinFoundHits = 10;

    bool kludgeCmsHitErrors = false;
    bool backwardFit = false;
    bool backwardSearch = true;
    bool includePCA = false;

    bool json_dump_before = false;
    bool json_dump_after = false;
    bool json_verbose = false;
    std::vector<std::string> json_patch_filenames;
    std::vector<std::string> json_load_filenames;
    std::string json_save_iters_fname_fmt;
    bool json_save_iters_include_iter_info_preamble = false;

    void RecalculateDependentConstants() {}
  }  // namespace Config

}  // end namespace mkfit
