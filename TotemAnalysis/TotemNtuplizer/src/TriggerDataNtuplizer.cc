/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemAnalysis/TotemNtuplizer/interface/TriggerDataNtuplizer.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "TTree.h"

#include <map>

using namespace std;
using namespace edm;

ClassImp(TriggerData)

TriggerDataNtuplizer::TriggerDataNtuplizer(const edm::ParameterSet &ps) : Ntuplizer(ps) {
    	rawEventLabel = ps.getParameter<edm::InputTag>("RawEventLabel");
    }


void TriggerDataNtuplizer::CreateBranches(const edm::EventSetup&, TTree *tree)
{
  tree->Branch("trigger_data.", &data);
}

//----------------------------------------------------------------------------------------------------

void TriggerDataNtuplizer::FillEvent(const edm::Event &event, const edm::EventSetup &es)
{
  Handle< Totem::RawEvent > input;
  event.getByLabel(rawEventLabel, input);

  data.type = input->triggerData.type;
  data.event_num = input->triggerData.event_num;
  data.bunch_num = input->triggerData.bunch_num;
  data.src_id = input->triggerData.src_id;
  data.orbit_num = input->triggerData.orbit_num;
  data.revision_num = input->triggerData.revision_num;
  data.run_num = input->triggerData.run_num;
  data.trigger_num = input->triggerData.trigger_num;
  data.inhibited_triggers_num = input->triggerData.inhibited_triggers_num;
  data.input_status_bits = input->triggerData.input_status_bits;
}
