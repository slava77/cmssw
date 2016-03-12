#include "RecoTotemRP/RPInverseParameterization/interface/RPInverse2SidedParameterization.h"
#include "TotemProtonTransport/TotemRPProtonTransportParametrization/interface/LHCOpticsApproximator.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHitDebug.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <cassert>
#include <iostream>
#include "TMath.h"
#include "TRandom2.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/RPRecoProtMADXVariables.h"


//x, y, z, theta_x0, theta_y0, ksi0, theta_x1, theta_y1, ksi1 - canonical MAD coordinates
//0  1  2      3          4      5       6        7       8

RPInverse2SidedParameterization::RPInverse2SidedParameterization(
    const edm::ParameterSet& conf, const BeamOpticsParams & BOPar) 
 : strategy_(2), BOPar_(BOPar)
{
  verbosity_=0;
  primary_vertex_set_=false;
  
  typedef RPReconstructedProtonPair index;
  
  rec_precision_.resize(index::dimension);
  rec_precision_[index::nx] = conf.getParameter<double>("ReconstructionPrecisionX");
  rec_precision_[index::ny] = conf.getParameter<double>("ReconstructionPrecisionY");
  rec_precision_[index::nz] = conf.getParameter<double>("ReconstructionPrecisionZ");
  
  rec_precision_[index::ntheta_x0] = conf.getParameter<double>("ReconstructionPrecisionThetaX");
  rec_precision_[index::ntheta_x1] = conf.getParameter<double>("ReconstructionPrecisionThetaX");
  
  rec_precision_[index::ntheta_y0] = conf.getParameter<double>("ReconstructionPrecisionThetaY");
  rec_precision_[index::ntheta_y1] = conf.getParameter<double>("ReconstructionPrecisionThetaY");
  
  rec_precision_[index::nksi0] = conf.getParameter<double>("ReconstructionPrecisionKsi");
  rec_precision_[index::nksi1] = conf.getParameter<double>("ReconstructionPrecisionKsi");
  
  inverse_param_random_seed_ = conf.getParameter<int>("InverseParamRandSeed");
  random_engine_.SetSeed(inverse_param_random_seed_);
  
  elastic_reconstruction_ =  conf.getParameter<bool>("ElasticScatteringReconstruction"); 
  
//  default_init_values_.push_back(0.0);  //x
//  default_init_values_.push_back(0.0);  //y
//  default_init_values_.push_back(0.0);  //z
//  default_init_values_.push_back(0.0);  //thetax0
//  default_init_values_.push_back(0.0);  //thetay0
//  default_init_values_.push_back(-0.07);  //ksi0
//  default_init_values_.push_back(0.0);  //thetax1
//  default_init_values_.push_back(0.0);  //thetay1
//  default_init_values_.push_back(-0.07);  //ksi1
  
  nm_params_.Add("x", 0., rec_precision_[index::nx]);
  nm_params_.Add("y", 0., rec_precision_[index::ny]);
  nm_params_.Add("z", 0., rec_precision_[index::nz]);
  nm_params_.Add("theta_x_0", 0., rec_precision_[index::ntheta_x0]);
  nm_params_.Add("theta_y_0", 0., rec_precision_[index::ntheta_y0]);
  nm_params_.Add("ksi_0", -0.2, rec_precision_[index::nksi0]);
  nm_params_.Add("theta_x_1", 0., rec_precision_[index::ntheta_x1]);
  nm_params_.Add("theta_y_1", 0., rec_precision_[index::ntheta_y1]);
  nm_params_.Add("ksi_1", -0.2, rec_precision_[index::nksi1]);
  
  inverse_param_right_ = std::auto_ptr<RPInverseParameterization>(new RPInverseParameterization(
      1.0, conf, BOPar));
  inverse_param_left_ = std::auto_ptr<RPInverseParameterization>(new RPInverseParameterization(
      -1.0, conf, BOPar));
  
  converged_chisqndf_max_ = conf.getParameter<double>("MaxChiSqNDFOfConvergedProton");
}


void RPInverse2SidedParameterization::Verbosity(int ver)
{
  verbosity_ = ver;
  inverse_param_right_->Verbosity(ver);
  inverse_param_left_->Verbosity(ver);
}

