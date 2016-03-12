#include "RecoTotemRP/RPInverseParameterization/interface/ReconstructionVarianceService.h"
#include <iostream>
#include <string>
#include "TFile.h"
#include "TMath.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

ReconstructionVarianceService::ReconstructionVarianceService(const edm::ParameterSet& conf) {
	service_initialized_ = false;
	InitTransportApproximatios(conf);
}

//[m], [rad]
void ReconstructionVarianceService::AddScatteringPlane(double z, double scat_angle) {
	scattering_planes_[TMath::Abs(z)] = scat_angle * scat_angle;
	if (verbosity_) {
		std::cout << "Scattering plane added z=" << z << " sigma_theta=" << scat_angle << std::endl;
	}
}

//[m], [m]
void ReconstructionVarianceService::AddDetectorPlane(double z, double resolution) {
	detector_planes_[TMath::Abs(z)] = resolution * resolution;
	if (verbosity_) {
		std::cout << "Detector plane added z=" << z << " sigma_resol=" << resolution << std::endl;
	}
}

void ReconstructionVarianceService::ResetCovarianceMatrix() {
	service_initialized_ = false;
	scattering_planes_.clear();
	detector_planes_.clear();
}

//[m], [rad], [m], [rad], -1.0..0
void ReconstructionVarianceService::SetEstimatedProtonState(double x, double madxthx, double y,
        double madxthy, double xi, double direction) {
	estimated_madx_proton_state_[0] = x;
	estimated_madx_proton_state_[1] = madxthx;
	estimated_madx_proton_state_[2] = y;
	estimated_madx_proton_state_[3] = madxthy;
	estimated_madx_proton_state_[4] = xi;

	direction_ = direction;
	if (direction_ == 0.0) {
		std::cout << "direction=" << direction << std::endl;
		throw cms::Exception("ReconstructionVarianceService::SetEstimatedProtonState")
		        << " direction_ is invalid";
	}

	service_initialized_ = true;

	if (verbosity_) {
		std::cout << "Proton state set" << std::endl;
	}
}

void ReconstructionVarianceService::InitTransportApproximatios(const edm::ParameterSet& conf) {
	verbosity_ = conf.getParameter<int> ("Verbosity");
	if (verbosity_) {
		std::cout << "ReconstructionVarianceService::InitTransportApproximatios" << std::endl;
	}
}

bool ReconstructionVarianceService::CalculateEffectiveLengthVectors(
        scattering_map_type::const_iterator it_scat, TMatrixD &EffLenX, TMatrixD &EffLenY) {
	EffLenX.ResizeTo(detector_planes_.size(), 1);
	EffLenY.ResizeTo(detector_planes_.size(), 1);

	readout_det_set_type::const_iterator redout_it;
	int i = 0;
	bool vector_eq_zero = true;
	for (redout_it = detector_planes_.begin(); redout_it != detector_planes_.end(); ++redout_it, ++i) {
        double scat_z = it_scat->first;
        double det_z = redout_it->first;
        if (scat_z < det_z) {
            double length = det_z - scat_z;
            EffLenX(i, 0) = length;
            EffLenY(i, 0) = length;
            vector_eq_zero = false;
        }
	}
	if (verbosity_) {
		std::cout << "CalculateEffectiveLengthVectors x:" << std::endl;
		PrintMatrix(std::cout, EffLenX);
		std::cout << "CalculateEffectiveLengthVectors y:" << std::endl;
		PrintMatrix(std::cout, EffLenY);
	}
	return !vector_eq_zero;
}

void ReconstructionVarianceService::InitiateResolutionMatrices(TMatrixD &var_x, TMatrixD &var_y) {
	var_x.ResizeTo(detector_planes_.size(), detector_planes_.size());
	var_y.ResizeTo(detector_planes_.size(), detector_planes_.size());
	var_x *= 0.0;
	var_y *= 0.0;

	readout_det_set_type::const_iterator redout_it;
	int i = 0;
	for (redout_it = detector_planes_.begin(); redout_it != detector_planes_.end(); ++redout_it, ++i) {
		var_x(i, i) = redout_it->second;
		var_y(i, i) = redout_it->second;
	}
	if (verbosity_) {
		std::cout << "InitiateResolutionMatrices x:" << std::endl;
		PrintMatrix(std::cout, var_x);
		std::cout << "InitiateResolutionMatrices y:" << std::endl;
		PrintMatrix(std::cout, var_y);
	}
}

