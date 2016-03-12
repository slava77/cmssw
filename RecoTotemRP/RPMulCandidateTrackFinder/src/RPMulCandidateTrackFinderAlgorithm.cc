#include "RecoTotemRP/RPMulCandidateTrackFinder/interface/RPMulCandidateTrackFinderAlgorithm.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "HepMC/SimpleVector.h"


RPMulCandidateTrackFinderAlgorithm::RPMulCandidateTrackFinderAlgorithm(const edm::ParameterSet& conf)
 : conf_(conf)
{
  verbosity_ = conf.getParameter<int>("Verbosity");
  road_width_ = conf.getParameter<double>("RoadSize");  // [mm]
  minimal_hits_count_per_road_ = conf.getParameter<unsigned int>("MinHitsPerRoad");
  minimal_dets_count_per_road_ = conf.getParameter<unsigned int>("MinDetsPerRoad");
  maximum_hits_multiplicity_per_det_ = conf.getParameter<unsigned int>("MaxHitsPerDetector");

  std::vector<unsigned int> rpList = conf.getParameter< std::vector<unsigned int> >("RPList");
  output_ = conf.getParameter<int>("Output");
  outputHitsNumTree_ = conf.getParameter<int>("ProduceHitsNumTree");
  outputRPHitsPlot_ = conf.getParameter<int>("ProduceRPHitsPlot");
  outputRPRoadsPlot_ = conf.getParameter<int>("ProduceRPRoadsPlot");
  outputRPTracksPlot_ = conf.getParameter<int>("ProduceRPTracksPlot");

  if(output_)
  {
    for(unsigned int i = 0; i < rpList.size(); i++)
    {
      // initialize RP_ID ==> Hits_Information_Index map
      rp_hits_map[rpList[i]] = i;

      // initialize u/v_nhits_det_trees and create branches for each detector
      if(outputHitsNumTree_)
      {
        // create U Hit Num RP Tree
        oss.str("");
        oss << "U_Hit_Num_Tree_RP_" << std::setw(3) << std::setfill('0') << rpList[i];
        u_nhits_det_trees[i] = new TTree(oss.str().c_str(), "U Hit Num");
        // create V Hit Num RP Tree
        oss.str("");
        oss << "V_Hit_Num_Tree_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        v_nhits_det_trees[i] = new TTree(oss.str().c_str(), "V Hit Num");
        // create branches for each detector of this Roman Pot
        for(int j = 0; j < 5; j++)
        {
          oss.str("");
          oss << "U_DET" << 2 * j + 1;
          u_nhits_det_trees[i]->Branch(oss.str().c_str(), &u_nhits_det_array[i][j], (oss.str() + "/I").c_str());
          oss.str("");
          oss << "V_DET" << 2 * j;
          v_nhits_det_trees[i]->Branch(oss.str().c_str(), &v_nhits_det_array[i][j], (oss.str() + "/I").c_str());;
        }
      }

      // initialize u/v_avg_nhits_det_hist and u/v_hits_pos_rp_hist
      if(outputRPHitsPlot_)
      {
        oss.str("");
        oss << "U_Hit_Num_Per_Det_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        u_nhits_det_hist[i] = new TH1I(oss.str().c_str(), "Number of U hits per detector", 
            maximum_hits_multiplicity_per_det_, 0, maximum_hits_multiplicity_per_det_); 
        oss.str("");
        oss << "V_Hit_Num_Per_Det_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        v_nhits_det_hist[i] = new TH1I(oss.str().c_str(), "Number of V hits per detector", 
            maximum_hits_multiplicity_per_det_, 0, maximum_hits_multiplicity_per_det_);
        oss.str("");
        oss << "U_Hit_Pos_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        u_hits_pos_rp_hist[i] = new TH1D(oss.str().c_str(), "U hits postion distribution", 500, -20.0, 20.0);
        oss.str("");
        oss << "V_Hit_Pos_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        v_hits_pos_rp_hist[i] = new TH1D(oss.str().c_str(), "V hits postion distribution", 500, -20.0, 20.0);
      }

      // initialize u/v_nroads_rp_hist and uv_nroads_pair_rp_hist
      if(outputRPRoadsPlot_)
      {
        oss.str("");
        oss << "U_Road_Total_Num_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        u_nroads_rp_hist[i] = new TH1I(oss.str().c_str(), "Total number of U roads", 
            maximum_hits_multiplicity_per_det_, 0, maximum_hits_multiplicity_per_det_);
        oss.str("");
        oss << "V_Road_Total_Num_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        v_nroads_rp_hist[i] = new TH1I(oss.str().c_str(), "Total number of V roads", 
            maximum_hits_multiplicity_per_det_, 0, maximum_hits_multiplicity_per_det_);
        oss.str("");
        oss << "UV_Road_Pair_Hist_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        uv_nroads_pair_rp_hist[i] = new TH2I(oss.str().c_str(), "(U,V) roads pair", 
            maximum_hits_multiplicity_per_det_, 0, maximum_hits_multiplicity_per_det_,
            maximum_hits_multiplicity_per_det_, 0, maximum_hits_multiplicity_per_det_);
      }

      // initialize track_pos_rp_dist
      if(outputRPTracksPlot_)
      {
        oss.str("");
        oss << "Track_Pos_UeqVeq1_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        track_pos_rp_hist[i][0] = new TH2D(oss.str().c_str(), "Candidate track position distribution (U = V = 1)", 500, -20., +20., 500, -20., +20.);
        oss.str("");
        oss << "Track_Pos_UeqVgt1_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        track_pos_rp_hist[i][1] = new TH2D(oss.str().c_str(), "Candidate track position distribution (U = V > 1)", 500, -20., +20., 500, -20., +20.);
        oss.str("");
        oss << "Track_Pos_UneqV_RP" << std::setw(3) << std::setfill('0') << rpList[i];
        track_pos_rp_hist[i][2] = new TH2D(oss.str().c_str(), "Candidate track position distribution (U != V)", 500, -20., +20., 500, -20., +20.);
      }
    } // for(;;) 
  } // if(output_)
}


