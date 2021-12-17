#ifndef RecoTracker_MkFitCore_interface_Config_h
#define RecoTracker_MkFitCore_interface_Config_h

#include <algorithm>
#include <cmath>
#include <string>  // won't compile on clang gcc for mac OS w/o this!
#include <map>
#include <vector>

namespace mkfit {

  // Cram this in here for now ...
  class TrackerInfo;
  class IterationsInfo;
  //------------------------------------------------------------------------------

  enum PropagationFlagsEnum {
    PF_none = 0,

    PF_use_param_b_field = 0x1,
    PF_apply_material = 0x2
  };

  struct PropagationFlags {
    union {
      struct {
        bool use_param_b_field : 1;
        bool apply_material : 1;
        // Could add: bool use_trig_approx  -- now Config::useTrigApprox = true
        // Could add: int  n_iter : 8       -- now Config::Niter = 5
      };

      unsigned int _raw_;
    };

    PropagationFlags() : _raw_(0) {}

    PropagationFlags(int pfe)
        : use_param_b_field(pfe & PF_use_param_b_field), apply_material(pfe & PF_apply_material) {}
  };

  //------------------------------------------------------------------------------

  using IntVec = std::vector<int>;
  using IntVec_i = IntVec::iterator;

  // Enum for input seed options
  enum seedOpts { simSeeds, cmsswSeeds, findSeeds };
  typedef std::map<std::string, std::pair<seedOpts, std::string> > seedOptsMap;

  // Enum for seed cleaning options
  enum cleanOpts { noCleaning, cleanSeedsN2, cleanSeedsPure, cleanSeedsBadLabel };
  typedef std::map<std::string, std::pair<cleanOpts, std::string> > cleanOptsMap;

  // Enum for cmssw matching options
  enum matchOpts { trkParamBased, hitBased, labelBased };
  typedef std::map<std::string, std::pair<matchOpts, std::string> > matchOptsMap;

  //------------------------------------------------------------------------------

  namespace Config {
    extern TrackerInfo TrkInfo;
    extern IterationsInfo ItrInfo;

    // default file version
    constexpr int FileVersion = 1;

    // math general --> from namespace TMath
    constexpr float PI = 3.14159265358979323846;
    constexpr float TwoPI = 6.28318530717958647692;
    constexpr float PIOver2 = Config::PI / 2.0f;
    constexpr float PIOver4 = Config::PI / 4.0f;
    constexpr float PI3Over4 = 3.0f * Config::PI / 4.0f;
    constexpr float InvPI = 1.0f / Config::PI;
    constexpr float RadToDeg = 180.0f / Config::PI;
    constexpr float DegToRad = Config::PI / 180.0f;
    constexpr float sol = 0.299792458;  // speed of light in nm/s
    constexpr double Sqrt2 = 1.4142135623730950488016887242097;
    constexpr double OOSqrt2 = 1.0 / Config::Sqrt2;

    // general parameters of matrices
    constexpr int nParams = 6;

    // config on main + mkFit
    extern int nTracks;  //defined in Config.cc by default or when reading events from file
    extern int nEvents;
    extern int nItersCMSSW;
    extern bool loopOverFile;
    // XXXXMT: nTracks should be thrown out ... SMatrix and Event allocate some arrays on this
    // which can be wrong for real data or in multi-event environment

    extern std::string geomPlugin;

    // config on main -- for geometry; XXXXMT to be thrown out, too
    constexpr int nLayers = 10;  // default: 10; cmssw tests: 13, 17, 26 (for endcap)

    // New layer constants for common barrel / endcap. I'd prefer those to go
    // into some geometry definition "plugin" -- they belong more into some Geom
    // namespace, too.
    // XXXX This needs to be generalized for other geometries !
    // TrackerInfo more or less has all this information (or could have it).
    extern int nTotalLayers;         // To be set by geometry plugin.
    constexpr int nMaxTrkHits = 64;  // Used for array sizes in MkFitter/Finder, max hits in toy MC
    constexpr int nAvgSimHits = 32;  // Used for reserve() calls for sim hits/states

    constexpr float fRadialSpacing = 4.;
    constexpr float fRadialExtent = 0.01;
    constexpr float fInnerSensorSize = 5.0;  // approximate sensor size in cm
    constexpr float fOuterSensorSize = Config::fInnerSensorSize * 2.;
    constexpr float fEtaDet = 1;  // default: 1; cmssw tests: 2, 2.5

    constexpr float cmsDeltaRad = 2.5;  //fixme! using constant 2.5 cm, to be taken from layer properties

