/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *   Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

// this include is a dirty trick
#include "TotemRawData/Readers/plugins/Event_hacked.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/InputSourceMacros.h"
#include "DataFormats/Provenance/interface/RunAuxiliary.h"
#include "DataFormats/Provenance/interface/LuminosityBlockAuxiliary.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"
#include "DataFormats/Provenance/interface/ProcessHistoryRegistry.h"
#include "FWCore/Framework/interface/EventPrincipal.h"
#include "FWCore/Framework/interface/InputSource.h"
#include "DataFormats/Provenance/interface/EventID.h"

#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/Readers/interface/SRSFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/SimpleVFATFrameCollection.h"

#include "TotemRawData/DataFormats/interface/TotemRunNumber.h"

#include <iostream>
#include <iomanip>

//----------------------------------------------------------------------------------------------------

struct TriggerDataBuffer
{
  struct Entry
  {
    bool valid;
    unsigned int file;
    unsigned int fpos;
    Totem::TriggerData data;

    Entry() : valid(false), file(0), fpos(0)
    {
    }
  };

  std::vector<Entry> data;

  unsigned int cursor;

  TriggerDataBuffer(int size = 20) : data(size), cursor(0)
  {
  }

  void Push(unsigned long file, unsigned long fpos, const Totem::TriggerData &d)
  {
    data[cursor].valid = true;
    data[cursor].file = file;
    data[cursor].fpos = fpos;
    data[cursor].data = d;

    cursor = (cursor + 1) % data.size();
  }

  bool Search(unsigned long file, unsigned long fpos, Totem::TriggerData &d) const
  {
    bool found = false;
    for (unsigned int offset = 1; offset <= data.size(); offset++)
    {
      unsigned int idx = (cursor + data.size() - offset) % data.size();
      if (! data[idx].valid)
        continue;

      if (data[idx].file == file && data[idx].fpos == fpos)
      {
        found = true;
        d = data[idx].data;
      }
    }

    return found;
  }
};

//----------------------------------------------------------------------------------------------------

/**
 * \ingroup TotemRawDataLibrary
 * \brief Reads TOTEM raw data from a collection of files according to a specified mapping.
**/

using namespace edm;
using namespace std;
using namespace Totem;

class RawDataMappingSource : public InputSource
{
  public:
    RawDataMappingSource(const edm::ParameterSet &, const edm::InputSourceDescription&);
    virtual ~RawDataMappingSource();

    virtual void beginJob();
    virtual void endJob();

  protected:
    unsigned int verbosity;

    /// name of file with index mapping
    std::string mappingFileName;
    
    /// name of file with input-file list
    std::string listFileName;

    /// list of input raw-data files
    std::vector<std::string> inputFileNames;

    /// run number
    unsigned int runNumber;

  private:
    /// list of the next state items
    deque<ItemType> items;
    
    /// tries to load a next raw event and updates the list of next states 'items'
    void LoadRawDataEvent();

    /// called by the framework to determine the next state (run, lumi, event, stop, ...)
    /// here it simply returns the popped state from the 'items' queue
    virtual ItemType getNextItemType();

    /// called by the framework for item type 'IsRun'
    virtual std::shared_ptr<RunAuxiliary> readRunAuxiliary_();

    /// called by the framework for item type 'IsLumi'
    virtual std::shared_ptr<LuminosityBlockAuxiliary> readLuminosityBlockAuxiliary_();

    /// called by the framework for item type 'IsEvent'
    /// the cached (pre-loaded) raw data (currentRawEvent) are inserted into the event
    virtual void readEvent_(EventPrincipal& eventPrincipal);

    /// pre-loaded raw event
    auto_ptr<RawEvent> currentRawEvent;

    /// ID of the current (next) event
    EventID eventID;

    /// UNIX timestamp of the previous event
    time_t previousTimestamp;

    /// mapping file handle
    FILE *f_map;

    /// buffer for (misaligned) trigger data
    TriggerDataBuffer triggerBuffer;

    /// VFAT frame collection
    SimpleVFATFrameCollection *frameCollection, *frameCollectionTr;

    /// map: file index --> file object
    map<unsigned long, SRSFile *> inputFileMap;

    /// returns pointer to the file object given a file index, opens the file if needed
    SRSFile* GetFile(unsigned long idx);
};

//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;
using namespace Totem;

//----------------------------------------------------------------------------------------------------

