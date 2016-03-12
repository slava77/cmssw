/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/


#ifndef _Totem_TestECProgress_h_
#define _Totem_TestECProgress_h_

#include <vector>

#include "TotemRawDataLibrary/DataFormats/interface/FramePosition.h"

namespace Totem {

class DataFile;

/**
 * Tests the progress of EC values.
 *
 * Reads a few events from the beginning of a data file and seletcs VFATs with correct EC progress.
**/

class TestECProgress
{
  public:
    /// auxiliary structure to keep pairs (VFAT ID, VFATFrame position)
    struct Entry
    {
      unsigned short ID;
      FramePosition index;
    };

    /**
     * Takes the TotemSlinkFile and reads numEvChck events from the beginning of the file. Returns array
     * workingVFATs with VFATs with correct EC progress. If printResults is true, it prints results on 
     * screen. Works in two regimes
     * \li strictBeginning = true: Designed for scan runs. All EC values in the first event are required 
     * to be 0. EC = 99 must be followed by EC = 0.
     * \li strictBeginning = false: Designed for data-taking runs. EC values in the beggining are unconstrained.
     * EC = 255 must be followed by EC = 0. 
     *
     * After numEvChck events are checked, one more test is performed. It checks whether all VFATs selected 
     * as working have the same EC value. If not a warning is launched.
     **/ 
    static unsigned short run(std::vector<Entry> &workingVFATs, DataFile *, unsigned short numEvChck = 10,
    bool strictBeginning = true, unsigned int verbosity = 0);
};

} // namespace

#endif
