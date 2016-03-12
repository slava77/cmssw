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

#include <vector>


/**
 * \brief Filters events with selected event numbers.
 */
class EventNumberArrayFilter : public edm::EDFilter
{
  public:
    std::vector<unsigned int> selectedEventNumbers;

    EventNumberArrayFilter(const edm::ParameterSet &);

  protected:
    virtual bool filter(edm::Event&, const edm::EventSetup &);
    virtual void endJob() {}
};


using namespace edm;
using namespace std;

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

EventNumberArrayFilter::EventNumberArrayFilter(const ParameterSet &pSet) :
  selectedEventNumbers(pSet.getParameter< vector<unsigned int> >("selectedEventNumbers"))
{
}

//----------------------------------------------------------------------------------------------------

bool EventNumberArrayFilter::filter(edm::Event &event, const EventSetup &es)
{
  auto it = find(selectedEventNumbers.begin(), selectedEventNumbers.end(), event.id().event());

  return (it != selectedEventNumbers.end());
}

DEFINE_FWK_MODULE(EventNumberArrayFilter);
