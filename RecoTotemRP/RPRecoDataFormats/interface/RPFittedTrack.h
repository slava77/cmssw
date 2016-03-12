/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 * 	Hubert Niewiadomski
 *   Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPFittedTrack_h
#define RecoTotemRP_RPRecoDataFormats_RPFittedTrack_h

#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"

#include "TROOT.h"
#include "TVector3.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TMath.h"

class RPDetHitPoint: public RPRecoHit {
public:
	RPDetHitPoint(const RPRecoHit &hit, const TVector3 &space_point_on_det,
			double residual, double pull) :
			RPRecoHit(hit), space_point_on_det_(space_point_on_det), residual_(
					residual), pull_(pull) {
	}
	RPDetHitPoint() :
			RPRecoHit(), residual_(0), pull_(0) {
	}
	virtual ~RPDetHitPoint() {
	}
	inline const TVector3 & HitGlobalCoord() const {
		return space_point_on_det_;
	}
	inline void HitGlobalCoord(const TVector3 & space_point_on_det) {
		space_point_on_det_ = space_point_on_det;
	}
	inline double Residual() const {
		return residual_;
	}
	inline void Residual(double residual) {
		residual_ = residual;
	}
	inline double Pull() const {
		return pull_;
	}
	inline void Pull(double pull) {
		pull_ = pull;
	}
	inline double GetPullNormalization() const {
		return residual_ / pull_;
	}

private:
	TVector3 space_point_on_det_; ///< mm
	double residual_; ///< mm
	double pull_; ///< standardised residual
};

/**
 *\brief A track fit through RP.
 *
 * x = x0+tx*(z-z0) y = ...
 *
 * z0 is defined below
 * x any y refer to the global (x, y) system with the beam at (x = 0, y = 0).
 * Only VALID tracks (IsValid()==true) can be later used for physics reconstruction!
 **/
class RPFittedTrack {
public:
	enum {
		dimension = 4
	}; ///< parameter vector size
	enum {
		covarianceSize = dimension * dimension
	}; ///< covariance matrix size

	RPFittedTrack() : /*track_params_vector_(4),*/
			z0_(0),
			/*par_covariance_matrix_(4,4),*/chiSquared_(0), valid_(false), u_id_(
					0), v_id_(0), sourceTrackCandidateValid(false) {
	}
	RPFittedTrack(double z0, const TVectorD & track_params_vector,
			const TMatrixD &par_covariance_matrix, double chiSquared);
	virtual ~RPFittedTrack() {
	}
	inline int GetHitEntries() const {
		return track_hits_vector_.size();
	}
	inline const RPDetHitPoint & GetHit(int i) const {
		return track_hits_vector_[i];
	}
	inline void AddHit(const RPDetHitPoint &hit) {
		track_hits_vector_.push_back(hit);
	}
	inline double X0() const {
		return track_params_vector_[0];
	}
	inline double X0Sigma() const {
		return TMath::Sqrt(CovarianceMatrixElement(0, 0));
	}
	inline double X0Variance() const {
		return CovarianceMatrixElement(0, 0);
	}
	inline double Y0() const {
		return track_params_vector_[1];
	}
	inline double Y0Sigma() const {
		return TMath::Sqrt(CovarianceMatrixElement(1, 1));
	}
	inline double Y0Variance() const {
		return CovarianceMatrixElement(1, 1);
	}
	inline double Z0() const {
		return z0_;
	}
	inline void Z0(double z0) {
		z0_ = z0;
	}
	inline double GetTx() const {
		return track_params_vector_[2];
	}
	inline double GetTxSigma() const {
		return TMath::Sqrt(CovarianceMatrixElement(2, 2));
	}
	inline double GetTy() const {
		return track_params_vector_[3];
	}
	inline double GetTySigma() const {
		return TMath::Sqrt(CovarianceMatrixElement(3, 3));
	}
	inline TVector3 GetDirectionVector() const {
		TVector3 vect(GetTx(), GetTy(), 1);
		vect.SetMag(1.0);
		return vect;
	}
	TVectorD ParameterVector() const;
	void ParameterVector(const TVectorD & track_params_vector);
	TMatrixD CovarianceMatrix() const;
	void CovarianceMatrix(const TMatrixD &par_covariance_matrix);
	inline double ChiSquared() const {
		return chiSquared_;
	}
	inline void ChiSquared(double & chiSquared) {
		chiSquared_ = chiSquared;
	}
	inline double ChiSquaredOverN() const {
		return chiSquared_ / (track_hits_vector_.size() - 4);
	}

	inline TVector2 GetTrackPoint(double z) const ///< returns x,y vector
			{
		double delta_z = z - z0_;
		return TVector2(
				track_params_vector_[0] + track_params_vector_[2] * delta_z,
				track_params_vector_[1] + track_params_vector_[3] * delta_z);
	}
	inline TVector3 TrackCentrePoint() {
		return TVector3(track_params_vector_[0], track_params_vector_[1], z0_);
	}

	TMatrixD TrackPointInterpolationCovariance(double z) const;
	inline bool IsValid() const {
		return valid_;
	}
	inline void IsValid(bool valid) {
		valid_ = valid;
	}

	inline void Reset() {
		track_hits_vector_.clear();
	}
	inline void SetUVid(unsigned int u, unsigned int v) {
		u_id_ = u;
		v_id_ = v;
	}
	inline unsigned int GetUid() const {
		return u_id_;
	}
	inline unsigned int GetVid() const {
		return v_id_;
	}

	inline bool IsSourceTrackCandidateValid() const{
		return sourceTrackCandidateValid;
	}

	inline void SetSourceTrackCandidateValidity(bool val){
		sourceTrackCandidateValid = val;
	}
	RPTrackCandidate sourceTrackCandidate; ///< track candidate that has been used as source for this fit

private:
	inline const double& CovarianceMatrixElement(int i, int j) const {
		return par_covariance_matrix_[i * dimension + j];
	}
	inline double& CovarianceMatrixElement(int i, int j) {
		return par_covariance_matrix_[i * dimension + j];
	}
	std::vector<RPDetHitPoint> track_hits_vector_;
	//TVectorD track_params_vector_;  //x0, y0, tx, ty; x = x0+tx*(z-z0) ...
	double track_params_vector_[dimension];
	double z0_; ///< filled from TotemRPGeometry::GetRPGlobalTranslation
	//TMatrixD par_covariance_matrix_;  //x0, y0, tx, ty...
	double par_covariance_matrix_[covarianceSize];

	double chiSquared_;
	bool valid_;
	unsigned int u_id_, v_id_; ///<if there are multiple tracks the uv combination id is preserved here
	bool sourceTrackCandidateValid;
};

#endif

