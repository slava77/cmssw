/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef RecoTotemRP_RPTrackCandidateCollectionFitter_RPTrackCandidateCollectionFitter_h
#define RecoTotemRP_RPTrackCandidateCollectionFitter_RPTrackCandidateCollectionFitter_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "RecoTotemRP/RPTrackCandidateFitter/interface/RPTrackCandidateFitter.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrackCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"

#include <string>

/**
 *\brief Fits every track candidate in RPTrackCandidateCollection.
 *
 * Result is collection of fitted tracks (RPFittedTrackCollection).
 * The fit itself is performed by class RPTrackCandidateFitter.
 **/
class RPTrackCandidateCollectionFitter : public edm::EDProducer
{
  public:
    explicit RPTrackCandidateCollectionFitter(const edm::ParameterSet& conf);
    virtual ~RPTrackCandidateCollectionFitter();
    virtual void beginJob();
    virtual void produce(edm::Event& e, const edm::EventSetup& c);

    /// Fits the collection of track candidates, one by one by using the RPTrackCandidateFitter::FitTrack.
    void run(const RPTrackCandidateCollection &input, RPFittedTrackCollection &output,
      const TotemRPGeometry &rp_geometry);
  
  private:
    void run();
    int verbosity_;

    /// The name of the pattern-recognition module.
    std::string trackCandidateCollectionProducer;
    edm::InputTag trackCandidateCollectionLabel;

    edm::EDGetTokenT<RPTrackCandidateCollection> trackCandidateCollectionToken;
    //std::string track_coll_cand_label_;
    
    /// A watcher to detect geometry changes.
    edm::ESWatcher<RealGeometryRecord> geometryWatcher;
    
    /// The instance of the fitter module
    RPTrackCandidateFitter the_track_candidate_fitter_;
};

#endif