//x, y, z, theta_x0, theta_y0, ksi0, theta_x1, theta_y1, ksi1 - canonical MAD coordinates
//0  1  2      3          4      5       6        7       8
double RPInverse2SidedParameterization::operator()(const std::vector<double>& par) const
{
  typedef RPReconstructedProtonPair index;
  
  assert(par.size() == 9);

  double chi2 = 0.;

  //MADX canonical variables
  //(x, theta_x, y, theta_y, ksi) [mm, rad, mm, rad, -1..0]
  std::vector<double> par_left(5);
  std::vector<double> par_right(5);
  
  index::Fill5DimReconstructionVectorRight(par, par_right);
  index::Fill5DimReconstructionVectorLeft(par, par_left);
  
  chi2 += inverse_param_right_->GetRPChi2Contribution(par_right);
  chi2 += inverse_param_left_->GetRPChi2Contribution(par_left);

  if(primary_vertex_set_)
    chi2 += ChiSqPrimaryVertexContrib(par);
  
  if(elastic_reconstruction_)
    chi2 += ElasticReconstrChi2Contrib(par_left, par_right);
    
  if(verbosity_)
  {
    for(int i=0; i<9; i++)
    {
      std::cout<<i<<"="<<par[i]<<", ";
    }
    std::cout<<"chi2="<<chi2<<std::endl;
  }
  
  return chi2;
}


double RPInverse2SidedParameterization::ElasticReconstrChi2Contrib(const std::vector<double> &par_left, const std::vector<double> &par_right) const
{
  double chi2_contr = 0;
  double xi_beam_mean = BOPar_.GetMeanXi();
  double xi_beam_smearing = BOPar_.GetSigmaXi();
  double var_xi_beam_smearing = xi_beam_smearing*xi_beam_smearing; 
  
  double right_xi_diff = par_right[4] - xi_beam_mean;
  double right_xi_diff_chi2_contrib = right_xi_diff*right_xi_diff/var_xi_beam_smearing; 
  chi2_contr += right_xi_diff_chi2_contrib;

  double left_xi_diff = par_left[4] - xi_beam_mean;
  double left_xi_diff_chi2_contrib = left_xi_diff*left_xi_diff/var_xi_beam_smearing; 
  chi2_contr += left_xi_diff_chi2_contrib;
  
  RPRecoProtMADXVariables madx_var_left, madx_var_right;
  
  madx_var_left.ThetaX = par_left[1];
  madx_var_left.ThetaY = par_left[3];
  madx_var_left.Xi = par_left[4];
  
  HepMC::FourVector p_left = BOPar_.LeftProtonMADXCanonicalVariablesToP(madx_var_left);
  double th_x_left = TMath::ASin(p_left.x()/p_left.rho()) - BOPar_.GetCrossingAngleX();
  double th_y_left = TMath::ASin(p_left.y()/p_left.rho()) - BOPar_.GetCrossingAngleY();

  madx_var_right.ThetaX = par_right[1];
  madx_var_right.ThetaY = par_right[3];
  madx_var_right.Xi = par_right[4];
  
  HepMC::FourVector p_right = BOPar_.RightProtonMADXCanonicalVariablesToP(madx_var_right);
  double th_x_right = TMath::ASin(p_right.x()/p_right.rho()) - BOPar_.GetCrossingAngleX();
  double th_y_right = TMath::ASin(p_right.y()/p_right.rho()) - BOPar_.GetCrossingAngleY();
  
  double diff_thx = th_x_right + th_x_left;
  double diff_thy = th_y_right + th_y_left; 
  
  double beam_divergence_x = BOPar_.GetBeamDivergenceX();
  double beam_divergence_y = BOPar_.GetBeamDivergenceY();
      
  double diff_thx_contrib_chi2 = (diff_thx*diff_thx) / (2.0*beam_divergence_x*beam_divergence_x); 
  double diff_thy_contrib_chi2 = (diff_thy*diff_thy) / (2.0*beam_divergence_y*beam_divergence_y);
  
  chi2_contr += diff_thx_contrib_chi2;
  chi2_contr += diff_thy_contrib_chi2;
  
  return chi2_contr;
}


