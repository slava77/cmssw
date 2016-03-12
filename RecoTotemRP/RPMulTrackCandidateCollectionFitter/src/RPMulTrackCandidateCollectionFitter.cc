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

#include "RecoTotemRP/RPMulTrackCandidateCollectionFitter/interface/RPMulTrackCandidateCollectionFitter.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"


RPMulTrackCandidateCollectionFitter::RPMulTrackCandidateCollectionFitter(const edm::ParameterSet& conf)
   : the_track_candidate_fitter_(conf), readReconstructedPatterns_(false)
{

  edm::LogInfo("RPMulTrackCandidateCollectionFitter") << "[RPMulTrackCandidateCollectionFitter::RPMulTrackCandidateCollectionFitter] Constructing object...";
  verbosity_ = conf.getParameter<int>("Verbosity");
  if(conf.exists("readReconstructedPatterns"))  //check if "readReconstructedPatterns" variable is defined in configuration file
  {
    readReconstructedPatterns_ = conf.getParameter<bool> ("readReconstructedPatterns");
    reconstructedPatternsInstance_ = conf.getParameter<string> ("reconstructedPatternsInstance");
  }else{
	  rPMulTrackCandidateCollectionLabel = conf.getParameter<edm::InputTag>("RPMulTrackCandidateCollectionLabel");
      rPMulTrackCandidateCollectionToken = consumes< RPMulTrackCandidateCollection >(rPMulTrackCandidateCollectionLabel);
  }
  
  produces< RPMulFittedTrackCollection > ();
}


RPMulTrackCandidateCollectionFitter::~RPMulTrackCandidateCollectionFitter()
{
  edm::LogInfo("RPMulTrackCandidateCollectionFitter") << "[RPMulTrackCandidateCollectionFitter::~RPMulTrackCandidateCollectionFitter] Destructing object...";
}


void RPMulTrackCandidateCollectionFitter::beginJob()
{
  if(verbosity_)
  {
    edm::LogInfo("RPMulTrackCandidateCollectionFitter") << "[RPMulTrackCandidateCollectionFitter::beginJob]";
  }
}


void RPMulTrackCandidateCollectionFitter::produceFromMultitrackCandidates(edm::Event& e, const edm::EventSetup& c, RPMulFittedTrackCollection& fitTrackColl)
{
  // Step A: Get event setup information
  edm::ESHandle<TotemRPGeometry> Totem_RP_geometry;
  c.get<RealGeometryRecord>().get(Totem_RP_geometry);
  
  // Step B: Get Inputs
  edm::Handle< RPMulTrackCandidateCollection > input;
  e.getByToken(rPMulTrackCandidateCollectionToken, input);
 
  if( !Totem_RP_geometry.isValid() )
  {
    std::cout<<"RPMulTrackCandidateCollectionFitter: TotemRPGeometry missing, exiting."<<std::endl;
    exit(0);
  }
  
  if( !input.isValid() )
  {
    std::cout<<"RPMulTrackCandidateCollectionFitter: RPMulTrackCandidateCollection missing, exiting."<<std::endl;
    exit(0);
  }
 
  // Step C: produce output product
  if(input->size())
  {
    run(*input, fitTrackColl, *Totem_RP_geometry);
  }

  if (verbosity_)
  {
    RPMulFittedTrackCollection::const_iterator it;
    std::cout << e.id() << std::endl;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
    for(it = fitTrackColl.begin(); it != fitTrackColl.end(); it++)
    {
      std::cout << "RP " << it->first << ": " << (it->second).size() << " fitted tracks" << std::endl;
    }
  } 

}


double RPMulTrackCandidateCollectionFitter::calculateMeanValue(const std::vector<RPRecoHit> &coll, std::vector<RPRecoHit> &obj_coll)
{
  obj_coll.clear();
  double mean = 0.;
  for(unsigned int i=0; i<coll.size(); ++i)
  {
    mean += coll[i].Position();
    obj_coll.push_back(coll[i]);
  }
  mean = mean/coll.size();
  return mean;
}



