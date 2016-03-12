/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wr??bel (wroblisko@gmail.com)
*   Jan Ka??par (jan.kaspar@gmail.com)
*   Marcin Borratynski (mborratynski@gmail.com)
*
****************************************************************************/

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "TotemRawData/RawToDigi/interface/CounterChecker.h"
#include "TotemRawData/RawToDigi/interface/Raw2DigiProducer.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrame.h"
#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawData/RawToDigi/interface/Raw2DigiStatus.h"
#include "TotemCondFormats/DAQInformation/interface/AnalysisMask.h"
#include "TotemCondFormats/DAQInformation/interface/DAQMapping.h"
#include "TotemCondFormats/DataRecord/interface/TotemDAQMappingRecord.h"
 
#include <iostream>
#include <fstream>

using namespace std;
using namespace edm;
using namespace Totem;

//#define DEBUG

//----------------------------------------------------------------------------------------------------

Raw2DigiProducer::Raw2DigiProducer(edm::ParameterSet const& conf) :
  verbosity(conf.getUntrackedParameter<unsigned int>("verbosity", 0)),
  printErrorSummary(conf.getUntrackedParameter<unsigned int>("printErrorSummary", 1)),
  printUnknownFrameSummary(conf.getUntrackedParameter<unsigned int>("printUnknownFrameSummary", 1)),

  testFootprint(conf.getParameter<unsigned int>("testFootprint")),
  testCRC(conf.getParameter<unsigned int>("testCRC")),
  testID(conf.getParameter<unsigned int>("testID")),
  testECRaw(conf.getParameter<unsigned int>("testECRaw")),
  testECDAQ(conf.getParameter<unsigned int>("testECDAQ")),
  testECMostFrequent(conf.getParameter<unsigned int>("testECMostFrequent")),
  testBCMostFrequent(conf.getParameter<unsigned int>("testBCMostFrequent")),
  
  EC_min(conf.getUntrackedParameter<unsigned int>("EC_min", 10)),
  BC_min(conf.getUntrackedParameter<unsigned int>("BC_min", 10)),
  
  EC_fraction(conf.getUntrackedParameter<double>("EC_fraction", 0.6)),
  BC_fraction(conf.getUntrackedParameter<double>("BC_fraction", 0.6))
{
  rawEventLabel = conf.getParameter<InputTag>("RawEventLabel");
  rawEventLabelToken_ = consumes<RawEvent>(rawEventLabel);

//  rpCCProductLabel = conf.getUntrackedParameter<std::string>("rpCCProductLabel", "rpCCOutput");
  rpDataProductLabel = conf.getUntrackedParameter<std::string>("rpDataProductLabel", "rpDataOutput");
  conversionStatusLabel = conf.getUntrackedParameter<std::string>("conversionStatusLabel", "conversionStatus");

  // RP data
  produces< edm::DetSetVector<RPStripDigi> > (rpDataProductLabel);

  // RP CC
//  produces < std::vector <RPCCBits> > (rpCCProductLabel);

  // T2 data


  // status
  produces <Raw2DigiStatus>(conversionStatusLabel);
}
  
//----------------------------------------------------------------------------------------------------
 
Raw2DigiProducer::~Raw2DigiProducer()
{
}

//----------------------------------------------------------------------------------------------------
 
void Raw2DigiProducer::beginRun(const edm::Run& run,const edm::EventSetup& es)
{
  // generate connections for t1
}

//----------------------------------------------------------------------------------------------------
 
void Raw2DigiProducer::endJob()
{
  if (printErrorSummary) {
    cout << "* Error summary (error signature: number of such events)" << endl;
    for (map<Totem::FramePosition, map<VFATStatus, unsigned int> >::iterator vit = errorSummary.begin();
        vit != errorSummary.end(); ++vit) {
      //map<FramePosition, VFATInfo>::const_iterator mappingIter = mapping->VFATMapping.find(vit->first);
      //cout << "  " << vit->first << " (VFAT " << mappingIter->second.hwId << "):" << endl;
      cout << "  " << vit->first << endl;
      for (map<VFATStatus, unsigned int>::iterator it = vit->second.begin(); it != vit->second.end();
          ++it) {
        cout << "    " << it->first << ": " << it->second << endl;
      }
    }
  }

  if (printUnknownFrameSummary) {
    cout << "* Frames found in data, but not in the mapping (frame position: number of events)" << endl;
    for (map<FramePosition, unsigned int>::iterator it = unknownSummary.begin();
        it != unknownSummary.end(); ++it) {
      cout << "  " << it->first << ":" << it->second << endl;
    }
  }
}

