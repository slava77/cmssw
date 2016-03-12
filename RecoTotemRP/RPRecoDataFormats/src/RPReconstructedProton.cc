#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "TMath.h"


RPReconstructedProton::RPReconstructedProton()
{
  ZeroData();
}


void RPReconstructedProton::ZeroData()
{
  for(int i=0; i<dimension; ++i)
  {
    variables_[i]=0.0;
    fitted_[i]=false;
    for(int j=0; j<dimension; ++j)
    {
      covariance_[i*dimension + j]=0.0;
    }
  }
  valid_=false;
}

int RPReconstructedProton::FreeParametersNumber() const
{
  int count = 0;
  for(int i=0; i<dimension; ++i)
  {
    if(fitted_[i])
      ++count;
  }
  return count;
}


//double RPReconstructedProton::t() const
//{
//  double E1 = E0_;
//  double p02 = E1*E1 - mp0_*mp0_;
//  double p0 = TMath::Sqrt(p02);
//  double ksi_plus_1 = Ksi() + 1.0;
////  double p = ksi_plus_1*p0;
//  double p2 = p02*ksi_plus_1*ksi_plus_1;
//  double E22 = mp0_*mp0_ + p2; 
//  double E2 = TMath::Sqrt(E22);
////  double px = Theta_x()*p0;
////  double py = Theta_y()*p0;
//  double px2 = Theta_x()*Theta_x()*p02;
//  double py2 = Theta_y()*Theta_y()*p02;
//  double pz2 = p2 - px2 - py2;
//  double pz = TMath::Sqrt(pz2);
//  
//  double dE2 = (E1 - E2)*(E1 - E2);
//  double dp2 = px2 + py2 + (p0-pz)*(p0-pz);
//  
//  double t = dE2 - dp2;
//  
//  return t;
//}


//double RPReconstructedProton::Momentum_to_t(double px, double py, double pz) const //GeV
//{
//  pz = TMath::Abs(pz);
//  double p02 = E0_*E0_ - mp0_*mp0_;
//  double p0 = TMath::Sqrt(p02);
//  double E = TMath::Sqrt(mp0_*mp0_ + px*px+py*py+pz*pz);
//  double dE2 = (E0_ - E)*(E0_ - E);
//  double dp2 = px*px + py*py + (p0-pz)*(p0-pz);
//  
//  return dE2 - dp2;
//}


//double RPReconstructedProton::Momentum_to_ksi(double px, double py, double pz) const //GeV
//{
//  double p02 = E0_*E0_ - mp0_*mp0_;
//  double p0 = TMath::Sqrt(p02);
//  double p2 = px*px + py*py + pz*pz;
//  double p = TMath::Sqrt(p2);
//  double ksi = (p - p0)/p0;
//  return ksi;
//}


//const double RPReconstructedProton::mp0_ = 0.9382723128;  //Gev
//const double RPReconstructedProton::E0_ = 7e3;  //GeV
//const double RPReconstructedProton::p0_ = TMath::Sqrt(E0_*E0_ - mp0_*mp0_);

RPRecoProtMADXVariables RPReconstructedProton::GetMADXVariables() const
{
  RPRecoProtMADXVariables var;
  var.ThetaX = Theta_x();
  var.ThetaY = Theta_y();
  var.Xi = Ksi();
  return var;
}

std::ostream &operator<<(std::ostream &out, const RPReconstructedProton &prot)
{
  return out;
}