/// ---------------------------------------------------------------------------------------------------

void RPMulCandidateTrackFinderAlgorithm::BuildTrackCandidates(
    unsigned int rp_copy_no, 
    const std::map<unsigned int, std::vector<RPRecoHit> > & det_u_hits, 
    const std::map<unsigned int, std::vector<RPRecoHit> > & det_v_hits, 
    RPMulTrackCandidateCollection& output, 
    const TotemRPGeometry & rp_geometry)
{
  std::vector< std::vector<RPRecoHit> > u_roads, v_roads;    // hit vector for U and V roads
  std::vector<double> u_roads_mean, v_roads_mean;            // local mean position for U and V roads

  // Find roads from U and V hits separately
  FindRecoHitRoads(det_u_hits, u_roads, u_roads_mean, rp_geometry);
  FindRecoHitRoads(det_v_hits, v_roads, v_roads_mean, rp_geometry);

  // build candidate tracks from pairs of U/V roads (hits clusters)
  if(u_roads.size() != 0 && v_roads.size() != 0)
  {
    for(unsigned int i = 0; i < u_roads.size(); i++)
    {
      for(unsigned int j = 0; j < v_roads.size(); j++)
      {
        // check if the virtual track built from U/V roads can exist in the 
        // sensitive area of a detector
        if(!det_topology_.IsHit(u_roads_mean[i], v_roads_mean[j]))
        {
          if(verbosity_)
          {
            std::cout << "ignore on candidate track" << std::endl;
          }
          continue;
        }
        // calculate the reliability of a certain U&V candidate track
        double weight = CalcCandidateTrackWeight(u_roads_mean[i], v_roads_mean[j], u_roads[i], v_roads[j]);
        RPTrackCandidate tr_cand;
        tr_cand.InsertHits(u_roads[i], 0);
        tr_cand.InsertHits(v_roads[j], 0);
        tr_cand.Weight(weight);
        tr_cand.SetUVid(i,j);
        //TODO: build the candidate track if the weight satisfies a certain condition
        output[rp_copy_no].push_back(tr_cand); 
      }
    }
  }

  if(verbosity_)
  {
    std::cout << "RP " << rp_copy_no
              << "  U hits clusters: " << u_roads.size()
              << "  V hits clusters: " << v_roads.size() << std::endl;
  }

  if(output_)
  {
    std::map<unsigned int, std::vector<RPRecoHit> >::const_iterator u_det_hits_it, v_det_hits_it;
    std::vector<RPRecoHit>::const_iterator hits_it;
    int idx = rp_hits_map[rp_copy_no];
    int u_nhits = 0;
    int v_nhits = 0;
    TH2D *tr_hist = NULL;

    for(unsigned int i = 0; i < 5; i++)
    {
      // U Detector
      u_det_hits_it = det_u_hits.find(2*i+1);
      u_nhits = ((u_det_hits_it == det_u_hits.end()) ? 0 : (u_det_hits_it->second).size());  // assume the present detectors always has hits
      // V Detector
      v_det_hits_it = det_v_hits.find(2*i);
      v_nhits = ((v_det_hits_it == det_v_hits.end()) ? 0 : (v_det_hits_it->second).size());  // assume the present detectors always has hits

      if(outputHitsNumTree_)
      {
        u_nhits_det_array[idx][i] = u_nhits;
        v_nhits_det_array[idx][i] = v_nhits;
      }

      if(outputRPHitsPlot_)
      {
        if(u_det_hits_it != det_u_hits.end())
        {
          u_nhits_det_hist[idx]->Fill((u_det_hits_it->second).size());
          for(hits_it = (u_det_hits_it->second).begin(); hits_it != (u_det_hits_it->second).end(); hits_it++)
          {
            u_hits_pos_rp_hist[idx]->Fill(hits_it->Position());
          }
        }
        if(v_det_hits_it != det_v_hits.end())
        {
          v_nhits_det_hist[idx]->Fill((v_det_hits_it->second).size());
          for(hits_it = (v_det_hits_it->second).begin(); hits_it != (v_det_hits_it->second).end(); hits_it++)
          {
            v_hits_pos_rp_hist[idx]->Fill(hits_it->Position());
          }
        }
      }
    }

    if(outputHitsNumTree_)
    {
      u_nhits_det_trees[idx]->Fill();
      v_nhits_det_trees[idx]->Fill();
    }

    if(outputRPRoadsPlot_)
    {
      unsigned int u_nroads = u_roads.size();
      unsigned int v_nroads = v_roads.size();
      if(u_nroads != 0)
      {
        u_nroads_rp_hist[idx]->Fill(u_nroads);
      }
      if(v_nroads != 0)
      {
        v_nroads_rp_hist[idx]->Fill(v_nroads);
      }
      if(u_nroads != 0 || v_nroads != 0)
      {
        uv_nroads_pair_rp_hist[idx]->Fill(u_nroads, v_nroads);
      }
    }

    if(outputRPTracksPlot_)
    {
      if(u_roads.size() != 0 && v_roads.size() != 0)
      {
        if(u_roads.size() == v_roads.size())
        {
          tr_hist = ((u_roads.size() == 1) ? track_pos_rp_hist[idx][0] : track_pos_rp_hist[idx][1]);
        }
        else
        {
          tr_hist = track_pos_rp_hist[idx][2];
        }
        for(unsigned int ui = 0; ui < u_roads.size(); ui++)
        {
          for(unsigned int vi = 0; vi < v_roads.size(); vi++)
          {
            tr_hist->Fill(u_roads_mean[ui], v_roads_mean[vi]);
          }
        }
      }
    }
  }
}


