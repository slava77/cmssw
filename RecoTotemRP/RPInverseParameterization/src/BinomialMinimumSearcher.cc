#include "RecoTotemRP/RPInverseParameterization/interface/BinomialMinimumSearcher.h"

BinomialMinimumSearcher::BinomialMinimumSearcher(double xi_min, double xi_max,
    double radom_output_prob, TRandom2 & rand_engine) :
  rand_engine_(rand_engine)
{
  xi_min_=xi_min;
  xi_max_=xi_max;
  minimum_element_ = xi_chi2_ndf_map_.end();
  radom_output_prob_ = radom_output_prob;
}

void BinomialMinimumSearcher::InsertElement(double xi, double chisqndf)
{
  double cur_xi_min;
  double cur_chisqndf_min;
  
  if (minimum_element_!=xi_chi2_ndf_map_.end())
  {
//    std::cout<<"Current min. element xi="<<minimum_element_->first
//        <<" chisqndf="<<minimum_element_->second<<std::endl;
//    std::cout<<"New element inserted xi="<<xi<<" chisqndf="<<chisqndf
//        <<std::endl;
    
    cur_xi_min = minimum_element_->first;
    cur_chisqndf_min = minimum_element_->second;
    
    xi_chi2_ndf_map_[xi] = chisqndf;
    
    if (cur_chisqndf_min>chisqndf)
      cur_xi_min = xi;
    
    minimum_element_ = xi_chi2_ndf_map_.find(cur_xi_min);
//    std::cout<<"Minimum element xi="<<minimum_element_->first<<" chisqndf="
//        <<minimum_element_->second<<std::endl;
  }
  else
  {
    xi_chi2_ndf_map_[xi] = chisqndf;
    minimum_element_ = xi_chi2_ndf_map_.find(xi);
//    std::cout<<"Minimum element xi="<<minimum_element_->first<<" chisqndf="
//        <<minimum_element_->second<<std::endl;
  }
}

void BinomialMinimumSearcher::Clear()
{
  xi_chi2_ndf_map_.clear();
  minimum_element_ = xi_chi2_ndf_map_.end();
}

double BinomialMinimumSearcher::GetXiCandidate(bool random_mut)
{
  if (xi_chi2_ndf_map_.size()==0)
  {
    if (TMath::Abs(xi_min_)<TMath::Abs(xi_max_))
      return xi_min_;
    else
      return xi_max_;
  }
  else if (xi_chi2_ndf_map_.size()==1)
  {
    if (TMath::Abs(xi_min_)<TMath::Abs(xi_max_))
      return xi_max_;
    else
      return xi_min_;
  }
  else
  {
    if (!random_mut || radom_output_prob_==0.0 || rand_engine_.Uniform(0, 1.0) >= radom_output_prob_)
      return ClosestToTheMinimum();
    else
    {
      double proposed_cand = rand_engine_.Uniform(xi_min_, xi_max_);
//      std::cout<<"Random init, Proposed xi candidate="<<proposed_cand
//          <<std::endl;
      return proposed_cand;
    }
  }
}

double BinomialMinimumSearcher::ClosestToTheMinimum() //assumed that there are at least 2 entries in the map 
{
//  std::cout<<"ClosestToTheMinimum(), map size="<<xi_chi2_ndf_map_.size()
//      <<std::endl;
  xi_chi2_map_type::iterator right_iter = minimum_element_;
  xi_chi2_map_type::iterator left_iter = minimum_element_;
  
  double xi_0 = minimum_element_->first;
  double chisqndf_0 = minimum_element_->second;
  double xi_1 = xi_0;
  double chisqndf_1 = minimum_element_->second;
  double dist = 0;
  
//  std::cout<<"Min xi="<<xi_0<<" chisqndf="<<chisqndf_0<<std::endl;
  
  ++right_iter;
  if (right_iter!=xi_chi2_ndf_map_.end())
  {
//    std::cout<<"Right Xi="<<right_iter->first<<" chisqndf="<<right_iter->second
//        <<std::endl;
    xi_1 = right_iter->first;
    chisqndf_1 = right_iter->second;
    dist = TMath::Abs(xi_1 - xi_0);
  }
  
  if (left_iter!=xi_chi2_ndf_map_.begin())
  {
    --left_iter;
    double temp_dist = TMath::Abs(xi_0 - left_iter->first);
    if (temp_dist>dist)
    {
      xi_1 = left_iter->first;
      chisqndf_1 =left_iter->second;
    }
    else if (temp_dist==dist && chisqndf_1 > left_iter->second)
    {
      xi_1 = left_iter->first;
      chisqndf_1 =left_iter->second;
    }
//    std::cout<<"Left Xi="<<left_iter->first<<" chisqndf="<<left_iter->second
//        <<std::endl;
  }
  double chisqndf_1_scaled = TMath::Sqrt(chisqndf_1);
  double chisqndf_0_scaled = TMath::Sqrt(chisqndf_0);
  double proposed_cand =(chisqndf_1_scaled*xi_0 + chisqndf_0_scaled*xi_1)
      /(chisqndf_1_scaled+chisqndf_0_scaled);
  //      double proposed_cand = (xi_1 + xi_0)/2.0; 
//  std::cout<<"Proposed xi candidate="<<proposed_cand<<std::endl;
  return proposed_cand;
}

double BinomialMinimumSearcher::GetBestXiCandidate()
{
  return minimum_element_->first;
}
