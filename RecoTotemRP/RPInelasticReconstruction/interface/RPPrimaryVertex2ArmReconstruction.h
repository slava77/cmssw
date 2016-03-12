#ifndef RecoTotemRP_RPInelasticReconstruction_RPPrimaryVertex2ArmReconstruction_h
#define RecoTotemRP_RPInelasticReconstruction_RPPrimaryVertex2ArmReconstruction_h


#include "RecoTotemRP/RPInverseParameterization/interface/RPInverse2SidedParameterization.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPair.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPairCollection.h"
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


//Data Formats
#include <iostream>
#include <string>
#include <vector>


class RPPrimaryVertex2ArmReconstruction : public edm::EDProducer
{
  public:
    typedef std::map<RPId, RP2DHit> rec_tracks_collection;
    
    explicit RPPrimaryVertex2ArmReconstruction(const edm::ParameterSet& conf);
    virtual ~RPPrimaryVertex2ArmReconstruction();
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
    bool CollectionContainsBotRP(const rec_tracks_collection& track_coll);
    bool CollectionContainsTopRP(const rec_tracks_collection& track_coll);
    void AddMultipleScatteringVarianceContribution(rec_tracks_collection &coll, 
      double relative_effective_length_x, double relative_effective_length_y, 
      double rp_multiple_scattering_sigma, int no_of_pots);
    void AddInStationMultipleScatteringContribution(rec_tracks_collection &col, 
          double rp_multiple_scattering_sigma);
    bool FindPrimaryVertex(edm::Event& e);

    void Reconstruct(const rec_tracks_collection &rec_col_1, 
      const rec_tracks_collection &rec_col_2, const rec_tracks_collection &rec_col_3, 
      const rec_tracks_collection &rec_col_4, RPInverse2SidedParameterization &inv_par, 
      RPReconstructedProtonPairCollection & rec_prot_pair_col, bool eternal_prim_vert);
    
    const edm::ParameterSet conf_;
    int verbosity_;

    std::auto_ptr<RPInverse2SidedParameterization> inv_param_;
    
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
    bool elastic_scattering_reconstruction_;
    TVector3 primary_vertex_; 
    TVector3 primary_vertex_error_; 
    TVector3 primary_vertex_nom_pos_;
    
    double relative_effective_length_150_220_x_right_;
    double relative_effective_length_150_220_y_right_;
    double relative_effective_length_150_220_x_left_;
    double relative_effective_length_150_220_y_left_;
    double rp_multiple_scattering_sigma_;
    
    TRandom2 rand_;
    RPFitResolution resol_degrad_service_;
    
    station_rp_ids_type station_150_right_ids_, station_150_left_ids_, 
          station_220_right_ids_, station_220_left_ids_;

    station_rp_ids_type left_arm_top_pot_ids_, left_arm_bot_pot_ids_, 
          right_arm_top_pot_ids_, right_arm_bot_pot_ids_;
    
    BeamOpticsParams BOPar_;
};



#endif
