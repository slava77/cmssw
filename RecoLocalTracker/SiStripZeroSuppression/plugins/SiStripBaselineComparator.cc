// -*- C++ -*-
//
// Package:    SiStripBaselineAnalyzer
// Class:      SiStripBaselineAnalyzer
// 
/**\class SiStripBaselineAnalyzer SiStripBaselineAnalyzer.cc Validation/SiStripAnalyzer/src/SiStripBaselineAnalyzer.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Ivan Amos Cali
//         Created:  Mon Jul 28 14:10:52 CEST 2008
//
//
 

// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"

#include "DataFormats/SiStripDigi/interface/SiStripProcessedRawDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripClusterCollection.h"

#include "CondFormats/SiStripObjects/interface/SiStripPedestals.h"
#include "CondFormats/DataRecord/interface/SiStripPedestalsRcd.h"

#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripPedestalsSubtractor.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripCommonModeNoiseSubtractor.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripRawProcessingFactory.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "DataFormats/Provenance/interface/RunLumiEventNumber.h"

//ROOT inclusion
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "THStack.h"
#include <vector>

//
// class decleration
//

class SiStripBaselineComparator : public edm::EDAnalyzer {
   public:
      explicit SiStripBaselineComparator(const edm::ParameterSet&);
      ~SiStripBaselineComparator() override;


   private:
      void beginJob() override ;
      void analyze(const edm::Event&, const edm::EventSetup&) override;
      void endJob() override ;
      
	  edm::InputTag srcClusters_;
	  edm::InputTag srcClusters2_;
      
          edm::Service<TFileService> fs_;
 
 
	  TH1F* h1_nOldClusters_;
	  TH1F* h1_nMatchedClusters_;
          TH1F* h1_nSplitClusters_;
          TProfile* h1_matchingMult_;
          TProfile* h1_matchedWidth_;
          TProfile* h1_matchedCharges_;
};


SiStripBaselineComparator::SiStripBaselineComparator(const edm::ParameterSet& conf){
   
  srcClusters_ =  conf.getParameter<edm::InputTag>( "srcClusters" );
  srcClusters2_ =  conf.getParameter<edm::InputTag>( "srcClusters2" );
  
  h1_nOldClusters_ = fs_->make<TH1F>("nOldClusters","nOldClusters;ClusterSize", 128, 0, 128);
  h1_nMatchedClusters_ = fs_->make<TH1F>("nMatchedClusters","nMatchedClusters;ClusterSize", 128, 0, 128);
  h1_nSplitClusters_ = fs_->make<TH1F>("nSplitClusters","nMatchedClusters;ClusterSize; n Split Clusters", 128, 0, 128);
  h1_matchingMult_ = fs_->make<TProfile>("matchingMult","matchingMult;ClusterSize; average number of clusters if split", 128, 0, 128);
  h1_matchedWidth_ = fs_->make<TProfile>("matchedWidth","matchedWidth;ClusterSize;average #Delta Width", 128, 0, 128);
  h1_matchedCharges_ = fs_->make<TProfile>("matchedCharges","matchedCharges;ClusterSize;cluster Charge ratio", 128, 0, 128);
}


SiStripBaselineComparator::~SiStripBaselineComparator()
{
}

void
SiStripBaselineComparator::analyze(const edm::Event& e, const edm::EventSetup& es)
{
   edm::Handle<edmNew::DetSetVector<SiStripCluster> > clusters;
   e.getByLabel(srcClusters_,clusters);
   edm::Handle<edmNew::DetSetVector<SiStripCluster> > clusters2;
   e.getByLabel(srcClusters2_,clusters2);

   edmNew::DetSetVector<SiStripCluster>::const_iterator itClusters = clusters->begin();
   for ( ; itClusters != clusters->end(); ++itClusters ){
     for ( edmNew::DetSet<SiStripCluster>::const_iterator clus = itClusters->begin(); clus != itClusters->end(); ++clus){
       h1_nOldClusters_->Fill(clus->amplitudes().size(),1);
       int nMatched = 0; 
       int charge1 = 0;
       for( auto itAmpl = clus->amplitudes().begin(); itAmpl != clus->amplitudes().end(); ++itAmpl) charge1 += *itAmpl;
       std::vector< int > matchedWidths;      
       std::vector< int > matchedCharges;      

       //scan other set of clusters
       edmNew::DetSetVector<SiStripCluster>::const_iterator itClusters2 = clusters2->begin();
       for ( ; itClusters2 != clusters2->end(); ++itClusters2 ){
         if(itClusters->id() != itClusters2->id()) continue;
         for ( edmNew::DetSet<SiStripCluster>::const_iterator clus2 = itClusters2->begin(); clus2 != itClusters2->end(); ++clus2){
           int charge2 = 0;
           for( auto itAmpl = clus2->amplitudes().begin(); itAmpl != clus2->amplitudes().end(); ++itAmpl) charge2 += *itAmpl;
           int strip=clus->firstStrip();
	   for( auto itAmpl = clus->amplitudes().begin(); itAmpl != clus->amplitudes().end(); ++itAmpl){
             if(clus2->firstStrip() == strip){
               if(nMatched>0){
                 if(nMatched==1) h1_nSplitClusters_->Fill(clus->amplitudes().size(),1);  
                 matchedWidths.push_back(clus2->amplitudes().size());
                 matchedCharges.push_back(charge2);
                 nMatched++;
               }
               if(nMatched==0){
                 matchedWidths.push_back(clus2->amplitudes().size());
                 matchedCharges.push_back(charge2);
                 nMatched++;
                 h1_nMatchedClusters_->Fill(clus->amplitudes().size(),1);  
               }
               break;
             }
	     ++strip;
           }
         }
       }
       for(int i = 0; i<nMatched; i++){
         if(matchedWidths.at(i)-clus->amplitudes().size()<1000) h1_matchedWidth_->Fill(clus->amplitudes().size(),matchedWidths.at(i)-clus->amplitudes().size());
         if(charge1 != 0 && matchedCharges.at(i)/(float)charge1<10000000) h1_matchedCharges_->Fill(clus->amplitudes().size(),matchedCharges.at(i)/(float)charge1);
       }
       if(nMatched>1) h1_matchingMult_->Fill(clus->amplitudes().size(),nMatched);      
     }
   }

      //clusters	  
      /*    int nclust = 0;
	  edmNew::DetSetVector<SiStripCluster>::const_iterator itClusters = clusters->begin();
	  for ( ; itClusters != clusters->end(); ++itClusters ){
		for ( edmNew::DetSet<SiStripCluster>::const_iterator clus =	itClusters->begin(); clus != itClusters->end(); ++clus){
		  if(itClusters->id() == detId){
		    int firststrip = clus->firstStrip();
	            //std::cout << "Found cluster in detId " << detId << " " << firststrip << " " << clus->amplitudes().size() << " -----------------------------------------------" << std::endl;		
     		    strip=0;
		    for( auto itAmpl = clus->amplitudes().begin(); itAmpl != clus->amplitudes().end(); ++itAmpl){
		      h1Clusters_->Fill(firststrip+strip, *itAmpl);
		      ++strip;
		    }
		  }
                  
                  //cluster plots from here on
                   
                  if(ClusterDists == false){
		    nclust++;

     		    int strip2=0;
                    double charge = 0;
                    double mean = 0;
                    double sigma = 0;
		    for( auto itAmpl = clus->amplitudes().begin(); itAmpl != clus->amplitudes().end(); ++itAmpl){
                      charge += *itAmpl;
		      ++strip2;
                      mean += strip2*(*itAmpl);
		      sigma += strip2*strip2*(*itAmpl);
		    }
                    h1ClusterCharge_->Fill(charge);
                    h1ClusterWidth_->Fill(strip2);
                    mean = mean/charge;
                    h1ClusterMean_->Fill(mean);
                    sigma = TMath::Power((sigma/charge-mean*mean),0.5);
                    h1ClusterSigma_->Fill(sigma);
            }              
	 }
      }*/
}


// ------------ method called once each job just before starting event loop  ------------
void SiStripBaselineComparator::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SiStripBaselineComparator::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiStripBaselineComparator);

