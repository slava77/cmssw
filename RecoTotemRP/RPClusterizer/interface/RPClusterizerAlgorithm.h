#ifndef RecoTotemRP_RPClusterizer_RPClusterizerAlgorithm_h_
#define RecoTotemRP_RPClusterizer_RPClusterizerAlgorithm_h_

#include <vector>
#include <set>
#include "DataFormats/TotemRPDataTypes/interface/RPStripDigi.h"
#include "DataFormats/TotemRPDataTypes/interface/RPDigCluster.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include "TVector3.h"
#include <iostream>

class RPClusterizerAlgorithm
{
  public:
    RPClusterizerAlgorithm(const edm::ParameterSet& param);
    ~RPClusterizerAlgorithm();
    
    int BuildClusters(const std::vector<RPStripDigi> &digi, std::vector<RPDigCluster> &clusters);
    
//    inline int GetClusterMultiplicity() const {return clusters_.size();};
//    double GetClusterSpread() const; //in strips
//    double GetClusterSpreadSigma() const; //in strips

  private:
    void Reset();
    typedef std::set<RPStripDigi> RPStripDigiSet;
    RPStripDigiSet strip_digi_set_;  //input digi set, strip by strip
//    typedef map<int, RPTypes::SingleDigiPrimaryMapType > RPStripLinksMap;
//    RPStripLinksMap strip_links_map_;  //input digi links, strip by strip
    
    typedef std::vector<RPDigCluster> RP_Cluster_Map;
//    RP_Cluster_Map clusters_;  //std::vector of clusters produced for the current event
    
//    typedef std::vector<RPTypes::SingleDigiPrimaryMapType> RP_Cluster_Primary_Links_Map;
//    RP_Cluster_Primary_Links_Map links_;  //links for the clusters found in the current event
    
//    typedef std::map<int, double> internal_link_map_type;
//    internal_link_map_type internal_cluster_link_map; //psimhit to charge generated from it
    //used on the basis of single event only, reset every event
    
    const edm::ParameterSet &param_;
    int verbosity_;
    
//    inline void AddLinksToInternalHitsMap(int strip_no)
//    {
//      RPTypes::SingleDigiPrimaryMapType::iterator it = striplinks__map_[strip_no].begin();
//      RPTypes::SingleDigiPrimaryMapType::iterator it_end = striplinks__map_[strip_no].end();
//      for(; it!=it_end; ++it)
//      {
//        internal_cluster_link_map[it->first] += it->second;
//      }
//    };
    
//    inline void ResetInternalHitLinkMap() {internal_cluster_link_map.clear();};
//    inline void PushBackCurrentClusterLinks()
//    {
//      internal_link_map_type::const_iterator it_beg = internal_cluster_link_map.begin();
//      internal_link_map_type::const_iterator it_end = internal_cluster_link_map.end();
//      links_.push_back(RPTypes::SingleDigiPrimaryMapType(it_beg, it_end));
//    }
};

#endif  //RecoTotemRP_RPClusterizer_RPClusterizerAlgorithm_h_
