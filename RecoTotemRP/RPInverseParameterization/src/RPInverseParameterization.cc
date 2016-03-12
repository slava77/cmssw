#include "RecoTotemRP/RPInverseParameterization/interface/RPInverseParameterization.h"
#include "TotemProtonTransport/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHitDebug.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <cassert>
#include <iostream>
#include <vector>
#include "TMath.h"
#include "TRandom2.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
//#include "RecoTotemRP/TMVA/interface/GeneticFitter.h"
//#include "RecoTotemRP/TMVA/interface/Interval.h"
//#include "RecoTotemRP/TMVA/interface/Timer.h"
//#include "RecoTotemRP/TMVA/interface/Types.h"
//#include "RecoTotemRP/TMVA/interface/Tools.h"
//#include "RecoTotemRP/TMVA/interface/Config.h"


//#include <Minuit2/VariableMetricMinimizer.h>
//#include <Minuit2/FunctionMinimum.h>
//#include "Minuit2/MnStrategy.h"
//#include "Minuit2/VariableMetricMinimizer.h"
//#include "Minuit2/MnUserParameters.h"
//#include "Minuit2/MnUserCovariance.h"

//using namespace ROOT::Minuit2;

//beam_direction: +1.0 beam to the right, -1.0 - beam to the left
RPInverseParameterization::RPInverseParameterization(double beam_direction, const edm::ParameterSet& conf,
        const BeamOpticsParams & BOPar) :
	beam_direction_(beam_direction / TMath::Abs(beam_direction)), strategy_(2), smart_init_values_(5) {
	BOPar_ = BOPar;
	verbosity_ = 0;
	primary_vertex_set_ = false;
	fit_x_out_coords_ = true;
	fit_y_out_coords_ = true;

	rec_precision_.resize(5);
	rec_precision_[0] = conf.getParameter<double> ("ReconstructionPrecisionX");
	rec_precision_[1] = conf.getParameter<double> ("ReconstructionPrecisionThetaX");
	rec_precision_[2] = conf.getParameter<double> ("ReconstructionPrecisionY");
	rec_precision_[3] = conf.getParameter<double> ("ReconstructionPrecisionThetaY");
	rec_precision_[4] = conf.getParameter<double> ("ReconstructionPrecisionKsi");

	min_random_init_.resize(5);
	max_random_init_.resize(5);
	init_values_.resize(5);
	min_random_init_[0] = conf.getParameter<double> ("InitMinX") + BOPar.GetBeamDisplacementX() * 1000.0;
	min_random_init_[1] = conf.getParameter<double> ("InitMinThetaX") + BOPar.GetCrossingAngleX();
	min_random_init_[2] = conf.getParameter<double> ("InitMinY") + BOPar.GetBeamDisplacementY() * 1000.0;
	min_random_init_[3] = conf.getParameter<double> ("InitMinThetaY") + BOPar.GetCrossingAngleY();
	min_random_init_[4] = conf.getParameter<double> ("InitMinKsi");

	max_random_init_[0] = conf.getParameter<double> ("InitMaxX") + BOPar.GetBeamDisplacementX() * 1000.0;
	max_random_init_[1] = conf.getParameter<double> ("InitMaxThetaX") + BOPar.GetCrossingAngleX();
	max_random_init_[2] = conf.getParameter<double> ("InitMaxY") + BOPar.GetBeamDisplacementY() * 1000.0;
	max_random_init_[3] = conf.getParameter<double> ("InitMaxThetaY") + BOPar.GetCrossingAngleY();
	max_random_init_[4] = conf.getParameter<double> ("InitMaxKsi");

	random_iterations_ = conf.getParameter<int> ("InitIterationsNumber");
	inverse_param_random_seed_ = conf.getParameter<int> ("InverseParamRandSeed");
	random_engine_.SetSeed(inverse_param_random_seed_);

	elastic_reconstruction_ = conf.getParameter<bool> ("ElasticScatteringReconstruction");

	default_init_values_.push_back(BOPar_.GetBeamDisplacementX());
	default_init_values_.push_back(BOPar_.GetCrossingAngleX());
	default_init_values_.push_back(BOPar_.GetBeamDisplacementY());
	default_init_values_.push_back(BOPar_.GetCrossingAngleY());
	default_init_values_.push_back((max_random_init_[4] + min_random_init_[4]) / 2.0);

	smart_init_values_.SetValues(default_init_values_);

	nm_params_.Add("x", 0., rec_precision_[0]);
	nm_params_.Add("theta_x", 0., rec_precision_[1]);
	nm_params_.Add("y", 0., rec_precision_[2]);
	nm_params_.Add("theta_y", 0., rec_precision_[3]);
	nm_params_.Add("ksi", -0.2, rec_precision_[4]);

	binom_min_search_ = std::auto_ptr<BinomialMinimumSearcher>(
	        new BinomialMinimumSearcher(min_random_init_[4], max_random_init_[4],
	                conf.getParameter<double> ("RandomSearchProbability"), random_engine_));

	converged_chisqndf_max_ = conf.getParameter<double> ("MaxChiSqNDFOfConvergedProton");
	xi_edge_ = conf.getParameter<double> ("MaxAllowedReconstructedXi");
	xi_steepness_factor_ = conf.getParameter<double> ("OutOfXiRangePenaltyFactor");
	init_converged_chisq_ = conf.getParameter<double> ("MaxChiSqOfConvergedInitialisation");
	rp_expected_resolution_ = conf.getParameter<double> ("ExpectedRPResolution");
	multiple_scat_sigma_ = conf.getParameter<double> ("RPMultipleScatteringSigma");

	compute_full_variance_matrix_ = conf.getParameter<bool> ("ComputeFullVarianceMatrix");
	if (compute_full_variance_matrix_) {
		rec_variance_service_ = std::auto_ptr<ReconstructionVarianceService>(
		        new ReconstructionVarianceService(conf));
	}
	variance_marices_initialised_ = false;
	xyCorrelation = false;
	if (conf.exists("xyCorrelation")) {
		xyCorrelation = conf.getParameter<bool> ("xyCorrelation");
	}
}