    // config for material effects in cmssw
    constexpr float rangeZME = 300.;
    constexpr int nBinsZME = 300;
    constexpr float rangeRME = 120.;
    constexpr int nBinsRME = 120;

    constexpr float Rl[136] = {
        0.018, 0.031, 0.017, 0.023, 0.018, 0.028, 0.021, 0.040, 0.066, 0.039, 0.069, 0.040, 0.103, 0.098, 0.028, 0.038,
        0.025, 0.034, 0.037, 0.078, 0.048, 0.064, 0.168, 0.085, 0.144, 0.033, 0.157, 0.078, 0.014, 0.032, 0.052, 0.012,
        0.026, 0.038, 0.015, 0.035, 0.061, 0.015, 0.035, 0.043, 0.015, 0.036, 0.033, 0.010, 0.021, 0.022, 0.093, 0.084,
        0.100, 0.194, 0.093, 0.108, 0.200, 0.093, 0.084, 0.100, 0.194, 0.093, 0.108, 0.200, 0.038, 0.075, 0.038, 0.075,
        0.038, 0.075, 0.038, 0.075, 0.038, 0.075, 0.038, 0.075, 0.039, 0.078, 0.039, 0.078, 0.039, 0.078, 0.039, 0.078,
        0.039, 0.078, 0.039, 0.078, 0.046, 0.023, 0.046, 0.023, 0.046, 0.046, 0.023, 0.046, 0.023, 0.046, 0.048, 0.024,
        0.048, 0.024, 0.048, 0.048, 0.024, 0.048, 0.024, 0.048, 0.055, 0.027, 0.055, 0.027, 0.055, 0.055, 0.027, 0.055,
        0.027, 0.055, 0.043, 0.021, 0.043, 0.043, 0.043, 0.021, 0.043, 0.043, 0.040, 0.020, 0.040, 0.040, 0.040, 0.020,
        0.040, 0.040, 0.014, 0.028, 0.028, 0.014, 0.028, 0.028};

    constexpr float Xi[136] = {
        0.039e-03, 0.062e-03, 0.029e-03, 0.037e-03, 0.032e-03, 0.049e-03, 0.044e-03, 0.080e-03, 0.147e-03, 0.086e-03,
        0.162e-03, 0.092e-03, 0.214e-03, 0.207e-03, 0.062e-03, 0.081e-03, 0.051e-03, 0.068e-03, 0.078e-03, 0.155e-03,
        0.110e-03, 0.138e-03, 0.321e-03, 0.166e-03, 0.311e-03, 0.077e-03, 0.371e-03, 0.185e-03, 0.035e-03, 0.069e-03,
        0.104e-03, 0.025e-03, 0.051e-03, 0.072e-03, 0.033e-03, 0.069e-03, 0.114e-03, 0.033e-03, 0.071e-03, 0.083e-03,
        0.033e-03, 0.073e-03, 0.064e-03, 0.021e-03, 0.043e-03, 0.043e-03, 0.216e-03, 0.209e-03, 0.185e-03, 0.309e-03,
        0.216e-03, 0.255e-03, 0.369e-03, 0.216e-03, 0.209e-03, 0.185e-03, 0.309e-03, 0.216e-03, 0.255e-03, 0.369e-03,
        0.083e-03, 0.166e-03, 0.083e-03, 0.166e-03, 0.083e-03, 0.166e-03, 0.083e-03, 0.166e-03, 0.083e-03, 0.166e-03,
        0.083e-03, 0.166e-03, 0.088e-03, 0.175e-03, 0.088e-03, 0.175e-03, 0.088e-03, 0.175e-03, 0.088e-03, 0.175e-03,
        0.088e-03, 0.175e-03, 0.088e-03, 0.175e-03, 0.104e-03, 0.052e-03, 0.104e-03, 0.052e-03, 0.104e-03, 0.104e-03,
        0.052e-03, 0.104e-03, 0.052e-03, 0.104e-03, 0.110e-03, 0.055e-03, 0.110e-03, 0.055e-03, 0.110e-03, 0.110e-03,
        0.055e-03, 0.110e-03, 0.055e-03, 0.110e-03, 0.130e-03, 0.065e-03, 0.130e-03, 0.065e-03, 0.130e-03, 0.130e-03,
        0.065e-03, 0.130e-03, 0.065e-03, 0.130e-03, 0.097e-03, 0.048e-03, 0.097e-03, 0.097e-03, 0.097e-03, 0.048e-03,
        0.097e-03, 0.097e-03, 0.089e-03, 0.045e-03, 0.089e-03, 0.089e-03, 0.089e-03, 0.045e-03, 0.089e-03, 0.089e-03,
        0.030e-03, 0.061e-03, 0.061e-03, 0.030e-03, 0.061e-03, 0.061e-03};

