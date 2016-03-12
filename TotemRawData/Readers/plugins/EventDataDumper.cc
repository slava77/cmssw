/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*  Michal Marciniec (michal.marciniec@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"

/**
 * This is version of RawDataDumper. It's output is adjusted to be used by RunInfoExtractor - offline
 * software module that populates TOTEM Offline Database with events' data.
 **/
class EventDataDumper : public edm::EDAnalyzer
{
  public:
    EventDataDumper(const edm::ParameterSet &ps) {
    	RawEventLabel = ps.getParameter<edm::InputTag>("RawEventLabel");
    }

    ~EventDataDumper() {}

  private:
    virtual void beginJob();
    virtual void beginRun(edm::Run const&, edm::EventSetup const&) {}
    virtual void analyze(const edm::Event &e, const edm::EventSetup &es);
    virtual void endRun(edm::Run const&, edm::EventSetup const&) {}
    virtual void endJob();
    edm::InputTag RawEventLabel;
};

//----------------------------------------------------------------------------------------------------

using namespace edm;

//----------------------------------------------------------------------------------------------------

void EventDataDumper::beginJob()
{
//	printf("EVENT_DATA_START\n");
}

//----------------------------------------------------------------------------------------------------

void EventDataDumper::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
  Handle< Totem::RawEvent > rawData;
  event.getByLabel(RawEventLabel, rawData);

  //printf("%u,%u\n", event.id().event(), event.time().unixTime());
}

//----------------------------------------------------------------------------------------------------

void EventDataDumper::endJob()
{
//	printf("EVENT_DATA_END\n");
//	fflush(stdout);
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(EventDataDumper);
