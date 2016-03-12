#ifndef RecoTotemRPRPInverseParameterizationinterfaceBinomialMinimumSearcher_h
#define RecoTotemRPRPInverseParameterizationinterfaceBinomialMinimumSearcher_h

#include "TRandom2.h"
#include <iostream>
#include <map>
#include <vector>
#include "TMath.h"


class BinomialMinimumSearcher
{
  public:
    BinomialMinimumSearcher(double xi_min, double xi_max,
        double radom_output_prob, TRandom2 & rand_engine);
    void InsertElement(double xi, double chisqndf);
    void Clear();
    double GetXiCandidate(bool random_mut);
    double GetBestXiCandidate();
  private:
    double ClosestToTheMinimum();
    
    double xi_min_, xi_max_;
    typedef std::map<double, double> xi_chi2_map_type;
    xi_chi2_map_type xi_chi2_ndf_map_;
    xi_chi2_map_type::iterator minimum_element_;
    double radom_output_prob_;
    TRandom2 & rand_engine_;
};


#endif