    extern float RlgridME[Config::nBinsZME][Config::nBinsRME];
    extern float XigridME[Config::nBinsZME][Config::nBinsRME];

    // This will become layer dependent (in bits). To be consistent with min_dphi.
    static constexpr int m_nphi = 256;

    // config on Event
    /*MM: chi2Cut is now set in IterationsParams*/
    //extern    float chi2Cut; // default: 15; cmssw: 30 (set in TrackerInfo plugin)
    //extern    float chi2CutOverlap; // default: 5; cmssw: 3.5
    //extern    float pTCutOverlap; // default: 0; cmssw: 1

    // the following are only used in SMatrix version
    constexpr float nSigma = 3.;
    constexpr float minDPhi = 0.01;  // default: 0.;  cmssw tests: 0.01;
    constexpr float maxDPhi = Config::PI;
    constexpr float minDEta = 0.;
    constexpr float maxDEta = 1.0;

    // Configuration for simulation info
    // CMS beam spot width 25um in xy and 5cm in z
    constexpr float beamspotX = 0.1;
    constexpr float beamspotY = 0.1;
    constexpr float beamspotZ = 1.0;

    // XXMT4K minPt was 0.5. Figure out what is the new limit for 90cm or be
    // more flexible about finding fewer hits. Or postprocess looper candidates.
    constexpr float minSimPt = 1;
    constexpr float maxSimPt = 10.;

    // XXMT Hardhack -- transition region excluded in Simulation::setupTrackByToyMC()
    constexpr float minSimEta = -2.4;
    constexpr float maxSimEta = 2.4;
    // For testing separate EC-/BRL/EC+; -2.3--1.5 / -0.9-0.9 / 1.5-2.3
    //constexpr float minSimEta =  -0.9;
    //constexpr float maxSimEta =   0.9;

    constexpr float hitposerrXY = 0.01;  // resolution is 100um in xy --> more realistic scenario is 0.003
    constexpr float hitposerrZ = 0.1;    // resolution is 1mm in z
    constexpr float hitposerrR = Config::hitposerrXY / 10.0f;  // XXMT4K ??? I don't get this ...
    constexpr float varXY = Config::hitposerrXY * Config::hitposerrXY;
    constexpr float varZ = Config::hitposerrZ * Config::hitposerrZ;
    constexpr float varR = Config::hitposerrR * Config::hitposerrR;

    // scattering simulation
    constexpr float X0 =
        9.370;  // cm, from http://pdg.lbl.gov/2014/AtomicNuclearProperties/HTML/silicon_Si.html // Pb = 0.5612 cm
    constexpr float xr =
        0.1;  //  -assumes radial impact. This is bigger than what we have in main --> shouldn't it be the parameter below??? if radial impact??
    //const     float xr = std::sqrt(Config::beamspotX*Config::beamspotX + Config::beamspotY*Config::beamspotY);

    // Config for seeding
    /*MM: nlayers_per_seed is now set in IterationParams*/
    //extern    int   nlayers_per_seed;         // default: 3, cms sets from geom plugin
    constexpr int nlayers_per_seed_max = 4;  // Needed for allocation of arrays on stack.
    constexpr float chi2seedcut = 9.0;
    constexpr float lay01angdiff =
        0.0634888;  // analytically derived... depends on geometry of detector --> from mathematica ... d0 set to one sigma of getHypot(bsX,bsY)
    constexpr float lay02angdiff = 0.11537;
    constexpr float dEtaSeedTrip =
        0.06;  // for almost max efficiency --> empirically derived... depends on geometry of detector
    constexpr float dPhiSeedTrip =
        0.0458712;  // numerically+semianalytically derived... depends on geometry of detector
    // Recalculated in seedTest as it depends on nlayers_per_seed
    // static const float seed_z2cut= (nlayers_per_seed * fRadialSpacing) / std::tan(2.0f*std::atan(std::exp(-1.0f*dEtaSeedTrip)));
    constexpr float seed_z0cut = beamspotZ * 3.0f;   // 3cm
    constexpr float seed_z1cut = hitposerrZ * 3.6f;  // 3.6 mm --> to match efficiency from chi2cut
    constexpr float seed_d0cut = 0.5f;               // 5mm
    extern bool cf_seeding;