/// ---------------------------------------------------------------------------------------------------

void RPMulCandidateTrackFinderAlgorithm::WriteAllPlots(TFile *of)
{
  // write all plots into different folders
  if(output_)
  {
    TDirectory *dir;
    std::map<unsigned int, int>::const_iterator it;
    int idx;
 
    if(outputHitsNumTree_)
    {
      dir = of->mkdir("HitsNumDetTrees", "Hits Number Per Detector for each Roman Pot");
      for(unsigned int i = 0; i < rp_hits_map.size(); i++)
      {
        dir->WriteTObject(u_nhits_det_trees[i]);
        dir->WriteTObject(v_nhits_det_trees[i]);
      }
    }

    if(outputRPHitsPlot_)
    {
      dir = of->mkdir("HitsRPHists", "Hits Number and Distribution for each Roman Pot");
      for(it = rp_hits_map.begin(); it != rp_hits_map.end(); it++)
      {
        oss.str("");
        oss << "RP" << std::setw(3) << std::setfill('0') << it->first << "_hits_hists";
        idx = rp_hits_map[it->first];
        TCanvas *c4h = new TCanvas(oss.str().c_str(), "Hits Number and Distribution", 10, 10, 800, 800);
        c4h->SetFillColor(41);
        c4h->Divide(2, 2);
        // in top left pad, draw the U hits number per RP
        c4h->cd(1);
        u_nhits_det_hist[idx]->Draw();
        // in top right pad, draw the V hits number per RP
        c4h->cd(2);
        v_nhits_det_hist[idx]->Draw();
        // in bottom left pad, draw the U hits distribution per RP
        c4h->cd(3);
        u_hits_pos_rp_hist[idx]->Draw();
        // in bottom right pad, draw the V hits distribution per RP
        c4h->cd(4);
        v_hits_pos_rp_hist[idx]->Draw();
        // write this canvas
        dir->WriteTObject(c4h);
      }
    }

    if(outputRPRoadsPlot_)
    {
      dir = of->mkdir("RoadsRPHists", "Roads Number and Pair for each Roman Pot");
      for(it = rp_hits_map.begin(); it != rp_hits_map.end(); it++)
      {
        oss.str("");
        oss << "RP" << std::setw(3) << std::setfill('0') << it->first << "_roads_hists";
        idx = rp_hits_map[it->first];
        TCanvas *c3h = new TCanvas(oss.str().c_str(), "Roads Number and Pair", 10, 10, 900, 600);
        TPad *pad1 = new TPad("pad1", "U Roads Number", 0.01, 0.51, 0.32, 0.99, 21);
        TPad *pad2 = new TPad("pad2", "V Roads Number", 0.01, 0.01, 0.32, 0.49, 21);
        TPad *pad3 = new TPad("pad3", "(U,V) Roads Pair", 0.34, 0.01, 0.99, 0.99, 41);
        pad1->Draw();
        pad2->Draw();
        pad3->Draw();
        // in the 1st pad, draw the U roads number per RP
        pad1->cd();
        u_nroads_rp_hist[idx]->Draw();
        // in the 2nd pad, draw the V roads number per RP
        pad2->cd();
        v_nroads_rp_hist[idx]->Draw();
        // in the 3rd pad, draw the (U,V) roads pair per RP
        pad3->cd();
        pad3->SetGrid();
        uv_nroads_pair_rp_hist[idx]->GetXaxis()->SetTitle("U Roads");
        uv_nroads_pair_rp_hist[idx]->GetYaxis()->SetTitle("V Roads");
        uv_nroads_pair_rp_hist[idx]->Draw("text");
        // write this canvas
        dir->WriteTObject(c3h);
      }
    }

    if(outputRPTracksPlot_)
    {
      dir = of->mkdir("TracksRPHists", "Track Position Distribution for each Roman Pot");
      for(it = rp_hits_map.begin(); it != rp_hits_map.end(); it++)
      {
        oss.str("");
        oss << "RP" << std::setw(3) << std::setfill('0') << it->first << "_track_hist";
        idx = rp_hits_map[it->first];
        TCanvas *c2h = new TCanvas(oss.str().c_str(), "Track Position Distribution", 10, 10, 800, 800);
        c2h->SetFillColor(41);
        c2h->Divide(2, 2);
        // in the top left pad, draw the track position distribution (U = V = 1)
        c2h->cd(1);
        track_pos_rp_hist[idx][0]->GetXaxis()->SetTitle("V");
        track_pos_rp_hist[idx][0]->GetYaxis()->SetTitle("U");
        track_pos_rp_hist[idx][0]->SetMarkerColor(1);
        track_pos_rp_hist[idx][0]->SetMarkerStyle(20);
        track_pos_rp_hist[idx][0]->SetMarkerSize(0.3);
        track_pos_rp_hist[idx][0]->Draw();
        DrawRPPlaneEnvelope();
        // in the top right pad, draw the track position distribution (U = V > 1)
        c2h->cd(2); 
        track_pos_rp_hist[idx][1]->GetXaxis()->SetTitle("V");
        track_pos_rp_hist[idx][1]->GetYaxis()->SetTitle("U");
        track_pos_rp_hist[idx][1]->SetMarkerColor(2);
        track_pos_rp_hist[idx][1]->SetMarkerStyle(20);
        track_pos_rp_hist[idx][1]->SetMarkerSize(0.3);
        track_pos_rp_hist[idx][1]->Draw();
        DrawRPPlaneEnvelope();
        // in the bottom left pad, draw the track position distribution (U != V)
        c2h->cd(3);
        track_pos_rp_hist[idx][2]->GetXaxis()->SetTitle("V");
        track_pos_rp_hist[idx][2]->GetYaxis()->SetTitle("U");
        track_pos_rp_hist[idx][2]->SetMarkerColor(6);
        track_pos_rp_hist[idx][2]->SetMarkerStyle(20);
        track_pos_rp_hist[idx][2]->SetMarkerSize(0.3);
        track_pos_rp_hist[idx][2]->Draw();
        DrawRPPlaneEnvelope();
        // write this canvas
        dir->WriteTObject(c2h);
      } 
    }
  }
}


