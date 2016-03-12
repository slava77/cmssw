/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"

/**
 * \brief Module to print basic raw-event information.
 **/
class RawDataDumper : public edm::EDAnalyzer
{
  public:
    RawDataDumper(const edm::ParameterSet &ps)
    {
      rawEventLabel = ps.getParameter<edm::InputTag>("RawEventLabel");
    }

    ~RawDataDumper() {}

  private:
    edm::InputTag rawEventLabel;
    virtual void beginJob() {}
    virtual void beginRun(edm::Run const&, edm::EventSetup const&) {}
    virtual void analyze(const edm::Event &e, const edm::EventSetup &es);
    virtual void endRun(edm::Run const&, edm::EventSetup const&) {}
    virtual void endJob() {}
};

//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

void RawDataDumper::analyze(const edm::Event& event, const edm::EventSetup& setup)
{
    Handle< Totem::RawEvent > rawData;
    event.getByLabel(rawEventLabel, rawData);

    // CMSSW event labels
    //printf("from CMSSW: run=%u, event=%u, UNIX timestamp=%u\n", event.id().run(), event.id().event(),
      //event.time().unixTime());

    // DAQ event labels
 //   printf("from DAQ: run=%u, event=%u, timestamp=%lu\n", rawData->triggerData.run_num,
   //   rawData->triggerData.event_num, rawData->timestamp);
}

//----------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(RawDataDumper);
