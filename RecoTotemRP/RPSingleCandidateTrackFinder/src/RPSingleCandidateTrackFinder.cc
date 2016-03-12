#include "RecoTotemRP/RPSingleCandidateTrackFinder/interface/RPSingleCandidateTrackFinder.h"
#include "RecoTotemRP/RPSingleCandidateTrackFinder/interface/RPSingleCandidateTrackFinderAlgorithm.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/TotemRecords/interface/RealGeometryRecord.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include <iostream>

#include "FWCore/Framework/interface/MakerMacros.h"

RPSingleCandidateTrackFinder::RPSingleCandidateTrackFinder(const edm::ParameterSet& conf)
   : conf_(conf), RPSingleCandidateTrackFinderAlgorithm_(conf)
{
  edm::LogInfo("RPSingleCandidateTrackFinder") << "[RPSingleCandidateTrackFinder::RPSingleCandidateTrackFinder] Constructing object...";
  verbosity_ = conf.getParameter<int>("Verbosity");
//  rprecohit_producer_ = conf.getParameter<std::string>("RPRecoHitProducer");
  if(!conf.exists("RPRecoHitLabel")){
	  cout<<"expecting RPRecoHitLabel parameter with module name for type RPRecoHit"<<endl;
  }
  recohit_label_ = conf.getParameter<edm::InputTag>("RPRecoHitLabel");
//  single_track_candidate_collect_label_ = conf.getParameter<std::string>("TrackCollectionLabelLabel");
//  produces< RPTrackCandidateCollection > (single_track_candidate_collect_label_);
  produces<RPRecognizedPatternsCollection> ();
  produces< RPTrackCandidateCollection > ();
  recohit_label_Token_ = consumes<edm::DetSetVector<RPRecoHit> >(recohit_label_);
}


RPSingleCandidateTrackFinder::~RPSingleCandidateTrackFinder()
{
  edm::LogInfo("RPSingleCandidateTrackFinder") << "[RPSingleCandidateTrackFinder::~RPSingleCandidateTrackFinder] Destructing object...";
}


void RPSingleCandidateTrackFinder::beginJob()
{
  if(verbosity_)
  {
    edm::LogInfo("RPSingleCandidateTrackFinder") << "[RPSingleCandidateTrackFinder::beginJob]";
  }
}


void RPSingleCandidateTrackFinder::produce(edm::Event& e, const edm::EventSetup& c)
{
  // Step A: Get event setup information
  //for the moment - ideal geometry
  edm::ESHandle<TotemRPGeometry> Totem_RP_geometry;
  c.get<RealGeometryRecord>().get(Totem_RP_geometry);
  
  // Step B: Get Inputs
  edm::Handle< edm::DetSetVector<RPRecoHit> > input;
 
  // Step C: produce output product
  std::auto_ptr<RPTrackCandidateCollection> trackCandidateCollection(new RPTrackCandidateCollection());
  auto_ptr<RPRecognizedPatternsCollection> patternsCollection(new RPRecognizedPatternsCollection());
  
  //e.getByLabel(rprecohit_producer_, recohit_label_, input);
 // e.getByLabel(recohit_label_, input);
    e.getByToken(recohit_label_Token_, input);
/* 
    if(verbosity_)
  {
	std::cout << ">> RPSingleCandidateTrackFinder::produce > " << e.id() << std::endl;
	std::cout << "RPRecoHits.size() = " << input->size() << std::endl;
  }
*/
  if(input->size())
    run(*input, *patternsCollection.get(), *trackCandidateCollection.get(), *Totem_RP_geometry);
   
  /// test
/*
	if(verbosity_){
  		std::cout << "*** " << trackCandidateCollection->size() << " TRACK CANDIDATES ***" << std::endl;

		RPTrackCandidateCollection::iterator it;
		for (it = trackCandidateCollection->begin(); it != trackCandidateCollection->end(); ++it) {
  			std::cout << "  TrackCandidate in RPId " << it->first << std::endl;
		}
	}
*/
  /// end test
 
  // Step D: write trackCandidateCollection to file
  e.put(trackCandidateCollection);
  e.put(patternsCollection);
}
  

void RPSingleCandidateTrackFinder::run(const edm::DetSetVector<RPRecoHit> & input,
  RPRecognizedPatternsCollection &patternCollection,
  RPTrackCandidateCollection& output, const TotemRPGeometry & rp_geometry)
{
	/**
	 * This method takes DetSetVector<RPRecoHit> and decouples U and V hits.
	 * Then, the result is map<RPId, uv_pair_vec_reco_hits>
	 * 		uv_pair_vec_reco_hits is pair< vector<RPRecoHit> , vector<RPRecoHit> >
	 * 			first part for U, second for V
	 * The map is traversed and for each entry the following method is called
	 * 		RPSingleCandidateTrackFinderAlgorithm_.BuildSingleTrackCandidates(RPId, U hits, V hits, output, rp_geometry)
	 **/

  typedef std::vector<RPRecoHit> vec_reco_hits;
  typedef std::pair<vec_reco_hits, vec_reco_hits> uv_pair_vec_reco_hits;
  typedef std::map<unsigned int, uv_pair_vec_reco_hits> rp_copy_no_hits_uv_map;
  rp_copy_no_hits_uv_map the_map;
  
  //gather the hits in the roman pots separately for u and v strip coordinates
  //std::cout<<"Received recohits: "<<std::endl;
  edm::DetSetVector<RPRecoHit>::const_iterator it;
  for(it=input.begin(); it!=input.end(); ++it)
  {
    edm::DetSet<RPRecoHit>::const_iterator hits_it;
    for(hits_it = it->begin(); hits_it != it->end(); ++hits_it)
    {
      TotRPDetId tot_rp_det_id(hits_it->DetId());
      unsigned int rp_id = tot_rp_det_id.RPCopyNumber();
      
      uv_pair_vec_reco_hits &pair_ref = the_map[rp_id];
      
      if(tot_rp_det_id.IsStripsCoordinateUDirection())
      {
        pair_ref.first.push_back(*hits_it);
        //std::cout<<"U, ";
      }
      else
      {
        pair_ref.second.push_back(*hits_it);
        //std::cout<<"V, ";
      }

    //std::cout<<"rp_id="<<rp_id<<" DetectorDecId()="<<tot_rp_det_id.DetectorDecId()<<" Position="<<hits_it->Position()<<std::endl;
    }
  }
  
  rp_copy_no_hits_uv_map::const_iterator rp_cp_it;
  for(rp_cp_it = the_map.begin(); rp_cp_it != the_map.end(); ++rp_cp_it)
  {  
    RPSingleCandidateTrackFinderAlgorithm_.BuildSingleTrackCandidates(rp_cp_it->first, 
        rp_cp_it->second.first, rp_cp_it->second.second, patternCollection, output, rp_geometry);
  }
}

DEFINE_FWK_MODULE(RPSingleCandidateTrackFinder);

