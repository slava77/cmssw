/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors:
* 	Hubert Niewiadomski
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "RecoTotemRP/RPTrackCandidateCollectionFitter/interface/RPTrackCandidateCollectionFitter.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrackCollection.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"

#include <iostream>


RPTrackCandidateCollectionFitter::RPTrackCandidateCollectionFitter(const edm::ParameterSet& conf)
   : the_track_candidate_fitter_(conf)
{
  edm::LogInfo("RPTrackCandidateCollectionFitter") << "[RPTrackCandidateCollectionFitter::RPTrackCandidateCollectionFitter] Constructing object...";
  verbosity_ = conf.getParameter<int>("Verbosity");
  trackCandidateCollectionProducer = conf.getParameter<std::string>("RPTrackCandCollProducer");
  trackCandidateCollectionLabel = conf.getParameter<edm::InputTag>("RPTrackCandidateCollectionLabel");

  if (trackCandidateCollectionProducer.empty())
    trackCandidateCollectionToken = consumes<RPTrackCandidateCollection>(trackCandidateCollectionLabel);
  else
    trackCandidateCollectionToken = consumes<RPTrackCandidateCollection>(trackCandidateCollectionProducer);

  produces< RPFittedTrackCollection > ();
}


RPTrackCandidateCollectionFitter::~RPTrackCandidateCollectionFitter()
{
  edm::LogInfo("RPTrackCandidateCollectionFitter") << "[RPTrackCandidateCollectionFitter::~RPTrackCandidateCollectionFitter] Destructing object...";
}


void RPTrackCandidateCollectionFitter::beginJob()
{
  if(verbosity_)
  {
    edm::LogInfo("RPTrackCandidateCollectionFitter") << "[RPTrackCandidateCollectionFitter::beginJob]";
  }
}


void RPTrackCandidateCollectionFitter::produce(edm::Event& e, const edm::EventSetup& c)
{
  // Step A: Get event setup information
  edm::ESHandle<TotemRPGeometry> Totem_RP_geometry;
  c.get<RealGeometryRecord>().get(Totem_RP_geometry);

  if (geometryWatcher.check(c))
    the_track_candidate_fitter_.Reset();
  
  // Step B: Get Input
  edm::Handle< RPTrackCandidateCollection > input;

  e.getByToken(trackCandidateCollectionToken, input);

  // Step C: produce output product
  RPFittedTrackCollection fitted_tracks_collection;
  
  if(input->size())
    run(*input, fitted_tracks_collection, *Totem_RP_geometry);

  if (verbosity_)
    std::cout << "putting " << fitted_tracks_collection.size() << " RPFittedTrack objects to event " << e.id() << std::endl;
   
  // Step D: create and fill output collection
  std::auto_ptr<RPFittedTrackCollection> output(new RPFittedTrackCollection(fitted_tracks_collection) );

  // Step D: write output to file
  //std::cout<<"Event put to the output..."<<std::endl;
  e.put(output);
}


void RPTrackCandidateCollectionFitter::run(const RPTrackCandidateCollection & input, 
    RPFittedTrackCollection& output, const TotemRPGeometry & rp_geometry)
{
  RPTrackCandidateCollection::const_iterator in_it;
  for(in_it=input.begin(); in_it!=input.end(); ++in_it)
  {
    //std::cout<<"Track to be fitted, rp id="<<in_it->first<<std::endl;
    RPFittedTrack fitted_track;
    CLHEP::Hep3Vector rp_glob_trans = rp_geometry.GetRPGlobalTranslation(in_it->first);
    //std::cout<<"rp_glob_trans="<<rp_glob_trans<<std::endl;
    the_track_candidate_fitter_.FitTrack(in_it->second, rp_glob_trans.z(), fitted_track, rp_geometry);
    //std::cout<<"Fitted track received."<<std::endl;
    if(fitted_track.IsValid())
    {
      output[in_it->first]=fitted_track;
    }
    TVector3 v=fitted_track.TrackCentrePoint();

#if 0
    if(verbosity_)
    {
      if( (
          in_it->first == 100 && v.y()<3
          || in_it->first == 101 && v.y()>-3
          || in_it->first == 102 && v.x()<4
          || in_it->first == 103 && v.x()<4
          || in_it->first == 104 && v.y()<3
          || in_it->first == 105 && v.y()>-3
          || in_it->first == 120 && v.y()<5.5
          || in_it->first == 121 && v.y()>-5.5
          || in_it->first == 122 && v.x()<4
          || in_it->first == 123 && v.x()<4
          || in_it->first == 124 && v.y()<5.5
          || in_it->first == 125 && v.y()>-5.5
          ) && fitted_track.IsValid())
      {
        std::cout<<"Wrong track!!!"<<std::endl;
        std::cout<<std::endl<<"track rp="<<in_it->first<<" point="<<v.x()<<","<<v.y()<<","<<v.z()<<std::endl;
        std::cout<<"Par. vector"<<std::endl;
        tot_rp::Print(std::cout, fitted_track.ParameterVector());
        std::cout<<"Cov. matrix"<<std::endl;
        tot_rp::Print(std::cout, fitted_track.CovarianceMatrix());
        std::cout<<" ChiSquared="<<fitted_track.ChiSquared()<<" ChiSquaredOverN="<<fitted_track.ChiSquaredOverN()
        <<" IsValid="<<fitted_track.IsValid()<<" track candidate Fitable="<<in_it->second.Fitable()<<std::endl;
        
        RPTrackCandidate::range rg = in_it->second.recHits();
        std::cout<<"RecoHits:"<<std::endl;
        for(RPTrackCandidate::const_iterator it = rg.first; it!=rg.second; ++it)
        {
          TotRPDetId id_dec(it->DetId());
          std::cout<<"det id: "<<id_dec.DetectorDecId()<<" pos:"<<it->Position()<<std::endl;
        }

        std::cout<<"Printing clusters:"<<std::endl;
        for(edm::DetSetVector<RPDigCluster>::const_iterator it_1 = input_clusters.begin();
          it_1 != input_clusters.end(); it_1++)
        {
          TotRPDetId id_clust(it_1->id);
          if(id_clust.RPCopyNumber() == in_it->first)
          {
            std::cout<<"===digi clusters det. id.:"<<id_clust.DetectorDecId()<<std::endl;
            for(edm::DetSet<RPDigCluster>::const_iterator it_cluster = (*it_1).begin(); 
              it_cluster!=(*it_1).end(); ++it_cluster)
            {
              std::cout<<"clu. pos:"<<it_cluster->CentreStripPos()<<std::endl;
            }
          }
        }
      }
    }
#endif
  }
}

DEFINE_FWK_MODULE(RPTrackCandidateCollectionFitter);