//MADX canonical variables
//(x, theta_x, y, theta_y, ksi) [mm, rad, mm, rad, -1..0]
double RPInverseParameterization::SimplifiedChiSqCalculation(const std::vector<double>& par) const {
	if (verbosity_)
		std::cout << "RPInverseParameterization::SimplifiedChiSqCalculation" << std::endl;

	typedef std::map<double, std::pair<double, double> > rp_z_hit_map_type;
	rp_z_hit_map_type diff_map_xy;
	rp_z_hit_map_type hit_out_map_xy;

	double chi2 = 0.;

	//convert position from [mm] to [m]
	double par_m[5];
	for (unsigned int i = 0; i < par.size(); ++i)
		par_m[i] = par[i];
	par_m[0] = par_m[0] / 1000.;
	par_m[2] = par_m[2] / 1000.;

	hits_at_rp_type::const_iterator it = hits_at_rp_.begin();
	hits_at_rp_type::const_iterator end = hits_at_rp_.end();
	for (; it != end; ++it) {
		transport_to_rp_type::const_iterator tr_it = transport_to_rp_.find(it->first);
		if (tr_it == transport_to_rp_.end()) {
			std::cout << it->first << " RP proton transport parameterization missing, fatal error"
			        << std::endl;
			for (transport_to_rp_type::const_iterator it1 = transport_to_rp_.begin(); it1
			        != transport_to_rp_.end(); ++it1) {
				std::cout << it1->first << ", ";
			}
			std::cout << std::endl;
			assert(false);
		}
		double out[2];
		double penalty = tr_it->second.ParameterOutOfRangePenalty(par_m);
		tr_it->second.Transport2D(par_m, out, false);

		double x_rp_deb = it->second.X() * 0.001; //convert [mm] to [m]
		double y_rp_deb = it->second.Y() * 0.001; //convert [mm] to [m]
		double z_rp_deb = it->second.Z() * 0.001; //convert [mm] to [m]
		diff_map_xy[z_rp_deb] = std::pair<double, double>(x_rp_deb - out[0], y_rp_deb - out[1]);
		hit_out_map_xy[z_rp_deb] = std::pair<double, double>(out[0], out[1]);

		double x_rp = it->second.X();
		double y_rp = it->second.Y();
		double vx_rp = it->second.Vx();
		double vy_rp = it->second.Vy();

		//convert to from [m] to [mm]
		out[0] = out[0] * 1000.;
		out[1] = out[1] * 1000.;

		if (fit_x_out_coords_)
			chi2 += (x_rp - out[0]) * (x_rp - out[0]) / vx_rp;
		if (fit_y_out_coords_)
			chi2 += (y_rp - out[1]) * (y_rp - out[1]) / vy_rp;

		chi2 += penalty;
	}

	if (verbosity_) {
		std::cout << "out reco hit pos.: ";
		for (rp_z_hit_map_type::iterator it1 = hit_out_map_xy.begin(); it1 != hit_out_map_xy.end(); ++it1) {
			std::cout << "(" << it1->second.first << "," << it1->second.second << "," << it1->first << "), ";
		}
		std::cout << std::endl;
		std::cout << "out reco residuals: ";
		for (rp_z_hit_map_type::iterator it1 = diff_map_xy.begin(); it1 != diff_map_xy.end(); ++it1) {
			std::cout << "(" << it1->second.first << "," << it1->second.second << "," << it1->first << "), ";
		}
		std::cout << std::endl;
	}

	return chi2;
}

