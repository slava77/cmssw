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

#include "TotemRawData/DataFormats/interface/TotemRunNumber.h"

#include <iostream>
#include <iomanip>

/**
 * \ingroup TotemRawDataLibrary
 * \brief Reads in TOTEM raw data file(s) and puts Totem::RawEvent into the edm::Event structure.
 *
 * Each input file is assigned one CMS run number. If setRunNumberFromFileName is set to false,
 * the runs are enumerated consecutively: 1, 2, 3... If setRunNumberFromFileName is true, run
 * numbers are extracted from file names. The following file name format is expected:
 * \verbatim path/some text<run>some text<event-builder>some text<process>some text<file>some text\endverbatim
 * The "/" denotes the last slash in the path. <run>, <event-builder>, <process> and <file> contain digits
 * only. The CMSSW run number is compiled by class TotemRunNumber.
 *
 * The module starts a new luminosity block whenever the UNIX timestamp of raw event changes.
 * In CMSSW 7.x it is essential for synchronisation with EventSetup: it can only be updated at
 * luminosity-block boundaries.
**/

using namespace edm;
using namespace std;
using namespace Totem;

class RawDataSource : public InputSource
{
  public:
    struct FileInfo
	{
      std::string fileName;         ///< path to the file
      Totem::DataFile *file;        ///< instance of appropriate reader class
      unsigned int collectionIdx;   ///< index in the 'collections' list
      edm::RunNumber_t runNumber;   ///< associated run number
    };

    RawDataSource(const edm::ParameterSet &, const edm::InputSourceDescription&);
    virtual ~RawDataSource();
    virtual void beginJob();
    virtual void endJob();

  protected:
    unsigned int verbosity;

    std::vector<std::string> fileNames;                     ///< vector of raw data files names
    bool setRunNumberFromFileName;                          ///< whether run number shall be set from file names (see GetRunNumberFromFileName method)
    unsigned int printProgressFrequency;                    ///< frequency with which the progress (i.e. event number) is to be printed

    unsigned int fileIdx;                                   ///< current file index (within files), counted from 0

    friend class ProcessManager;
    std::vector<FileInfo> files;                            ///< to keep information about opened files
    std::vector<Totem::VFATFrameCollection *> collections;  ///< the list of instantiated data collections

    /// \brief extracts run number from a file name
    edm::RunNumber_t GetRunNumberFromFileName(const std::string &str);

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
};

//----------------------------------------------------------------------------------------------------

using namespace std;
using namespace edm;
using namespace Totem;


//#define DEBUG 1

//----------------------------------------------------------------------------------------------------

RawDataSource::RawDataSource(const edm::ParameterSet& pSet, const edm::InputSourceDescription& desc) :
  InputSource(pSet, desc),
  verbosity(pSet.getUntrackedParameter<unsigned int>("verbosity", 0)),
  fileNames(pSet.getUntrackedParameter<vector<string> >("fileNames")),
  setRunNumberFromFileName(pSet.getParameter<bool>("setRunNumberFromFileName")),
  printProgressFrequency(pSet.getUntrackedParameter<unsigned int>("printProgressFrequency", 0)),
  eventID(0, 0, 0),
  previousTimestamp(0)
{
#ifdef DEBUG
  cout << ">> RawDataSource::RawDataSource" << endl;
#endif
  produces<Totem::RawEvent>();
}

//----------------------------------------------------------------------------------------------------

RawDataSource::~RawDataSource()
{
}

//----------------------------------------------------------------------------------------------------

std::shared_ptr<RunAuxiliary> RawDataSource::readRunAuxiliary_()
{
#ifdef DEBUG
  printf(">> RawDataSource::readRunAuxiliary_\n");
#endif

  Timestamp ts_beg(currentRawEvent->timestamp << 32);
  Timestamp ts_end(Timestamp::endOfTime().value() - 0);

  return std::shared_ptr < RunAuxiliary > (new RunAuxiliary(eventID.run(), ts_beg, ts_end));
}

//----------------------------------------------------------------------------------------------------

