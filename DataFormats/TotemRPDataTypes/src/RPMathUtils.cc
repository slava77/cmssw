#include "DataFormats/TotemRPDataTypes/interface/RPMathUtils.h"

namespace tot_rp
{
  void Print(std::ostream &o, const TMatrixD &mt)
  {
    for(int i=0; i<mt.GetNrows(); ++i)
      {
	for(int j=0; j<mt.GetNcols(); ++j)
	  {
	    o<<"("<<i<<","<<j<<") "<<mt[i][j]<<" ";
	  }
	o<<std::endl;
      }
  }
  
  void Print(std::ostream &o, const TVectorD &vt)
  {
    for(int i=0; i<vt.GetNrows(); ++i)
      {
        o<<"("<<i<<") "<<vt[i]<<" ";
      }
    o<<std::endl;
  }
}
