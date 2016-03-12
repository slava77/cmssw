/****************************************************************************
 *
 * This module is directly copied from 
 * RecoTotemRP/RPMulCandidateTrackFinderAlgorithm and expanded to output 
 * multiple track candidates per event.
 * Original Authors:
 *   Hubert Niewiadomski (Hubert.Niewiadomski@cern.ch)
 * Secondary Authors:
 *   Zhang Zhengkui (zhang.zhengkui.fin@gmail.com)
 *
 ****************************************************************************/


#ifndef RecoTotemRP_RPMulCandidateTrackFinder_RPMulCandidateTrackFinderAlgorithm_h
#define RecoTotemRP_RPMulCandidateTrackFinder_RPMulCandidateTrackFinderAlgorithm_h


//edm
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//Data format
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulTrackCandidateCollection.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"

//Geometry
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "Geometry/TotemRPDetTopology/interface/RPTopology.h"

//C++ and STL
#include <iostream>
#include <limits>
#include <vector>
#include <map>
#include <functional>

//ROOT
#include "TFile.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TH2D.h"
#include "TLine.h"
#include "TMath.h"
#include "TRint.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"


class RPMulCandidateTrackFinderAlgorithm
{
  public:
    RPMulCandidateTrackFinderAlgorithm(const edm::ParameterSet& conf);
    void BuildTrackCandidates(unsigned int rp_copy_no, 
        const std::map<unsigned int, std::vector<RPRecoHit> > & det_u_hits, 
        const std::map<unsigned int, std::vector<RPRecoHit> > & det_v_hits, 
        RPMulTrackCandidateCollection& output, 
        const TotemRPGeometry & rp_geometry);
    void WriteAllPlots(TFile *of);

  private:

    typedef std::map<unsigned int, double> strip_rough_alignment_map; //det 32 bit id vs. shift value
    
    class RecoHitCluster
    {
      public:
        RecoHitCluster() : g_pos_sum_(0.0), l_pos_sum_(0.0) {}
        inline const std::vector<RPRecoHit> & GetRecHits() { return hit_vect_; }
        inline const std::map<unsigned int, int> & GetHitNumDetMap() { return det_nhit_map_; }
        inline double GetMeanPosition() { return g_pos_sum_ / hit_vect_.size(); }
        inline double GetMeanPositionLocal() { return l_pos_sum_ / hit_vect_.size(); }
        inline unsigned int GetHitsNumber() { return hit_vect_.size(); }
        inline unsigned int GetActiveDetsNumber() { return det_nhit_map_.size(); }

        void AddHit(const RPRecoHit &hit, double pos)
        {
          hit_vect_.push_back(hit);
          g_pos_sum_ += pos;
          l_pos_sum_ += hit.Position();
          ++det_nhit_map_[TotRPDetId(hit.DetId()).Detector()];
        }

      private:
        // the vector of all hits belonging to this cluster (or road)
        std::vector<RPRecoHit> hit_vect_;
        // the map of detector id (0 - 9) ==> hits number
        std::map<unsigned int, int, std::less<unsigned int> > det_nhit_map_;
        // the sum of all hits from this cluster (or road) to calculating the global mean position
        // of this road
        double g_pos_sum_;
        // the sum of all hits from this cluster (or road) to calculating the local mean position
        // of this road
        double l_pos_sum_;
    };

    double GetDetStripAlignment(unsigned int det_id, const TotemRPGeometry & rp_geometry);
    void FindRecoHitRoads(const std::map<unsigned int, std::vector<RPRecoHit> > & det_hits,
        std::vector< std::vector<RPRecoHit> > & hits_clusters,
        std::vector<double> & roads_mean,
        const TotemRPGeometry & rp_geometry);
    double CalcCandidateTrackWeight(const double u_mean, const double v_mean,
        const std::vector<RPRecoHit> & u_hits_vec,
        const std::vector<RPRecoHit> & v_hits_vec);
    void DrawRPPlaneEnvelope();
 
    const edm::ParameterSet& conf_;
    int verbosity_;
    double road_width_;
    unsigned int minimal_hits_count_per_road_;           // the minimal amount of (U/V) hits for building a valid (U/V) road
    unsigned int minimal_dets_count_per_road_;           // the minimal amount of detectors triggered for building a valid (U/V) road
    unsigned int maximum_hits_multiplicity_per_det_;     // the maximum allowable number of (U/V) hits per detector in order to filter out noisy shower case 
  
    strip_rough_alignment_map the_align_map_;
    RPTopology det_topology_;

    int output_;
    int outputHitsNumTree_;
    int outputRPHitsPlot_;
    int outputRPRoadsPlot_;
    int outputRPTracksPlot_;

    std::ostringstream oss;
    std::map<unsigned int, int> rp_hits_map;  // Roman Pot Id ==> index of hits information
    int u_nhits_det_array[24][5];      // number of U hits on five detectors for each Roman Pot array
    int v_nhits_det_array[24][5];      // number of V hits on five detectors for each Roman Pot array
    TTree *u_nhits_det_trees[24];      // number of U hits on five detectors for each Roman Pot trees
    TTree *v_nhits_det_trees[24];      // number of V hits on five detectors for each Roman Pot trees
    TH1I *u_nhits_det_hist[24];        // number of U hits per detector for each Roman Pot histograms
    TH1I *v_nhits_det_hist[24];        // number of V hits per detector for each Roman Pot histograms
    TH1D *u_hits_pos_rp_hist[24];      // U hits position distribution in local coordinate for each Roman Pot histogram
    TH1D *v_hits_pos_rp_hist[24];      // V hits position distribution in local coordinate for each Roman Pot histogram
    TH1I *u_nroads_rp_hist[24];        // total number of U roads for each Roman Pot histograms
    TH1I *v_nroads_rp_hist[24];        // total number of V roads for each Roman Pot histograms
    TH2I *uv_nroads_pair_rp_hist[24];  // (U,V) roads number pair for each Roman Pot 2D histograms
    TH2D *track_pos_rp_hist[24][3];    // Track position distribution histgrams, [..][0] U = V = 1, [..][1] U = V > 1, [..][2] U != V
};


#endif
