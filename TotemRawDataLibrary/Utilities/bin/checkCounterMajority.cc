/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrame.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrameCollection.h"
#include "TotemRawDataLibrary/Utilities/interface/TestECProgress.h"

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <map>

using namespace Totem;
using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
  printf("USAGE: checkCounterMajority [options] <data file>\n");
  printf("OPTIONS:\n");
  printf("    -events <num>    number of events to process\n");
  printf("                     default: process all available events\n");
}

//----------------------------------------------------------------------------------------------------

struct VFATData
{
  FramePosition p;
  VFATFrame *f;

  VFATData(FramePosition _p, VFATFrame *_f) : p(_p), f(_f) {}
};

//----------------------------------------------------------------------------------------------------

struct OptoRxState
{
  bool initialized;
  unsigned int event;

  unsigned int numVFATsFailing;

  unsigned int numVFATsWithMinorityEC;
  vector<FramePosition> vfatsWithMinorityEC;

  OptoRxState() : initialized(false), event(0), numVFATsFailing(0), numVFATsWithMinorityEC(0) {}
};

//----------------------------------------------------------------------------------------------------

void AnalyzeOne(const vector<VFATData> &input, OptoRxState &output)
{
  // frequency map: EC --> count
  map<unsigned int, unsigned int> fMap;
  for (vector<VFATData>::const_iterator it = input.begin(); it != input.end(); ++it)
  {
    // discard failing VFATs
    if (!it->f->checkFootprint() || !it->f->checkCRC())
      continue;

    unsigned int EC = it->f->getEC();
    map<unsigned int, unsigned int>::iterator fit = fMap.find(EC);
    if (fit == fMap.end())
      fMap[EC] = 0;
    else
      fMap[EC]++;
  }

  // find maximum
  unsigned int max_f = 0;
  unsigned int max_EC = 0;
  for (map<unsigned int, unsigned int>::iterator it = fMap.begin(); it != fMap.end(); ++it)
  {
    if (it->second > max_f)
    {
      max_f = it->second;
      max_EC = it->first;
    }
  }

  //printf("    max_EC = %i, max_f = %i\n", max_EC, max_f);

  // count VFATs failing and with different EC
  unsigned int failing = 0;
  unsigned int diffEC = 0;
  for (vector<VFATData>::const_iterator it = input.begin(); it != input.end(); ++it)
  {
    // discard failing VFATs
    if (!it->f->checkFootprint() || !it->f->checkCRC())
    {
      failing++;
      continue;
    }

    unsigned int EC = it->f->getEC();
    if (EC != max_EC)
    {
      diffEC++;
      output.vfatsWithMinorityEC.push_back(it->p);
    }
  }

  //printf("    diffEC = %i\n", diffEC);

  // save output
  output.numVFATsFailing = failing;
  output.numVFATsWithMinorityEC = diffEC;
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  // parse command-line parameters
  unsigned short inputIndex = 0;
  unsigned int maxEvents = 0;
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-events") == 0)
    {
      if (++i >= argc)
      {
        printf("ERROR: Option -events requires an argument.\n");
        PrintUsage();
        return 1;
      }

      maxEvents = atoi(argv[i]);

      continue;
    }

    if (argv[i][0] != '-')
    {
      inputIndex = i;
      continue;
    }

    printf("ERROR: Unrecognized option `%s'.\n", argv[i]);
    PrintUsage();
    return 5;
  }

  if (inputIndex == 0)
  {
	printf("ERROR: You must specify an input file.\n");
	PrintUsage();
	return 6;
  }

  // open file
  DataFile* input = DataFile::OpenStandard(argv[inputIndex]);
  if (!input)
  {
    printf("Error in opening file\n");
    return 2;
  }

  RawEvent *event = input->CreateEvent();

  // define which OptoRx's shall be controlled
  map<unsigned int, OptoRxState> states;
  states[417] = OptoRxState();
  states[418] = OptoRxState();
  states[425] = OptoRxState();
  states[426] = OptoRxState();
  states[437] = OptoRxState();
  states[445] = OptoRxState();

  // event loop
  input->StartIndexing();
  unsigned long evIdx = 0;
  while (!input->GetNextEvent(event))
  {
    //printf(">> event: index=%lu, daqNumber=%lu\n", evIdx, event->dataEventNumber);

    // split VFATs per OptoRxId
    map<unsigned short, vector<VFATData> > vfatMap;
    for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
    {
      unsigned short OptoRxId = it.Position().GetFullOptoRxId();
      vfatMap[OptoRxId].push_back(VFATData(it.Position(), (VFATFrame *) it.Data()));
      
      //printf("* %u --> %x\n", OptoRxId, OptoRxId >> 7);
    } 

    // check each required OptoRx
    for (map<unsigned int, OptoRxState>::iterator it = states.begin(); it != states.end(); ++it)
    {
      const unsigned int &OptoRxId = it->first;
      OptoRxState &stOld = it->second;

      // are data for the OptoRx available in this event?
      map<unsigned short, vector<VFATData> >::iterator dit = vfatMap.find(OptoRxId);
      if (dit == vfatMap.end())
      {
        printf(">> OptoRx Id=0x%03x not available in event %lu\n", OptoRxId, evIdx);
        continue;
      }

      // run analysis
      OptoRxState stNew;
      stNew.initialized = true;
      stNew.event = evIdx;
      AnalyzeOne(dit->second, stNew);

      // compare new and old states
      if (!stOld.initialized || stOld.numVFATsWithMinorityEC + 4 <= stNew.numVFATsWithMinorityEC
        || stOld.numVFATsFailing + 4 <= stNew.numVFATsFailing)
      {
        printf(">> OptoRxId=0x%03x=%x:%02x:%x > change of state\n", OptoRxId,
          (OptoRxId >> 7) & 7,
          (OptoRxId >> 2) & 31,
          (OptoRxId >> 0) & 3
        );

        if (stOld.initialized)
          printf("    old: event=%u, numVFATsFailing=%u, numVFATsWithMinorityEC=%u\n", stOld.event, stNew.numVFATsFailing, stOld.numVFATsWithMinorityEC);
        else
          printf("    old: not initialised\n");

        printf("    new: event=%u, numVFATsFailing=%u, numVFATsWithMinorityEC=%u\n", stNew.event, stNew.numVFATsFailing, stNew.numVFATsWithMinorityEC);

        if (stNew.vfatsWithMinorityEC.size() > 0)
        {
          printf("         VFATsWithMinorityEC: ");
          for (unsigned int i = 0; i < stNew.vfatsWithMinorityEC.size(); i++)
          {
            const FramePosition &fp = stNew.vfatsWithMinorityEC[i];
            printf("%u:%u, ", fp.GetGOHId(), fp.GetIdxInFiber());
          }
          printf("\n");
        }

        if (stNew.numVFATsFailing >= 8)
        {
          printf("    !!!!!!!!!!!!!!!!!!!!!!!!! CHECK FAILING VFATS !!!!!!!!!!!!!!!!!!!!!!!!!\n");
          printf("\a\a\a\a"); // should make a beep
        }

        if (stNew.numVFATsWithMinorityEC >= 8)
        {
          printf("    !!!!!!!!!!!!!!!!!!!!!!!!! CHECK SYNCHRONISATION !!!!!!!!!!!!!!!!!!!!!!!!!\n");
          printf("\a\a\a\a"); // should make a beep
        }
        
        stOld = stNew;
      }
    }

    // stop if number of events has reached maximum
    if (maxEvents > 0 && evIdx >= maxEvents - 1)
      break;

    evIdx++;
  }

  // release objects
  delete input;

  return 0;
}
