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

class ProbQXYAna : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit ProbQXYAna(const edm::ParameterSet&);
  ~ProbQXYAna() override = default;

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

  // ----------member data ---------------------------
  const edm::EDGetTokenT<std::vector<pat::PackedCandidate>> packedCandidate_;
  const edm::EDGetTokenT<std::vector<pat::IsolatedTrack>> trackToken_;
  const edm::EDGetTokenT<edm::Association<std::vector<reco::DeDxHitInfo>>> dedxToken_;
};

using namespace reco;
using namespace std;
using namespace edm;

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
      dedxToken_(consumes<edm::Association<std::vector<reco::DeDxHitInfo>>>(iConfig.getParameter<edm::InputTag>("dedx"))) {}

void ProbQXYAna::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
  unsigned int numTrack = 0;
  
  for (unsigned int c = 0; c < IsotrackCollectionHandle->size(); c++) {
   numTrack++;

   edm::Ref<std::vector<pat::IsolatedTrack>> track = edm::Ref<std::vector<pat::IsolatedTrack>>(IsotrackCollectionHandle, c);

   LogPrint("ProbQXYAna") << "Analyzing run " << iEvent.id().run() << " / event " << iEvent.id().event();
   if ( track->pt() < 50.1) continue;
   LogPrint("ProbQXYAna") << " track with pT =  " << track->pt();

   float probQonTrack = track->probQonTrack();
   float probXYonTrack = track->probXYonTrack();
   float probQonTrackNoLayer1 = track->probQonTrackNoLayer1();
   float probXYonTrackNoLayer1 = track->probXYonTrackNoLayer1();
   LogPrint("ProbQXYAna") << "  --------------------------------------------------";
   LogPrint("ProbQXYAna") << "  For track " << numTrack;
   LogPrint("ProbQXYAna") << "  >> probQonTrack: " << probQonTrack << " and probXYonTrack: " << probXYonTrack;
   LogPrint("ProbQXYAna") << "  >> probQonTrackNoLayer1: " << probQonTrackNoLayer1
                           << " and probXYonTrackNoLayer1: " << probXYonTrackNoLayer1;

   for (unsigned int i = 0; i < cands->size(); i++) {
    const pat::PackedCandidate& pc = (*cands)[i];
    if (pc.hasTrackDetails()) {
     const reco::Track* pseudoTrack = &(pc.pseudoTrack());
     if (abs(pseudoTrack->pt()-track->pt())>.1) {
       continue;
     } else {
        LogPrint("ProbQXYAna") << " >>>> Matching packed candidate found";
     }
     LogPrint("ProbQXYAna") << "  >>>> pseudo track with pT =  " << pseudoTrack->pt();
    
     LogPrint("ProbQXYAna") << "  >>>>>> transientTrack builder"; 
     reco::TransientTrack transientTrack = transientTrackBuilder->build(*pseudoTrack);

     LogPrint("ProbQXYAna") << " Get DeDxHitInfo";
     const reco::DeDxHitInfo* dedxHits = nullptr;
     reco::DeDxHitInfoRef dedxHitsRef;
     dedxHitsRef = (*dedxMiniH)[track];
     if (!dedxHitsRef.isNull()) {
       dedxHits = &(*dedxHitsRef);
     } else {
       LogPrint("ProbQXYAna") << " dedxHits is null";
       continue;
     }

     LogPrint("ProbQXYAna") << " Loop through the hits on track, num hit is " << dedxHits->size();
     for(unsigned int iHit = 0; iHit < dedxHits->size(); iHit++) {
      const SiPixelCluster* siPixelCluster = dedxHits->pixelCluster(iHit);
      //auto geomdetunit = dynamic_cast<const PixelGeomDetUnit*>(dedxHits->detId(iHit)->detUnit());
      //auto const& topol = geomdetunit->specificTopology();

      const std::vector<SiPixelCluster::Pixel> pixelsVec = siPixelCluster->pixels();
      int minPixelRow = 161;
      int minPixelCol = 417;

      LogPrint("ProbQXYAna") << " Loop through the pixels corresponding to the cluster attached to the hit" ;
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


        LogPrint("ProbQXYAna") << "  >>>>>> get the startingStateP";

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

        GlobalVector startingPosition(track_vx, track_vy, track_vz);
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

        const auto detIDforThisHit = dedxHits->detId(iHit);
        const GeomDet* geomDet = theTracker.idToDet(detIDforThisHit);
        TrajectoryStateOnSurface extraptsos = thePropagator->propagate(startingStateP, geomDet->specificSurface());
        LogPrint("ProbQXYAna") << "  >>>>>> LocalTrajectoryParameters calculation";
        LocalTrajectoryParameters ltp = extraptsos.localParameters();
        LocalVector localDir = ltp.momentum()/ltp.momentum().mag();
        //float cotAlpha=0.2;
        //float cotBeta=0.2;
	float cotAlpha = atan2(localDir.z(), localDir.x());
	float cotBeta =  atan2(localDir.z(), localDir.y());
	float locBx = 1.;
	if(cotBeta < 0.) locBx = -1.;
	float locBz = locBx;
	if(cotAlpha < 0.) locBz = -locBx;
	int TemplID1=-9999;
	TemplID1=templateDBobject_->getTemplateID(detIDforThisHit);
	templ.interpolate(TemplID1, 0.f, 0.f, 0.f, 0.f);
	SiPixelTemplateReco::ClusMatrix clusterPayload{&clusbuf[0][0], xdouble, ydouble, mrow,mcol};

        // Running the actualy 1D Template Reco
        LogPrint("ProbQXYAna") << " Running the actualy 1D Template Reco" ;
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

        cout << "ProbQ in this cluster " << probQ << endl;
        }
    } // end loop on hits corresponding to the track
   } // end if on the packed candidate having track details 
   } // end loop on the packed candidate collection
  } // end loop on isolated track collection
}

//define this as a plug-in
DEFINE_FWK_MODULE(ProbQXYAna);
