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
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"

#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ctime>


using namespace Totem;
using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
  printf("USAGE: analyzeFile [options] <data file>\n");
  printf("OPTIONS:\n");
  printf("    -events <num>    number of events to process\n");
  printf("                     default: process all available events\n");
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
        printf("ERROR: Option -events requires a parameter.\n");
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

  // event loop
  unsigned long eventCount = 0;
  RawEvent *event = input->CreateEvent();
  while (true)
  {
    printf("\n----------------------------------------------------------------------------------------------------\n");

    // load next event
    if (input->GetNextEvent(event))
      break;

    // total events counter
    eventCount++;  

    // count "working" chips
    unsigned int wc = 0;
    for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
      if (it.Data()->checkFootprint())
        wc++;

    printf("> event %lu\n\tDAQ event number: %lu\n\tDAQ timestamp: %s\t%u out of %u VFATs active\n",
      eventCount, event->dataEventNumber, ctime(&event->timestamp), wc, event->frames->Size());

    // print metadata
    for (map<unsigned int, OptoRxMetaData>::const_iterator it = event->optoRxMetaData.begin(); 
      it != event->optoRxMetaData.end(); ++it)
    {
      printf("\tOptoRx = 0x%x: BX = 0x%x, LV1 = 0x%x\n", it->first, it->second.BX, it->second.LV1);
    }

    // print all VFATs
    for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
    {
      if (!it.Data()->checkFootprint())
        continue;

      cout << it.Position() << " > ";
      it.Data()->Print();

      //vector<unsigned char> ac = it.second->getActiveChannels();
      //printf(" (%lu)\n", ac.size());
    }

    // stop if number of events has reached maximum
    if (maxEvents > 0 && eventCount >= maxEvents)
      break;
  }
    
  // info
  printf("\n>> STATISTICS:\n");
  printf("Events loaded: %lu\n", eventCount);
  printf("Corrupted events: %lu\n", input->GetEventsCorrupted());
  return 0;
}