void RPInverse2SidedParameterization::PrintFittedHitsInfo(std::ostream &o)
{
  hits_at_rp_type::const_iterator it = hits_at_rp_.begin();
  hits_at_rp_type::const_iterator end = hits_at_rp_.end();
  for(; it!=end; ++it)
  {
    double vx_rp = it->second.Vx();
    double vy_rp = it->second.Vy();
    
    std::cout<<"par["<<it->first<<"]sigma_x="<<TMath::Sqrt(vx_rp)
      <<" par["<<it->first<<"]sigma_y="<<TMath::Sqrt(vy_rp)<<std::endl;
  }
//  if(primary_vertex_set_)
//    std::cout<<"vertex_sigma_x="<<TMath::Sqrt(variance_vertex_x)
//    <<" vertex_sigma_y="<<TMath::Sqrt(variance_vertex_y)<<std::endl;
}


//x, y, z, theta_x0, theta_y0, ksi0, theta_x1, theta_y1, ksi1 - canonical MAD coordinates
//0  1  2      3          4      5       6        7       8
double RPInverse2SidedParameterization::ChiSqPrimaryVertexContrib(const std::vector<double>& par) const
{
  typedef RPReconstructedProtonPair index;
  if(!primary_vertex_set_)
    return 0.;

  double err_x = primary_vertex_error_.X();
  double err_y = primary_vertex_error_.Y();
  double err_z = primary_vertex_error_.Z();
  
  double diff_x = par[index::nx] - primary_vertex_.X();
  double diff_y = par[index::ny] - primary_vertex_.Y();
  double diff_z = par[index::nz] - primary_vertex_.Z();
  
  double chi_sq_prim_vert_contrib = diff_x*diff_x/(err_x*err_x) 
        + diff_y*diff_y/(err_y*err_y) + diff_z*diff_z/(err_z*err_z);
        
  return chi_sq_prim_vert_contrib;
}


void RPInverse2SidedParameterization::AddProtonAtRPCollection(const hits_at_rp_type &hits_at_rp)
{
  //hits_at_rp_.insert(hits_at_rp.begin(), hits_at_rp.end());
  for(hits_at_rp_type::const_iterator it = hits_at_rp.begin(); it!=hits_at_rp.end();
      ++it)
  {
    AddProtonAtRP(it->first, it->second);
  }
}

void RPInverse2SidedParameterization::AddParameterizationsRight(const transport_to_rp_type& param_map)
{
  transport_to_rp_.insert(param_map.begin(), param_map.end());
  inverse_param_right_->SetParameterizations(param_map);
}


void RPInverse2SidedParameterization::AddParameterizationsLeft(const transport_to_rp_type& param_map)
{
  transport_to_rp_.insert(param_map.begin(), param_map.end());
  inverse_param_left_->SetParameterizations(param_map);
}


void RPInverse2SidedParameterization::RemoveRomanPots()
{
  transport_to_rp_.clear(); ClearEvent();
  inverse_param_right_->RemoveRomanPots();
  inverse_param_left_->RemoveRomanPots();
}


void RPInverse2SidedParameterization::ClearEvent()
{
  hits_at_rp_.clear(); 
  primary_vertex_set_=false;
  inverse_param_right_->ClearEvent();
  inverse_param_left_->ClearEvent();
}

void RPInverse2SidedParameterization::AddProtonAtRP(RPId rp_id, const RP2DHit &hit)
{
  hits_at_rp_[rp_id]=hit;
  if(RightArm(rp_id))
  {
    inverse_param_right_->AddProtonAtRP(rp_id, hit);
  }
  else if(LeftArm(rp_id))
  {
    inverse_param_left_->AddProtonAtRP(rp_id, hit);
  }
}


void RPInverse2SidedParameterization::SetPrimaryVertex(const TVector3 &vert, 
    const TVector3 &error)
{
  primary_vertex_ = vert;
  primary_vertex_error_ = error;
  primary_vertex_set_=true;
  inverse_param_right_->SetPrimaryVertex(vert, error);
  inverse_param_left_->SetPrimaryVertex(vert, error);
}


