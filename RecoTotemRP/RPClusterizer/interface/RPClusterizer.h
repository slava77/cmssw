#ifndef RPClusterizer_h
#define RPClusterizer_h

/** \class RPClusterizer
 *
 * RPClusterizer is the EDProducer subclass which clusters
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
#include "DataFormats/TotemRPDataTypes/interface/RPStripDigi.h"
//Clusterizer
#include "RecoTotemRP/RPClusterizer/interface/RPClusterizerAlgorithm.h"
 
#include <iostream>
#include <memory>
#include <string>
 
 
class RPClusterizer : public edm::EDProducer
{
  public:
  
    explicit RPClusterizer(const edm::ParameterSet& conf);
  
    virtual ~RPClusterizer();
  
    virtual void beginJob();
  
    virtual void produce(edm::Event& e, const edm::EventSetup& c);
  
  private:
    void run(const edm::DetSetVector<RPStripDigi>& input,std::vector<edm::DetSet<RPDigCluster> > & output);
    edm::ParameterSet conf_;
    int verbosity_;
    RPClusterizerAlgorithm RPClusterizerAlgorithm_;
    //std::string digiProducer_;
    //std::string digiLabel_;
    //std::string clusterLabel_;
    edm::InputTag digiInputTag_;
    edm::EDGetTokenT<edm::DetSetVector<RPStripDigi> >digiInputTagToken_;



};
  
#endif