//MADX canonical variables
//(x, theta_x, y, theta_y, ksi) [mm, rad, mm, rad, -1..0]
double RPInverseParameterization::FullVarianceCalculation(const std::vector<double>& par) const {
	if (verbosity_)
		std::cout << "RPInverseParameterization::FullVarianceCalculation" << std::endl;
	double chi2 = 0.;

	//convert position from [mm] to [m]
	double par_m[5];
	for (unsigned int i = 0; i < par.size(); ++i)
		par_m[i] = par[i];
	par_m[0] = par_m[0] / 1000.;
	par_m[2] = par_m[2] / 1000.;

	TMatrixD diff_vec_x(hits_at_rp_.size(), 1);
	TMatrixD diff_vec_y(hits_at_rp_.size(), 1);
	typedef std::map<double, std::pair<double, double> > rp_z_hit_map_type;
	rp_z_hit_map_type diff_map_xy;
	rp_z_hit_map_type hit_out_map_xy;

	hits_at_rp_type::const_iterator it = hits_at_rp_.begin();
	hits_at_rp_type::const_iterator end = hits_at_rp_.end();
	for (; it != end; ++it) {
		transport_to_rp_type::const_iterator tr_it = transport_to_rp_.find(it->first);
		if (tr_it == transport_to_rp_.end()) {
			std::cout << it->first << " RP proton transport parameterization missing, fatal error"
			        << std::endl;
			for (transport_to_rp_type::const_iterator it1 = transport_to_rp_.begin(); it1
			        != transport_to_rp_.end(); ++it1) {
				std::cout << it1->first << ", ";
			}
			std::cout << std::endl;
			assert(false);
		}
		double out[2];
		double penalty = tr_it->second.ParameterOutOfRangePenalty(par_m);
		chi2 += penalty;
		tr_it->second.Transport2D(par_m, out, false);

		double x_rp = it->second.X() * 0.001; //convert [mm] to [m]
		double y_rp = it->second.Y() * 0.001; //convert [mm] to [m]
		double z_rp = it->second.Z() * 0.001; //convert [mm] to [m]

		diff_map_xy[z_rp] = std::pair<double, double>(x_rp - out[0], y_rp - out[1]);
		hit_out_map_xy[z_rp] = std::pair<double, double>(out[0], out[1]);
	}

	if (verbosity_) {
		std::cout << "out reco hit pos.: ";
		for (rp_z_hit_map_type::iterator it1 = hit_out_map_xy.begin(); it1 != hit_out_map_xy.end(); ++it1) {
			std::cout << "(" << it1->second.first << "," << it1->second.second << "," << it1->first << "), ";
		}
		std::cout << std::endl;
		std::cout << "out reco residuals: ";
		for (rp_z_hit_map_type::iterator it1 = diff_map_xy.begin(); it1 != diff_map_xy.end(); ++it1) {
			std::cout << "(" << it1->second.first << "," << it1->second.second << "," << it1->first << "), ";
		}
		std::cout << std::endl;
	}
	if (xyCorrelation) { // if we are simulating correlated x and y, we have got only one matrix,
						//common for x and y
		TMatrixD diffVec(2 * hits_at_rp_.size(), 1);
		int i = 0;
		for (rp_z_hit_map_type::const_iterator it = diff_map_xy.begin(); it != diff_map_xy.end(); ++it, ++i) {
			diffVec(i, 0) = it->second.first;
		}
		i = hits_at_rp_.size();
		for (rp_z_hit_map_type::const_iterator it = diff_map_xy.begin(); it != diff_map_xy.end(); ++it, ++i) {
			diffVec(i, 0) = it->second.second;
		}
		TMatrixD prod_r(inv_var_x_, TMatrixD::kMult, diffVec);
		TMatrixD chisq(diffVec, TMatrixD::kTransposeMult, prod_r);
		chi2 += chisq(0,0);

	} else {
		int i = 0;
		for (rp_z_hit_map_type::const_iterator it = diff_map_xy.begin(); it != diff_map_xy.end(); ++it, ++i) {
			diff_vec_x(i, 0) = it->second.first;
			diff_vec_y(i, 0) = it->second.second;
		}

		TMatrixD prod_r_x(inv_var_x_, TMatrixD::kMult, diff_vec_x);
		TMatrixD prod_r_y(inv_var_y_, TMatrixD::kMult, diff_vec_y);

		TMatrixD chisq_x(diff_vec_x, TMatrixD::kTransposeMult, prod_r_x);
		TMatrixD chisq_y(diff_vec_y, TMatrixD::kTransposeMult, prod_r_y);

		chi2 += chisq_x(0, 0);
		chi2 += chisq_y(0, 0);
	}

	return chi2;
}