void RPInverse2SidedParameterization::FindInitialValues(
    RPReconstructedProtonPair &rec_proton_pair)
{
  RPReconstructedProton rec_proton_right_;
  rec_proton_right_.ZDirection(1.0);
  RPReconstructedProton rec_proton_left_;
  rec_proton_left_.ZDirection(-1.0);
  
  inverse_param_right_->Fit(rec_proton_right_, true);
  inverse_param_left_->Fit(rec_proton_left_, true);
  
  double x_mean = (rec_proton_right_.X() + rec_proton_left_.X())/2.0;
  double y_mean = (rec_proton_right_.Y() + rec_proton_left_.Y())/2.0;
  
  rec_proton_pair.X3D(x_mean);
  rec_proton_pair.Y3D(y_mean);
  rec_proton_pair.Z3D(0.0);
  
  rec_proton_pair.ThetaXLeft(rec_proton_left_.Theta_x());
  rec_proton_pair.ThetaYLeft(rec_proton_left_.Theta_y());
  rec_proton_pair.KsiLeft(rec_proton_left_.Ksi());
  rec_proton_pair.ThetaXRight(rec_proton_right_.Theta_x());
  rec_proton_pair.ThetaYRight(rec_proton_right_.Theta_y());
  rec_proton_pair.KsiRight(rec_proton_right_.Ksi());
  
  if(elastic_reconstruction_)
  {
    rec_proton_pair.KsiLeft(BOPar_.GetMeanXi());
    rec_proton_pair.KsiRight(BOPar_.GetMeanXi());
  }
  
  if(verbosity_)
  {
    std::cout<<"Initialization proton pair:"<<std::endl;
    std::cout<<rec_proton_pair<<std::endl;
  }
  
  inverse_param_right_->InitializeMatrixBasedFitting(rec_proton_right_);
  inverse_param_left_->InitializeMatrixBasedFitting(rec_proton_left_);
}


void RPInverse2SidedParameterization::SetInitialParameters(
        RPReconstructedProtonPair &rec_proton_pair)
{
  degrees_of_freedom_ = hits_at_rp_.size()*2 - rec_proton_pair.FreeParametersNumber();
  if(primary_vertex_set_)
  {
    degrees_of_freedom_+=3;
  }
  
  if(elastic_reconstruction_)
  {
    degrees_of_freedom_+=4; //xi_x1==0, xi_2==0, thx_1==thx_2, thy1==thy2
  }
    
  assert(degrees_of_freedom_>0);
  
  for(int i=0; i<RPReconstructedProtonPair::dimension; ++i)
  {
    if(verbosity_)
      std::cout<<"Fitted parameter "<<i<<std::endl;
    if(nm_params_.Parameters()[i].IsFixed())
      nm_params_.Release(i);
        
//    nm_params_.SetValue(i, init_values_[i]);
    nm_params_.SetValue(i, rec_proton_pair.Parameter(i));
    nm_params_.SetError(i, rec_precision_[i]);
  }
}


bool RPInverse2SidedParameterization::Fit(RPReconstructedProtonPair &rec_proton_pair)
{
  // starting values for parameters
//  FindInitialParameterValues(rec_proton_pair);
  FindInitialValues(rec_proton_pair);
  SetInitialParameters(rec_proton_pair);
  
  if(verbosity_)
    PrintProtonsAtRP();
  ROOT::Minuit2::FunctionMinimum min = theMinimizer_.Minimize(*this, nm_params_, strategy_, 5000);
//  if(verbosity_)
//    std::cout<<min<<std::endl;
  bool res = SetResults(min, rec_proton_pair);
//  if(!res)
//  {
//    if(verbosity_)
//      std::cout<<"Did not converge, once again, but more carefully..."<<std::endl;
//    FindInitialParameterValuesPrecisely(rec_proton_pair);
//
//    min = theMinimizer_.Minimize(*this, nm_params_, strategy_, 5000);
//    res = SetResults(min, rec_proton_pair);
//    if(verbosity_ && !res)
//    {
//      std::cout<<"Did not converge, failed."<<std::endl;
//    }
//  }
  
  return res;
}