/// ---------------------------------------------------------------------------------------------------

void RPMulCandidateTrackFinderAlgorithm::FindRecoHitRoads(const std::map<unsigned int, std::vector<RPRecoHit> > & det_hits, 
    std::vector< std::vector<RPRecoHit> > & hits_clusters, std::vector<double> & roads_mean, const TotemRPGeometry & rp_geometry)
{
  std::vector<RecoHitCluster> recohit_cluster_vect;
  std::map<unsigned int, std::vector<RPRecoHit> >::const_iterator det_hits_it;
  std::vector<RPRecoHit>::const_iterator hits_it;

  for(det_hits_it = det_hits.begin(); det_hits_it != det_hits.end(); det_hits_it++)
  {
    // reject hits in too noisy detectors such as when shower happens
    if((det_hits_it->second).size() > maximum_hits_multiplicity_per_det_)
    {
      continue;
    }

    for(hits_it = (det_hits_it->second).begin(); hits_it != (det_hits_it->second).end(); hits_it++)
    {
      double hit_position = hits_it->Position() + GetDetStripAlignment(hits_it->DetId(), rp_geometry);
      if(verbosity_)
      {
        std::cout << "Det " << det_hits_it->first <<"    Orig Hit Pos = " << hits_it->Position() << "    Align Hit Pos = " << hit_position << "    aligh = " << GetDetStripAlignment(hits_it->DetId(), rp_geometry) << std::endl;
      }
      unsigned int reco_hit_clust_v_siz = recohit_cluster_vect.size();
      unsigned int j = 0;
      // the minimun distance from the hit to a certain road
      double min_dist = std::numeric_limits<double>::max(); 
      // the index of certain road which is the nearest away from this hit
      int idx = -1;    

      // traverse all roads saved in recohit_cluster_vect and group this
      // hit to the road which is the nearest away from the this hit.
      for(j = 0; j < reco_hit_clust_v_siz; j++)
      {
        // check whether the hit falls into a road
        double dist = TMath::Abs(recohit_cluster_vect[j].GetMeanPosition() - hit_position);
        if(dist < road_width_ && min_dist > dist)
        {
          min_dist = dist;
          idx = j;
        }
      }

      if(idx == -1)
      {
        // no road found -> add new road
        RecoHitCluster new_recohit_cluster;
        new_recohit_cluster.AddHit(*hits_it, hit_position);
        recohit_cluster_vect.push_back(new_recohit_cluster);
      }
      else
      {
        // add this hit to an existing road
        recohit_cluster_vect[idx].AddHit(*hits_it, hit_position);
      }
    }
  }

  // travel through hits_cluster_vector which holds all roads
  // and select all valid roads which satisfy the following conditions: 
  // [1] there are enough hits on this road
  // [2] there are enough detectors triggered on this road
  for(unsigned int i = 0; i < recohit_cluster_vect.size(); i++)
  {
    if(recohit_cluster_vect[i].GetHitsNumber() >= minimal_hits_count_per_road_ &&
       recohit_cluster_vect[i].GetActiveDetsNumber() >= minimal_dets_count_per_road_)
    {
      if(verbosity_)
      {
        std::cout << "Road " << i << ":  Mean Pos Local = " << recohit_cluster_vect[i].GetMeanPositionLocal() << "  Mean Pos Global = " << recohit_cluster_vect[i].GetMeanPosition()  << std::endl;
      }
      hits_clusters.push_back(recohit_cluster_vect[i].GetRecHits());
      roads_mean.push_back(recohit_cluster_vect[i].GetMeanPositionLocal());
    }
  }
}


