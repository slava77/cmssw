#include "RecoTotemRP/RPSingleCandidateTrackFinder/interface/RPSingleCandidateTrackFinderAlgorithm.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "HepMC/SimpleVector.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "TMath.h"
#include <iostream>

RPSingleCandidateTrackFinderAlgorithm::RPSingleCandidateTrackFinderAlgorithm(const edm::ParameterSet& conf)
 : conf_(conf)
{
  verbosity_ = conf.getParameter<int>("Verbosity");
  road_width_ = conf.getParameter<double>("RoadSize");  // [mm]
  minimal_hits_count_per_cooridinate_ = conf.getParameter<int>("MinHitsPerCoord");
  maximum_hits_multiplicity_per_det_ = conf.getParameter<int>("MaxHitsPerDetector");
  reduce_weights_with_multiplicity_ = conf.getParameter<bool>("ReduceWeightsWithMultiplicity");
}


void RPSingleCandidateTrackFinderAlgorithm::BuildSingleTrackCandidates(
    unsigned int rp_copy_no, 
    const std::vector<RPRecoHit> & u_hits, const std::vector<RPRecoHit> &v_hits,
    RPRecognizedPatternsCollection &patterns,
    RPTrackCandidateCollection& output, const TotemRPGeometry & rp_geometry)
{
  // run search per projection 
  std::vector< std::vector<RPRecoHit> > u_hits_clusters, v_hits_clusters;
  std::vector<double> u_hits_weights, v_hits_weights;
  
  patterns[rp_copy_no].source = RPRecognizedPatterns::sParallel;
/* 
  if (verbosity_ > 1)
    printf("\tRP %u\n", rp_copy_no);

  if (verbosity_ > 1)
    printf("\t\tU patterns\n");
*/
  FindRecoHitRoads(u_hits, u_hits_clusters, u_hits_weights, patterns[rp_copy_no].uLines, rp_geometry);
/*                                                                                       
  if (verbosity_ > 1)                                                                  
    printf("\t\tV patterns\n");                                                        
*/
  FindRecoHitRoads(v_hits, v_hits_clusters, v_hits_weights, patterns[rp_copy_no].vLines, rp_geometry);

  // number of patterns (= roads) with weight over threshold
  int u_multiplicity = CheckTrackMultiplicity(u_hits_weights);
  int v_multiplicity = CheckTrackMultiplicity(v_hits_weights);

  // produce final result
  bool activity = (u_hits.size() + v_hits.size())>=5;
  //std::cout<<"** activity:"<<activity<<std::endl;

  //std::cout<<"rp_copy_no="<<rp_copy_no<<std::endl;
  if (u_hits_weights.size()==0 || v_hits_weights.size()==0 || u_multiplicity!=1 || v_multiplicity!=1)
  {
    if(activity)
    {
      RPTrackCandidate tr_cand;
      tr_cand.Fittable(false);
      output[rp_copy_no] = tr_cand;
	  //std::cout << "Saved non-fittable RPTrackCandidate for ID = " << rp_copy_no << endl;
    }
  }
  else
  {
    unsigned int best_u_ind = MaxElementIndex(u_hits_weights);
    unsigned int best_v_ind = MaxElementIndex(v_hits_weights);
    RPTrackCandidate tr_cand;
    tr_cand.Fittable(true);
    tr_cand.InsertHits(u_hits_clusters[best_u_ind], u_hits_weights[best_u_ind]);
    tr_cand.InsertHits(v_hits_clusters[best_v_ind], v_hits_weights[best_v_ind]);
    output[rp_copy_no] = tr_cand;
	//std::cout << "Saved fittable RPTrackCandidate for ID = " << rp_copy_no << endl;
  }
  RPTrackCandidate::range rn = output[rp_copy_no].recHits();
  //std::cout<<"rpid:"<<rp_copy_no<<", strips: ";
  for(RPTrackCandidate::const_iterator it=rn.first; it!=rn.second; ++it)
  {
    //std::cout<<it->Position()<<", ";
  }
  //std::cout<<std::endl<<"weight:"<<output[rp_copy_no].Weight()<<std::endl;
  //std::cout<<"fitable:"<<output[rp_copy_no].Fittable()<<std::endl;
  //std::cout<<"size:"<<output[rp_copy_no].Size()<<std::endl;
}

/**
 * it's assumed, that vector is nonempty
 **/
unsigned int RPSingleCandidateTrackFinderAlgorithm::MaxElementIndex(const std::vector<double> &vect)
{
  double val = vect[0];
  unsigned int ind = 0;
  for(unsigned int i=1; i<vect.size(); ++i)
  {
    if(vect[i]>val)
    {
      val = vect[i];
      ind = i;
    }
  }
  return ind;
}

/**
 * Returns the number of roads with weight larger or equal to the chosen threshold
 **/
unsigned int RPSingleCandidateTrackFinderAlgorithm::CheckTrackMultiplicity(
    const std::vector<double> &vect)
{
  unsigned int track_mult = 0;
  for(unsigned int i=0; i<vect.size(); ++i)
  {
    if(vect[i]>=minimal_hits_count_per_cooridinate_)
      ++track_mult;
  }
//  std::cout<<"track_mult="<<track_mult<<std::endl;
  return track_mult;
}


/**
 * Input: U or V hits vector
 * Output: vector of roads and vector of corresponding weights
 * 		road is represented by a vector of hits belonging to the road
 **/ 