//----------------------------------------------------------------------------------------------------
 
void Raw2DigiProducer::produce(edm::Event& e, const edm::EventSetup& es)
{
  rpDataOutput = auto_ptr<edm::DetSetVector<RPStripDigi> > (new edm::DetSetVector<RPStripDigi>);  
//  rpCCOutput = auto_ptr< std::vector<RPCCBits> > (new std::vector<RPCCBits>);
  
  stringstream ees;

#ifdef DEBUG
  printf(">> Raw2DigiProducer::produce\n");
  printf("\tevent #%u at %p\n", e.id().event(), (void *) &e);
#endif

  // get input
  Handle< RawEvent >  input;
//  e.getByLabel(rawEventLabel, input);
  e.getByToken(rawEventLabelToken_, input);
  if (!input.isValid())
    return;

#ifdef DEBUG
  printf("\tevent.frames at %p\n", (void *)input->frames);
  printf("\tevent.dataEventNumber = %lu\n", input->dataEventNumber);
  printf("\tcollection type %s\n", input->frames->GetClassName().c_str());
#endif

  // get mapping
  ESHandle<DAQMapping> mapping;
  es.get<TotemDAQMappingRecord>().get(mapping);

  // get analysis mask to mask channels
  ESHandle<AnalysisMask> analysisMask;
  es.get<TotemDAQMappingRecord>().get(analysisMask);

  // map which will contain FramePositions from mapping, which data is missing in raw event
  map<FramePosition, VFATInfo> missingFrames(mapping->VFATMapping);

  // prepare products
  rpDataBeginJob();  

  // expected EC number for all frames in collection
  // CMSSW events are counted from 1, raw events from 0 => the shift by -1
  unsigned long ECExpected = (e.id().event() - 1) % 0x100; 

  // object containing the information about the conversion
  auto_ptr<Raw2DigiStatus> conversionStatus(new Raw2DigiStatus());
  
  // EC and BC checks (wrt. the most frequent value), BC checks per subsystem
  CounterChecker ECChecker(CounterChecker::ECChecker, "EC", EC_min, EC_fraction, verbosity);
  map<unsigned int, CounterChecker> BCCheckers;
  BCCheckers[TotemSymbID::RP] = CounterChecker(CounterChecker::BCChecker, "BC/RP", BC_min, BC_fraction, verbosity);

  // aplly tests to all frames
  for (VFATFrameCollection::Iterator fr(input->frames); !fr.IsEnd(); fr.Next()) {
    stringstream fes;
    bool stopProcessing = false;

    // contain information about processed frame
    VFATStatus &actualStatus = (*conversionStatus)[fr.Position()];
    
    // skip unlisted positions (VFATInfo)
    map<FramePosition, VFATInfo>::const_iterator mappingIter = mapping->VFATMapping.find(fr.Position());
    if (mappingIter == mapping->VFATMapping.end()) {
      unknownSummary[fr.Position()]++;
      continue;
    }

    // remove from missingFrames
    map<FramePosition, VFATInfo>::iterator iter = missingFrames.find(fr.Position());
    missingFrames.erase(iter);

    // check footprint
    if (testFootprint != tfNoTest && !fr.Data()->checkFootprint())
    {
      fes << "    invalid footprint\n";
      if ((testFootprint == tfErr))
      {
        actualStatus.setFootprintError();
        stopProcessing = true;
      }
    }
    
    // check CRC
    if (testCRC != tfNoTest && !fr.Data()->checkCRC())
    {
      fes << "    CRC failure\n";
      if (testCRC == tfErr)
      {
        actualStatus.setCRCError();
        stopProcessing = true;
      }
    }

    // check the id mismatch
    if (testID != tfNoTest && fr.Data()->isIDPresent() && (fr.Data()->getChipID() & 0xFFF) != (mappingIter->second.hwID & 0xFFF))
    {
      fes << "    ID mismatch (data: 0x" << hex << fr.Data()->getChipID()
        << ", mapping: 0x" << mappingIter->second.hwID  << dec << ", symbId: " << mappingIter->second.symbolicID.symbolicID << ")\n";
      if (testID == tfErr)
      {
        actualStatus.setIDMismatch();
        stopProcessing = true;
      }
    }

    // EC progress check (wrt. raw data event number, i.e. EC expected)
    if (testECRaw != tfNoTest && fr.Data()->isECPresent() && fr.Data()->getEC() != ECExpected)
    {
      fes << "    EC (" << fr.Data()->getEC() << ") doesn't match the expectation form event number (" << ECExpected << ")\n";
      if (testECRaw == tfErr)
      {
        actualStatus.setECProgressError();
        stopProcessing = true;
      }
    }

    // check, if EC number from VFATFrame is the same as obtained from DAQ (only if the there is not ECProgressError)
    if (testECDAQ != tfNoTest && fr.Data()->isECPresent() && !actualStatus.isECProgressError()
        && ((fr.Data()->getEC()%0x100) != (input->dataEventNumber%0x100)))
    {
      fes << "    EC (" << fr.Data()->getEC() << ") doesn't match DAQ event counter (" << input->dataEventNumber << ")\n";
      if (testECDAQ == tfErr)
      { 
        actualStatus.setECProgressError();
        stopProcessing = true;
      }
    }    

    // if there were errors, put the information to ees buffer
    if (verbosity > 0 && !fes.rdbuf()->str().empty())
    {
      string message = (stopProcessing) ? "(and will be dropped)" : "(but will be used though)";
      if (verbosity > 2)
      {
        ees << "  Frame at " << fr.Position() << " seems corrupted " << message << ":\n";
        ees << fes.rdbuf();
      } else
        ees << "  Frame at " << fr.Position() << " seems corrupted " << message << ".\n";
    }

    // if there were serious errors, do not process this frame
    if (stopProcessing)
      continue;
    
    // fill EC and BC values to the statistics
    if (fr.Data()->isECPresent())
      ECChecker.Fill(fr.Data()->getEC(), fr.Position());

    if (fr.Data()->isBCPresent())
      BCCheckers[mappingIter->second.symbolicID.subSystem].Fill(fr.Data()->getBC(), fr.Position());
  }

  // analyze EC and BC statistics
  if (testECMostFrequent != tfNoTest)
    ECChecker.Analyze(*conversionStatus, (testECMostFrequent == tfErr), ees);

  if (testBCMostFrequent != tfNoTest)
  {
    for (map<unsigned int, CounterChecker>::iterator it = BCCheckers.begin(); it != BCCheckers.end(); ++it)
      it->second.Analyze(*conversionStatus, (testBCMostFrequent == tfErr), ees);
  }

  // save the information about missing frames to conversionStatus
  for (map<FramePosition, VFATInfo>::iterator iter = missingFrames.begin(); iter != missingFrames.end(); iter++)
  {
    VFATStatus &actualStatus = (*conversionStatus)[iter->first];
    actualStatus.setMissing();
    if (verbosity > 1) 
      ees << "Frame for VFAT " << iter->first << " is not present in the data.\n";
  }

  // print error message
  if (verbosity > 0 && !ees.rdbuf()->str().empty()) {
    char buf[20];
    sprintf(buf, "%u.%04u", e.id().run() / 10000, e.id().run() % 10000);
    if (verbosity > 1)
      ERROR("Raw2DigiProducer") << "event " << buf << ":" << e.id().event() <<
        " contains the following problems:\n" << ees.rdbuf() << c_endl;
    else
      ERROR("Raw2DigiProducer") << "event " << buf << ":" << e.id().event() << 
        " contains problems." << c_endl;
  }

  // update error summary
  if (printErrorSummary) {
    for (Raw2DigiStatus::iterator it = conversionStatus->begin(); it != conversionStatus->end(); ++it) {
      if (!it->second.OK()) {
        map<VFATStatus, unsigned int> &m = errorSummary[it->first];
        m[it->second]++;
      }
    }
  }

  // process all GOOD frames
  for (VFATFrameCollection::Iterator fr(input->frames); !fr.IsEnd(); fr.Next()) {
    map<FramePosition, VFATInfo>::const_iterator mappingIter = mapping->VFATMapping.find(fr.Position());
    if (mappingIter == mapping->VFATMapping.end())
      continue;

    VFATStatus &actualStatus = (*conversionStatus)[fr.Position()];

    if (!actualStatus.OK())
      continue;

    // prepare analysis mask class
    map<TotemSymbID, VFATAnalysisMask>::const_iterator analysisIter = analysisMask->analysisMask.find(mappingIter->second.symbolicID);

    // find analysis mask
    VFATAnalysisMask anMa;
    anMa.fullMask = false;
    if (analysisIter != analysisMask->analysisMask.end()) {            
      // if there is some information about masked channels - save it into conversionStatus
      anMa = analysisIter->second;
      if (anMa.fullMask)
        actualStatus.setFullyMaskedOut();
      else
        actualStatus.setPartiallyMaskedOut();
    }
    
    // decide which method should process that frame
    switch (mappingIter->second.symbolicID.subSystem) {
      case TotemSymbID::RP:  
        switch (mappingIter->second.type) {
          case VFATInfo::data:
            rpDataProduce(e, fr, mappingIter->second, anMa);    
            break;
          case VFATInfo::CC:
            //rpCCProduce(e, fr, mappingIter->second, anMa);
            break;
        }  
        break;

      case TotemSymbID::T1:
      //  t1DataProduce(e, fr, mappingIter->second);
        break;

      case TotemSymbID::T2:
        if (mappingIter->second.type==VFATInfo::data)
      //    t2DataProduce(e, fr, mappingIter->second, anMa);
        break;
    }
  }

  // commit products to event
  e.put(rpDataOutput, rpDataProductLabel);
  //e.put(rpCCOutput, rpCCProductLabel);
  e.put(conversionStatus, conversionStatusLabel);

  rpDataEndJob(e);
}

