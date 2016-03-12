#ifndef RecoTotemRP_RPInelasticReconstruction_RPInverse2SidedParameterization_h
#define RecoTotemRP_RPInelasticReconstruction_RPInverse2SidedParameterization_h

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
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPair.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPairCollection.h"
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
#include "RecoTotemRP/RPInverseParameterization/interface/InitState.h"
#include "RecoTotemRP/RPInverseParameterization/interface/BinomialMinimumSearcher.h"
#include "RecoTotemRP/RPInverseParameterization/interface/ReconstructionVarianceService.h"
#include <memory>
#include "RecoTotemRP/RPInverseParameterization/interface/RPInverseParameterization.h"


class RPInverse2SidedParameterization : public ROOT::Minuit2::FCNBase
{
  public:
    typedef std::map<RPId, LHCOpticsApproximator> transport_to_rp_type;
    typedef std::map<RPId, RP2DHit> hits_at_rp_type;
    
    RPInverse2SidedParameterization(const edm::ParameterSet& conf,
        const BeamOpticsParams & BOPar);
    virtual ~RPInverse2SidedParameterization() {}
    double operator()(const std::vector<double>& par) const;
    double SimplifiedChiSqCalculation(const std::vector<double>& par) const;
    double FullVarianceCalculation(const std::vector<double>& par) const;
    virtual double Up() const {return 1.0;}

    void AddRomanPot(RPId rp_id, const LHCOpticsApproximator &approx);
    void AddParameterizationsRight(const transport_to_rp_type& param_map);
    void AddParameterizationsLeft(const transport_to_rp_type& param_map);
    void RemoveRomanPots();
    void ClearEvent();
    void AddProtonAtRP(RPId rp_id, const RP2DHit &hit);
    void AddProtonAtRPCollection(const hits_at_rp_type &hits_at_rp);
    void SetPrimaryVertex(const TVector3 &vert, const TVector3 &error);
    
    bool Fit(RPReconstructedProtonPair &rec_proton_pair);  //rec_proton_pair serves as input and output
        //input: sets some variables and specifies which ones to fit   
        //output: returns fitted values and corresponding error matrix
    void Verbosity(int ver);
    int Verbosity() {return verbosity_;}
    void PrintFittedHitsInfo(std::ostream &o);
    
  private:
    void FindInitialValues(RPReconstructedProtonPair &rec_proton_pair);
    void SetInitialParameters(RPReconstructedProtonPair &rec_proton_pair);
    bool SetResults(const ROOT::Minuit2::FunctionMinimum &min, 
          RPReconstructedProtonPair &rec_proton_pair);
    void PrintProtonsAtRP();
    void SetPulls(const std::vector< ROOT::Minuit2::MinuitParameter > & raw_pars, 
          RPReconstructedProtonPair &rec_proton_pair);
    double ElasticReconstrChi2Contrib(const std::vector<double> &par_left, const std::vector<double> &par_right) const;
    double ChiSqPrimaryVertexContrib(const std::vector<double>& par) const;
    void FindInitialParameterValues(RPReconstructedProtonPair &rec_proton_pair);
//    void FindInitialParameterValues(RPReconstructedProtonPair &rec_proton_pair, 
//            unsigned int param_id);
//    void FindInitialParameterValuesPrecisely(RPReconstructedProtonPair &rec_proton_pair);
    inline bool RightArm(RPId id) const {return id>=100;}
    inline bool LeftArm(RPId id) const {return id<100;}
    
    transport_to_rp_type transport_to_rp_;
    hits_at_rp_type hits_at_rp_;
    int verbosity_;
//    const double beam_energy_;  //GeV
//    const double mp0_;
//    const double beam_momentum_;  //GeV
    
    ROOT::Minuit2::MnUserParameters nm_params_;
    int degrees_of_freedom_;
    ROOT::Minuit2::VariableMetricMinimizer theMinimizer_;
    ROOT::Minuit2::MnStrategy strategy_;
    bool primary_vertex_set_;
    TVector3 primary_vertex_;
    TVector3 primary_vertex_error_;
    std::vector<double> rec_precision_;
//    std::vector<double> init_values_;
//    std::vector<double> default_init_values_;
    int inverse_param_random_seed_;
    TRandom2 random_engine_;
    std::auto_ptr<RPInverseParameterization> inverse_param_right_;
    std::auto_ptr<RPInverseParameterization> inverse_param_left_;

    double converged_chisqndf_max_;
    
    BeamOpticsParams BOPar_;
    bool elastic_reconstruction_;
};

#endif