/// ---------------------------------------------------------------------------------------------------

double RPMulCandidateTrackFinderAlgorithm::GetDetStripAlignment(unsigned int det_id, const TotemRPGeometry & rp_geometry)
{
  // first look at the map of saved alignments
  strip_rough_alignment_map::iterator it = the_align_map_.find(det_id);
  if(it!=the_align_map_.end())
  {
    return it->second;
  }
  
  // produce the alignment if not found above
  HepMC::ThreeVector readout_vect_ = det_topology_.GetStripReadoutAxisDir(); 
  CLHEP::Hep3Vector readout_vect_mc_;
  readout_vect_mc_.setX( readout_vect_.x());
  readout_vect_mc_.setY( readout_vect_.y());
  readout_vect_mc_.setZ( readout_vect_.z());
  CLHEP::Hep3Vector strip_dir = rp_geometry.LocalToGlobalDirection(det_id, readout_vect_mc_);
  CLHEP::Hep3Vector det_translation = rp_geometry.GetDetTranslation(det_id);
  double readout_shift = 
    (strip_dir.x()*det_translation.x() + strip_dir.y()*det_translation.y())
        /TMath::Sqrt(strip_dir.x()*strip_dir.x()+strip_dir.y()*strip_dir.y());
        
  the_align_map_[det_id] = readout_shift;
  return readout_shift;
}


