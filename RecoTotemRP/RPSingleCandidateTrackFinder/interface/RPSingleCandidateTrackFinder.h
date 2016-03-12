#ifndef RecoTotemRP_RPSingleCandidateTrackFinder_RPSingleCandidateTrackFinder_h
#define RecoTotemRP_RPSingleCandidateTrackFinder_RPSingleCandidateTrackFinder_h

// edm
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// Data Formats
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"

// RPSingleCandidateTrackFinder
#include "RecoTotemRP/RPSingleCandidateTrackFinder/interface/RPSingleCandidateTrackFinderAlgorithm.h"
 
#include <iostream>
#include <memory>
#include <string>

class RPSingleCandidateTrackFinder : public edm::EDProducer
{
  public:
    explicit RPSingleCandidateTrackFinder(const edm::ParameterSet& conf);
    virtual ~RPSingleCandidateTrackFinder();
    virtual void beginJob();
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
  
  private:
    void run(const edm::DetSetVector<RPRecoHit> & input,
        RPRecognizedPatternsCollection &patternCollection,
        RPTrackCandidateCollection& output, const TotemRPGeometry & rp_geometry);
    const edm::ParameterSet conf_;
    int verbosity_;
//    std::string rprecohit_producer_;
//    std::string recohit_label_;
//    std::string single_track_candidate_collect_label_;
    edm::InputTag recohit_label_;
    edm::EDGetTokenT<edm::DetSetVector<RPRecoHit> >recohit_label_Token_;
    RPSingleCandidateTrackFinderAlgorithm RPSingleCandidateTrackFinderAlgorithm_;
};

#endif

