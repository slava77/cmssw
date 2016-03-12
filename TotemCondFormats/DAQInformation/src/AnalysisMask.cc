/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wróbel (wroblisko@gmail.com)
*
****************************************************************************/

#include "FWCore/Utilities/interface/typelookup.h"

#include "TotemCondFormats/DAQInformation/interface/AnalysisMask.h"


void AnalysisMask::Insert(const TotemSymbID &sid, const VFATAnalysisMask &vam)
{
  // TODO
  analysisMask[sid] = vam;
}

TYPELOOKUP_DATA_REG(AnalysisMask);

