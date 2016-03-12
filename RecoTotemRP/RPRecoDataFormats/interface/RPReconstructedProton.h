#ifndef RecoTotemRP_RPRecoDataFormats_RPReconstructedProton_h
#define RecoTotemRP_RPRecoDataFormats_RPReconstructedProton_h

#include "TNamed.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHitDebug.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "TVector2.h"
#include <map>
#include <ostream>
#include "TMath.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/RPRecoProtMADXVariables.h"

class RPReconstructedProton
{
  //in case of array access to the kinematic variables, the order of reconstructed 
  //parameters is : x, theta_x, y, theta_y, ksi - canonical MAD coordinates
  //[mm, rad, mm, rad, -1 0]
  //energy and momentum and related in GeV
  //functions converting MADX variables to general physics values to be added
  
  public:
    enum { dimension = 5 };
    enum { cov_dimension = dimension*dimension };
    enum { nx=0, ntheta_x=1, ny=2, ntheta_y=3, nksi=4 };
    RPReconstructedProton();
    virtual ~RPReconstructedProton() {}
    
    typedef std::map<RPId, RP2DHitDebug> debug_hits_map_type;
    inline double X() const {return variables_[nx];}
    inline double Y() const {return variables_[ny];}
    inline double Theta_x() const {return variables_[ntheta_x];}
    inline double Theta_y() const {return variables_[ntheta_y];}
    inline double Theta_x_angle() const {return Theta_x()/(1.0+Ksi());}
    inline double Theta_y_angle() const {return Theta_y()/(1.0+Ksi());}
    inline double Ksi() const {return variables_[nksi];}
    inline double ZDirection() const {return z_direction_;}
//    inline double Phi() const {return TVector2(Theta_x(), Theta_y()).Phi();}
//    double t() const;
    inline void X(double val) {variables_[nx]=val;}
    inline void Y(double val) {variables_[ny]=val;}
    inline void Theta_x(double val) {variables_[ntheta_x]=val;}
    inline void Theta_y(double val) {variables_[ntheta_y]=val;}
    inline void Ksi(double val) {variables_[nksi]=val;}
    inline void ZDirection(double val) {z_direction_=val;}
    
//    inline double P() const {return p0_*(Ksi()+1.0);}
//    inline double Px() const {return P()*Theta_x_angle();}
//    inline double Py() const {return P()*Theta_y_angle();}
//    inline double Pz() const {return z_direction_*TMath::Sqrt(P()*P()-Px()*Px()-Py()*Py());}
    
    inline double Parameter(int n) const {return variables_[n];}
    inline void Parameter(int n, double val) {variables_[n]=val;}
    
    inline void CovarianceMartixElement(int n, int m, double val) {covariance_[n*dimension + m]=val;}
    inline double CovarianceMartixElement(int n, int m) const {return covariance_[n*dimension + m];}
    
    inline void Fitted(int n, bool val) {fitted_[n]=val;}
    inline bool Fitted(int n) const {return fitted_[n];}
    
    inline void Valid(bool val) {valid_=val;}
    inline bool Valid() const {return valid_;}
    
    inline void Chi2(double val) {Chi_2_=val;}
    inline double Chi2() const {return Chi_2_;}
    
    inline void Chi2Norm(double val) {Chi_2_over_n_=val;}
    inline double Chi2Norm() const {return Chi_2_over_n_;}
    
    inline void DegreesOfFreedom(int val) {degrees_of_freedom_=val;}
    inline int DegreesOfFreedom() const {return degrees_of_freedom_;}
    
    int FreeParametersNumber() const;
    
    void AddDebugHit(RPId rp_id, const RP2DHitDebug& deb_hit) {debug_hits_[rp_id]=deb_hit;}
    const debug_hits_map_type& DebugHits() const {return debug_hits_;}
    RPRecoProtMADXVariables GetMADXVariables() const;
    
//    double Momentum_to_t(double px, double py, double pz) const;  //GeV
//    double Momentum_to_ksi(double px, double py, double pz) const;  //GeV
    
    friend std::ostream &operator<<(std::ostream &out, const RPReconstructedProton &prot);
    
  private:
    void ZeroData();
    
    bool valid_;
    double variables_[dimension];
    double z_direction_;  //right: +1, left: -1
    double covariance_[cov_dimension];
    bool fitted_[dimension];
    double Chi_2_;
    double Chi_2_over_n_;
    int degrees_of_freedom_;
    debug_hits_map_type debug_hits_;
    
//    static const double mp0_;
//    static const double E0_;
//    static const double p0_;
};

std::ostream &operator<<(std::ostream &out, const RPReconstructedProton &prot);

#endif