//MADX canonical variables
//(x, theta_x, y, theta_y, ksi) [mm, rad, mm, rad, -1..0]
double RPInverseParameterization::GetRPChi2Contribution(const std::vector<double>& par) const {
	double chi2 = 0.0;
	if (compute_full_variance_matrix_ && variance_marices_initialised_)
		chi2 = FullVarianceCalculation(par);
	else
		chi2 = SimplifiedChiSqCalculation(par);

	//temporary
	double xi_penalty = 0.0;
	if (par[4] > xi_edge_)
		xi_penalty = TMath::Exp((par[4] - xi_edge_) * xi_steepness_factor_) - 1.0;

	chi2 += xi_penalty;

	return chi2;
}

//MADX canonical variables
//(x, theta_x, y, theta_y, ksi) [mm, rad, mm, rad, -1..0]
double RPInverseParameterization::operator()(const std::vector<double>& par) const {
	assert(par.size() == 5);
	double chi2 = 0.0;
	chi2 = GetRPChi2Contribution(par);

	if (elastic_reconstruction_)
		chi2 += ElasticReconstrChi2Contrib(par);

	if (primary_vertex_set_)
		chi2 += ChiSqPrimaryVertexContrib(par);

	if (verbosity_) {
		for (int i = 0; i < 5; i++) {
			std::cout << i << "=" << par[i] << ", ";
		}
		std::cout << "chi2=" << chi2 << std::endl;
	}

	return chi2;
}

double RPInverseParameterization::ElasticReconstrChi2Contrib(const std::vector<double>& par) const {
	double xi_beam_mean = BOPar_.GetMeanXi();
	double xi_beam_smearing = BOPar_.GetSigmaXi();
	double var_xi_beam_smearing = xi_beam_smearing * xi_beam_smearing;

	double xi_diff = par[4] - xi_beam_mean;
	double xi_diff_chi2_contrib = xi_diff * xi_diff / var_xi_beam_smearing;

	return xi_diff_chi2_contrib;
}

void RPInverseParameterization::PrintFittedHitsInfo(std::ostream &o) {
	hits_at_rp_type::const_iterator it = hits_at_rp_.begin();
	hits_at_rp_type::const_iterator end = hits_at_rp_.end();
	for (; it != end; ++it) {
		//    double x_rp = it->second.X();
		//    double y_rp = it->second.Y();
		double vx_rp = it->second.Vx();
		double vy_rp = it->second.Vy();

		std::cout << "par[" << it->first << "]sigma_x=" << TMath::Sqrt(vx_rp) << " par[" << it->first
		        << "]sigma_y=" << TMath::Sqrt(vy_rp) << std::endl;
	}
	//  if(primary_vertex_set_)
	//    std::cout<<"vertex_sigma_x="<<TMath::Sqrt(variance_vertex_x)
	//    <<" vertex_sigma_y="<<TMath::Sqrt(variance_vertex_y)<<std::endl;
}

