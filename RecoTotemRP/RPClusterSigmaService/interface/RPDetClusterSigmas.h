#ifndef RecoTotemRP_RPClusterSigmaService_RPDetClusterSigmas_h
#define RecoTotemRP_RPClusterSigmaService_RPDetClusterSigmas_h

#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"

class RPDetClusterSigmas
{
  public:
    RPDetClusterSigmas();
    inline double GetClusterSigma(RPDetId rp_id, int clu_size, double avg_strip_no) const { return 0.0191;};
};

#endif  //RecoTotemRP_RPClusterSigmaService_RPDetClusterSigmas_h
