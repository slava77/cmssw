/****************************************************************************
 *
 * This module is directly copied from RecoTotemRP/RPTrackCandidateCollectionFitter
 * and expanded to hold multiple fitted track per Roman Pot per event.
 * Original Authors:
 *   Hubert Niewiadomski (Hubert.Niewiadomski@cern.ch)
 *   Jan Kašpar (jan.kaspar@gmail.com)
 * Secondary Authors:
 *   Zhang Zhengkui (zhang.zhengkui.fin@gmail.com)
 *
 ****************************************************************************/

#ifndef RecoTotemRP_RPMulTrackCandidateCollectionFitter_RPMulTrackCandidateCollectionFitter_h
#define RecoTotemRP_RPMulTrackCandidateCollectionFitter_RPMulTrackCandidateCollectionFitter_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulFittedTrackCollection.h"
#include "RecoTotemRP/RPTrackCandidateFitter/interface/RPTrackCandidateFitter.h"
#include "Geometry/TotemRPDetTopology/interface/RPTopology.h"

#include <string>
#include <vector>


/**
 *\brief Fits every track candidate in RPMulTrackCandidateCollection.
 *
 * Result is collection of fitted tracks (RPMulFittedTrackCollection). The fit itself is performed by class RPTrackCandidateFitter.
 **/
class RPMulTrackCandidateCollectionFitter : public edm::EDProducer
{
  public:
    explicit RPMulTrackCandidateCollectionFitter(const edm::ParameterSet& conf);
    virtual ~RPMulTrackCandidateCollectionFitter();
    virtual void beginJob();
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
    void run(const RPMulTrackCandidateCollection & input, 
        RPMulFittedTrackCollection& output, const TotemRPGeometry & rp_geometry);
    void produceFromMultitrackCandidates(edm::Event& e, const edm::EventSetup& c, RPMulFittedTrackCollection& fitTrackColl);
    void produceFromReconstructedPatterns(edm::Event& e, const edm::EventSetup& c, RPMulFittedTrackCollection& fitTrackColl);
  
  private:
    edm::InputTag rPMulTrackCandidateCollectionLabel;
    edm::EDGetTokenT< RPMulTrackCandidateCollection > rPMulTrackCandidateCollectionToken;
    double calculateMeanValue(const std::vector<RPRecoHit> &coll, std::vector<RPRecoHit> &obj_coll);
    
    int verbosity_;
    RPTrackCandidateFitter the_track_candidate_fitter_;
    bool readReconstructedPatterns_;
    std::string reconstructedPatternsInstance_;
    RPTopology det_topology_;

    /// A watcher to detect geometry changes.
    edm::ESWatcher<RealGeometryRecord> geometryWatcher;
};

#endif

