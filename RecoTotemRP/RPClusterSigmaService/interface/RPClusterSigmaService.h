#ifndef RecoTotemRP_RPClusterSigmaService_RPClusterSigmaService_h
#define RecoTotemRP_RPClusterSigmaService_RPClusterSigmaService_h


// -*- C++ -*-
//
// Package:    RPClusterSigmaService
// Class:      RPClusterSigmaService
// 
/**\class RPClusterSigmaService RPClusterSigmaService.h RecoTotemRP/RPClusterSigmaService/interface/RPClusterSigmaService.h

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Hubert NIEWIADOMSKI
//         Created:  Thu May 10 13:53:04 CEST 2007
//


// system include files
#include <memory>
#include "boost/shared_ptr.hpp"

// user include files
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "RecoTotemRP/RPServiceRecords/interface/RPClusterSigmaServiceRecord.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "RecoTotemRP/RPClusterSigmaService/interface/RPDetClusterSigmas.h"
//
// class decleration
//

class RPClusterSigmaService : public edm::ESProducer, public edm::EventSetupRecordIntervalFinder
{
  public:
    RPClusterSigmaService(const edm::ParameterSet&);
    ~RPClusterSigmaService();

    typedef std::auto_ptr<RPDetClusterSigmas> ReturnType;

    ReturnType produce(const RPClusterSigmaServiceRecord&);
  protected:
    virtual void setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
        const edm::IOVSyncValue &,edm::ValidityInterval &);
    
  private:
      // ----------member data ---------------------------
};

#endif  //RecoTotemRP_RPClusterSigmaService_RPClusterSigmaService_h