void RPSingleCandidateTrackFinderAlgorithm::FindRecoHitRoads(const std::vector<RPRecoHit> & hits, 
    std::vector< std::vector<RPRecoHit> > & hits_clusters, std::vector<double> &weights,
    vector<RPRecognizedPatterns::Line> &lines,
    const TotemRPGeometry & rp_geometry)
{
  //std::cout<<"FindRecoHitRoads: hits.size()="<<hits.size()<<std::endl;

  /// compute map<DetId, number of hits>
  det_hits_multiplicity_type det_hits_multiplicity;
  for(unsigned int i = 0; i<hits.size(); ++i)
  {
    ++(det_hits_multiplicity[hits[i].DetId()]);
  }
  
  /// main loop - traverse of all hits
  std::vector<RecoHitCluster> recohit_cluster_vect; 
  //std::cout<<"FindRecoHitRoads: grouping the hits..."<<std::endl;
  for(unsigned int i=0; i<hits.size(); ++i)
  {
		/// reject hits in noisy detectors
    if(det_hits_multiplicity[hits[i].DetId()] > maximum_hits_multiplicity_per_det_)
      continue;
      
		/// compute weight: w = 1/hits_in_plane
    double hit_weight = ComputeHitWeight(hits[i].DetId(), det_hits_multiplicity);
    
    if(hit_weight>0.0)
    {
      //std::cout<<"FindRecoHitRoads: add the hit: "<<std::endl;
      double hit_position = hits[i].Position() + GetDetStripAlignment(hits[i].DetId(), rp_geometry);
      unsigned int reco_hit_clust_v_siz = recohit_cluster_vect.size();
      unsigned int j;

	  /// traverse all roads saved in recohit_cluster_vect
      for(j=0; j<reco_hit_clust_v_siz; ++j)
      {
        //std::cout<<"clust dist:"<<TMath::Abs(recohit_cluster_vect[j].GetMeanPosition() - hit_position)<<std::endl;
		/// check whether the hit falls into the road
        if( TMath::Abs(recohit_cluster_vect[j].GetMeanPosition() - hit_position)<road_width_ )
          break;
      }
            
      if(reco_hit_clust_v_siz == 0 || j == reco_hit_clust_v_siz)
      {
		/// no road found -> add new road
        //std::cout<<"FindRecoHitRoads: hit added, new cluster: pos="<<hit_position<<", weight="<<hit_weight<<std::endl;
        RecoHitCluster new_recohit_cluster;
        new_recohit_cluster.AddHit(hits[i], hit_position, hit_weight);
        recohit_cluster_vect.push_back(new_recohit_cluster);
      }
      else
      {
		/// add this hit to an existing road
        //std::cout<<"FindRecoHitRoads: hit added, cluster id."<<j<<", pos="<<hit_position<<", weight="<<hit_weight<<std::endl;
        recohit_cluster_vect[j].AddHit(hits[i], hit_position, hit_weight);
      }
    }  //if
  } //for i
  
  
  // write out the results (conversion from vector<RecoHitCluster> to
  // vector< vector<RPRecoHit> > hits_clusters and vector<double> weights)
  // (save only those with weights over threshold)
  //std::cout<<"FindRecoHitRoads: scan the hit clusters..."<<std::endl;
  for(unsigned int i=0; i<recohit_cluster_vect.size(); ++i)
  {
    //std::cout<<"recohit_cluster_vect[i].GetHitsNumber()="<<recohit_cluster_vect[i].GetHitsNumber()<<std::endl;
    if(recohit_cluster_vect[i].GetHitsNumber()>=minimal_hits_count_per_cooridinate_)
    {
      //std::cout<<"Hits cluster selected, size="<<recohit_cluster_vect[i].GetRecHits().size()<<std::endl;
      hits_clusters.push_back(recohit_cluster_vect[i].GetRecHits());
      weights.push_back(recohit_cluster_vect[i].GetTotalWeight());

      RPRecognizedPatterns::Line line;
      line.a = 0.;
      line.b = recohit_cluster_vect[i].GetMeanPosition();
      line.w = recohit_cluster_vect[i].GetTotalWeight();
      line.hits = recohit_cluster_vect[i].GetRecHits();
      lines.push_back(line);
      //std::cout << "line a = "<< line.a << " line b = "<< line.b << " line w =" << line.w <<std::endl;
      //if (verbosity_ > 1)printf("\t\t\ta = %.3f, b = %.3f, w = %.3f\n", line.a, line.b, line.w);
    }
  }
}


double RPSingleCandidateTrackFinderAlgorithm::GetDetStripAlignment(unsigned int det_id, const TotemRPGeometry & rp_geometry)
{
	/// first look at the map of saved alignments
  strip_rough_alignment_map::iterator it = the_align_map_.find(det_id);
  if(it!=the_align_map_.end())
  {
    //std::cout<<"readout shift="<<it->second<<" det.id."<<TotRPDetId(det_id).DetectorDecId()<<std::endl;
    return it->second;
  }
  
	/// produce the alignment if not found above
  //std::cout<<"det_id="<<det_id<<std::endl;
  HepMC::ThreeVector readout_vect_ = det_topology_.GetStripReadoutAxisDir(); 
  CLHEP::Hep3Vector readout_vect_mc_;
  readout_vect_mc_.setX( readout_vect_.x());
  readout_vect_mc_.setY( readout_vect_.y());
  readout_vect_mc_.setZ( readout_vect_.z());
  CLHEP::Hep3Vector strip_dir = rp_geometry.LocalToGlobalDirection(det_id, readout_vect_mc_);
  //std::cout<<"readout_vect_ " << strip_dir <<std::endl;
  CLHEP::Hep3Vector det_translation = rp_geometry.GetDetTranslation(det_id);
  double readout_shift = 
    (strip_dir.x()*det_translation.x() + strip_dir.y()*det_translation.y())
        /TMath::Sqrt(strip_dir.x()*strip_dir.x()+strip_dir.y()*strip_dir.y());
        
  the_align_map_[det_id] = readout_shift;
  return readout_shift;
}

