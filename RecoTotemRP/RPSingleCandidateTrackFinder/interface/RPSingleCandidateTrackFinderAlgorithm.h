#ifndef RecoTotemRP_RPRecoHitProducer_RPSingleCandidateTrackFinderAlgorithm_h
#define RecoTotemRP_RPRecoHitProducer_RPSingleCandidateTrackFinderAlgorithm_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "Geometry/TotemRPDetTopology/interface/RPTopology.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <vector>
#include <map>
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatternsCollection.h"

class RPSingleCandidateTrackFinderAlgorithm
{
  public:
    int verbosity_;
    typedef std::map<unsigned int, int> det_hits_multiplicity_type;
    typedef std::map<unsigned int, double> strip_rough_alignment_map; // det 32 bit id vs. shift value

    RPSingleCandidateTrackFinderAlgorithm(const edm::ParameterSet& conf);

    void BuildSingleTrackCandidates(unsigned int rp_copy_no, 
        const std::vector<RPRecoHit> & u_hits, const std::vector<RPRecoHit> &v_hits,
        RPRecognizedPatternsCollection &patterns,
        RPTrackCandidateCollection& output, 
        const TotemRPGeometry & rp_geometry);
    
  private:

    class RecoHitCluster
    {
      public:
        RecoHitCluster() : pos_sum_(0.0), tot_weight_(0.0) {}
        inline const std::vector<RPRecoHit> &GetRecHits() {return hit_vect_;}
        //inline const std::vector<double> &GetRecHitsWeights() {return weight_vector_;}
        inline double GetMeanPosition() {return pos_sum_/tot_weight_;}
        inline double GetTotalWeight() {return tot_weight_;}
        inline int GetHitsNumber() {return hit_vect_.size();}
        
        void AddHit(const RPRecoHit &hit, double pos, double weight) 
        {
          hit_vect_.push_back(hit); /*weight_vector_.push_back(weight); */
          pos_sum_+=pos*weight; tot_weight_+=weight;
        }
        
      private:
        std::vector<RPRecoHit> hit_vect_;
        /*std::vector<double> weight_vector_;*/
        double pos_sum_;
        double tot_weight_;
    };

    inline double ComputeHitWeight(unsigned int det_id, det_hits_multiplicity_type &mult_map)
    {
      if (!reduce_weights_with_multiplicity_)
        return 1.;

      int count = mult_map[det_id];
      //std::cout<<"mult_map[det_id]="<<mult_map[det_id]<<std::endl;
      if(count>0)
        return 1.0/count;
      else 
        return 0;
    }
    
    unsigned int MaxElementIndex(const std::vector<double> &vect);
    unsigned int CheckTrackMultiplicity(const std::vector<double> &vect);
    double GetDetStripAlignment(unsigned int det_id, const TotemRPGeometry & rp_geometry);
    void FindRecoHitRoads(const std::vector<RPRecoHit> & hits, 
        std::vector< std::vector<RPRecoHit> > & hits_clusters, std::vector<double> &weights,
        vector<RPRecognizedPatterns::Line> &lines,
        const TotemRPGeometry & rp_geometry);
  
    const edm::ParameterSet& conf_;
    int minimal_hits_count_per_cooridinate_;
    int maximum_hits_multiplicity_per_det_;
    bool reduce_weights_with_multiplicity_;
    
    double road_width_;
    strip_rough_alignment_map the_align_map_;
    RPTopology det_topology_;
};

#endif