    // Config for propagation - could/should enter into PropagationFlags?!
    constexpr int Niter = 5;
    constexpr int NiterSim = 10;  // Can make more steps due to near volume misses.
    constexpr bool useTrigApprox = true;

    // PropagationFlags as used during finding and fitting. Defined for each Geom in its plugin.
    extern bool finding_requires_propagation_to_hit_pos;
    extern PropagationFlags finding_inter_layer_pflags;
    extern PropagationFlags finding_intra_layer_pflags;
    extern PropagationFlags backward_fit_pflags;
    extern PropagationFlags forward_fit_pflags;
    extern PropagationFlags seed_fit_pflags;
    extern PropagationFlags pca_prop_pflags;

    // Config for Bfield. Note: for now the same for CMS-2017 and CylCowWLids.
    constexpr float Bfield = 3.8112;
    constexpr float mag_c1 = 3.8114;
    constexpr float mag_b0 = -3.94991e-06;
    constexpr float mag_b1 = 7.53701e-06;
    constexpr float mag_a = 2.43878e-11;

    // Config for SelectHitIndices
    // Use extra arrays to store phi and q of hits.
    // MT: This would in principle allow fast selection of good hits, if
    // we had good error estimates and reasonable *minimal* phi and q windows.
    // Speed-wise, those arrays (filling AND access, about half each) cost 1.5%
    // and could help us reduce the number of hits we need to process with bigger
    // potential gains.
#ifdef CONFIG_PhiQArrays
    extern bool usePhiQArrays;
#else
    constexpr bool usePhiQArrays = true;
#endif

    // Config for seeding as well... needed bfield
    constexpr float maxCurvR = (100 * minSimPt) / (sol * Bfield);  // in cm

    // Config for Hit and BinInfoUtils
    constexpr int nPhiPart = 1260;
    constexpr float fPhiFactor = nPhiPart / TwoPI;
    constexpr int nEtaPart = 11;
    constexpr int nEtaBin = 2 * nEtaPart - 1;

    constexpr float fEtaFull = 2 * Config::fEtaDet;
    constexpr float lEtaPart = Config::fEtaFull / float(Config::nEtaPart);
    constexpr float lEtaBin = Config::lEtaPart / 2.;
    constexpr float fEtaOffB1 = Config::fEtaDet;
    constexpr float fEtaFacB1 = Config::nEtaPart / Config::fEtaFull;
    constexpr float fEtaOffB2 = Config::fEtaDet - Config::fEtaFull / (2 * Config::nEtaPart);
    constexpr float fEtaFacB2 =
        (Config::nEtaPart > 1 ? (Config::nEtaPart - 1) / (Config::fEtaFull - Config::fEtaFull / Config::nEtaPart) : 1);

    // This is for extra bins narrower ... thinking about this some more it
    // seems it would be even better to have two more exta bins, hanging off at
    // both ends.
    //
    // Anyway, it doesn't matter ... as with wide vertex region this eta binning
    // won't make much sense -- will have to be done differently for different
    // track orgin hypotheses. In about a year or so.

    // Config for Conformal fitter --> these change depending on inward/outward, which tracks used (MC vs reco), geometry, layers used, track params generated...
    // parameters for layers 0,4,9
    constexpr float blowupfit = 10.0;
    constexpr float ptinverr049 =
        0.0078;  // 0.0075; // errors used for MC only fit, straight from sim tracks, outward with simple geometry
    constexpr float phierr049 = 0.0017;    // 0.0017;
    constexpr float thetaerr049 = 0.0033;  // 0.0031;
    // parameters for layers 0,1,2 // --> ENDTOEND with "real seeding", fit is outward by definition, with poly geo
    constexpr float ptinverr012 = 0.12007;  // 0.1789;  -->old values from only MC seeds
    constexpr float phierr012 = 1.0;        // found empirically 0.00646; // 0.0071
    constexpr float thetaerr012 = 0.2;      // also found empirically 0.01366; // 0.0130;

    // config on fitting
    extern bool cf_fitting;

    extern bool mtvLikeValidation;
    extern bool mtvRequireSeeds;
    // Selection of simtracks from CMSSW. Used in Event::clean_cms_simtracks() and MkBuilder::prep_cmsswtracks()
    extern int cmsSelMinLayers;