//par: x, theta_x, y, theta_y, ksi
//[mm, rad, mm, rad, -1..0]
double RPInverseParameterization::ChiSqPrimaryVertexContrib(const std::vector<double>& par) const {
	if (!primary_vertex_set_)
		return 0.;
	double ksi_plus_1 = par[4] + 1.0;
	double px = par[1];
	double py = par[3];
	double pz = ksi_plus_1 * beam_direction_;
	TVector3 prot_dir_z(px / pz, py / pz, 1.0);

	double x_at_vertex = par[0] + primary_vertex_.Z() * prot_dir_z.X();
	double y_at_vertex = par[2] + primary_vertex_.Z() * prot_dir_z.Y();

	double sigma_x_from_z = prot_dir_z.X() * primary_vertex_error_.Z();
	double sigma_y_from_z = prot_dir_z.Y() * primary_vertex_error_.Z();

	double variance_vertex_x = primary_vertex_error_.X() * primary_vertex_error_.X() + sigma_x_from_z
	        * sigma_x_from_z;
	double variance_vertex_y = primary_vertex_error_.Y() * primary_vertex_error_.Y() + sigma_y_from_z
	        * sigma_y_from_z;

	double x_diff = primary_vertex_.X() - x_at_vertex;
	double y_diff = primary_vertex_.Y() - y_at_vertex;

	double chi_sq_prim_vert_contrib = x_diff * x_diff / variance_vertex_x + y_diff * y_diff
	        / variance_vertex_y;

	return chi_sq_prim_vert_contrib;
}

void RPInverseParameterization::AddProtonAtRPCollection(const hits_at_rp_type &hits_at_rp) {
	hits_at_rp_.insert(hits_at_rp.begin(), hits_at_rp.end());
}

void RPInverseParameterization::SetPrimaryVertex(const TVector3 &vert, const TVector3 &error) {
	primary_vertex_ = vert;
	primary_vertex_error_ = error;
	primary_vertex_set_ = true;
}

void RPInverseParameterization::SetInitialParameters(RPReconstructedProton &rec_proton) {
	degrees_of_freedom_ = hits_at_rp_.size() * 2 - smart_init_values_.FreeParameterNumber();

	if (elastic_reconstruction_)
		degrees_of_freedom_ += 1;

	if (primary_vertex_set_)
		degrees_of_freedom_ += 2;

	assert(degrees_of_freedom_ > 0);

	for (int i = 0; i < smart_init_values_.ParameterNumber(); ++i) {
		if (smart_init_values_.IsParameterFixed(i) && !nm_params_.Parameters()[i].IsFixed())
			nm_params_.Fix(i);
		else if (!smart_init_values_.IsParameterFixed(i) && nm_params_.Parameters()[i].IsFixed())
			nm_params_.Release(i);

		nm_params_.SetValue(i, smart_init_values_.GetValue(i));
		nm_params_.SetError(i, rec_precision_[i]);
	}

	if (verbosity_) {
		std::cout << "Minimisation initialization:" << std::endl;
		for (int i = 0; i < RPReconstructedProton::dimension; ++i) {
			std::cout << smart_init_values_.GetValue(i) << ", ";
		}
		std::cout << std::endl;
		for (int i = 0; i < smart_init_values_.ParameterNumber(); ++i) {
			std::cout << smart_init_values_.IsParameterFixed(i) << ", ";
		}
		std::cout << std::endl;
	}
}

//
//
//void RPInverseParameterization::FindInitialParameterValues(
//        RPReconstructedProton &rec_proton)
//{
//  if(verbosity_)
//    std::cout<<"searching for parameter initial values..."<<std::endl;
//    
//  init_values_ = default_init_values_;
//  
//  FindInitialParameterValues(rec_proton, RPReconstructedProton::nksi);
//  FindInitialParameterValues(rec_proton, RPReconstructedProton::ntheta_y);
//  FindInitialParameterValues(rec_proton, RPReconstructedProton::ntheta_x);
//  FindInitialParameterValues(rec_proton, RPReconstructedProton::nx);
//  FindInitialParameterValues(rec_proton, RPReconstructedProton::ny);
//}
//
//
//void RPInverseParameterization::FindInitialParameterValuesPrecisely(
//        RPReconstructedProton &rec_proton)
//{
//  if(verbosity_)
//    std::cout<<"searching again for precise parameter initial values..."<<std::endl;
//    
//  init_values_ = default_init_values_;
//  
//  for(int i=0; i<3; ++i)
//  {
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::nksi);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::ntheta_y);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::ntheta_x);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::nx);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::ny);
//  }
//  for(int i=0; i<2; ++i)
//  {
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::nksi);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::ntheta_x);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::ntheta_y);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::ny);
//    FindInitialParameterValues(rec_proton, RPReconstructedProton::nx);
//  }
//}
//
//
//
//void RPInverseParameterization::FindInitialParameterValues(
//        RPReconstructedProton &rec_proton, unsigned int param_id)
//{
//  if(!rec_proton.Fitted(param_id))
//    return;
//    
//  if(verbosity_)
//    std::cout<<"searching for parameter initial values param id="<<param_id<<std::endl;
//
//  std::vector<double> temp_values = init_values_;
//  double temp_chi2 = operator()(temp_values);
//  
//  for(int iter = 0; iter<random_iterations_; ++iter)
//  {
//    temp_values[param_id] = random_engine_.Uniform(min_random_init_[param_id],
//            max_random_init_[param_id]);
//    double new_chi2 = operator()(temp_values);
//
//    if(verbosity_)
//      std::cout<<"iter:"<<iter<<" chi2="<<new_chi2<<std::endl;
//
//    if(new_chi2<temp_chi2)
//    {
//      temp_chi2 = new_chi2;
//      init_values_ = temp_values;
//      if(verbosity_)
//      {
//        std::cout<<"better values found: ";
//        for(unsigned int i=0; i<5; ++i)
//        {
//          std::cout<<init_values_[i]<<", ";
//        }
//        std::cout<<"chi2="<<temp_chi2<<std::endl;
//      }
//    }
//  }
//}


