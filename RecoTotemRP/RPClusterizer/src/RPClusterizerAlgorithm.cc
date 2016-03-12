#include "RecoTotemRP/RPClusterizer/interface/RPClusterizerAlgorithm.h"
#include <iostream>


RPClusterizerAlgorithm::RPClusterizerAlgorithm(const edm::ParameterSet &param)
 :param_(param)
{
  verbosity_ = param_.getParameter<int>("Verbosity");
}

RPClusterizerAlgorithm::~RPClusterizerAlgorithm()
{
}

//void RPClusterizerAlgorithm::Reset()
//{
//	strip_digi_set_.clear();
////  strip_links_map_.clear();
//  clusters_.clear();
////  links_.clear();
//  if(verbosity_)
//  {
//    std::cout<<"RPClusterizerAlgorithm: reset done"<<std::endl;
//  }
//}

//void RPClusterizerAlgorithm::SetDigi(const std::vector<RPStripDigi> &digi)
//{
//  //Reset();
//  strip_digi_set_.insert( digi.begin(), digi.end() );
//}

int RPClusterizerAlgorithm::BuildClusters(const std::vector<RPStripDigi> &digi, std::vector<RPDigCluster> &clusters)
{
  strip_digi_set_.clear();
  clusters.clear();
  strip_digi_set_.insert( digi.begin(), digi.end() );
//  links_.clear();
  if(strip_digi_set_.size()==0)
    return 0;
    
  bool iter_beg=true;
  int cluster_beg=-16;
  int cluster_end;
  int prev_strip=-16;
  int cur_strip;
  
//  ResetInternalHitLinkMap();

  RPDetId rp_det_id = 0;
  if(strip_digi_set_.begin()!=strip_digi_set_.end())
    rp_det_id = strip_digi_set_.begin()->GetDetId();

  for(RPStripDigiSet::const_iterator i=strip_digi_set_.begin(); i!=strip_digi_set_.end(); ++i)
  {
    cur_strip=i->GetStripNo();
    bool non_continuity = (cur_strip!=prev_strip+1);
    
    if(iter_beg)
    {
      cluster_beg=cur_strip;
      iter_beg=false;
//      AddLinksToInternalHitsMap(cur_strip);
    }
    else if(non_continuity)
    {
      cluster_end=prev_strip;
      clusters.push_back(RPDigCluster(rp_det_id, (unsigned short)cluster_beg, 
      (unsigned short) cluster_end));
      
//    PushBackCurrentClusterLinks();
//    ResetInternalHitLinkMap();
//    AddLinksToInternalHitsMap(cur_strip);

      cluster_beg=cur_strip;
    }
//    else
//    {
//      AddLinksToInternalHitsMap(cur_strip);
//    }
    
    prev_strip=cur_strip;
  }
  
  if(!iter_beg)
  {
    cluster_end=prev_strip;
    clusters.push_back(RPDigCluster(rp_det_id, (unsigned short)cluster_beg, 
    (unsigned short) cluster_end));
//    PushBackCurrentClusterLinks();
  }
  if(verbosity_)
  {
    for(unsigned int i=0; i<clusters.size(); ++i)
    {
      std::cout<<"Cluster, DetId="<<clusters[i].DetId()<<" StrBeg="<<clusters[i].StrBeg()
          <<" StrEnd="<<clusters[i].StrEnd()<<" Pos="<<clusters[i].CentreStripPos()<<std::endl;
    }
  }
    
  return clusters.size();
}

//double RPClusterizerAlgorithm::GetClusterSpread() const //in strips
//{
//  int clu_no = clusters_.size();
//  if(clu_no<=1)
//    return 0;
//
//  double min_pos=1000;
//  double max_pos=-1;
//  for(int i=0; i<clu_no; ++i)
//  {
//    if(clusters_[i].Pos()<min_pos)
//      min_pos = clusters_[i].Pos();
//    if(clusters_[i].Pos()>max_pos)
//      max_pos = clusters_[i].Pos();
//  }
//  return max_pos - min_pos;
//  //return _strip_digi_set.rbegin()->GetStripNo() - _strip_digi_set.begin()->GetStripNo() + 1;
//  
////    
////  double sum_of_pos_sq = 0;
////  double sum_of_pos = 0;
////  for(int i=0; i<clu_no; ++i)
////  {
////    double pos = clusters_[i].Pos();
////    sum_of_pos += pos;
////    sum_of_pos_sq += pos*pos;
////  }
////  return sqrt( sum_of_pos_sq/clu_no - sum_of_pos*sum_of_pos/clu_no/clu_no );
//}
//
//
//double RPClusterizerAlgorithm::GetClusterSpreadSigma() const //in strips
//{
//  int clu_no = clusters_.size();
//  if(clu_no<=1)
//    return 0;
// 
//  double sum_of_pos_sq = 0;
//  double sum_of_pos = 0;
//  for(int i=0; i<clu_no; ++i)
//  {
//    double pos = clusters_[i].Pos();
//    sum_of_pos += pos;
//    sum_of_pos_sq += pos*pos;
//  }
//  return sqrt( sum_of_pos_sq/clu_no - sum_of_pos*sum_of_pos/clu_no/clu_no );
//}

