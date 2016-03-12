#ifndef RecoTotemRP_RPInelasticReconstruction_RPPrimaryVertexInelasticReconstruction_RunI_h
#define RecoTotemRP_RPInelasticReconstruction_RPPrimaryVertexInelasticReconstruction_RunI_h


#include "RecoTotemRP/RPInverseParameterization/interface/RPInverseParameterization.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrackCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include "TVector3.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "TRandom2.h"
#include "RecoTotemRP/RPRomanPotResolutionService/interface/RPFitResolution.h"
#include "TotemCondFormats/BeamOpticsParamsObjects/interface/BeamOpticsParams.h"
#include "TotemCondFormats/DataRecord/interface/BeamOpticsParamsRcd.h"
#include <memory>
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "HepMC/GenEvent.h"


//Data Formats
#include <iostream>
#include <string>
#include <vector>
#include <map>

/**
 \brief Inelastic proton reconstruction for the old RP configuration (150 + 220m stations).
**/
class RPPrimaryVertexInelasticReconstruction_RunI : public edm::EDProducer
{
  public:
    typedef std::map<RPId, RP2DHit> rec_tracks_collection;
    
    explicit RPPrimaryVertexInelasticReconstruction_RunI(const edm::ParameterSet& conf);
    virtual ~RPPrimaryVertexInelasticReconstruction_RunI();
    virtual void beginRun(edm::Run const&, edm::EventSetup const&);
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
    void InitInverseParametrizationFitter();
  
  private:
    typedef std::vector<int> station_rp_ids_type;

    edm::InputTag rpFittedTrackCollectionLabel;
    edm::EDGetTokenT<RPFittedTrackCollection> rpFittedTrackCollectionToken;
    int SelectHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll, const station_rp_ids_type& st_ids);
    int Select220RightHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll);
    int Select220LeftHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll);
    int Select150RightHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll);
    int Select150LeftHits(const RPFittedTrackCollection &tracks, rec_tracks_collection & coll);
    void AddInStationMultipleScatteringContribution(rec_tracks_collection &col, 
          double rp_multiple_scattering_sigma);
    bool FindPrimaryVertex(edm::Event& e);

    void Reconstruct(const rec_tracks_collection &rec_col_1, 
      const rec_tracks_collection &rec_col_2, RPInverseParameterization &inv_par, 
      RPReconstructedProtonCollection & rec_prot_col, double zdirection, bool eternal_prim_vert);
    
    const edm::ParameterSet conf_;
    int verbosity_;
    std::auto_ptr<RPInverseParameterization> inv_param_right_;
    std::auto_ptr<RPInverseParameterization> inv_param_left_;
    
    std::string param_file_name_220_right_;
    std::string param_file_name_220_left_;
    std::string param_file_name_150_right_;
    std::string param_file_name_150_left_;
    std::string param_prefix_220_right_;
    std::string param_prefix_220_left_;
    std::string param_prefix_150_right_;
    std::string param_prefix_150_left_;
    std::string right_beam_postfix_;
    std::string left_beam_postfix_;

    edm::InputTag HepMCProductLabel;
    edm::EDGetTokenT<edm::HepMCProduct> HepMCProductToken;
    
    bool station_150_mandatory_in_reconstruction_;
    bool station_220_mandatory_in_reconstruction_;
    bool external_primary_vertex_;
    bool set_primary_vertex_to_zero_;
    bool any_station_in_reconstruction_;
    TVector3 primary_vertex_; 
    TVector3 primary_vertex_nom_pos_;
    TVector3 primary_vertex_error_; 
    
    double relative_effective_length_150_220_x_right_;
    double relative_effective_length_150_220_y_right_;
    double relative_effective_length_150_220_x_left_;
    double relative_effective_length_150_220_y_left_;
    double rp_multiple_scattering_sigma_;
    
    TRandom2 rand_;
    RPFitResolution resol_degrad_service_;
    
    station_rp_ids_type station_150_right_ids_, station_150_left_ids_, 
          station_220_right_ids_, station_220_left_ids_;
    BeamOpticsParams BOPar_;

    typedef std::map<int, const HepMC::GenParticle*> primary_prot_map_type;
//    primary_prot_map_type primary_protons_;
};



#endif
