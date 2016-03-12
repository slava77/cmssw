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
#include <csignal>

using namespace Totem;
using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
  printf("USAGE: readFile [options] <data file>\n");
  printf("OPTIONS:\n");
  printf("    -events <num>    number of events to process\n");
  printf("                     default: process all available events\n");
}

//----------------------------------------------------------------------------------------------------
// SIG_INT handling

bool shallStop = false;

void SigIntHandler(int)
{
    printf(">> User interrupt.\n");
    shallStop = true;
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

  RawEvent *event = input->CreateEvent();

  input->StartIndexing();
  unsigned long long evCounter = 0;
  time_t firstTS, lastTS;
  double mean_skew = 0;
  double mean_ldc_count = 0, mean_ldc_spread = 0;
  map<unsigned int, pair<unsigned int, double> > perLDCStat;

  signal(SIGINT, SigIntHandler);
  while (!shallStop)
  {
    if (input->GetNextEvent(event))
      break;

    // first and last GDC timestamp
    if (!evCounter)
      firstTS = event->timestamp;
    lastTS = event->timestamp;

    // LDC statistics
    unsigned int S1=0;
    double St=0, Stt=0;
    for (map<unsigned int, time_t>::iterator it = event->ldcTimeStamps.begin(); it != event->ldcTimeStamps.end(); ++it)
    {
      S1 += 1;
      St += it->second;
      Stt += it->second * it->second;

      perLDCStat[it->first].first++;
      perLDCStat[it->first].second += (event->timestamp - it->second);
    }

    double ldc_mean = (S1 > 0) ? (St / S1) : 0.;
    double ldc_var = (S1 > 1) ? (Stt - St*St/S1) / (S1 - 1) : 0.;
    double ldc_spread = (ldc_var >= 0.) ? sqrt(ldc_var) : 0.;

    // GDC - LDC time skew
    mean_skew += (event->timestamp - ldc_mean);
    mean_ldc_count += S1;
    mean_ldc_spread += ldc_spread;

    // event counter
    evCounter++;

    // stop if number of events has reached maximum
    if (maxEvents > 0 && evCounter >= maxEvents)
      break;
  }

  mean_skew /= evCounter;
  mean_ldc_count /= evCounter;
  mean_ldc_spread /= evCounter;

  printf("%llu events read\n", evCounter);
  printf("mean LDC number: %.1f\n", mean_ldc_count);
  printf("mean LDC spread: %.1f s\n", mean_ldc_spread);
  printf("mean GDC - LDC time skew: %.1f s\n", mean_skew);

  for (map<unsigned int, pair<unsigned int, double> >::iterator it = perLDCStat.begin(); it != perLDCStat.end(); ++it)
    printf("\tmean GDC time skew wrt. LDC #%u: %.1f s\n", it->first, it->second.second / it->second.first);

  printf("first (GDC) timestamp: %s", ctime(&firstTS));
  printf("last (GDC) timestamp: %s", ctime(&lastTS));

  delete input;

  return 0;
}
