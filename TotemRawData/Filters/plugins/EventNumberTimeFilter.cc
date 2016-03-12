/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"



/**
 * \brief Filters events in selected event-number, raw-event-number and event-timestamp range.
 */
class EventNumberTimeFilter : public edm::EDFilter
{
  public:
    edm::InputTag rawEventLabel;

    struct criterion
    {
      bool active;
      unsigned int min, max;
    };

    criterion eventNumber;
    criterion rawEventNumber;
    criterion timestamp;  ///< UNIX timestamp

    EventNumberTimeFilter(const edm::ParameterSet &);

  protected:
    bool prevStatus;

    virtual bool filter(edm::Event&, const edm::EventSetup &);
    virtual void endJob() {}
};


using namespace edm;
using namespace std;
using namespace Totem;


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

EventNumberTimeFilter::EventNumberTimeFilter(const ParameterSet &pSet) :
  prevStatus(false)
{
  rawEventLabel = pSet.getParameter<InputTag>("rawEventLabel");

  const ParameterSet &enps = pSet.getParameterSet("eventNumber");
  eventNumber.active = enps.getParameter<bool>("active");
  eventNumber.min = enps.getParameter<unsigned int>("min");
  eventNumber.max = enps.getParameter<unsigned int>("max");
  
  const ParameterSet &renps = pSet.getParameterSet("rawEventNumber");
  rawEventNumber.active = renps.getParameter<bool>("active");
  rawEventNumber.min = renps.getParameter<unsigned int>("min");
  rawEventNumber.max = renps.getParameter<unsigned int>("max");
  
  const ParameterSet &tsps = pSet.getParameterSet("timestamp");
  timestamp.active = tsps.getParameter<bool>("active");
  timestamp.min = tsps.getParameter<unsigned int>("min");
  timestamp.max = tsps.getParameter<unsigned int>("max");
}

//----------------------------------------------------------------------------------------------------

bool EventNumberTimeFilter::filter(edm::Event &event, const EventSetup &es)
{
  bool status = true;

  // event number check
  if (eventNumber.active)
  {
    if (event.id().event() < eventNumber.min)
      status = false;
    if (event.id().event() > eventNumber.max)
      status = false;
  }

  // timestamp check
  if (timestamp.active)
  {
    if (event.time().unixTime() < timestamp.min)
      status = false;
    if (event.time().unixTime() > timestamp.max)
      status = false;
  }

  // get raw event
  Handle< RawEvent > rEv;
  event.getByLabel(rawEventLabel, rEv);
  if (!rEv.isValid())
    status = false;

  // raw event number check
  if (rawEventNumber.active)
  {
    if (rEv->dataEventNumber < rawEventNumber.min)
      status = false;
    if (rEv->dataEventNumber > rawEventNumber.max)
      status = false;
  }

  //printf("> event %i:%i, time %llu > status = %i, prevStatus = %i\n", event.id().run(),
  //event.id().event(), event.time().unixTime(), status, prevStatus);

  if (prevStatus != status)
  {
    time_t unixTime = event.time().unixTime();
    char timeStr[50];
    strftime(timeStr, 50, "%F %T", localtime(&unixTime));

   // printf(">> EventNumberTimeFilter::filter > Change of status (%s --> %s) at run=%i, event=%i, timestamp=%lu (%s)\n",
   //   ((prevStatus ? "pass" : "block")),
   //   ((status ? "pass" : "block")),
   //   event.id().run(), event.id().event(), unixTime, timeStr);
  }

  prevStatus = status;
  return status;
}

DEFINE_FWK_MODULE(EventNumberTimeFilter);
