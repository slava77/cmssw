#include "RecoTauTag/RecoTau/interface/TauDiscriminationProducerBase.h"

/* 
 * class PFRecoTauDiscriminationByLeadingObjectPtCut
 * created : October 08 2008,
 * revised : Wed Aug 19 17:13:04 PDT 2009
 * Authors : Simone Gennai (SNS), Evan Friis (UC Davis)
 */

using namespace reco;

class PFRecoTauDiscriminationByLeadingObjectPtCut : public PFTauDiscriminationProducerBase  {
   public:
      explicit PFRecoTauDiscriminationByLeadingObjectPtCut(const edm::ParameterSet& iConfig):PFTauDiscriminationProducerBase(iConfig){   
         chargedOnly_     = iConfig.getParameter<bool>("UseOnlyChargedHadrons");
         minPtLeadObject_ = iConfig.getParameter<double>("MinPtLeadingObject");
      }
      ~PFRecoTauDiscriminationByLeadingObjectPtCut() override{} 
      double discriminate(const PFTauRef& pfTau) const override;
   private:
      bool chargedOnly_;
      double minPtLeadObject_;
};

double PFRecoTauDiscriminationByLeadingObjectPtCut::discriminate(const PFTauRef& thePFTauRef) const
{
   double leadObjectPt = -1.;
   if( chargedOnly_ )
   {
      // consider only charged hadrons.  note that the leadChargedHadrCand is the highest pt
      // charged signal cone object above the quality cut level (typically 0.5 GeV).  
      if( thePFTauRef->leadChargedHadrCand().isNonnull() )
      {
         leadObjectPt = thePFTauRef->leadChargedHadrCand()->pt();
      }
   } 
   else
   {
      // If using the 'leading pion' option, require that:
      //   1) at least one charged hadron exists above threshold (thePFTauRef->leadChargedHadrCand().isNonnull())
      //   2) the lead PFCand exists.  In the case that the highest pt charged hadron is above the PFRecoTauProducer threshold 
      //      (typically 5 GeV), the leadCand and the leadChargedHadrCand are the same object.  If the leadChargedHadrCand
      //      is below 5GeV, but there exists a neutral PF particle > 5 GeV, it is set to be the leadCand
      if( thePFTauRef->leadCand().isNonnull() && thePFTauRef->leadChargedHadrCand().isNonnull() )
      {
         leadObjectPt = thePFTauRef->leadCand()->pt();
      }
   }

   return ( leadObjectPt > minPtLeadObject_ ? 1. : 0. );
}

DEFINE_FWK_MODULE(PFRecoTauDiscriminationByLeadingObjectPtCut);

/*
void PFRecoTauDiscriminationByLeadingPionPtCut::produce(edm::Event& iEvent,const edm::EventSetup& iEventSetup){
   edm::Handle<PFTauCollection> thePFTauCollection;
   iEvent.getByLabel(PFTauProducer_,thePFTauCollection);


   auto thePFTauDiscriminatorByLeadingPionPtCut = std::make_unique<PFTauDiscriminator(PFTauRefProd>(thePFTauCollection));

   //loop over the PFTau candidates
   for(size_t iPFTau=0;iPFTau<thePFTauCollection->size();++iPFTau) {
      PFTauRef thePFTauRef(thePFTauCollection,iPFTau);
      PFTau thePFTau=*thePFTauRef;
      double theleadTrackPtCutDiscriminator = 0.;   
      // fill the AssociationVector object
      if (!thePFTau.leadCand() || !thePFTau.leadChargedHadrCand()) 
      {
         theleadTrackPtCutDiscriminator=0.;
      }
      else if(thePFTau.leadCand()->pt() > minPtLeadTrack_) theleadTrackPtCutDiscriminator=1.;

      thePFTauDiscriminatorByLeadingPionPtCut->setValue(iPFTau,theleadTrackPtCutDiscriminator);
   }

   iEvent.put(std::move(thePFTauDiscriminatorByLeadingPionPtCut));

}
   
*/
