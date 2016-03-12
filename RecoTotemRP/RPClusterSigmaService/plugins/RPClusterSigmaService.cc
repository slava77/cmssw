#include "RecoTotemRP/RPClusterSigmaService/interface/RPClusterSigmaService.h"
#include "FWCore/Framework/interface/SourceFactory.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
RPClusterSigmaService::RPClusterSigmaService(const edm::ParameterSet& iConfig)
{
   //the following line is needed to tell the framework what
   // data is being produced
   setWhatProduced(this);

   //now do what ever other initialization is needed
}


RPClusterSigmaService::~RPClusterSigmaService()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to produce the data  ------------
RPClusterSigmaService::ReturnType
RPClusterSigmaService::produce(const RPClusterSigmaServiceRecord& iRecord)
{
//   edm::ESHandle<RPDetClusterSigmas> clu_sigmas;
//   iRecord.get(clu_sigmas);
   using namespace edm::es;
   std::auto_ptr<RPDetClusterSigmas> pRPDetClusterSigmas(new RPDetClusterSigmas() ) ;

   return pRPDetClusterSigmas ;
}

void RPClusterSigmaService::setIntervalFor(const edm::eventsetup::EventSetupRecordKey &,
                                               const edm::IOVSyncValue & iosv, 
                                               edm::ValidityInterval & oValidity)
{
   edm::ValidityInterval infinity(iosv.beginOfTime(), iosv.endOfTime());
   oValidity = infinity;
}

DEFINE_FWK_EVENTSETUP_SOURCE(RPClusterSigmaService);
