#ifndef RecoSV_DeepFlavour_ChargedCandidateConverter_h
#define RecoSV_DeepFlavour_ChargedCandidateConverter_h

#include "RecoBTag/DeepFlavour/interface/deep_helpers.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

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
    
      template <typename CandidateType, typename JetType,
                typename TrackInfoBuilderType,
                typename ChargedCandidateFeaturesType>
      static void PackedCandidateToFeatures(const CandidateType * c_pf,
                                    const JetType & jet,
                                    const TrackInfoBuilderType & track_info,
                                    const float & drminpfcandsv,
                                    ChargedCandidateFeaturesType & c_pf_features) {
    
        // track_info has to be built before passing as parameter

        // could be inlined
        float etasign = 1.;
        if (jet.eta()<0) etasign =-1.;

        c_pf_features.pt = c_pf->pt();
        c_pf_features.eta = c_pf->eta();
        c_pf_features.phi = c_pf->phi();
        c_pf_features.ptrel = catch_infs_and_bound(c_pf->pt()/jet.pt(),
                                                         0,-1,0,-1);
        c_pf_features.erel = catch_infs_and_bound(c_pf->energy()/jet.energy(),
                                                        0,-1,0,-1);
        c_pf_features.phirel = catch_infs_and_bound(fabs(reco::deltaPhi(c_pf->phi(),jet.phi())),
                                                          0,-2,0,-0.5);
        c_pf_features.etarel = catch_infs_and_bound(fabs(c_pf->eta()-jet.eta()),
                                                          0,-2,0,-0.5);
        c_pf_features.deltaR = catch_infs_and_bound(reco::deltaR(*c_pf,jet),
                                                         0,-0.6,0,-0.6);
        c_pf_features.dxy = catch_infs_and_bound(fabs(c_pf->dxy()),0,-50,50);


        c_pf_features.dxyerrinv = catch_infs_and_bound(1/c_pf->dxyError(),
                                                             0,-1, 10000.);

        c_pf_features.dxysig = catch_infs_and_bound(fabs(c_pf->dxy()/c_pf->dxyError()),
                                                          0.,-2000,2000);


        c_pf_features.dz = c_pf->dz();
        c_pf_features.VTX_ass = c_pf->pvAssociationQuality();
        c_pf_features.fromPV = c_pf->fromPV();
        c_pf_features.vertexChi2=c_pf->vertexChi2();
        c_pf_features.vertexNdof=c_pf->vertexNdof();
        c_pf_features.vertexNormalizedChi2=c_pf->vertexNormalizedChi2();
        c_pf_features.vertex_rho=catch_infs_and_bound(c_pf->vertex().rho(),
                                                            0,-1,50);
        c_pf_features.vertex_phirel=reco::deltaPhi(c_pf->vertex().phi(),jet.phi());
        c_pf_features.vertex_etarel=etasign*(c_pf->vertex().eta()-jet.eta());
        c_pf_features.vertexRef_mass=c_pf->vertexRef()->p4().M();

        c_pf_features.puppiw = c_pf->puppiWeight();

        const auto & pseudo_track =  c_pf->pseudoTrack();
        auto cov_matrix = pseudo_track.covariance();

        c_pf_features.dptdpt =    catch_infs_and_bound(cov_matrix[0][0],0,-1,1);
        c_pf_features.detadeta=   catch_infs_and_bound(cov_matrix[1][1],0,-1,0.01);
        c_pf_features.dphidphi=   catch_infs_and_bound(cov_matrix[2][2],0,-1,0.1);

        /*
         * what makes the most sense here if a track is used in the fit... cerntainly no btag
         * for now leave it a t zero
         * infs and nans are set to poor quality
         */
        //zero if pvAssociationQuality ==7 ?
        c_pf_features.dxydxy =    catch_infs_and_bound(cov_matrix[3][3],7.,-1,7);
        c_pf_features.dzdz =      catch_infs_and_bound(cov_matrix[4][4],6.5,-1,6.5); 
        c_pf_features.dxydz =     catch_infs_and_bound(cov_matrix[3][4],6.,-6,6); 
        c_pf_features.dphidxy =   catch_infs(cov_matrix[2][3],-0.03);
        c_pf_features.dlambdadz=  catch_infs(cov_matrix[1][4],-0.03);


        c_pf_features.btagPf_trackMomentum   =catch_infs_and_bound(track_info.getTrackMomentum(),0,0 ,1000);
        c_pf_features.btagPf_trackEta        =catch_infs_and_bound(track_info.getTrackEta()   ,  0,-5,5);
        c_pf_features.btagPf_trackEtaRel     =catch_infs_and_bound(track_info.getTrackEtaRel(),  0,-5,15);
        c_pf_features.btagPf_trackPtRel      =catch_infs_and_bound(track_info.getTrackPtRel(),   0,-1,4);
        c_pf_features.btagPf_trackPPar       =catch_infs_and_bound(track_info.getTrackPPar(),    0,-1e5,1e5 );
        c_pf_features.btagPf_trackDeltaR     =catch_infs_and_bound(track_info.getTrackDeltaR(),  0,-5,5 );
        c_pf_features.btagPf_trackPtRatio    =catch_infs_and_bound(track_info.getTrackPtRatio(), 0,-1,10 );
        c_pf_features.btagPf_trackPParRatio  =catch_infs_and_bound(track_info.getTrackPParRatio(),0,-10,100);
        c_pf_features.btagPf_trackSip3dVal   =catch_infs_and_bound(track_info.getTrackSip3dVal(), 0, -1,1e5 );
        c_pf_features.btagPf_trackSip3dSig   =catch_infs_and_bound(track_info.getTrackSip3dSig(), 0, -1,4e4 );
        c_pf_features.btagPf_trackSip2dVal   =catch_infs_and_bound(track_info.getTrackSip2dVal(), 0, -1,70 );
        c_pf_features.btagPf_trackSip2dSig   =catch_infs_and_bound(track_info.getTrackSip2dSig(), 0, -1,4e4 );
        c_pf_features.btagPf_trackDecayLen   =0;
        c_pf_features.btagPf_trackJetDistVal =catch_infs_and_bound(track_info.getTrackJetDistVal(),0,-20,1 );
        c_pf_features.btagPf_trackJetDistSig =catch_infs_and_bound(track_info.getTrackJetDistSig(),0,-1,1e5 );

        // TO DO: we can do better than that by including reco::muon informations
        c_pf_features.isMu = 0;
        if(abs(c_pf->pdgId())==13) {
            c_pf_features.isMu = 1;
        }
        // TO DO: we can do better than that by including reco::electron informations
        c_pf_features.isEl = 0;
        if(abs(c_pf->pdgId())==11) {
            c_pf_features.isEl = 1;

        }

        c_pf_features.charge = c_pf->charge();
        c_pf_features.lostInnerHits = catch_infs(c_pf->lostInnerHits(),2);
        c_pf_features.chi2 = catch_infs_and_bound(pseudo_track.normalizedChi2(),300,-1,300);
        //for some reason this returns the quality enum not a mask.
        c_pf_features.quality = pseudo_track.qualityMask();

        c_pf_features.drminsv = catch_infs_and_bound(drminpfcandsv,0,-0.4,0,-0.4);
    
      } 
    
      template <typename CandidateType, typename JetType,
                typename TrackInfoBuilderType,
                typename ChargedCandidateFeaturesType,
                typename VertexType>
      static void RecoCandidateToFeatures(const CandidateType * c_pf,
                                           const JetType & jet,
                                           const TrackInfoBuilderType & track_info,
                                           const float & drminpfcandsv, const float & puppiw,
                                           const int & pv_ass_quality,
                                           const VertexType & pv, 
                                           ChargedCandidateFeaturesType & c_pf_features) {
    
        // track_info has to be built before passing as parameter
    
        // could be inlined
        float etasign = 1.;
        if (jet.eta()<0) etasign =-1.;
    
        c_pf_features.pt = c_pf->pt();
        c_pf_features.eta = c_pf->eta();
        c_pf_features.phi = c_pf->phi();
        c_pf_features.ptrel = catch_infs_and_bound(c_pf->pt()/jet.pt(),
                                                         0,-1,0,-1);
        c_pf_features.erel = catch_infs_and_bound(c_pf->energy()/jet.energy(),
                                                        0,-1,0,-1);
        c_pf_features.phirel = catch_infs_and_bound(fabs(reco::deltaPhi(c_pf->phi(),jet.phi())),
                                                          0,-2,0,-0.5);
        c_pf_features.etarel = catch_infs_and_bound(fabs(c_pf->eta()-jet.eta()),
                                                          0,-2,0,-0.5);
        c_pf_features.deltaR = catch_infs_and_bound(reco::deltaR(*c_pf,jet),
                                                         0,-0.6,0,-0.6);
        //c_pf_features.dxy = catch_infs_and_bound(fabs(c_pf->dxy()),0,-50,50);
    
    
        //c_pf_features.dxyerrinv = catch_infs_and_bound(1/c_pf->dxyError(),
        //                                                     0,-1, 10000.);
    
        //c_pf_features.dxysig = catch_infs_and_bound(fabs(c_pf->dxy()/c_pf->dxyError()),
        //                                                  0.,-2000,2000);
    
    
        //c_pf_features.dz = c_pf->dz();
        c_pf_features.VTX_ass = (float) pat::PackedCandidate::PVAssociationQuality(qualityMap[pv_ass_quality]);
        if (c_pf->trackRef().isNonnull() && 
            pv->trackWeight(c_pf->trackRef()) > 0.5 &&
            pv_ass_quality == 7) {
          c_pf_features.VTX_ass = (float) pat::PackedCandidate::UsedInFitTight;
        }
        //c_pf_features.fromPV = c_pf->fromPV();
        c_pf_features.vertexChi2=c_pf->vertexChi2();
        c_pf_features.vertexNdof=c_pf->vertexNdof();
        c_pf_features.vertexNormalizedChi2=c_pf->vertexNormalizedChi2();
        c_pf_features.vertex_rho=catch_infs_and_bound(c_pf->vertex().rho(),
                                                            0,-1,50);
        c_pf_features.vertex_phirel=reco::deltaPhi(c_pf->vertex().phi(),jet.phi());
        c_pf_features.vertex_etarel=etasign*(c_pf->vertex().eta()-jet.eta());
        //c_pf_features.vertexRef_mass=c_pf->vertexRef()->p4().M();
    
        c_pf_features.puppiw = puppiw;
    
        const auto & pseudo_track =  (c_pf->bestTrack()) ? *c_pf->bestTrack() : reco::Track();
        auto cov_matrix = pseudo_track.covariance();
    
        c_pf_features.dptdpt =    catch_infs_and_bound(cov_matrix[0][0],0,-1,1);
        c_pf_features.detadeta=   catch_infs_and_bound(cov_matrix[1][1],0,-1,0.01);
        c_pf_features.dphidphi=   catch_infs_and_bound(cov_matrix[2][2],0,-1,0.1);
    
        /*
         * what makes the most sense here if a track is used in the fit... cerntainly no btag
         * for now leave it a t zero
         * infs and nans are set to poor quality
         */
        //zero if pvAssociationQuality ==7 ?
        c_pf_features.dxydxy =    catch_infs_and_bound(cov_matrix[3][3],7.,-1,7);
        c_pf_features.dzdz =      catch_infs_and_bound(cov_matrix[4][4],6.5,-1,6.5); 
        c_pf_features.dxydz =     catch_infs_and_bound(cov_matrix[3][4],6.,-6,6); 
        c_pf_features.dphidxy =   catch_infs(cov_matrix[2][3],-0.03);
        c_pf_features.dlambdadz=  catch_infs(cov_matrix[1][4],-0.03);
    
    
        c_pf_features.btagPf_trackMomentum   =catch_infs_and_bound(track_info.getTrackMomentum(),0,0 ,1000);
        c_pf_features.btagPf_trackEta        =catch_infs_and_bound(track_info.getTrackEta()   ,  0,-5,5);
        c_pf_features.btagPf_trackEtaRel     =catch_infs_and_bound(track_info.getTrackEtaRel(),  0,-5,15);
        c_pf_features.btagPf_trackPtRel      =catch_infs_and_bound(track_info.getTrackPtRel(),   0,-1,4);
        c_pf_features.btagPf_trackPPar       =catch_infs_and_bound(track_info.getTrackPPar(),    0,-1e5,1e5 );
        c_pf_features.btagPf_trackDeltaR     =catch_infs_and_bound(track_info.getTrackDeltaR(),  0,-5,5 );
        c_pf_features.btagPf_trackPtRatio    =catch_infs_and_bound(track_info.getTrackPtRatio(), 0,-1,10 );
        c_pf_features.btagPf_trackPParRatio  =catch_infs_and_bound(track_info.getTrackPParRatio(),0,-10,100);
        c_pf_features.btagPf_trackSip3dVal   =catch_infs_and_bound(track_info.getTrackSip3dVal(), 0, -1,1e5 );
        c_pf_features.btagPf_trackSip3dSig   =catch_infs_and_bound(track_info.getTrackSip3dSig(), 0, -1,4e4 );
        c_pf_features.btagPf_trackSip2dVal   =catch_infs_and_bound(track_info.getTrackSip2dVal(), 0, -1,70 );
        c_pf_features.btagPf_trackSip2dSig   =catch_infs_and_bound(track_info.getTrackSip2dSig(), 0, -1,4e4 );
        c_pf_features.btagPf_trackDecayLen   =0;
        c_pf_features.btagPf_trackJetDistVal =catch_infs_and_bound(track_info.getTrackJetDistVal(),0,-20,1 );
        c_pf_features.btagPf_trackJetDistSig =catch_infs_and_bound(track_info.getTrackJetDistSig(),0,-1,1e5 );
    
        // TO DO: we can do better than that by including reco::muon informations
        c_pf_features.isMu = 0;
        if(abs(c_pf->pdgId())==13) {
            c_pf_features.isMu = 1;
        }
        // TO DO: we can do better than that by including reco::electron informations
        c_pf_features.isEl = 0;
        if(abs(c_pf->pdgId())==11) {
            c_pf_features.isEl = 1;
    
        }
    
        c_pf_features.charge = c_pf->charge();
        // c_pf_features.lostInnerHits = catch_infs(c_pf->lostInnerHits(),2);
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
        c_pf_features.drminsv = catch_infs_and_bound(drminpfcandsv,0,-0.4,0,-0.4);
    
      } 

  };

  // static data member (avoid undefined ref)
  constexpr int ChargedCandidateConverter::qualityMap[8]; 


}

#endif //RecoSV_DeepFlavour_ChargedCandidateConverter_h
