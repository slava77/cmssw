/*
 * adapting to CTPPS pixel detector March 2017 - F.Ferro
 */


#include "EventFilter/CTPPSRawToDigi/interface/CTPPSPixelRawToDigi.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/CTPPSDigi/interface/CTPPSPixelDigi.h"

#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"

#include "EventFilter/CTPPSRawToDigi/interface/CTPPSPixelDataFormatter.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"

using namespace std;


CTPPSPixelRawToDigi::CTPPSPixelRawToDigi( const edm::ParameterSet& conf ) 
  : config_(conf)

{

  FEDRawDataCollection_ = consumes <FEDRawDataCollection> (config_.getParameter<edm::InputTag>("InputLabel"));

// Products
  produces< edm::DetSetVector<CTPPSPixelDigi> >();

//CablingMap could have a label //Tav
  includeErrors_ = config_.getParameter<bool> ("IncludeErrors");
  mappingLabel_ = config_.getParameter<std::string> ("mappingLabel"); 

  if(includeErrors_){
    produces< edm::DetSetVector<CTPPSPixelDataError> >();
    produces<DetIdCollection>();
    produces<DetIdCollection>("UserErrorModules");
  }
}


CTPPSPixelRawToDigi::~CTPPSPixelRawToDigi() {
  edm::LogInfo("CTPPSPixelRawToDigi")  << " CTPPSPixelRawToDigi destructor!";

}

void CTPPSPixelRawToDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<bool>("IncludeErrors",true);
  desc.add<edm::InputTag>("InputLabel",edm::InputTag("rawDataCollector"));
  desc.add<std::string>("mappingLabel","RPix");
  descriptions.add("ctppsPixelDigis", desc);
}


void CTPPSPixelRawToDigi::produce( edm::Event& ev,
				   const edm::EventSetup& es) 
{

  edm::Handle<FEDRawDataCollection> buffers;
  ev.getByToken(FEDRawDataCollection_, buffers);

  edm::ESHandle<CTPPSPixelDAQMapping> mapping;

  bool data_exist=false;
  for(int fed = FEDNumbering::MINCTPPSPixelsFEDID; fed <=  FEDNumbering::MAXCTPPSPixelsFEDID; fed++){
    const FEDRawData& tempRawData = buffers->FEDData( fed );
    if(tempRawData.size()!=0){
      data_exist=true;
      break;
    }
  }
/// create product (digis & errors)
  auto collection = std::make_unique<edm::DetSetVector<CTPPSPixelDigi>>();

  auto errorcollection = std::make_unique<edm::DetSetVector<CTPPSPixelDataError>>();
  auto tkerror_detidcollection = std::make_unique<DetIdCollection>();
  auto usererror_detidcollection = std::make_unique<DetIdCollection>();

  if(data_exist){
  es.get<CTPPSPixelDAQMappingRcd>().get( mapping);

    fedIds_   = mapping->fedIds();

    CTPPSPixelDataFormatter formatter(mapping->ROCMapping);
    formatter.setErrorStatus(includeErrors_);

    bool errorsInEvent = false;
    CTPPSPixelDataFormatter::DetErrors nodeterrors;


    for (auto aFed = fedIds_.begin(); aFed != fedIds_.end(); ++aFed) {
      int fedId = *aFed;
      
      edm::LogInfo("CTPPSPixelRawToDigi")<< " PRODUCE DIGI FOR FED: " <<  dec <<fedId << endl;

      CTPPSPixelDataFormatter::Errors errors;
/// get event data for this fed
      const FEDRawData& fedRawData = buffers->FEDData( fedId );
      
      formatter.interpretRawData( errorsInEvent, fedId, fedRawData, *collection, errors);
    
      if(includeErrors_) {
	typedef CTPPSPixelDataFormatter::Errors::iterator IE;
	for (IE is = errors.begin(); is != errors.end(); is++) {
	  uint32_t errordetid = is->first;
	  if (errordetid==RPixErrorChecker::dummyDetId) {           // errors given dummy detId must be sorted by Fed
	    nodeterrors.insert( nodeterrors.end(), errors[errordetid].begin(), errors[errordetid].end() );
	  } else {
	    edm::DetSet<CTPPSPixelDataError>& errorDetSet = errorcollection->find_or_insert(errordetid);
	    errorDetSet.data.insert(errorDetSet.data.end(), is->second.begin(), is->second.end());	  
	  }
	}
      }
    }

    if(includeErrors_) {
      edm::DetSet<CTPPSPixelDataError>& errorDetSet = errorcollection->find_or_insert(RPixErrorChecker::dummyDetId);
      errorDetSet.data = nodeterrors;
    }
    if (errorsInEvent) LogDebug("CTPPSPixelRawToDigi") << "Error words were stored in this event";
  }
///send digis and errors back to framework 
  ev.put(std::move(collection));

  if(includeErrors_){
    ev.put(std::move(errorcollection));
  }
}

DEFINE_FWK_MODULE( CTPPSPixelRawToDigi);
