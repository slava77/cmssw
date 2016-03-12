/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemAnalysis/TotemNtuplizer/interface/RawMetaDataNtuplizer.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "TTree.h"

#include <map>

using namespace std;
using namespace edm;
using namespace Totem;

ClassImp(EventMetaData)


RawMetaDataNtuplizer::RawMetaDataNtuplizer(const edm::ParameterSet &ps) : Ntuplizer(ps) {
	rawEventLabel = ps.getParameter< edm::InputTag >("RawEventLabel");
}



void RawMetaDataNtuplizer::CreateBranches(const edm::EventSetup&, TTree *tree)
{
  tree->Branch("event_info.", &data);
}

//----------------------------------------------------------------------------------------------------

void RawMetaDataNtuplizer::FillEvent(const edm::Event &event, const edm::EventSetup &es)
{
  Handle< RawEvent > input;
  event.getByLabel(rawEventLabel, input);

  data.run_no = event.id().run();
  data.event_no = event.id().event();
  data.timestamp = event.time().unixTime();
  data.daq_event_number = input->dataEventNumber;

  //printf("%lu, %lu: %lu\n", data.run_no, data.event_no, input->optoRxMetaData.size());

  data.optoRx_Id.clear();
  data.optoRx_BX.clear();
  data.optoRx_LV1.clear();
  for (map<unsigned int, OptoRxMetaData>::const_iterator it = input->optoRxMetaData.begin();
        it != input->optoRxMetaData.end(); ++it) {
    data.optoRx_Id.push_back(it->first);
    data.optoRx_BX.push_back(it->second.BX);
    data.optoRx_LV1.push_back(it->second.LV1);
    //printf("  %x, %u\n", it->first, it->second.BX);
  }
}