/// ---------------------------------------------------------------------------------------------------

// Calculate the reliability of a certain candidate track from a pair of U/V
// hits clusters (or roads).
double RPMulCandidateTrackFinderAlgorithm::CalcCandidateTrackWeight(const double u_mean, const double v_mean,
    const std::vector<RPRecoHit> & u_hits_vec,
    const std::vector<RPRecoHit> & v_hits_vec)
{
  //TODO: Calculate reliability based on the hit distribution probability density function from 
  //      simulation and real data
  return 0;
}


/// ---------------------------------------------------------------------------------------------------

// Draw Roman Pot Plane Envelope
void RPMulCandidateTrackFinderAlgorithm::DrawRPPlaneEnvelope()
{
  CLHEP::Hep3Vector pA,pB,pC,pD,pE;
  double half_len = 18.0325; // half length of detector's edge
  double cut = 20.25338527;  // length of the edge adjacent to the cut

  pA.setX(half_len);
  pA.setY(-half_len);

  pB.setX(half_len);
  pB.setY(half_len);

  pC.setX(-half_len);
  pC.setY(half_len);

  pD.setX(-half_len);
  pD.setY(half_len-cut);

  pE.setX(half_len-cut);
  pE.setY(-half_len);

  TLine *lAB = new TLine(pA.x(), pA.y(), pB.x(), pB.y());
  lAB->SetLineColor(4);
  lAB->Draw();
  TLine *lBC = new TLine(pB.x(), pB.y(), pC.x(), pC.y());
  lBC->SetLineColor(4);
  lBC->Draw();
  TLine *lCD = new TLine(pC.x(), pC.y(), pD.x(), pD.y());
  lCD->SetLineColor(4);
  lCD->Draw();
  TLine *lDE = new TLine(pD.x(), pD.y(), pE.x(), pE.y());
  lDE->SetLineColor(4);
  lDE->Draw();
  TLine *lEA = new TLine(pE.x(), pE.y(), pA.x(), pA.y());
  lEA->SetLineColor(4);
  lEA->Draw();
}

