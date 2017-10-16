
#include "DataFormats/DeepFormats/interface/JetFeatures.h"
#include "DataFormats/DeepFormats/interface/SecondaryVertexFeatures.h"
#include "DataFormats/DeepFormats/interface/ShallowTagInfoFeatures.h"
#include "DataFormats/DeepFormats/interface/NeutralCandidateFeatures.h"
#include "DataFormats/DeepFormats/interface/ChargedCandidateFeatures.h"
#include "DataFormats/DeepFormats/interface/DeepFlavourFeatures.h"
#include "DataFormats/DeepFormats/interface/DeepFlavourTagInfo.h"

namespace DataFormats_DeepFormats {

  struct dictionary {
    btagbtvdeep::JetFeatures jet_features;
    btagbtvdeep::SecondaryVertexFeatures secondary_vertex_features;
    btagbtvdeep::ShallowTagInfoFeatures shallow_tag_info_features;
    btagbtvdeep::NeutralCandidateFeatures neutral_candidate_features;
    btagbtvdeep::ChargedCandidateFeatures charged_candidate_features;
    btagbtvdeep::DeepFlavourFeatures deep_flavour_features;

    reco::DeepFlavourTagInfo deep_flavour_tag_info;
    reco::DeepFlavourTagInfoCollection deep_flavour_tag_info_collection;
    reco::DeepFlavourTagInfoRef deep_flavour_tag_info_collection_ref;
    reco::DeepFlavourTagInfoFwdRef deep_flavour_tag_info_collection_fwd_ref;
    reco::DeepFlavourTagInfoRefProd deep_flavour_tag_info_collection_ref_prod;
    reco::DeepFlavourTagInfoRefVector deep_flavour_tag_info_collection_ref_vector;
    edm::Wrapper<reco::DeepFlavourTagInfoCollection> deep_flavour_tag_info_collection_edm_wrapper;


  };
}

