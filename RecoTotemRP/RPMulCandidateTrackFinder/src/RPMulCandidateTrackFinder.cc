#include "RecoTotemRP/RPMulCandidateTrackFinder/interface/RPMulCandidateTrackFinder.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"

#include "FWCore/Framework/interface/MakerMacros.h"


RPMulCandidateTrackFinder::RPMulCandidateTrackFinder(const edm::ParameterSet& conf)
   : conf_(conf), RPMulCandidateTrackFinderAlgorithm_(conf)
{
  edm::LogInfo("RPMulCandidateTrackFinder") << "[RPMulCandidateTrackFinder::RPMulCandidateTrackFinder] Constructing object...";
  verbosity_ = conf.getParameter<int>("Verbosity");
  minimal_hits_count_per_rp_ = conf.getParameter<unsigned int>("MinHitsPerRP");
  plotFileName = conf.getParameter<std::string >("PlotFile");
  output_ = conf.getParameter<int>("Output");
  of = NULL;

  produces< RPMulTrackCandidateCollection > ();
  recohit_label_ = conf.getParameter<edm::InputTag>("RPRecoHitDetSetLabel");
  recohit_label_Token_ = consumes< edm::DetSetVector<RPRecoHit> >(recohit_label_);
}


RPMulCandidateTrackFinder::~RPMulCandidateTrackFinder()
{
  edm::LogInfo("RPMulCandidateTrackFinder") << "[RPMulCandidateTrackFinder::~RPMulCandidateTrackFinder] Destructing object...";
}


void RPMulCandidateTrackFinder::beginJob()
{
  if(verbosity_)
  {
    edm::LogInfo("RPMulCandidateTrackFinder") << "[RPMulCandidateTrackFinder::beginJob]";
  }
  if(output_)
  {
    of = TFile::Open(plotFileName.c_str(), "RECREATE");
  }
}


void RPMulCandidateTrackFinder::endJob()
{
  if(verbosity_)
  {
    edm::LogInfo("RPMulCandidateTrackFinder") << "[RPMulCandidateTrackFinder::endJob]";
  }
  if(output_)
  {
    RPMulCandidateTrackFinderAlgorithm_.WriteAllPlots(of);
    of->Close();
  }
}


void RPMulCandidateTrackFinder::produce(edm::Event& e, const edm::EventSetup& c)
{
   if(verbosity_)
     edm::LogInfo("RPMulCandidateTrackFinder") << "[RPMulCandidateTrackFinder] " << e.id() << std::endl;

  // Step A: Get event setup information
  edm::ESHandle<TotemRPGeometry> Totem_RP_geometry;
  c.get<RealGeometryRecord>().get(Totem_RP_geometry);

  // Step B: Get Inputs
  edm::Handle< edm::DetSetVector<RPRecoHit> > input;
  e.getByToken(recohit_label_Token_, input);

  // Step C: produce output product
  RPMulTrackCandidateCollection track_candidates_collection;

  if(input->size())
  {
    run(*input, track_candidates_collection, *Totem_RP_geometry);
  }

  // Step D: create and fill output collection
  std::auto_ptr<RPMulTrackCandidateCollection> output(new RPMulTrackCandidateCollection(track_candidates_collection) );

  // Step E: write output to file
  e.put(output);
}


void RPMulCandidateTrackFinder::run(const edm::DetSetVector<RPRecoHit> & input,
    RPMulTrackCandidateCollection& output, const TotemRPGeometry & rp_geometry)
{
    /**
     * This method takes DetSetVector<RPRecoHit> and decouples U and V hits into their
     * corresponding detector.
     * The the result is map<RPId, uv_pair_det_vec_reco_hits>
     *    uv_pair_vec_reco_hits is pair< det_vec_reco_hits , det_vec_reco_hits >
     *    det_vec_reco_hits is map<unsigned int, std::vector<RPRecoHit> > whose key is the detector id
     * first part for U, second for V
     * The map is traversed and for each entry the following method is called
     * RPMulCandidateTrackFinderAlgorithm_.BuildTrackCandidates(RPId, U hits, V hits, output, rp_geometry)
     **/
  typedef std::map<unsigned int, std::vector<RPRecoHit>, std::less<unsigned int> > det_vec_reco_hits;
  typedef std::pair<det_vec_reco_hits, det_vec_reco_hits> uv_pair_det_vec_reco_hits;
  typedef std::map<unsigned int, uv_pair_det_vec_reco_hits> rp_copy_no_hits_uv_map;
  rp_copy_no_hits_uv_map the_map;
  std::map<unsigned int, unsigned int> rp_hits_num_map;

  // gather the hits in the roman pots separately for u and v strip coordinates
  edm::DetSetVector<RPRecoHit>::const_iterator it;
  for(it=input.begin(); it!=input.end(); ++it)
  {
    edm::DetSet<RPRecoHit>::const_iterator hits_it;
    for(hits_it = it->begin(); hits_it != it->end(); ++hits_it)
    {
      TotRPDetId tot_rp_det_id(hits_it->DetId());
      unsigned int rp_id = tot_rp_det_id.RPCopyNumber();
      unsigned int det_id = tot_rp_det_id.Detector();

      uv_pair_det_vec_reco_hits &pair_ref = the_map[rp_id];
      ++rp_hits_num_map[rp_id];

      if(tot_rp_det_id.IsStripsCoordinateUDirection())
      {
        std::vector<RPRecoHit> &vec_ref = pair_ref.first[det_id];
        vec_ref.push_back(*hits_it);
      }
      else
      {
        std::vector<RPRecoHit> &vec_ref = pair_ref.second[det_id];
        vec_ref.push_back(*hits_it);
      }
    }
  }

  rp_copy_no_hits_uv_map::const_iterator rp_cp_it;
  for(rp_cp_it = the_map.begin(); rp_cp_it != the_map.end(); rp_cp_it++)
  {
    if(verbosity_)
    {
      std::cout << "RP " << rp_cp_it->first << std::endl;
      std::cout << "===========================================" << std::endl;
      det_vec_reco_hits::const_iterator det_hits_it;
      for(det_hits_it = rp_cp_it->second.first.begin(); det_hits_it != rp_cp_it->second.first.end(); det_hits_it++)
      {
        std::cout << ">>> Det " << det_hits_it->first << " U-Hits = " << det_hits_it->second.size() << std::endl;
      }
      std::cout << "-------------------------------------------" << std::endl;
      for(det_hits_it = rp_cp_it->second.second.begin(); det_hits_it != rp_cp_it->second.second.end(); det_hits_it++)
      {
        std::cout << ">>> Det " << det_hits_it->first << " V-Hits = " << det_hits_it->second.size() << std::endl;
      }
      std::cout << "Total (U+V) = " << rp_hits_num_map[rp_cp_it->first] << std::endl;
    }
    if(rp_hits_num_map[rp_cp_it->first] < minimal_hits_count_per_rp_)
    {
      // Too few U and V hits to make a track candidate
      continue;
    }
    RPMulCandidateTrackFinderAlgorithm_.BuildTrackCandidates(rp_cp_it->first, rp_cp_it->second.first, rp_cp_it->second.second, output, rp_geometry);
  }
}


DEFINE_FWK_MODULE(RPMulCandidateTrackFinder);

