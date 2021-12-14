// system include files
#include <memory>

// user include files
#include "TROOT.h"
#include "Math/Vavilov.h"
#include "Math/VavilovAccurate.h"
#include "Math/SpecFuncMathCore.h"
#include "Math/SpecFuncMathMore.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include <FWCore/Framework/interface/EventSetup.h>
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/PatCandidates/interface/IsolatedTrack.h"

#include "DataFormats/TrackReco/interface/SiPixelTrackProbQXY.h"
#include "DataFormats/TrackReco/interface/DeDxHitInfo.h"

#include "RecoLocalTracker/SiPixelRecHits/src/PixelCPEBase.cc"
#include "RecoLocalTracker/SiPixelRecHits/interface/PixelCPEBase.h"
#include "RecoLocalTracker/SiPixelRecHits/src/SiPixelTemplateReco.cc"
#include "CalibTracker/Records/interface/SiPixelTemplateDBObjectESProducerRcd.h"

#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "RecoBTag/FeatureTools/interface/TrackInfoBuilder.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/GeometrySurface/interface/PlaneBuilder.h"
#include "DataFormats/GeometrySurface/interface/BoundPlane.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "TrackingTools/TrajectoryParametrization/interface/GlobalTrajectoryParameters.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include "boost/multi_array.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <sys/time.h>

#include <cmath>
#include <cstdlib>

#include "TROOT.h"
#include "Math/Vavilov.h"
#include "Math/VavilovAccurate.h"
#include "Math/SpecFuncMathCore.h"
#include "Math/SpecFuncMathMore.h"

#include "TStyle.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h" 
#include "TProfile.h" 
#include "TVector3.h"
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TPostScript.h"

const int nMaxTrack = 10000;

class ProbQXYAna : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit ProbQXYAna(const edm::ParameterSet&);
  float combineProbs(float probOnTrackWMulti, int numRecHits);
  ~ProbQXYAna() override = default;

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  const edm::EDGetTokenT<std::vector<pat::PackedCandidate>> packedCandidate_;
  const edm::EDGetTokenT<std::vector<pat::IsolatedTrack>> trackToken_;
  const edm::EDGetTokenT<edm::Association<std::vector<reco::DeDxHitInfo>>> dedxToken_;
  TTree* smalltree;
  int numTrack; 
  float tree_track_probQ[nMaxTrack];
  float tree_track_probQNew[nMaxTrack];
};

using namespace reco;
using namespace std;
using namespace edm;
using namespace ROOT;
using namespace Math;

namespace {
  Surface::RotationType rotation(const GlobalVector& zDir) {
    GlobalVector zAxis = zDir.unit();
    GlobalVector yAxis(zAxis.y(), -zAxis.x(), 0);
    GlobalVector xAxis = yAxis.cross(zAxis);
    return Surface::RotationType(xAxis, yAxis, zAxis);
  }
}

ProbQXYAna::ProbQXYAna(const edm::ParameterSet& iConfig)
    : packedCandidate_(consumes<std::vector<pat::PackedCandidate>>(iConfig.getParameter<edm::InputTag>("packedCandidates"))),
      trackToken_(consumes<std::vector<pat::IsolatedTrack>>(iConfig.getParameter<edm::InputTag>("tracks"))),
      dedxToken_(consumes<edm::Association<std::vector<reco::DeDxHitInfo>>>(iConfig.getParameter<edm::InputTag>("dedx"))) {
  usesResource("TFileService");
  edm::Service<TFileService> fs;
  smalltree = fs->make<TTree>("ttree", "ttree");
  smalltree->Branch("numTrack", &numTrack);
  smalltree->Branch("track_probQ", tree_track_probQ, "track_probQ[numTrack]/F");
  smalltree->Branch("track_probQNew", tree_track_probQNew, "track_probQNew[numTrack]/F");

}

void ProbQXYAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
 /* static char outfile0[80], outfile1[80], outfile2[80];
  gStyle->SetOptFit(101);
  gStyle->SetHistLineWidth(2);
  static vector<TH1F*> hp(2);
  edm::Service<TFileService> fs;

  hp[0]  = fs->make<TH1F>("combProbQ","ProbQ on tracks (w/ combine);Combined on-track charge probability;Entries (1/bin)",100,0.,1.);
  hp[1]  = fs->make<TH1F>("combProbQNew","ProbQ on tracks (w/ combine) from MiniAOD;Combined on-track charge probability;Entries (1/bin)",100,0.,1.);

  for(unsigned int i=0; i<hp.size(); ++i) {
    hp[i]->SetLineColor(2);
    hp[i]->SetFillColor(38);
    hp[i]->SetMinimum(0.0);
  }
 */

  float clusbuf[TXSIZE][TYSIZE];
  int mrow=TXSIZE,mcol=TYSIZE;
  static float xrec, yrec, sigmax, sigmay, probx, proby,probQ;
  static int ix,iy, speed;
  bool xdouble[TXSIZE],ydouble[TYSIZE];
  int qbin,ierr;
  speed=-2;

  edm::ESHandle<MagneticField> magfield_;
  iSetup.get<IdealMagneticFieldRecord>().get(magfield_);

  edm::ESHandle<TransientTrackBuilder> transientTrackBuilder;
  iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", transientTrackBuilder);

  edm::ESHandle<TrackerGeometry> theTrackerGeometry;
  iSetup.get<TrackerDigiGeometryRecord>().get( theTrackerGeometry );
  const TrackerGeometry& theTracker(*theTrackerGeometry);

  ESHandle<Propagator> thePropagator;
  iSetup.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", thePropagator);

  // get packed candidate collection
  edm::Handle<std::vector<pat::PackedCandidate>> cands;
  iEvent.getByToken(packedCandidate_, cands);

  // get the track collection
  edm::Handle<std::vector<pat::IsolatedTrack>> IsotrackCollectionHandle;
  iEvent.getByToken(trackToken_, IsotrackCollectionHandle);

  // get the dEdxHitInfo object
  edm::Handle<edm::Association<std::vector<reco::DeDxHitInfo>>> dedxMiniH;
  iEvent.getByToken(dedxToken_, dedxMiniH);

  // Initialize 1D templates
  const SiPixelTemplateDBObject* templateDBobject_;
  edm::ESHandle<SiPixelTemplateDBObject> templateDBobject;
  iSetup.get<SiPixelTemplateDBObjectRcd>().get(templateDBobject);
  templateDBobject_ = templateDBobject.product();
  std::vector< SiPixelTemplateStore > thePixelTemp_;
  SiPixelTemplate templ(thePixelTemp_);


  if (!SiPixelTemplate::pushfile(*templateDBobject_, thePixelTemp_)) {
      cout << "\nERROR: Templates not filled correctly. Check the sqlite file. Using SiPixelTemplateDBObject version "
	   << (*templateDBobject_).version() << "\n\n";
  }
  numTrack = 0;
 
  // Loop through the isoTrack collection 
  for (unsigned int c = 0; c < IsotrackCollectionHandle->size(); c++) {
   edm::Ref<std::vector<pat::IsolatedTrack>> track = edm::Ref<std::vector<pat::IsolatedTrack>>(IsotrackCollectionHandle, c);

   //LogPrint("ProbQXYAna") << "Analyzing run " << iEvent.id().run() << " / event " << iEvent.id().event();
   if ( track->pt() < 50.1) continue;
   LogPrint("ProbQXYAna") << "  --------------------------------------------------";

   float probQonTrack = track->probQonTrack();
   float probXYonTrack = track->probXYonTrack();
   float probQonTrackNoLayer1 = track->probQonTrackNoLayer1();
   float probXYonTrackNoLayer1 = track->probXYonTrackNoLayer1();

   float probQonTrackWMulti = 1;
   float probXYonTrackWMulti = 1;
   int numRecHitsWithProb = 0;

   LogPrint("ProbQXYAna") << "  For track " << numTrack;
   LogPrint("ProbQXYAna") << "  >> probQonTrack: " << probQonTrack << " and probXYonTrack: " << probXYonTrack;
   LogPrint("ProbQXYAna") << "  >> probQonTrackNoLayer1: " << probQonTrackNoLayer1
                           << " and probXYonTrackNoLayer1: " << probXYonTrackNoLayer1;

   for (unsigned int i = 0; i < cands->size(); i++) {
    const pat::PackedCandidate& pc = (*cands)[i];
    if (pc.hasTrackDetails()) {
     const reco::Track* pseudoTrack = &(pc.pseudoTrack());
     if (abs(pseudoTrack->pt()-track->pt()) > 1) {
       continue;
     } else if (abs(pseudoTrack->eta() - track->eta()) > 1) {
       continue;
     } else if (abs(pseudoTrack->phi() - track->phi()) > 1) {
       continue;
     } else {
        LogPrint("ProbQXYAna") << "    >> Matching packed candidate found";
     }
     LogPrint("ProbQXYAna") << "    >> pseudo track with pT - iso track pt =  " << pseudoTrack->pt()-track->pt();

     float track_px = pseudoTrack->px();
     float track_py = pseudoTrack->py();
     float track_pz = pseudoTrack->pz();
     float track_phi = pseudoTrack->phi();
     float track_dz = pseudoTrack->dz();
     float track_dxy = pseudoTrack->dxy();
     float track_dsz = pseudoTrack->dsz();
     float track_lambda =  pseudoTrack->lambda();
     float track_qoverpError = pseudoTrack->qoverpError();
     float track_lambdaError = pseudoTrack->lambdaError();
     float track_phiError = pseudoTrack->phiError();
     float track_dxyError = pseudoTrack->dxyError();
     float track_dszError = pseudoTrack->dszError();
     int track_charge = pseudoTrack->charge();

     float sinphi = sin(track_phi);
     float cosphi = cos(track_phi);
     float sinlmb = sin(track_lambda);
     float coslmb = cos(track_lambda);
     float tanlmb = sinlmb/coslmb;
     float track_vz = track_dz;
     float track_vx = -sinphi*track_dxy - (cosphi/sinlmb)*track_dsz + (cosphi/tanlmb)*track_vz;
     float track_vy =  cosphi*track_dxy - (sinphi/sinlmb)*track_dsz + (sinphi/tanlmb)*track_vz;

     LogPrint("ProbQXYAna") << "    >> get the startingStateP";
     GlobalPoint startingPosition(track_vx, track_vy, track_vz);
     GlobalVector startingMomentum(track_px, track_py, track_pz);
     reco::TrackBase::CovarianceMatrix track_cov;
     track_cov(0,0) = pow(track_qoverpError,2);
     track_cov(1,1) = pow(track_lambdaError,2);
     track_cov(2,2) = pow(track_phiError,2);
     track_cov(3,3) = pow(track_dxyError,2);
     track_cov(4,4) = pow(track_dszError,2);
     CurvilinearTrajectoryError err(track_cov);
     PlaneBuilder pb;
     auto startingPlane = pb.plane(startingPosition, rotation(startingMomentum));

     TrajectoryStateOnSurface startingStateP(
             GlobalTrajectoryParameters(startingPosition, startingMomentum, track_charge, magfield_.product()),
             err, *startingPlane);

    
     reco::TransientTrack transientTrack = transientTrackBuilder->build(*pseudoTrack);

     const reco::DeDxHitInfo* dedxHits = nullptr;
     reco::DeDxHitInfoRef dedxHitsRef;
     dedxHitsRef = (*dedxMiniH)[track];
     if (!dedxHitsRef.isNull()) {
       dedxHits = &(*dedxHitsRef);
     } else {
       LogPrint("ProbQXYAna") << "      >> dedxHitsRef is null";
       continue;
     }

     LogPrint("ProbQXYAna") << "    >> Loop through the hits on track, num hit is " << dedxHits->size();
     for(unsigned int iHit = 0; iHit < dedxHits->size(); iHit++) {
      const SiPixelCluster* siPixelCluster;
      if (dedxHits!= nullptr) {
        siPixelCluster = dedxHits->pixelCluster(iHit);
      } else {
       LogPrint("ProbQXYAna") << "      >> After all this checks dedxHits is still null, skipping it";
       continue;
      }

      const auto detIDforThisHit = dedxHits->detId(iHit);
      if ((detIDforThisHit.subdetId()  != PixelSubdetector::PixelEndcap) && 
         (detIDforThisHit.subdetId() != PixelSubdetector::PixelBarrel)) { 
         //LogPrint("ProbQXYAna") << "      >> Not a Pixel hit, skipping it";
         continue;
      }

      const GeomDet* geomDet = theTracker.idToDet(detIDforThisHit);
      TrajectoryStateOnSurface extraptsos = thePropagator->propagate(startingStateP, geomDet->specificSurface());
      LogPrint("ProbQXYAna") << "      >> LocalTrajectoryParameters calculation";
      LocalTrajectoryParameters ltp = extraptsos.localParameters();
      LocalVector localDir = ltp.momentum()/ltp.momentum().mag();
      //auto geomdetunit = geomDet->detUnit();
      //const GeomDetUnit* geomdetunit = dynamic_cast<const GeomDetUnit*>(geomDet);
      //auto const& topol = geomdetunit->specificTopology();

      for(int j=0; j<TXSIZE; ++j) {for(int i=0; i<TYSIZE; ++i) {clusbuf[j][i] = 0.;} } 

      const std::vector<SiPixelCluster::Pixel> pixelsVec = siPixelCluster->pixels();
      int minPixelRow = 161;
      int minPixelCol = 417;

      LogPrint("ProbQXYAna") << "      >> Loop through the pixels corresponding to the cluster attached to the hit" ;
      for (unsigned int i = 0; i < pixelsVec.size(); ++i) {
          float pixx = pixelsVec[i].x;  // index as float=iteger, row index
          float pixy = pixelsVec[i].y;  // same, col index

          //bool bigInX = topol->isItBigPixelInX(int(pixx));
          //bool bigInY = topol.isItBigPixelInY(int(pixy));
          bool bigInX = false;
          bool bigInY = false;
          float pixel_charge = pixelsVec[i].adc;

	  //  Find lower left corner pixel and its coordinates
	  if((int)pixx < minPixelRow) {
	    minPixelRow = (int)pixx; 
	  }
	  if((int)pixy < minPixelCol) {
	    minPixelCol = (int)pixy;
	  }
          if (bigInX || bigInY ) continue;

	// Now fill the cluster buffer with charges

	  ix = (int)pixx - minPixelRow;
	  if(ix >= TXSIZE) continue;
	  iy = (int)pixy - minPixelCol;
	  if(iy >= TYSIZE) continue;
	  
	  clusbuf[ix][iy] = pixel_charge;

	  if ((int)pixx == 79 || (int)pixx == 80){
	    xdouble[ix] = true;
	  }
	  if ((int)pixy % 52 == 0 || (int)pixy % 52 == 51 ){
	    ydouble[iy] = true;
	  }
	}


        //float cotAlpha=0.2;
        //float cotBeta=0.2;
	float cotAlpha = atan2(localDir.z(), localDir.x());
	float cotBeta =  atan2(localDir.z(), localDir.y());
        if(fabsf(cotBeta) > 6.0) {
          continue;
          LogPrint("ProbQXYAna") << "        >> |cotBeta|>6.0, skipping it";
        }
	float locBx = 1.;
	if(cotBeta < 0.) locBx = -1.;
	float locBz = locBx;
	if(cotAlpha < 0.) locBz = -locBx;
	int TemplID1=-9999;
	TemplID1=templateDBobject_->getTemplateID(detIDforThisHit);
	templ.interpolate(TemplID1, 0.f, 0.f, 1.f, 1.f);
	SiPixelTemplateReco::ClusMatrix clusterPayload{&clusbuf[0][0], xdouble, ydouble, mrow,mcol};

        // Running the actualy 1D Template Reco
        LogPrint("ProbQXYAna") << "        >> Running the actualy 1D Template Reco" ;
        ierr = PixelTempReco1D(TemplID1,
                               cotAlpha, 
                               cotBeta, 
                               locBz, 
                               locBx, 
                               clusterPayload, 
                               templ, 
                               yrec, 
                               sigmay, 
                               proby, 
                               xrec, 
                               sigmax, 
                               probx, 
                               qbin, 
                               speed, 
                               probQ);  
        if(ierr != 0) {
          {printf("ID %d reco of cotalpha/cotbeta = %f/%f failed with error %d \n", TemplID1, cotAlpha, cotBeta, ierr);}
            continue;
        } else {
         if (probQ != 1e-05 || probQ > 0) {
           probQonTrackWMulti *= probQ;
           numRecHitsWithProb++;
           LogPrint("ProbQXYAna")  << "        >> Non-default ProbQ in this cluster " << probQ;
         }
         if (probx !=1.110223e-16 && proby != 1.110223e-16) probXYonTrackWMulti *= probx*proby;
        }
      } // end loop on hits corresponding to the track
     } // end if on the packed candidate having track details 
   } // end loop on the packed candidate collection
   float probQonTrackNew = combineProbs(probQonTrackWMulti,numRecHitsWithProb);
   float probXYonTrackNew = combineProbs(probXYonTrackWMulti,numRecHitsWithProb);
   LogPrint("ProbQXYAna") << "  >> probQonTrackNew: " << probQonTrackNew << " and probXYonTrackNew: " << probXYonTrackNew;
   tree_track_probQ[numTrack] = probQonTrack;
   tree_track_probQNew[numTrack] = probQonTrackNew;
   numTrack++;
    //  probQonTrackNoLayer1New = combineProbs(probQonTrackWMultiNoLayer1,numRecHitsNoLayer1);
    //  probXYonTrackNoLayer1New = combineProbs(probXYonTrackWMultiNoLayer1,numRecHitsNoLayer1);
  } // end loop on isolated track collection
  smalltree->Fill();
}

float ProbQXYAna::combineProbs(float probOnTrackWMulti, int numRecHits) {
  float logprobOnTrackWMulti = probOnTrackWMulti > 0 ? log(probOnTrackWMulti) : 0;
  float factQ = -logprobOnTrackWMulti;
  float probOnTrackTerm = 0.f;

  if (numRecHits == 1) {
    probOnTrackTerm = 1.f;
  } else if (numRecHits > 1) {
    probOnTrackTerm = 1.f + factQ;
    for (int iTkRh = 2; iTkRh < numRecHits; ++iTkRh) {
      factQ *= -logprobOnTrackWMulti / float(iTkRh);
      probOnTrackTerm += factQ;
    }
  }
  float probOnTrack = probOnTrackWMulti * probOnTrackTerm;

//  LogPrint("SiPixelTrackProbQXYProducer") << "  >> probOnTrack = " << probOnTrack << " = "
//                                            << probOnTrackWMulti << " * " << probOnTrackTerm;
 
  return probOnTrack;
}

//define this as a plug-in
DEFINE_FWK_MODULE(ProbQXYAna);