    // config on validation
    extern int nMinFoundHits;
    constexpr float minCMSSWMatchChi2[6] = {100, 100, 50, 50, 30, 20};
    constexpr float minCMSSWMatchdPhi[6] = {0.2, 0.2, 0.1, 0.05, 0.01, 0.005};
    extern bool quality_val;
    extern bool sim_val_for_cmssw;
    extern bool sim_val;
    extern bool cmssw_val;
    extern bool fit_val;
    extern bool readSimTrackStates;  // need this to fill pulls
    extern bool inclusiveShorts;
    extern bool keepHitInfo;
    extern bool tryToSaveSimInfo;
    extern matchOpts cmsswMatchingFW;
    extern matchOpts cmsswMatchingBK;

    // config on duplicate removal
    extern bool useHitsForDuplicates;
    extern bool removeDuplicates;
    extern const float maxdPhi;
    extern const float maxdPt;
    extern const float maxdEta;
    extern const float minFracHitsShared;
    extern const float maxdR;

    // duplicate removal: tighter version
    extern const float maxd1pt;
    extern const float maxdphi;
    extern const float maxdcth;
    extern const float maxcth_ob;
    extern const float maxcth_fw;

    // config on dead modules
    extern bool useDeadModules;

    // config on seed cleaning
    constexpr float track1GeVradius = 87.6;  // = 1/(c*B)
    constexpr float c_etamax_brl = 0.9;
    constexpr float c_dpt_common = 0.25;
    constexpr float c_dzmax_brl = 0.005;
    constexpr float c_drmax_brl = 0.010;
    constexpr float c_ptmin_hpt = 2.0;
    constexpr float c_dzmax_hpt = 0.010;
    constexpr float c_drmax_hpt = 0.010;
    constexpr float c_dzmax_els = 0.015;
    constexpr float c_drmax_els = 0.015;

    // sorting config (bonus,penalty)
    constexpr float validHitBonus_ = 4;
    constexpr float validHitSlope_ = 0.2;
    constexpr float overlapHitBonus_ = 0;  // set to negative for penalty
    constexpr float missingHitPenalty_ = 8;
    constexpr float tailMissingHitPenalty_ = 3;

    // Threading
    extern int numThreadsFinder;
    extern int numThreadsSimulation;
    extern int numThreadsEvents;

    extern int finderReportBestOutOfN;

    extern int numSeedsPerTask;

    // number of layer1 hits for finding seeds per task
    extern int numHitsPerTask;

    // seed options
    extern seedOpts seedInput;
    extern cleanOpts seedCleaning;

    extern bool useCMSGeom;
    extern bool readCmsswTracks;

    extern bool dumpForPlots;
    extern bool silent;

    extern bool kludgeCmsHitErrors;
    extern bool backwardFit;
    extern bool backwardSearch;
    extern bool includePCA;

    // NAN and silly track parameter tracking options
    constexpr bool nan_etc_sigs_enable = false;

    constexpr bool nan_n_silly_check_seeds = true;
    constexpr bool nan_n_silly_print_bad_seeds = false;
    constexpr bool nan_n_silly_fixup_bad_seeds = false;
    constexpr bool nan_n_silly_remove_bad_seeds = true;

    constexpr bool nan_n_silly_check_cands_every_layer = false;
    constexpr bool nan_n_silly_print_bad_cands_every_layer = false;
    constexpr bool nan_n_silly_fixup_bad_cands_every_layer = false;

    constexpr bool nan_n_silly_check_cands_pre_bkfit = true;
    constexpr bool nan_n_silly_check_cands_post_bkfit = true;
    constexpr bool nan_n_silly_print_bad_cands_bkfit = false;

    // ================================================================

    extern bool json_dump_before;
    extern bool json_dump_after;
    extern bool json_verbose;
    extern std::vector<std::string> json_patch_filenames;
    extern std::vector<std::string> json_load_filenames;
    extern std::string json_save_iters_fname_fmt;
    extern bool json_save_iters_include_iter_info_preamble;

    // ================================================================

    void RecalculateDependentConstants();

    inline float BfieldFromZR(const float z, const float r) {
      return (Config::mag_b0 * z * z + Config::mag_b1 * z + Config::mag_c1) * (Config::mag_a * r * r + 1.f);
    }

#ifndef MPT_SIZE
#if defined(__AVX512F__)
#define MPT_SIZE 16
#elif defined(__AVX__) || defined(__AVX2__)
#define MPT_SIZE 8
#elif defined(__SSE3__)
#define MPT_SIZE 4
#else
#define MPT_SIZE 8
#endif
#endif

#ifndef THREAD_BINDING
#define THREAD_BINDING spread
#endif

  };  // namespace Config

  inline float cdist(float a) { return a > Config::PI ? Config::TwoPI - a : a; }

}  // end namespace mkfit
#endif
