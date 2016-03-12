#ifndef RecoTotemRP_RPRecoDataFormats_RPReconstructedProtonPair_h
#define RecoTotemRP_RPRecoDataFormats_RPReconstructedProtonPair_h

#include "TNamed.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHitDebug.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "TVector2.h"
#include <map>
#include <ostream>
#include "TMath.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/RPRecoProtMADXVariables.h"


class RPReconstructedProtonPair
{
  //in case of array access to the kinematic variables, the order of reconstructed 
  //x, y, z, theta_x0, theta_y0, ksi0, theta_x1, theta_y1, ksi1 - canonical MAD coordinates
  //[mm, mm, mm, rad, rad, rad, rad, -1 0]
  //energy and momentum and related in GeV
  //functions converting MADX variables to general physics values available 
  //in TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h
  
  public:
    enum { dimension = 9 };
    enum { cov_dimension = dimension*dimension };
    enum { nx=0, ny=1, nz=2, ntheta_x0=3, ntheta_y0=4, nksi0=5, ntheta_x1=6, 
            ntheta_y1=7, nksi1=8 };
    RPReconstructedProtonPair();
    virtual ~RPReconstructedProtonPair() {}
    
    typedef std::map<RPId, RP2DHitDebug> debug_hits_map_type;
    inline double X3D() const {return variables_[nx];}
    inline double Y3D() const {return variables_[ny];}
    inline double Z3D() const {return variables_[nz];}
    
    double X0Left() const;  //to be implemented
    double Y0Left() const;
    double X0Right() const;
    double Y0Right() const;
    
    inline double ThetaXLeft() const {return variables_[ntheta_x0];}
    inline double ThetaYLeft() const {return variables_[ntheta_y0];}
    inline double ThetaXRight() const {return variables_[ntheta_x1];}
    inline double ThetaYRight() const {return variables_[ntheta_y1];}
    
    inline double ThetaXAngleLeft() const {return ThetaXLeft()/(1.0+KsiLeft());}
    inline double ThetaYAngleLeft() const {return ThetaYLeft()/(1.0+KsiLeft());}
    inline double ThetaXAngleRight() const {return ThetaXRight()/(1.0+KsiLeft());}
    inline double ThetaYAngleRight() const {return ThetaYRight()/(1.0+KsiLeft());}
    
    inline double KsiLeft() const {return variables_[nksi0];}
    inline double KsiRight() const {return variables_[nksi1];}
    
//    inline double PhiLeft() const {return TVector2(ThetaXLeft(), ThetaYLeft()).Phi();}
//    inline double PhiRight() const {return TVector2(ThetaXRight(), ThetaYRight()).Phi();}

//    double tLeft() const;
//    double tRight() const;
    
    inline void X3D(double val) {variables_[nx]=val;}
    inline void Y3D(double val) {variables_[ny]=val;}
    inline void Z3D(double val) {variables_[nz]=val;}
    
    inline void ThetaXLeft(double val) {variables_[ntheta_x0]=val;}
    inline void ThetaYLeft(double val) {variables_[ntheta_y0]=val;}
    inline void KsiLeft(double val) {variables_[nksi0]=val;}
    inline void ThetaXRight(double val) {variables_[ntheta_x1]=val;}
    inline void ThetaYRight(double val) {variables_[ntheta_y1]=val;}
    inline void KsiRight(double val) {variables_[nksi1]=val;}
    
    inline double ThetaXLeft() {return variables_[ntheta_x0];}
    inline double ThetaYLeft() {return variables_[ntheta_y0];}
    inline double KsiLeft() {return variables_[nksi0];}
    inline double ThetaXRight() {return variables_[ntheta_x1];}
    inline double ThetaYRight() {return variables_[ntheta_y1];}
    inline double KsiRight() {return variables_[nksi1];}
    
//    inline double PLeft() const {return p0_*(KsiLeft()+1.0);}
//    inline double PxLeft() const {return PLeft()*ThetaXAngleLeft();}
//    inline double PyLeft() const {return PLeft()*ThetaYAngleLeft();}
//    inline double PzLeft() const {return -1.0*TMath::Sqrt(PLeft()*PLeft()-
//          PxLeft()*PxLeft()-PyLeft()*PyLeft());}
    
//    inline double PRight() const {return p0_*(KsiRight()+1.0);}
//    inline double PxRight() const {return PRight()*ThetaXAngleRight();}
//    inline double PyRight() const {return PRight()*ThetaYAngleRight();}
//    inline double PzRight() const {return TMath::Sqrt(PRight()*PRight()-
//          PxRight()*PxRight()-PyRight()*PyRight());}
    
