#include "RecoTotemRP/RPRecoHitProducer/interface/RPRecoHitProducerAlgorithm.h"

void RPRecoHitProducerAlgorithm::BuildRecoHits(const edm::DetSet<RPDigCluster>& input, 
    edm::DetSet<RPRecoHit>& output)
{
  for(edm::DetSet<RPDigCluster>::const_iterator it = input.begin(); it!=input.end(); 
      ++it)
  {
    double sigma; //in [mm]
    if(cluster_sigmas_)
    {
      sigma = cluster_sigmas_->GetClusterSigma(it->DetId(), it->GetNumberOfStrips(), 
          it->CentreStripPos());
    }
    else
    {
      sigma = 0.0191;
    }
    output.push_back(RPRecoHit(it->DetId(), 
        rp_topology_.GetHitPositionInReadoutDirection(it->CentreStripPos()), sigma));
  }  
}