//returns chi2ndf
double RPInverseParameterization::GetInitialParameters(const ROOT::Minuit2::FunctionMinimum &min,
        RPReconstructedProton &rec_proton) {
	const ROOT::Minuit2::MnUserParameters & user_params = min.UserState().Parameters();
	double chi2 = min.UserState().Fval();
	double chi2_div_N = chi2 / degrees_of_freedom_;

	for (int i = 0; i < smart_init_values_.ParameterNumber(); ++i) {
		smart_init_values_.SetValue(i, user_params.Value(i));
	}
	rec_proton.X(user_params.Value((unsigned int) 0));
	rec_proton.Theta_x(user_params.Value((unsigned int) 1));
	rec_proton.Y(user_params.Value((unsigned int) 2));
	rec_proton.Theta_y(user_params.Value((unsigned int) 3));
	rec_proton.Ksi(user_params.Value((unsigned int) 4));

	if (verbosity_) {
		std::cout << "Min finished. Chisq=" << chi2 << " Chisq/NDF=" << chi2_div_N << std::endl;
	}
	return chi2;
}

//returns chi2ndf
double RPInverseParameterization::FitConstrainedXi(RPReconstructedProton &rec_proton, double xi) {
	smart_init_values_.SetValues(default_init_values_);
	smart_init_values_.ReleaseAll();
	smart_init_values_.FixValue(4, xi); //fix xi
	FitXYCoords();

	// starting values for parameters
	//  FindInitialParameterValues(rec_proton);
	//  FindinitialParamsWithGeneticAlgorithm(rec_proton);
	SetInitialParameters(rec_proton);
	if (verbosity_)
		PrintProtonsAtRP();
	ROOT::Minuit2::FunctionMinimum min = theMinimizer_.Minimize(*this, nm_params_, strategy_, 5000);

	if (verbosity_) {
		std::cout << "End of constrained fit" << std::endl;
	}

	return GetInitialParameters(min, rec_proton);
}

bool RPInverseParameterization::FitNonConstrained(RPReconstructedProton &rec_proton) {
	smart_init_values_.ReleaseAll();
	FitXYCoords();

	// starting values for parameters
	//  FindInitialParameterValues(rec_proton);
	//  FindinitialParamsWithGeneticAlgorithm(rec_proton);
	SetInitialParameters(rec_proton);
	if (verbosity_)
		PrintProtonsAtRP();
	ROOT::Minuit2::FunctionMinimum min = theMinimizer_.Minimize(*this, nm_params_, strategy_, 5000);
	//  double chi2 = min.UserState().Fval();
	//  double chi2_div_N = chi2/degrees_of_freedom_;

	bool converged = SetResults(min, rec_proton);
	GetInitialParameters(min, rec_proton);

	if (verbosity_) {
		std::cout << "End of nonconstrained fit" << std::endl;
	}
	return converged; //to be changed accordingly
}