    inline double Parameter(int n) const {return variables_[n];}
    inline void Parameter(int n, double val) {variables_[n]=val;}
    
    inline void CovarianceMartixElement(int n, int m, double val) {covariance_[n*dimension + m]=val;}
    inline double CovarianceMartixElement(int n, int m) const {return covariance_[n*dimension + m];}
    
    inline void Valid(bool val) {valid_=val;}
    inline bool Valid() const {return valid_;}
    
    inline void Chi2(double val) {Chi_2_=val;}
    inline double Chi2() const {return Chi_2_;}
    
    inline void Chi2Norm(double val) {Chi_2_over_n_=val;}
    inline double Chi2Norm() const {return Chi_2_over_n_;}
    
    inline int FreeParametersNumber() const {return dimension;}
    
    inline void DegreesOfFreedom(int val) {degrees_of_freedom_=val;}
    inline int DegreesOfFreedom() const {return degrees_of_freedom_;}
    
    void AddDebugHit(RPId rp_id, const RP2DHitDebug& deb_hit) {debug_hits_[rp_id]=deb_hit;}
    const debug_hits_map_type& DebugHits() const {return debug_hits_;}
    
//    double Momentum_to_t(double px, double py, double pz) const;  //GeV
//    double Momentum_to_ksi(double px, double py, double pz) const;  //GeV
    
    static inline double Vertex3DTo2DLeft_X(double mad_thetax, double ksi, 
        double vx, double vz) {return vx + mad_thetax/(1.0+ksi)*vz;}
    static inline double Vertex3DTo2DLeft_Y(double mad_thetay, double ksi, 
        double vy, double vz) {return vy + mad_thetay/(1.0+ksi)*vz;}
    static inline double Vertex3DTo2DRight_X(double mad_thetax, double ksi, 
        double vx, double vz) {return vx - mad_thetax/(1.0+ksi)*vz;}
    static inline double Vertex3DTo2DRight_Y(double mad_thetay, double ksi, 
        double vy, double vz) {return vy - mad_thetay/(1.0+ksi)*vz;}
        
    static void FillMADTransportNtupleLeft(const double *vect, double par[5]);
    static void FillMADTransportNtupleRight(const double *vect, double par[5]);
    
    inline static void FillMADTransportNtupleLeft(const std::vector<double> &vect, 
          double par[5]) {FillMADTransportNtupleLeft(&vect[0], par);}
    inline static void FillMADTransportNtupleRight(const std::vector<double> &vect, 
          double par[5]) {FillMADTransportNtupleRight(&vect[0], par);}
          
    inline void FillMADTransportNtupleLeft(double par[5]) const 
          {FillMADTransportNtupleLeft(variables_, par);}
    inline void FillMADTransportNtupleRight(double par[5]) const 
          {FillMADTransportNtupleRight(variables_, par);}
    RPRecoProtMADXVariables GetMADXVariablesRight() const;
    RPRecoProtMADXVariables GetMADXVariablesLeft() const;

    static void Fill5DimReconstructionVectorLeft(const std::vector<double> &vect, std::vector<double> &par);
    static void Fill5DimReconstructionVectorRight(const std::vector<double> &vect, std::vector<double> &par);
    
    friend std::ostream &operator<<(std::ostream &out, const RPReconstructedProtonPair &prot);
    
  private:
    void ZeroData();
    
    bool valid_;
    double variables_[dimension];
    double covariance_[cov_dimension];
    double Chi_2_;
    double Chi_2_over_n_;
    int degrees_of_freedom_;
    debug_hits_map_type debug_hits_;
    
//    static const double mp0_;
//    static const double E0_;
//    static const double p0_;
};

std::ostream &operator<<(std::ostream &out, const RPReconstructedProtonPair &prot);

#endif