bool RPInverse2SidedParameterization::SetResults(const ROOT::Minuit2::FunctionMinimum &min, 
      RPReconstructedProtonPair &rec_proton_pair)
{
  double chi2 = min.UserState().Fval();
  double chi2_div_N = chi2/degrees_of_freedom_;
  bool converged = chi2_div_N < converged_chisqndf_max_ || min.IsValid();
  
//  if(!converged)
//    std::cout<<"Not converged really!!!! chi2_div_N="<<chi2_div_N<<" > "<<converged_chisqndf_max_<<std::endl;

  rec_proton_pair.Valid(converged);
  if(!converged)
    return false;
     
  const ROOT::Minuit2::MnUserParameters & user_params = min.UserState().Parameters();
  const std::vector< ROOT::Minuit2::MinuitParameter > & raw_pars = user_params.Parameters ();
  const ROOT::Minuit2::MnUserCovariance & user_covar = min.UserState().Covariance();
  
  if(verbosity_)
  {
    std::cout<<"chi2:"<<chi2<<" chi2_div_N:"<<chi2_div_N<<std::endl;
    for(int i=0; i<RPReconstructedProtonPair::dimension; ++i)
    {
      std::cout<<"par "<<i<<" val="<<user_params.Value(i)<<" error="<<user_params.Error(i)<<std::endl;
    }
    std::cout<<min<<std::endl;
  }
  
  rec_proton_pair.Chi2(chi2);
  rec_proton_pair.Chi2Norm(chi2_div_N);
  rec_proton_pair.DegreesOfFreedom(degrees_of_freedom_);

  //copy parameters
  for(int i=0; i<RPReconstructedProtonPair::dimension; ++i)
  {
    rec_proton_pair.Parameter(i, user_params.Value(i));
  }
  
  SetPulls(raw_pars, rec_proton_pair);
  
  //copy variance matrix
  if(min.IsValid() && min.HasCovariance() && (RPReconstructedProtonPair::dimension == user_covar.Nrow()) )
  {
    for(int i=0; i<RPReconstructedProtonPair::dimension; ++i)
    {
      for(int j=0; j<RPReconstructedProtonPair::dimension; ++j)
      {
        rec_proton_pair.CovarianceMartixElement(i,j, user_covar(i, j));
      }
    }
  }
  return true;
}


void RPInverse2SidedParameterization::SetPulls(const std::vector< ROOT::Minuit2::MinuitParameter > & raw_pars, 
      RPReconstructedProtonPair &rec_proton_pair)
{
  //convert position from [mm] to [m]
  double par_left[5];
  double par_right[5];
  
  rec_proton_pair.FillMADTransportNtupleLeft(par_left);
  rec_proton_pair.FillMADTransportNtupleRight(par_right);
  
  double out[2];
  
  for(hits_at_rp_type::const_iterator it = hits_at_rp_.begin(); it!=hits_at_rp_.end(); 
      ++it)
  {
    double *par_m = LeftArm(it->first)?(par_left):(par_right);
    
    transport_to_rp_[it->first].Transport2D(par_m, out, false);
    out[0]*=1000;  //convert [m] to [mm]
    out[1]*=1000;
    RP2DHitDebug hit_deb(it->second);
    hit_deb.DeltaX(out[0]-hit_deb.X());
    hit_deb.DeltaY(out[1]-hit_deb.Y());
    hit_deb.PullX(hit_deb.DeltaX()/hit_deb.Sx());
    hit_deb.PullY(hit_deb.DeltaY()/hit_deb.Sy());
    rec_proton_pair.AddDebugHit(it->first, hit_deb);
  }
}


void RPInverse2SidedParameterization::PrintProtonsAtRP()
{
  for(hits_at_rp_type::iterator it = hits_at_rp_.begin(); it!=hits_at_rp_.end(); ++it)
  {
    std::cout<<"rp="<<it->first<<" Pos=("<<it->second.X()<<", "<<it->second.Y()<<") S=("
      <<it->second.Sx()<<", "<<it->second.Sy()<<")"<<std::endl;
  }
}

