#ifndef RecoTotemRPRPInverseParameterizationReconstructionVarianceService_h
#define RecoTotemRPRPInverseParameterizationReconstructionVarianceService_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <iostream>
#include <map>
#include "TMath.h"
#include "TMatrixD.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"

//the class calculates the covariance matrix based on the multiple scattering in the 
//detectors preceeding the ones used for the readout

//everything calculated in [m] and [rad]
class ReconstructionVarianceService {
  public:
    typedef std::map<double, double> scattering_map_type; //indexed by z, contains scat_angle variance
    typedef std::map<double, double> readout_det_set_type;  //indexed by z position of the readout planes together with its resolution variance
    ReconstructionVarianceService(const edm::ParameterSet& conf);
    void AddScatteringPlane(double z, double scat_angle);
    void AddDetectorPlane(double z, double resolution);
    void ResetCovarianceMatrix();
	void SetEstimatedProtonState(double x, double madxthx, double y, double madxthy, double xi,
	        double direction);
    void ComputeVarianceMatrices(TMatrixD &var_x, TMatrixD &var_y);
    void ComputeInvertedVarianceMatrices(TMatrixD &var_x, TMatrixD &var_y);
	void ComputeInvertedVarianceMatrix(TMatrixD &var);
	void PrintMatrix(std::ostream &o, const TMatrixD &m);
    
  private:
    void InitTransportApproximatios(const edm::ParameterSet& conf);
    void ComputeStraightSectionProjMatrix(TMatrixD &tr_mat, double length);
	bool CalculateEffectiveLengthVectors(scattering_map_type::const_iterator it_scat, TMatrixD &EffLenX,
	        TMatrixD &EffLenY);
    void InitiateResolutionMatrices(TMatrixD &var_x, TMatrixD &var_y);
	void AddScatteringContribution(scattering_map_type::const_iterator it_scat, TMatrixD &EffLenX,
	        TMatrixD &EffLenY, TMatrixD &var_x, TMatrixD &var_y);
    
    
    scattering_map_type scattering_planes_;
    readout_det_set_type detector_planes_;
    bool service_initialized_;
    double estimated_madx_proton_state_[5];
    double direction_; //>0 lhcb1 (right), <0 lhcb2 (left)

    int verbosity_;
};



#endif