//-------------------------------------------------------------------------------------------------
//                                           RP CC
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//                                             RP DATA
//-------------------------------------------------------------------------------------------------

void Raw2DigiProducer::rpDataBeginJob() 
{

}

//-------------------------------------------------------------------------------------------------

void Raw2DigiProducer::rpDataEndJob(edm::Event& e) 
{
}


//-------------------------------------------------------------------------------------------------

void Raw2DigiProducer::rpDataProduce(edm::Event& e, VFATFrameCollection::Iterator &fr,
  const VFATInfo &info, const VFATAnalysisMask &analysisMask)
{
#ifdef DEBUG
  cout <<  "void Raw2DigiProducer::rpDataProduce(VFATFrameCollection::Iterator &fr, hwID=0x" << hex << info.hwID << dec << ")\n";
#endif
  
  const VFATFrame *frame = fr.Data();
    
  // cache for error messages
  stringstream fes;
 
  // get IDs
  unsigned short symId = info.symbolicID.symbolicID;

  // add RPStripDigi for each hit
  unsigned int detId = TotRPDetId::DecToRawId(symId / 10);
  unsigned short offset = (symId % 10) * 128;
  const vector<unsigned char> activeCh = frame->getActiveChannels();
  DetSet<RPStripDigi> &detSet = rpDataOutput->find_or_insert(detId);

#ifdef DEBUG
  cout << "activeCh.size() = " << activeCh.size() << endl;
#endif

  for (unsigned int j = 0; j < activeCh.size(); j++) {
    // skip masked channels
    if (!analysisMask.fullMask && analysisMask.maskedChannels.find(j) == analysisMask.maskedChannels.end()) {
#ifdef DEBUG
      printf("\tdetSet.push_back(RPStripDigi(detId, offset + activeCh[%d]))\n", j);
#endif
      detSet.push_back(RPStripDigi(detId, offset + activeCh[j]));
    }
  }  
}

//-------------------------------------------------------------------------------------------------
//                                               T1
//-------------------------------------------------------------------------------------------------

DEFINE_FWK_MODULE(Raw2DigiProducer);
