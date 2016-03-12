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
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>

using namespace Totem;
using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
  printf("USAGE: checkFile [options] ... <data file>\n");
  printf("OPTIONS:\n");
  printf("\t-events <n>\twill check n first events, default 10\n");
  printf("\t-no-strict\twill allow first event with non-zero EC (for data taking)\n");
  printf("\t-xml\t\twill format output in XML format\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  unsigned short inputIndex = 0;
  unsigned int numEvChck = 10;
  bool strict = true;
  bool xml = false;

  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "-events"))
	{
		if (++i >= argc)
		{
			printf("ERROR: missing argument to -events option.\n");
			PrintUsage();
			return 1;
		}

		numEvChck = atoi(argv[i]);

		continue;
	}

    if (!strcmp(argv[i], "-no-strict"))
	{
		strict = false;
		continue;
	}

    if (!strcmp(argv[i], "-xml"))
	{
		xml = true;
		continue;
	}

    if (argv[i][0] != '-')
	{
		inputIndex = i;
		continue;
	}

    printf("ERROR: Unrecognized parameter `%s'.\n", argv[i]);
    PrintUsage();
    return 5;
  }

  if (!inputIndex)
  {
	printf("ERROR: You must specify input file.\n");
	PrintUsage();
	return 6;
  }

  if (numEvChck == 0)
    numEvChck = 10;

  DataFile* input = DataFile::OpenStandard(argv[inputIndex]);
  if (!input)
  {
    printf("Error in opening file.\n");
    return 2;
  }

  /// run test
  TestECProgress test;
  vector<TestECProgress::Entry> workingVFATs;
  unsigned short result = test.run(workingVFATs, input, numEvChck, strict, 0);
  if (result)
    return result;

  /// print results
  if (workingVFATs.size())
  {
    if (!xml)
      printf("Working VFATs are the following:\nID 12bit\tDAQ channel\n");
    else
      printf("<top>\n");

    for (unsigned int i = 0; i < workingVFATs.size(); i++)
    {
      if (!xml)
      {
        cout << "0x" << hex << workingVFATs[i].ID << "\t\t" << workingVFATs[i].index << endl;
      } else {
        cout << "\t <test_vfat id=" << hex << workingVFATs[i].ID << " ";
        workingVFATs[i].index.PrintXML();
        cout << ">" << endl;
      }
    }

    if (xml)
      printf("</top>\n");
  } else
    ERROR("checkFile") << "No working VFAT found." << c_endl;

  return 0;
}