std::shared_ptr<LuminosityBlockAuxiliary> RawDataSource::readLuminosityBlockAuxiliary_()
{
#ifdef DEBUG
  printf(">> RawDataSource::readLuminosityBlockAuxiliary_\n");
#endif

  // we create luminosity blocks of 1s duration
  Timestamp ts_beg(currentRawEvent->timestamp << 32);
  Timestamp ts_end(((currentRawEvent->timestamp + 1) << 32) - 1);

  return std::shared_ptr <LuminosityBlockAuxiliary> (new LuminosityBlockAuxiliary(eventID.run(),
    eventID.luminosityBlock(), ts_beg, ts_end));
}

//----------------------------------------------------------------------------------------------------

void RawDataSource::readEvent_(EventPrincipal& eventPrincipal)
{
#ifdef DEBUG
  printf(">> RawDataSource::readEvent_ : %lu, %lu | %u\n", currentRawEvent->dataEventNumber, currentRawEvent->timestamp, eventID.event());
#endif
  
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

  // TODO
  //if (printProgressFrequency > 0 && (event() % printProgressFrequency) == 0)
  //  cout << "\033[25Devent " << run() << ":" << event();
}

//----------------------------------------------------------------------------------------------------

void RawDataSource::LoadRawDataEvent()
{
#ifdef DEBUG
  printf(">> RawDataSource::LoadRawDataEvent\n");
#endif

  // prepare structure for the raw event
  currentRawEvent = auto_ptr<RawEvent>(new RawEvent(collections[files[fileIdx].collectionIdx], false));

  // load next raw event
  bool newFile = false;
  while (true)
  {
    unsigned int result = files[fileIdx].file->GetNextEvent(currentRawEvent.get());

    if (result == 0)
      break;
    else {
      // try moving to next file
      fileIdx++;
      newFile = true;

      // stop if there are no more files
      if (fileIdx >= files.size())
      {
        items.push_back(IsStop);
        return;
      }
    }
  }

#ifdef DEBUG
  printf("\t%lu, %lu\n", currentRawEvent->dataEventNumber, currentRawEvent->timestamp);
#endif

  bool beginning = (eventID.run() == 0);

  if (newFile || beginning)
  {
    items.push_back(IsRun);
    items.push_back(IsLumi);
    items.push_back(IsEvent);

    eventID = EventID(files[fileIdx].runNumber, 1, 1);

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

  eventID = EventID(eventID.run(), eventID.luminosityBlock(), eventID.event() + 1); // advance event number
  items.push_back(IsEvent);
}

//----------------------------------------------------------------------------------------------------

#ifdef DEBUG
InputSource::ItemType prevItem = InputSource::ItemType::IsInvalid;
#endif

InputSource::ItemType RawDataSource::getNextItemType()
{
  if (items.empty())
    LoadRawDataEvent();

  ItemType item = items.front();
  items.pop_front();

#ifdef DEBUG
  if (prevItem != item)
    printf(">> RawDataSource::getNextItemType > %u --> %u (run=%u, lumi block=%u, event=%u)\n", prevItem, item,
      eventID.run(), eventID.luminosityBlock(), eventID.event());

  prevItem = item;
#endif

  return item;
}

//----------------------------------------------------------------------------------------------------

void RawDataSource::beginJob()
{
#ifdef DEBUG
  printf(">> RawDataSource::beginJob\n");
#endif

  // try to open all files
  for (unsigned int i = 0; i < fileNames.size(); i++)
  {
	if(verbosity)
    {
		cout<< "\n>> RawDataSource::beginJob > Opening file `" << fileNames[i] << "'." << endl;
	}

    // add prefix
    FileInfo fi;
    char *cmsswPath = getenv("CMSSW_BASE");
    fi.fileName = fileNames[i];
    fi.fileName.erase(0, fi.fileName.find_first_not_of(' '));
    if (fi.fileName[0] != '/' && fi.fileName.find("./") != 0 && fi.fileName.find("://") == string::npos)
      if (cmsswPath)
        fi.fileName = string(cmsswPath) + string("/src/") + fi.fileName;

    // run number
    if (setRunNumberFromFileName)
      fi.runNumber = GetRunNumberFromFileName(fi.fileName);
    else
      fi.runNumber = i + 1;

    // open file
    fi.file = Totem::DataFile::OpenStandard(fi.fileName);
    if (!fi.file)
      throw cms::Exception("RawDataSource") << "Cannot open file " << fileNames[i] << std::endl;

    // start indexing
    fi.file->StartIndexing();

    // instantiate appropriete collection object (only once per type)
    bool found = false;
    for (unsigned int j = 0; j < collections.size(); j++)
    {
      if (fi.file->IsCollectionCompatible(collections[j]))
      {
        fi.collectionIdx = j;
        found = true;
        break;
      }
    }

#ifdef DEBUG
    printf("\tcollection found = %u\n", found);
#endif

    if (!found)
    {
      fi.collectionIdx = collections.size();
#ifdef DEBUG
      printf("\tcreating new collection\n");
#endif
      collections.push_back(fi.file->CreateCollection());
    }

    files.push_back(fi);
  }

  if (!files.size())
    throw cms::Exception("RawDataSource") << "No files to read." << std::endl;

  // print file info
  if (verbosity > 0)
  {
    printf(">> RawDataSource::beginJob > files opened:\n");
    printf("idx|   run.eb.p.file | file name                                                    | file type  | collection type\n");
    for (unsigned int i = 0; i < files.size(); i++)
    {
      TotemRunNumber trn(files[i].runNumber);
      unsigned int len = files[i].fileName.size();
      string shortFileName = (len <= 60) ? files[i].fileName : string("...") + files[i].fileName.substr(len - 57);
      printf("%2u | %15s | %60s | %10s | %25s", i, trn.ToStdString().c_str(), shortFileName.c_str(),
          files[i].file->GetClassName().c_str(), collections[files[i].collectionIdx]->GetClassName().c_str());
      cout << endl;
    }
  }

  // initialize the state of reader
  fileIdx = 0;

  items.push_back(IsFile);  // needed for the logic in InputSource::nextItemType 
}

//----------------------------------------------------------------------------------------------------

void RawDataSource::endJob()
{
#ifdef DEBUG
  printf(">> RawDataSource::endJob\n");
#endif

  for (unsigned int j = 0; j < collections.size(); j++)
  {
#ifdef DEBUG
    printf("\t%u\t%p\n", j, collections[j]);
#endif
    delete collections[j];
  }
}

//----------------------------------------------------------------------------------------------------

edm::RunNumber_t RawDataSource::GetRunNumberFromFileName(const string &path)
{
  // get file part of path
  size_t pos = path.find_last_of('/');
  string s = (pos == string::npos) ? path : path.substr(pos + 1);

  // build array of numerical bits in the name
  string digits = "0123456789";
  vector<unsigned int> nBits;
  pos = 0;
  while (pos != string::npos)
  {
    size_t p1 = s.find_first_of(digits, pos);
    if (p1 == string::npos)
      break;

    size_t p2 = s.find_first_not_of(digits, p1);

    string valStr = (p2 == string::npos) ? s.substr(p1) : s.substr(p1, p2 - p1);
    nBits.push_back(atoi(valStr.c_str()));

    pos = p2;
  }

  // compile CMSSW run number
  if (nBits.size() == 3)
  {
    // assumes the old format with no eventBuilderProcess number  
    TotemRunNumber trn(nBits[1], nBits[0], 1, nBits[2]);
    return trn.ToCMSSWRunNumber();
  }

  if (nBits.size() == 4)
  {
    TotemRunNumber trn(nBits[0], nBits[1], nBits[2], nBits[3]);
    return trn.ToCMSSWRunNumber();
  }

  throw cms::Exception("RawDataSource::GetRunNumberFromFileName")
    << "Run and file number cannot be extracted from file name `" << path << "'." << endl;
}

DEFINE_FWK_INPUT_SOURCE(RawDataSource);
