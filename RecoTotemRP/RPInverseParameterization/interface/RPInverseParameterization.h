#ifndef RecoTotemRP_RPInelasticReconstruction_RPInverseParameterization_h
#define RecoTotemRP_RPInelasticReconstruction_RPInverseParameterization_h

#include <Minuit2/FunctionMinimum.h>
#include <Minuit2/MnPrint.h>
#include <Minuit2/VariableMetricMinimizer.h>
#include <Minuit2/FCNBase.h>
#include <Minuit2/MnStrategy.h>
#include <Minuit2/VariableMetricMinimizer.h>
#include <Minuit2/MnUserParameters.h>
#include <Minuit2/CombinedMinimizer.h>

#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "TotemProtonTransport/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHitDebug.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TMatrixD.h"
#include "TVector.h"
#include "TVector3.h"
#include "TRandom2.h"
#include <map>
#include <vector>
#include <iostream>
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "HepMC/GenEvent.h"
#include "RecoTotemRP/RPInverseParameterization/interface/InitState.h"
#include "RecoTotemRP/RPInverseParameterization/interface/BinomialMinimumSearcher.h"
#include "RecoTotemRP/RPInverseParameterization/interface/ReconstructionVarianceService.h"

//#include "RecoTotemRP/TMVA/interface/IFitterTarget.h"
//#include "RecoTotemRP/TMVA/interface/MethodBase.h"
#include <memory>



class RPInverseParameterization : public ROOT::Minuit2::FCNBase/*, public TMVA::IFitterTarget*/
{
  public:
    typedef std::map<RPId, LHCOpticsApproximator> transport_to_rp_type;
    typedef std::map<RPId, RP2DHit> hits_at_rp_type;
    
    RPInverseParameterization(double beam_direction, const edm::ParameterSet& conf,
        const BeamOpticsParams & BOPar);
    virtual ~RPInverseParameterization() {}
    double operator()(const std::vector<double>& par) const;
    double GetRPChi2Contribution(const std::vector<double>& par) const;
    double SimplifiedChiSqCalculation(const std::vector<double>& par) const;
    double FullVarianceCalculation(const std::vector<double>& par) const;
//    Double_t EstimatorFunction( std::vector<Double_t>& parameters);
    virtual double Up() const {return 1.0;}

    void AddRomanPot(RPId rp_id, const LHCOpticsApproximator &approx);
    void SetParameterizations(const transport_to_rp_type& param_map) {transport_to_rp_ = param_map;}
    void RemoveRomanPots() {transport_to_rp_.clear(); ClearEvent();}
    void ClearEvent() {hits_at_rp_.clear(); primary_vertex_set_=false; xi_rec_constrained_=false;}
    void AddProtonAtRP(RPId rp_id, const RP2DHit &hit) {hits_at_rp_[rp_id]=hit;}
    void AddProtonAtRPCollection(const hits_at_rp_type &hits_at_rp);
    void SetPrimaryVertex(const TVector3 &vert, const TVector3 &error);
//    void SetPrimaryProton(const HepMC::FourVector &ip_proton) {ip_proton_=ip_proton;}
    void InitializeVarianceMatrixService();
    void InitializeMatrixBasedFitting(RPReconstructedProton &rec_proton);
    void ConstrainXiReconstruction(double xi_value, double xi_sigma) {xi_rec_constrained_ = true; xi_value_ = xi_value; xi_sigma_ = xi_sigma;};
    void ReleaseXiReconstruction() {xi_rec_constrained_ = false;};
    bool Fit(RPReconstructedProton &rec_proton, bool init_for_2_sided_fitting=false);  //rec_proton serves as input and output
        //input: sets some variables and specifies which ones to fit   
        //output: returns fitted values and corresponding error matrix
    void Verbosity(int ver) {verbosity_ = ver;}
    int Verbosity() {return verbosity_;}
    void PrintFittedHitsInfo(std::ostream &o);
    
  private:
    void InitializeFit(RPReconstructedProton &rec_proton);
    double FitConstrainedXi(RPReconstructedProton &rec_proton, double xi);
    bool FitNonConstrained(RPReconstructedProton &rec_proton);
    void SetInitialParameters(RPReconstructedProton &rec_proton);
    double GetInitialParameters(const ROOT::Minuit2::FunctionMinimum &min, 
        RPReconstructedProton &rec_proton);
    bool SetResults(const ROOT::Minuit2::FunctionMinimum &min, 
          RPReconstructedProton &rec_proton);
    void PrintProtonsAtRP();
    void SetPulls(const std::vector< ROOT::Minuit2::MinuitParameter > & raw_pars, 
          RPReconstructedProton &rec_proton);
    double ElasticReconstrChi2Contrib(const std::vector<double>& par) const;
    double ChiSqPrimaryVertexContrib(const std::vector<double>& par) const;
//    void FindInitialParameterValues(RPReconstructedProton &rec_proton);
//    void FindInitialParameterValues(RPReconstructedProton &rec_proton, 
//            unsigned int param_id);
//    void FindInitialParameterValuesPrecisely(RPReconstructedProton &rec_proton);
//    void FindinitialParamsWithGeneticAlgorithm(RPReconstructedProton &rec_proton);
    
    inline void FitXCoordsOnly() {fit_x_out_coords_=true; fit_y_out_coords_=false;}
    inline void FitYCoordsOnly() {fit_x_out_coords_=false; fit_y_out_coords_=true;}
    inline void FitXYCoords() {fit_x_out_coords_=true; fit_y_out_coords_=true;}
    
    transport_to_rp_type transport_to_rp_;
    hits_at_rp_type hits_at_rp_;
    int verbosity_;
//    const double beam_energy_;  //GeV
//    const double mp0_;
//    const double beam_momentum_;  //GeV
    const double beam_direction_;
    
    ROOT::Minuit2::MnUserParameters nm_params_;
    int degrees_of_freedom_;
    ROOT::Minuit2::VariableMetricMinimizer theMinimizer_;
    ROOT::Minuit2::MnStrategy strategy_;
    bool primary_vertex_set_;
    TVector3 primary_vertex_;
    TVector3 primary_vertex_error_;
    std::vector<double> rec_precision_;
    std::vector<double> min_random_init_;
    std::vector<double> max_random_init_;
    std::vector<double> init_values_;
    std::vector<double> default_init_values_;
    
    InitValues smart_init_values_;  //init values used in the partial minimization procedures
    
    bool fit_x_out_coords_;
    bool fit_y_out_coords_;
    
    int inverse_param_random_seed_;
    int random_iterations_;
    TRandom2 random_engine_;
    BeamOpticsParams BOPar_;
//    HepMC::FourVector ip_proton_;
    std::auto_ptr<BinomialMinimumSearcher> binom_min_search_;
    double converged_chisqndf_max_;
    double init_converged_chisq_;
    double xi_edge_;
    double xi_steepness_factor_;
    
    double rp_expected_resolution_;
    double multiple_scat_sigma_;
    bool compute_full_variance_matrix_;
    
    TMatrixD inv_var_x_, inv_var_y_;  //in [m, rad]
    bool variance_marices_initialised_;
    
    bool xi_rec_constrained_;
    double xi_value_;
    double xi_sigma_;
    
    std::auto_ptr<ReconstructionVarianceService> rec_variance_service_;
    bool elastic_reconstruction_;
//
    bool xyCorrelation;
};

#endif