void RPInverseParameterization::InitializeFit(RPReconstructedProton &rec_proton) {
	binom_min_search_->Clear();
	variance_marices_initialised_ = false;

	bool initialization_converged = false;

	for (int i = 0; !initialization_converged && i < random_iterations_; ++i) {
		if (verbosity_) {
			std::cout << "RPInverseParameterization::Fit, initialization iteration " << i << std::endl;
		}
		double init_xi = binom_min_search_->GetXiCandidate(true);
		double init_chisq = FitConstrainedXi(rec_proton, init_xi);
		initialization_converged = init_chisq < init_converged_chisq_;
		if (verbosity_) {
			std::cout << "init_xi=" << init_xi << " init_chisq=" << init_chisq << std::endl;
		}
		binom_min_search_->InsertElement(init_xi, init_chisq);
	}

	if (verbosity_) {
		std::cout << "RPInverseParameterization::Fit" << std::endl;
	}

	if (!initialization_converged) {
		if (verbosity_) {
			std::cout << "RPInverseParameterization::Restore best candidate" << std::endl;
		}
		double best_xi_candidate = binom_min_search_->GetBestXiCandidate();
		FitConstrainedXi(rec_proton, best_xi_candidate);
	}
}

bool RPInverseParameterization::Fit(RPReconstructedProton &rec_proton, bool init_for_2_sided_fitting) {
	InitializeFit(rec_proton);
	bool unconstrained_fit_converged = FitNonConstrained(rec_proton);

	if (compute_full_variance_matrix_ && !init_for_2_sided_fitting) {
		InitializeMatrixBasedFitting(rec_proton);
		unconstrained_fit_converged = FitNonConstrained(rec_proton);
	}

	return unconstrained_fit_converged;
}

void RPInverseParameterization::InitializeMatrixBasedFitting(RPReconstructedProton &rec_proton) {
	if (compute_full_variance_matrix_) {
		InitializeVarianceMatrixService();
		rec_variance_service_->SetEstimatedProtonState(rec_proton.X() * 0.001, rec_proton.Theta_x(),
		        rec_proton.Y() * 0.001, rec_proton.Theta_y(), rec_proton.Ksi(), rec_proton.ZDirection());
		if (xyCorrelation) { // for correlated x, y we have got only one matrix (two times bigger)
			rec_variance_service_->ComputeInvertedVarianceMatrix(inv_var_x_);
		} else {
			rec_variance_service_->ComputeInvertedVarianceMatrices(inv_var_x_, inv_var_y_);
		}
		variance_marices_initialised_ = true;
	}
}

bool RPInverseParameterization::SetResults(const ROOT::Minuit2::FunctionMinimum &min,
        RPReconstructedProton &rec_proton) {
	const ROOT::Minuit2::MnUserParameters & user_params = min.UserState().Parameters();
	const std::vector<ROOT::Minuit2::MinuitParameter> & raw_pars = user_params.Parameters();
	const ROOT::Minuit2::MnUserCovariance & user_covar = min.UserState().Covariance();
	double chi2 = min.UserState().Fval();
	double chi2_div_N = chi2 / degrees_of_freedom_;
	bool converged = chi2_div_N < converged_chisqndf_max_ || min.IsValid();

	rec_proton.Valid(converged);
	if (!converged)
		return false;

	if (verbosity_) {
		std::cout << "Minimised chi2:" << chi2 << " chi2_div_N:" << chi2_div_N << std::endl;
		for (int i = 0; i < RPReconstructedProton::dimension; ++i) {
			std::cout << "par " << i << " val=" << user_params.Value(i) << " error=" << user_params.Error(i)
			        << std::endl;
		}
		std::cout << min << std::endl;
	}

	rec_proton.Chi2(chi2);
	rec_proton.Chi2Norm(chi2_div_N);
	rec_proton.DegreesOfFreedom(degrees_of_freedom_);

	//copy parameters
	for (int i = 0; i < RPReconstructedProton::dimension; ++i) {
		rec_proton.Parameter(i, user_params.Value(i));
	}

	SetPulls(raw_pars, rec_proton);

	//copy variance matrix
	//  std::cout<<"Copying the variance matrix..."<<std::endl;
	if (min.IsValid() && min.HasCovariance() && (RPReconstructedProton::dimension == user_covar.Nrow()) ) 
  {
		for (int i = 0; i < RPReconstructedProton::dimension; ++i) 
    {
			for (int j = 0; j < RPReconstructedProton::dimension; ++j) 
      {
				rec_proton.CovarianceMartixElement(i, j, user_covar(i, j));
			}
		}
	}

	return true;
}