RawDataMappingSource::RawDataMappingSource(const edm::ParameterSet& pSet, const edm::InputSourceDescription& desc) :
  InputSource(pSet, desc),
  verbosity(pSet.getUntrackedParameter<unsigned int>("verbosity", 0)),
  mappingFileName(pSet.getUntrackedParameter<string>("mappingFileName")),
  listFileName(pSet.getUntrackedParameter<string>("listFileName")),
  runNumber(pSet.getUntrackedParameter<unsigned int>("runNumber")),

  eventID(0, 0, 0),
  previousTimestamp(0),
  f_map(NULL), triggerBuffer(20), frameCollection(NULL), frameCollectionTr(NULL)
{
  produces<Totem::RawEvent>();
}

//----------------------------------------------------------------------------------------------------

RawDataMappingSource::~RawDataMappingSource()
{
}

//----------------------------------------------------------------------------------------------------

std::shared_ptr<RunAuxiliary> RawDataMappingSource::readRunAuxiliary_()
{
  Timestamp ts_beg(currentRawEvent->timestamp << 32);
  Timestamp ts_end(Timestamp::endOfTime().value() - 0);

  return std::shared_ptr < RunAuxiliary > (new RunAuxiliary(eventID.run(), ts_beg, ts_end));
}

//----------------------------------------------------------------------------------------------------

std::shared_ptr<LuminosityBlockAuxiliary> RawDataMappingSource::readLuminosityBlockAuxiliary_()
{
  // we create luminosity blocks of 1s duration
  Timestamp ts_beg(currentRawEvent->timestamp << 32);
  Timestamp ts_end(((currentRawEvent->timestamp + 1) << 32) - 1);

  return std::shared_ptr <LuminosityBlockAuxiliary> (new LuminosityBlockAuxiliary(eventID.run(),
    eventID.luminosityBlock(), ts_beg, ts_end));
}

//----------------------------------------------------------------------------------------------------

void RawDataMappingSource::readEvent_(EventPrincipal& eventPrincipal)
{
  // create Event structure and fill with auxiliary data
  Timestamp ts(currentRawEvent->timestamp << 32);  // conversion from UNIX timestamp: see src/DataFormats/Provenance/interface/Timestamp.h
  bool isRealData = true;
  EventAuxiliary::ExperimentType expType(EventAuxiliary::Undefined);
  EventAuxiliary aux(eventID, processGUID(), ts, isRealData, expType);

  ProcessHistoryRegistry phr;
  eventPrincipal.fillEventPrincipal(aux, phr);
  ModuleCallingContext const* mcc = NULL;
  Event e(eventPrincipal, moduleDescription(), mcc);

  // put raw data into the event
  e.put(currentRawEvent);
  e.commit_();
}

//----------------------------------------------------------------------------------------------------

void RawDataMappingSource::LoadRawDataEvent()
{
  // prepare structure for the raw event
  currentRawEvent = auto_ptr<RawEvent>(new RawEvent(frameCollection, false));

  // try to get next line (event) from the mapping
  if (feof(f_map))
  {
    items.push_back(IsStop);
    return;
  }
  
  unsigned int idx;
  unsigned int da_file_idx, da_fpos;
  unsigned int tr_file_idx, tr_fpos;

  int ir = fscanf(f_map, "%u,%u,%u,%u,%u", &idx, &da_file_idx, &da_fpos, &tr_file_idx, &tr_fpos);
  if (ir == 0)
  {
    items.push_back(IsStop);
    return;
  }

  if (ir != 5)
  {
    if (feof(f_map))
    {
      items.push_back(IsStop);
      return;
    }

    throw cms::Exception("RawDataMappingSource::LoadRawDataEvent") << "Malformed row in the mapping file, only " << ir
      << " items read." << std::endl;
  }

  // load non-trigger data
  SRSFile *da_file = GetFile(da_file_idx);
  da_file->Seek(da_fpos);
  unsigned int readStatus = da_file->GetNextEvent(currentRawEvent.get());
  if (readStatus != 0)
    printf("ERROR: can't read non-trigger data. Mapping index %u.\n", idx);

  // save trigger data to buffer
  triggerBuffer.Push(da_file_idx, da_fpos, currentRawEvent.get()->triggerData);

  // get trigger data
  RawEvent event_tr(frameCollectionTr, false);
  bool foundInBuffer = triggerBuffer.Search(tr_file_idx, tr_fpos, event_tr.triggerData);
  //printf("    foundInBuffer = %i\n", foundInBuffer);

  if (!foundInBuffer)
  {
    SRSFile *tr_file = GetFile(tr_file_idx);
    tr_file->Seek(tr_fpos);
    unsigned int readStatus = tr_file->GetNextEvent(&event_tr);
    if (readStatus != 0)
      printf("ERROR: can't read trigger data. Mapping index %u.\n", idx);
  }

  // apply correction
  currentRawEvent.get()->triggerData = event_tr.triggerData;   

  // validation
  //printf("    %u, %u\n", currentRawEvent.get()->dataEventNumber & 0xFFFFFF, currentRawEvent.get()->triggerData.event_num);

  signed int offset = currentRawEvent.get()->triggerData.event_num - ( (currentRawEvent.get()->dataEventNumber+1) & 0xFFFFFF);
  if (offset < 0)
    offset += 0x1000000;

  if (offset != 0)
  {
    printf("ERROR: offset = %u. Mappping idx = %u. Event number: trigger = %u, daq = %lu. Trigger from buffer = %i\n",
      offset, idx, currentRawEvent.get()->triggerData.event_num, currentRawEvent.get()->dataEventNumber, foundInBuffer);
  }

  // new run ?
  bool beginning = (eventID.run() == 0);

  if (beginning)
  {
    items.push_back(IsRun);
    items.push_back(IsLumi);
    items.push_back(IsEvent);

    eventID = EventID(runNumber, 1, idx+1);

    previousTimestamp = currentRawEvent->timestamp;

    return;
  } 

  // new luminosity block ??
  if (currentRawEvent->timestamp != previousTimestamp)  // 1s resolution
  {
    items.push_back(IsLumi);
    eventID = EventID(eventID.run(), eventID.luminosityBlock() + 1, eventID.event()); // advance lumi number
  }

  previousTimestamp = currentRawEvent->timestamp;

  eventID = EventID(eventID.run(), eventID.luminosityBlock(), idx+1); // set new event number
  items.push_back(IsEvent);
}

