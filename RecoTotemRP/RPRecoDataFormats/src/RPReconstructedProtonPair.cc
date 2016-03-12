#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPair.h"
#include "TMath.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/RPRecoProtMADXVariables.h"
#include <vector>


RPReconstructedProtonPair::RPReconstructedProtonPair()
{
  ZeroData();
}


void RPReconstructedProtonPair::ZeroData()
{
  for(int i=0; i<dimension; ++i)
  {
    variables_[i]=0.0;
    for(int j=0; j<dimension; ++j)
    {
      covariance_[i*dimension + j]=0.0;
    }
  }
  valid_=false;
}


//double RPReconstructedProtonPair::tLeft() const
//{
//  return Momentum_to_t(PxLeft(), PyLeft(), PzLeft() );
//}


//double RPReconstructedProtonPair::tRight() const
//{
//  return Momentum_to_t(PxRight(), PyRight(), PzRight() );
//}


//double RPReconstructedProtonPair::Momentum_to_t(double px, double py, double pz) const //GeV
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


//double RPReconstructedProtonPair::Momentum_to_ksi(double px, double py, double pz) const //GeV
//{
//  double p02 = E0_*E0_ - mp0_*mp0_;
//  double p0 = TMath::Sqrt(p02);
//  double p2 = px*px + py*py + pz*pz;
//  double p = TMath::Sqrt(p2);
//  double ksi = (p - p0)/p0;
//  return ksi;
//}

std::ostream &operator<<(std::ostream &out, const RPReconstructedProtonPair &prot)
{
  out<<"Vertex:"<<std::endl;
  out<<"X="<<prot.X3D()<<std::endl;
  out<<"Y="<<prot.Y3D()<<std::endl;
  out<<"Z="<<prot.Z3D()<<std::endl;
  out<<"Left proton:"<<std::endl;
  out<<"thx="<<prot.ThetaXLeft()<<std::endl;
  out<<"thy="<<prot.ThetaYLeft()<<std::endl;
  out<<"xi= "<<prot.KsiLeft()<<std::endl;
  out<<"Right proton:"<<std::endl;
  out<<"thx="<<prot.ThetaXRight()<<std::endl;
  out<<"thy="<<prot.ThetaYRight()<<std::endl;
  out<<"xi= "<<prot.KsiRight()<<std::endl;
  
  return out;
}


double RPReconstructedProtonPair::X0Left() const
{
  return X3D() + ThetaXLeft()/(1.0+KsiLeft())*Z3D();
}


double RPReconstructedProtonPair::Y0Left() const
{
  return Y3D() + ThetaYLeft()/(1.0+KsiLeft())*Z3D();
}


double RPReconstructedProtonPair::X0Right() const
{
  return X3D() - ThetaXRight()/(1.0+KsiRight())*Z3D();
}


double RPReconstructedProtonPair::Y0Right() const
{
  return Y3D() - ThetaYRight()/(1.0+KsiRight())*Z3D();
}


void RPReconstructedProtonPair::FillMADTransportNtupleLeft(
    const double *vect, double par[5])
{
  par[0] = Vertex3DTo2DLeft_X(vect[ntheta_x0], 
        vect[nksi0], vect[nx], vect[nz])/1000.0;
  par[1] = vect[ntheta_x0];
  par[2] = Vertex3DTo2DLeft_Y(vect[ntheta_y0], 
        vect[nksi0], vect[ny], vect[nz])/1000.0;
  par[3] = vect[ntheta_y0];
  par[4] = vect[nksi0];
}


void RPReconstructedProtonPair::Fill5DimReconstructionVectorLeft(
    const std::vector<double> &vect, std::vector<double> &par)
{
  par.resize(5);
  par[0] = Vertex3DTo2DLeft_X(vect[ntheta_x0], 
        vect[nksi0], vect[nx], vect[nz]);
  par[1] = vect[ntheta_x0];
  par[2] = Vertex3DTo2DLeft_Y(vect[ntheta_y0], 
        vect[nksi0], vect[ny], vect[nz]);
  par[3] = vect[ntheta_y0];
  par[4] = vect[nksi0];
}


void RPReconstructedProtonPair::FillMADTransportNtupleRight(
    const double *vect, double par[5])
{
  par[0] = Vertex3DTo2DRight_X(vect[ntheta_x1], 
        vect[nksi1], vect[nx], vect[nz])/1000.0;
  par[1] = vect[ntheta_x1];
  par[2] = Vertex3DTo2DRight_Y(vect[ntheta_y1], 
        vect[nksi1], vect[ny], vect[nz])/1000.0;
  par[3] = vect[ntheta_y1];
  par[4] = vect[nksi1];
}


void RPReconstructedProtonPair::Fill5DimReconstructionVectorRight(
    const std::vector<double> &vect, std::vector<double> &par)
{
  par.resize(5);
  par[0] = Vertex3DTo2DRight_X(vect[ntheta_x1], 
        vect[nksi1], vect[nx], vect[nz]);
  par[1] = vect[ntheta_x1];
  par[2] = Vertex3DTo2DRight_Y(vect[ntheta_y1], 
        vect[nksi1], vect[ny], vect[nz]);
  par[3] = vect[ntheta_y1];
  par[4] = vect[nksi1];
}


RPRecoProtMADXVariables RPReconstructedProtonPair::GetMADXVariablesRight() const
{
  RPRecoProtMADXVariables var;
  var.ThetaX = ThetaXRight();
  var.ThetaY = ThetaYRight();
  var.Xi = KsiRight();
  return var;
}


RPRecoProtMADXVariables RPReconstructedProtonPair::GetMADXVariablesLeft() const
{
  RPRecoProtMADXVariables var;
  var.ThetaX = ThetaXLeft();
  var.ThetaY = ThetaYLeft();
  var.Xi = KsiLeft();
  return var;
}


//const double RPReconstructedProtonPair::mp0_ = 0.9382723128;  //Gev
//const double RPReconstructedProtonPair::E0_ = 7e3;  //GeV
//const double RPReconstructedProtonPair::p0_ = TMath::Sqrt(E0_*E0_ - mp0_*mp0_);