void RPMulTrackCandidateCollectionFitter::produceFromReconstructedPatterns(edm::Event& e, 
            const edm::EventSetup& c, RPMulFittedTrackCollection& fitTrackColl)
{
  edm::ESHandle<TotemRPGeometry> Totem_RP_geometry;
  c.get<RealGeometryRecord>().get(Totem_RP_geometry);
  
  edm::Handle< RPRecognizedPatternsCollection > patterns;
  e.getByLabel(reconstructedPatternsInstance_, "", patterns);
  
  if(!Totem_RP_geometry.isValid())
  {
    std::cout<<"RPMulTrackCandidateCollectionFitter: TotemRPGeometry missing, exiting."<<std::endl;
    exit(0);
  }
  
  if( !patterns.isValid() )
  {
    std::cout<<"RPMulTrackCandidateCollectionFitter: RPRecognizedPatternsCollection missing, exiting."<<std::endl;
    exit(0);
  }

  for (RPRecognizedPatternsCollection::const_iterator rpit = patterns->begin(); rpit != patterns->end(); ++rpit)
  {
    unsigned int rp = rpit->first;

    int i = 0;
    int j = 0; 
    for (std::vector<RPRecognizedPatterns::Line>::const_iterator ulit = rpit->second.uLines.begin(); ulit != rpit->second.uLines.end(); ++ulit)
    {
      for (std::vector<RPRecognizedPatterns::Line>::const_iterator vlit = rpit->second.vLines.begin(); vlit != rpit->second.vLines.end(); ++vlit)
      {
        std::vector<RPRecoHit> u_roads, v_roads;
                
        double mean_u = calculateMeanValue(ulit->hits, u_roads);
        double mean_v = calculateMeanValue(vlit->hits, v_roads);
        
        if(!det_topology_.IsHit(mean_u, mean_v))
        {
          if(verbosity_)
          {
            std::cout << "ignore on candidate track" << std::endl;
          }
          continue;
        }
        // calculate the reliability of a certain U&V candidate track
        // double weight = CalcCandidateTrackWeight(mean_u, mean_v, uhits, vhits);
        double weight = u_roads.size() + v_roads.size();
        
        RPTrackCandidate tr_cand;
        tr_cand.InsertHits(u_roads, 0);
        tr_cand.InsertHits(v_roads, 0);
        tr_cand.Weight(weight);
        tr_cand.SetUVid(i,j);
        //TODO: build the candidate track if the weight satisfies a certain condition
        
        //fit the track
        RPFittedTrack fitted_track;
        CLHEP::Hep3Vector rp_glob_trans = (*Totem_RP_geometry).GetRPGlobalTranslation(rp);
        
        the_track_candidate_fitter_.FitTrack(tr_cand, rp_glob_trans.z(), fitted_track, *Totem_RP_geometry);
        if(fitted_track.IsValid())
        {
          fitTrackColl[rp].push_back(fitted_track);
        }
        j++;
      }
      i++;
    }
  }
}


void RPMulTrackCandidateCollectionFitter::produce(edm::Event& e, const edm::EventSetup& c)
{
  if (geometryWatcher.check(c))
    the_track_candidate_fitter_.Reset();

  RPMulFittedTrackCollection mfitted_tracks_collection;
  
  if(readReconstructedPatterns_)
  {
    produceFromReconstructedPatterns(e,c, mfitted_tracks_collection);
  }
  else
  {
    produceFromMultitrackCandidates(e,c, mfitted_tracks_collection);
  }
  
  // Step D: create and fill output collection
  std::auto_ptr<RPMulFittedTrackCollection> output(new RPMulFittedTrackCollection(mfitted_tracks_collection));

  // Step E: write output to file
  e.put(output);
}


void RPMulTrackCandidateCollectionFitter::run(const RPMulTrackCandidateCollection & input, 
    RPMulFittedTrackCollection& output, const TotemRPGeometry & rp_geometry)
{
  RPMulTrackCandidateCollection::const_iterator in_it;
  std::vector<RPTrackCandidate>::const_iterator tr_it;
  for(in_it = input.begin(); in_it != input.end(); in_it++)
  {
    CLHEP::Hep3Vector rp_glob_trans = rp_geometry.GetRPGlobalTranslation(in_it->first);
    for(tr_it = (in_it->second).begin(); tr_it != (in_it->second).end(); tr_it++)
    {
      RPFittedTrack fitted_track;
      the_track_candidate_fitter_.FitTrack(*tr_it, rp_glob_trans.z(), fitted_track, rp_geometry);
      if(fitted_track.IsValid())
      {
        output[in_it->first].push_back(fitted_track);
      }
    }
  }
}

DEFINE_FWK_MODULE(RPMulTrackCandidateCollectionFitter);