void ReconstructionVarianceService::AddScatteringContribution(scattering_map_type::const_iterator it_scat,
        TMatrixD &EffLenX, TMatrixD &EffLenY, TMatrixD &var_x, TMatrixD &var_y) {
	double scat_var = it_scat->second;
	TMatrixD EffLenXScaled(EffLenX);
	EffLenXScaled *= scat_var;

	TMatrixD EffLenYScaled(EffLenY);
	EffLenYScaled *= scat_var;

	TMatrixD VarianceContribX(EffLenX, TMatrixD::kMultTranspose, EffLenXScaled);
	TMatrixD VarianceContribY(EffLenY, TMatrixD::kMultTranspose, EffLenYScaled);

	if (verbosity_) {
		std::cout << "AddScatteringContribution" << std::endl;
		PrintMatrix(std::cout, VarianceContribX);
		std::cout << std::endl;
		PrintMatrix(std::cout, VarianceContribY);
		std::cout << std::endl;
	}

	var_x += VarianceContribX;
	var_y += VarianceContribY;
}

void ReconstructionVarianceService::ComputeVarianceMatrices(TMatrixD &var_x, TMatrixD &var_y) {
	InitiateResolutionMatrices(var_x, var_y);
	scattering_map_type::const_iterator scat_it;
	for (scat_it = scattering_planes_.begin(); scat_it != scattering_planes_.end(); ++scat_it) {
		TMatrixD EffLenX, EffLenY;
		if (CalculateEffectiveLengthVectors(scat_it, EffLenX, EffLenY))
			AddScatteringContribution(scat_it, EffLenX, EffLenY, var_x, var_y);
	}
	if (verbosity_) {
		std::cout << "ComputeVarianceMatrices" << std::endl;
		PrintMatrix(std::cout, var_x);
		std::cout << std::endl;
		PrintMatrix(std::cout, var_y);
		std::cout << std::endl;
	}
}

void ReconstructionVarianceService::PrintMatrix(std::ostream &o, const TMatrixD &m) {
	for (int i = m.GetRowLwb(); i <= m.GetRowUpb(); ++i) {
		o << "|";
		for (int j = m.GetColLwb(); j <= m.GetColUpb(); ++j) {
			o << m(i, j) << "\t";
		}
		o << "|" << std::endl;
	}
}

/**
 * This function computes inverted variance matrices. These matrices are then used in RPInverseParametrization
 * during minimization of chi2 function.
 */
void ReconstructionVarianceService::ComputeInvertedVarianceMatrices(TMatrixD &var_x, TMatrixD &var_y) {
	ComputeVarianceMatrices(var_x, var_y);
	if (verbosity_ > 1) {
		std::cout << "-------------variance--------------\n";
		PrintMatrix(std::cout, var_x);
		PrintMatrix(std::cout, var_y);
	}
	var_x.Invert();
	var_y.Invert();
	if (verbosity_) {
		std::cout << "ComputeInvertedVarianceMatrices" << std::endl;
		PrintMatrix(std::cout, var_x);
		std::cout << std::endl;
		PrintMatrix(std::cout, var_y);
		std::cout << std::endl;
	}
}

/**
 * This function is very similar to ComputeInvertedVarianceMatrices(TMatrixD &var_x, TMatrixD &var_y) function.
 * The difference is that in this function we create only one covariance matrix (joined for x and y).
 */
void ReconstructionVarianceService::ComputeInvertedVarianceMatrix(TMatrixD &var) {
	var.ResizeTo(2 * detector_planes_.size(), 2 * detector_planes_.size());
	var *= 0;
	TMatrixD var_x, var_y;
	ComputeVarianceMatrices(var_x, var_y);
	for (uint i = 0; i < detector_planes_.size(); i++) {
		var(i, i) = var_x(i, i);
		var(i + detector_planes_.size(), i + detector_planes_.size()) = var_y(i, i);
	}
	if (verbosity_ > 1) {
		std::cout << "variance\n";
		PrintMatrix(std::cout, var);
	}
	var.Invert();
	if (verbosity_) {
		std::cout << "odwrocona\n";
		PrintMatrix(std::cout, var);
	}
}

