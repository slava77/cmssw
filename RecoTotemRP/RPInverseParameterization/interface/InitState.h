#ifndef RecoTotemRPRPInverseParameterizationinterfaceInitState_h
#define RecoTotemRPRPInverseParameterizationinterfaceInitState_h

#include <vector>
#include <map>
#include <iostream>
#include "TRandom2.h"

class InitValues
{
  public:
    InitValues(unsigned int i);
    void FixValue(unsigned int i, double val);
    void FixValue(unsigned int i);
    void ReleaseValue(unsigned int i, double val);
    void ReleaseValue(unsigned int i);
    inline void SetValue(unsigned int i, double val)
    {
      init_val_[i]=val;
    }
    inline double GetValue(unsigned int i)
    {
      return init_val_[i];
    }
    inline void SetValues(std::vector<double> vals)
    {
      if (vals.size()==init_val_.size())
        init_val_=vals;
    }
    inline const std::vector<double> &GetValues(unsigned int i)
    {
      return init_val_;
    }
    void ReleaseAll();
    inline void SetChiSqared(double val)
    {
      chi_sq_=val;
    }
    inline double GetChiSqared()
    {
      return chi_sq_;
    }
    inline const std::vector<unsigned int> & GetCurrentVariables()
    {
      return current_variables_;
    }
    inline int FreeParameterNumber()
    {
      return current_variables_.size();
    }
    inline int ParameterNumber()
    {
      return init_val_.size();
    }
    inline bool IsParameterFixed(unsigned int i)
    {
      return is_fixed_[i];
    }
    
  private:
    std::vector<double> init_val_;
    std::vector<bool> is_fixed_;
    std::vector<int> compacted_index_;
    std::vector<unsigned int> current_variables_;
    double chi_sq_;

    void ComputeCompactedIndex();
};


#endif
