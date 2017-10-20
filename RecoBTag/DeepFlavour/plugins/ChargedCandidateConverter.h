#ifndef RecoSV_DeepFlavour_ChargedCandidateConverter_h
#define RecoSV_DeepFlavour_ChargedCandidateConverter_h

#include "RecoBTag/DeepFlavour/plugins/deep_helpers.h"
#include "RecoBTag/DeepFlavour/plugins/TrackInfoBuilder.h"
#include "DataFormats/BTauReco/interface/ChargedCandidateFeatures.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

namespace btagbtvdeep {

  
  class ChargedCandidateConverter {

    public:

      // conversion map from quality flags used in PV association and miniAOD one
      constexpr static int qualityMap[8]  = {1,0,1,1,4,4,5,6};

      enum qualityFlagsShiftsAndMasks {
            assignmentQualityMask = 0x7, assignmentQualityShift = 0,
            trackHighPurityMask  = 0x8, trackHighPurityShift=3,
            lostInnerHitsMask = 0x30, lostInnerHitsShift=4,
            muonFlagsMask = 0x0600, muonFlagsShift=9
      };

      template <typename CandidateType>
      static void CommonCandidateToFeatures(const CandidateType * c_pf,
                                            const reco::Jet & jet,
                                            const TrackInfoBuilder & track_info,
                                            const float & drminpfcandsv,
                                            ChargedCandidateFeatures & c_pf_features) {

        c_pf_features.ptrel = catch_infs_and_bound(c_pf->pt()/jet.pt(),
                                                          0,-1,0,-1);
        // track_info has to be built before passing as parameter
        c_pf_features.btagPf_trackEtaRel     =catch_infs_and_bound(track_info.getTrackEtaRel(),  0,-5,15);
        c_pf_features.btagPf_trackPtRel      =catch_infs_and_bound(track_info.getTrackPtRel(),   0,-1,4);
        c_pf_features.btagPf_trackPPar       =catch_infs_and_bound(track_info.getTrackPPar(),    0,-1e5,1e5 );
        c_pf_features.btagPf_trackDeltaR     =catch_infs_and_bound(track_info.getTrackDeltaR(),  0,-5,5 );
        c_pf_features.btagPf_trackPParRatio  =catch_infs_and_bound(track_info.getTrackPParRatio(),0,-10,100);
        c_pf_features.btagPf_trackSip3dVal   =catch_infs_and_bound(track_info.getTrackSip3dVal(), 0, -1,1e5 );
        c_pf_features.btagPf_trackSip3dSig   =catch_infs_and_bound(track_info.getTrackSip3dSig(), 0, -1,4e4 );
        c_pf_features.btagPf_trackSip2dVal   =catch_infs_and_bound(track_info.getTrackSip2dVal(), 0, -1,70 );
        c_pf_features.btagPf_trackSip2dSig   =catch_infs_and_bound(track_info.getTrackSip2dSig(), 0, -1,4e4 );
        c_pf_features.btagPf_trackJetDistVal =catch_infs_and_bound(track_info.getTrackJetDistVal(),0,-20,1 );


        c_pf_features.drminsv = catch_infs_and_bound(drminpfcandsv,0,-0.4,0,-0.4);

      }

    
      static void PackedCandidateToFeatures(const pat::PackedCandidate * c_pf,
                                            const pat::Jet & jet,
                                            const TrackInfoBuilder & track_info,
                                            const float drminpfcandsv,
                                            ChargedCandidateFeatures & c_pf_features) {

        CommonCandidateToFeatures(c_pf, jet, track_info, drminpfcandsv, c_pf_features);
    
        c_pf_features.vtx_ass = c_pf->pvAssociationQuality();

        c_pf_features.puppiw = c_pf->puppiWeight();

        const auto & pseudo_track =  c_pf->pseudoTrack();
        c_pf_features.chi2 = catch_infs_and_bound(pseudo_track.normalizedChi2(),300,-1,300);
        // this returns the quality enum not a mask.
        c_pf_features.quality = pseudo_track.qualityMask();

      } 
    
      static void RecoCandidateToFeatures(const reco::PFCandidate * c_pf,
                                          const reco::Jet & jet,
                                          const TrackInfoBuilder & track_info,
                                          const float drminpfcandsv, const float puppiw,
                                          const int pv_ass_quality,
                                          const reco::VertexRef & pv, 
                                          ChargedCandidateFeatures & c_pf_features) {

        CommonCandidateToFeatures(c_pf, jet, track_info, drminpfcandsv, c_pf_features);
    
        c_pf_features.vtx_ass = (float) pat::PackedCandidate::PVAssociationQuality(qualityMap[pv_ass_quality]);
        if (c_pf->trackRef().isNonnull() && 
            pv->trackWeight(c_pf->trackRef()) > 0.5 &&
            pv_ass_quality == 7) {
          c_pf_features.vtx_ass = (float) pat::PackedCandidate::UsedInFitTight;
        }

        c_pf_features.puppiw = puppiw;
    
        const auto & pseudo_track =  (c_pf->bestTrack()) ? *c_pf->bestTrack() : reco::Track();
        c_pf_features.chi2 = catch_infs_and_bound(std::floor(pseudo_track.normalizedChi2()),300,-1,300);
    
        // conditions from PackedCandidate producer
        bool highPurity = c_pf->trackRef().isNonnull() && pseudo_track.quality(reco::Track::highPurity);
        // do same bit operations than in PackedCandidate
        uint16_t qualityFlags = 0;
        qualityFlags = (qualityFlags & ~trackHighPurityMask) | ((highPurity << trackHighPurityShift) & trackHighPurityMask);
        bool isHighPurity = (qualityFlags & trackHighPurityMask)>>trackHighPurityShift;
        // to do as in TrackBase
        uint8_t quality = (1 << reco::TrackBase::loose);
        if (isHighPurity) {
          quality |= (1 << reco::TrackBase::highPurity);
        } 
        c_pf_features.quality = quality; 
    
      } 

  };

  // static data member (avoid undefined ref)
  constexpr int ChargedCandidateConverter::qualityMap[8]; 


}

#endif //RecoSV_DeepFlavour_ChargedCandidateConverter_h
