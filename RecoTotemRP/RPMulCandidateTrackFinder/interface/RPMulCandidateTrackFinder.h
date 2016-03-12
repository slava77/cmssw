/****************************************************************************
 *
 * This module is directly copied from RecoTotemRP/RPMulCandidateTrackFinder
 * and expanded to output multiple track candidates per Roman Pot per event.
 * Original Authors:
 *   Hubert Niewiadomski (Hubert.Niewiadomski@cern.ch)
 * Secondary Authors:
 *   Zhang Zhengkui (zhang.zhengkui.fin@gmail.com)
 *
 ****************************************************************************/


#ifndef RecoTotemRP_RPMulCandidateTrackFinder_RPMulCandidateTrackFinder_h
#define RecoTotemRP_RPMulCandidateTrackFinder_RPMulCandidateTrackFinder_h


//edm
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//Data Formats
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulTrackCandidateCollection.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"

//Geometry
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"

//RPMulCandidateTrackFinder
#include "RecoTotemRP/RPMulCandidateTrackFinder/interface/RPMulCandidateTrackFinderAlgorithm.h"

//C++ and STL
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <functional>

//Root
#include "TFile.h"


class RPMulCandidateTrackFinder : public edm::EDProducer
{
  public:
    explicit RPMulCandidateTrackFinder(const edm::ParameterSet& conf);
    virtual ~RPMulCandidateTrackFinder();
    virtual void beginJob();
    virtual void endJob();
    virtual void produce(edm::Event& e, const edm::EventSetup& c);

  private:
    edm::InputTag recohit_label_;
    edm::EDGetTokenT<edm::DetSetVector<RPRecoHit> > recohit_label_Token_;
    void run(const edm::DetSetVector<RPRecoHit> & input, RPMulTrackCandidateCollection& output, const TotemRPGeometry & rp_geometry);
    const edm::ParameterSet conf_;
    RPMulCandidateTrackFinderAlgorithm RPMulCandidateTrackFinderAlgorithm_;
    int verbosity_;
    unsigned int minimal_hits_count_per_rp_; // the minimal amount of (U&V) hits for starting the track finding

    TFile *of;
    int output_;
    std::string plotFileName;
};



#endif
