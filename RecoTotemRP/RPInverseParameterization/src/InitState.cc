#include "RecoTotemRP/RPInverseParameterization/interface/InitState.h"

InitValues::InitValues(unsigned int i)
{
  init_val_.resize(i);
  is_fixed_.resize(i);
  compacted_index_.resize(i);
  ReleaseAll();
  ComputeCompactedIndex();
}

void InitValues::FixValue(unsigned int i, double val)
{
  init_val_[i]=val;
  is_fixed_[i]=true;
  ComputeCompactedIndex();
}

void InitValues::FixValue(unsigned int i)
{
  is_fixed_[i]=true;
  ComputeCompactedIndex();
}

void InitValues::ReleaseValue(unsigned int i, double val)
{
  init_val_[i]=val;
  is_fixed_[i]=false;
  ComputeCompactedIndex();
}

void InitValues::ReleaseValue(unsigned int i)
{
  is_fixed_[i]=false;
  ComputeCompactedIndex();
}

void InitValues::ReleaseAll()
{
  for (unsigned int j=0; j<is_fixed_.size(); j++)
    is_fixed_[j]=false;
  ComputeCompactedIndex();
}

void InitValues::ComputeCompactedIndex()
{
  unsigned int count=0;
  current_variables_.clear();
  for (unsigned int i=0; i<is_fixed_.size(); i++)
  {
    if (is_fixed_[i])
      compacted_index_[i]=-1;
    else
    {
      compacted_index_[i]=count;
      current_variables_.push_back(i);
      count++;
    }
  }
}
