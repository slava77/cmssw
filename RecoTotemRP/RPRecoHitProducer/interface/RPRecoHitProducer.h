#ifndef RecoTotemRP_RPRecoHitProducer_RPRecoHitProducer_h
#define RecoTotemRP_RPRecoHitProducer_RPRecoHitProducer_h

/** \class RPRecoHitProducer
 *
 * RPRecoHitProducer is the EDProducer subclass which clusters
 * SiStripDigi/interface/StripDigi.h to SiStripCluster/interface/SiStripCluster.h
 *
 * \author Hubert Niewiadomski, CERN
 *
 *
 ************************************************************/
 
//edm
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//Data Formats
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/TotemRPDataTypes/interface/RPDigCluster.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
//RecoHitProducer
#include "RecoTotemRP/RPRecoHitProducer/interface/RPRecoHitProducerAlgorithm.h"
#include "RecoTotemRP/RPClusterSigmaService/interface/RPDetClusterSigmas.h"
 
#include <iostream>
#include <memory>
#include <string>
 
 
class RPRecoHitProducer : public edm::EDProducer
{
  public:
  
    explicit RPRecoHitProducer(const edm::ParameterSet& conf);
  
    virtual ~RPRecoHitProducer();
  
    virtual void beginJob();
  
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
  
  private:
    void run(const edm::DetSetVector<RPDigCluster>& input,std::vector<edm::DetSet<RPRecoHit> > & output);
    const edm::ParameterSet conf_;
    int verbosity_;
    RPRecoHitProducerAlgorithm RPRecoHitProducerAlgorithm_;
    RPDetClusterSigmas the_cluster_sigma_;
    //std::string cluster_producer_;
    //std::string cluster_label_;
    //std::string rec_hit_label_;
    edm::InputTag cluster_label_;
    edm::EDGetTokenT<edm::DetSetVector<RPDigCluster> >cluster_label_Token_;
    edm::ESHandle<RPDetClusterSigmas> cluster_sigmas_;
};


#endif  //RecoTotemRP_RPRecoHitProducer_RPRecoHitProducer_h
