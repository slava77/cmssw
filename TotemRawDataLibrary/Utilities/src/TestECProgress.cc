/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "TotemRawDataLibrary/DataFormats/interface/CommonDef.h"
#include "TotemRawDataLibrary/Utilities/interface/TestECProgress.h"
#include "TotemRawDataLibrary/Readers/interface/DataFile.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrame.h"

#include <map>
#include <iostream>
#include <sstream>

using namespace std;

unsigned short Totem::TestECProgress::run(std::vector<Entry> &workingVFATs, DataFile *file, unsigned short numEvChck,
    bool strict, unsigned int verbosityTest)
{
  if (!file)
    return 1;

  if (verbosityTest)
    INFO("TestECProgress::run") << "EC progress test will probe " << numEvChck << 
      " events in " << (strict ? "strict" : "non-strict") << " regime." << c_endl;

  // map keeping VFAT status, working VFATs are presumed
  map<FramePosition, bool> vfatBad;

  // map keeping EC values to be checked (value from the previous event +1)
  map<FramePosition, unsigned int> checkEC;

   // search for a working VFAT (go through numEvChck events)
  RawEvent *event = file->CreateEvent();
  for (unsigned int evCount = 0; evCount < numEvChck; evCount++)
  {
    // try to load the next event
    if (file->GetNextEvent(event))
    {
      WARN("TestECProgress::run") << "There are only " << evCount << " events in input file." << c_endl;
      break;
    }

    if (verbosityTest > 9)
      cout << "---------- event " << evCount << " ----------" << c_endl;

    // initialize the above maps
    if (evCount == 0)
    {
      for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
      {
        vfatBad.insert(std::pair<FramePosition, bool>(it.Position(), false));
        checkEC.insert(std::pair<FramePosition, unsigned int>(it.Position(), 0));
      }
    }

    // start loop over VFATs
    for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
    {
      const FramePosition &fp = it.Position();
      const VFATFrame* frame = it.Data();
     
      // check whether it is known (it should be!)
      if (vfatBad.find(fp) == vfatBad.end())
        continue;

      // check whether it is still good
      if (vfatBad[fp])
        continue;
        
      // check footprint
      if (! (frame->checkFootprint()) )
      {
        vfatBad[fp] = true;
        if (verbosityTest > 9)
          cout << fp << ": bad footprint" << c_endl;
      }

      // check CRC
      if (! (frame->checkCRC()) )
      {
        vfatBad[fp] = true;
        if (verbosityTest > 9)
          cout << fp << ": bad crc" << c_endl;
      }

      // check event counter progress
      unsigned short EC = frame->getEC();
      if (EC != checkEC[fp] && !(evCount == 0 && !strict))
      {
        vfatBad[fp] = true; 
        if (verbosityTest > 9)
          cout << fp << ": bad EC (actual " << EC << ", expected " << checkEC[fp] << ")" << c_endl;
      } else {
        if (strict)
          checkEC[fp] = (EC + 1) % 100;
        else
          checkEC[fp] = (EC + 1) % 256;
      }
    }
  }

  // check whether all the EC are the same for all working VFATS
  char sameECFlag = 0;
  unsigned short sameECVal = 0;
  for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
  {
    const FramePosition &fp = it.Position();
    if (vfatBad[fp])
      continue;

    unsigned short EC = it.Data()->getEC();
    if (sameECFlag == 0)
    {
      sameECVal = EC;
      sameECFlag = 1;
    } else 
    {
      if (EC != sameECVal)
      {
        sameECFlag = 2;
        break;
      }
    }
  }

  // check the same EC result
  if (sameECFlag == 2)
  {
    stringstream ss;

    for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
    {
      const FramePosition &fp = it.Position();
      if (vfatBad[fp])
        continue;
      ss << "\n\tID = " << hex << it.Data()->getChipID() << ", EC = " << dec << it.Data()->getEC();
    }

    if (verbosityTest)
      WARN("TestECProgress::run") << "Working VFATs do not have the same EC in the end of test:" << ss.rdbuf() << c_endl;
    else
      WARN("TestECProgress::run") << "Working VFATs do not have the same EC in the end of test!" << c_endl;
  }

  // move results to vector and print
  workingVFATs.clear();
  stringstream ss;

  for (VFATFrameCollection::Iterator it(event->frames); !it.IsEnd(); it.Next())
  {
    const FramePosition &fp = it.Position();
    if (vfatBad[fp])
      continue;
    Entry e;
    e.ID = it.Data()->getChipID();
    e.index = fp;
    ss << "(0x" << hex << e.ID << "," << e.index << ") ";
    workingVFATs.push_back(e);
  }

  if (verbosityTest > 1)
    INFO("TestECProgress::run") << "Working VFATs are the following. (ID 12bit, DAQ channel)" << std::endl << ss.rdbuf() << c_endl;
  if (verbosityTest == 1)
      INFO("TestECProgress::run") << workingVFATs.size() << " VFATs found working." << c_endl;

  delete event;
  return 0;
}