void RPInverseParameterization::SetPulls(const std::vector<ROOT::Minuit2::MinuitParameter> & raw_pars,
        RPReconstructedProton &rec_proton) {
	double par_m[5];
	par_m[0] = raw_pars[0].Value() / 1000.;
	par_m[1] = raw_pars[1].Value();
	par_m[2] = raw_pars[2].Value() / 1000.;
	par_m[3] = raw_pars[3].Value();
	par_m[4] = raw_pars[4].Value();

	double out[2];

	for (hits_at_rp_type::const_iterator it = hits_at_rp_.begin(); it != hits_at_rp_.end(); ++it) {
		transport_to_rp_[it->first].Transport2D(par_m, out, false);
		out[0] *= 1000; //convert [m] to [mm]
		out[1] *= 1000;
		RP2DHitDebug hit_deb(it->second);
		hit_deb.DeltaX(out[0] - hit_deb.X());
		hit_deb.DeltaY(out[1] - hit_deb.Y());
		hit_deb.PullX(hit_deb.DeltaX() / hit_deb.Sx());
		hit_deb.PullY(hit_deb.DeltaY() / hit_deb.Sy());
		rec_proton.AddDebugHit(it->first, hit_deb);
	}
}

void RPInverseParameterization::PrintProtonsAtRP() {
	for (hits_at_rp_type::iterator it = hits_at_rp_.begin(); it != hits_at_rp_.end(); ++it) {
		std::cout << "rp=" << it->first << " Pos=(" << it->second.X() << ", " << it->second.Y() << ") S=("
		        << it->second.Sx() << ", " << it->second.Sy() << ")" << std::endl;
	}
}

//
//void RPInverseParameterization::FindinitialParamsWithGeneticAlgorithm(
//    RPReconstructedProton &rec_proton)
//{
//  if(verbosity_)
//    std::cout<<"RPInverseParameterization::GeneticAlgorithmInitialization"<<std::endl;
//  
//  init_values_ = default_init_values_;
//  
////  // define GA parameters
////  fGA_preCalc   = 1;
////  fGA_SC_steps  = 10;
////  fGA_SC_rate   = 5;
////  fGA_SC_factor = 0.95;
////  fGA_nsteps    = 30;
////  
////  // ranges
//  std::vector<TMVA::Interval*> ranges;
//
//  ranges.push_back(new TMVA::Interval(default_init_values_[0], default_init_values_[0]) );
//  ranges.push_back(new TMVA::Interval(min_random_init_[1], max_random_init_[1]) );
//  ranges.push_back(new TMVA::Interval(default_init_values_[2], default_init_values_[2]) );
//  ranges.push_back(new TMVA::Interval(min_random_init_[3], max_random_init_[3]) );
//  ranges.push_back(new TMVA::Interval(min_random_init_[4], max_random_init_[4]) );
//  
////    if(max_random_init_[i]>=min_random_init_[i])
////      ranges.push_back(new TMVA::Interval(min_random_init_[i], max_random_init_[i]) );
////    else
////      ranges.push_back(new TMVA::Interval(max_random_init_[i], min_random_init_[i]) );
//  
//  TMVA::GeneticFitter *gf = new TMVA::GeneticFitter( *this, "GA_name", ranges, "" );
//  
//  Int_t cycles = 1;  //Independent cycles of GA fitting
//  Int_t nsteps = 20;  //Number of steps for convergence
//  Int_t popSize = 100;  //Population size for GA
//  Int_t SC_steps = 7;  //Spread control, steps
//  Int_t SC_rate = 4;  //Spread control, rate: factor is changed depending on the rate
//  Double_t SC_factor = 0.95;  //Spread control, factor
//  Double_t convCrit = 0.001;  //Convergence criteria
//  
//  gf->SetParameters(cycles, nsteps, popSize, SC_steps, SC_rate,
//      SC_factor, convCrit);
//
//  gf->Run(init_values_);
//  //delete gf;
//}
//
//
//Double_t RPInverseParameterization::EstimatorFunction(std::vector<Double_t>& parameters)
//{
//   // interface to the estimate
//  return RPInverseParameterization::operator()(parameters);
//}

void RPInverseParameterization::InitializeVarianceMatrixService() {
	rec_variance_service_->ResetCovarianceMatrix();
	for (hits_at_rp_type::const_iterator it_det = hits_at_rp_.begin(); it_det != hits_at_rp_.end(); ++it_det) {
		//convert the values to [m]
		double det_z = it_det->second.Z() * 0.001;
		rec_variance_service_->AddScatteringPlane(det_z, multiple_scat_sigma_);
		rec_variance_service_->AddDetectorPlane(det_z, rp_expected_resolution_ * 0.001);
	}
}