//----------------------------------------------------------------------------------------------------

InputSource::ItemType RawDataMappingSource::getNextItemType()
{
  if (items.empty())
    LoadRawDataEvent();

  ItemType item = items.front();
  items.pop_front();

  return item;
}

//----------------------------------------------------------------------------------------------------

void RawDataMappingSource::beginJob()
{
  // adapt run number to the scheme
  runNumber = TotemRunNumber(runNumber, 11, 1, 0).ToCMSSWRunNumber();
  printf(">> CMS run number: %u\n", runNumber);

  // load input files
  printf(">> list of input files: %s\n", listFileName.c_str());

  FILE *f = fopen(listFileName.c_str(), "r");
  if (!f)
    throw cms::Exception("RawDataMappingSource::beginJob") << "Cannot open file list in '" << listFileName
      << "'." << std::endl;

  while (!feof(f))
  {
    char buf[500];
    char *res = fgets(buf, 500, f);

    if (res == NULL)
      continue;

    buf[strlen(buf)-1] = 0;
    inputFileNames.push_back(buf);
  }

  printf("    %lu input files loaded.\n", inputFileNames.size());

  // open mapping
  printf(">> mapping file: %s\n", mappingFileName.c_str());

  f_map = fopen(mappingFileName.c_str(), "r");
  if (!f_map)
    throw cms::Exception("RawDataMappingSource::beginJob") << "Cannot open mapping file '" << mappingFileName
      << "'." << std::endl;

  // prepare data structures
  frameCollection = new SimpleVFATFrameCollection;
  frameCollectionTr = new SimpleVFATFrameCollection;

  // needed for the logic in InputSource::nextItemType
  items.push_back(IsFile);   
}

//----------------------------------------------------------------------------------------------------

void RawDataMappingSource::endJob()
{
  if (f_map)
    fclose(f_map);

  if (frameCollection)
  {
    delete frameCollection;
    delete frameCollectionTr;
  }

  for (auto p : inputFileMap)
    delete p.second;
}

//----------------------------------------------------------------------------------------------------

SRSFile* RawDataMappingSource::GetFile(unsigned long idx)
{
  auto it = inputFileMap.find(idx);
  if (it != inputFileMap.end())
    return it->second;

  SRSFile *f = new SRSFile();
  auto status = f->Open(inputFileNames[idx]);
  if (status != DataFile::osOK)
  {
    throw cms::Exception("RawDataMappingSource::GetFile") << "Can't open file with idx " << idx
      << " and name '" << inputFileNames[idx] << "'." << std::endl;
  }
  
  inputFileMap[idx] = f;
  return f;
}

DEFINE_FWK_INPUT_SOURCE(RawDataMappingSource);
