/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#include "FWCore/Utilities/interface/typelookup.h"

#include "TotemCondFormats/DAQInformation/interface/DAQInformationRP.h"

void DAQInformationRP::Reset()
{
  readoutPositionToId.clear();
  readoutIdToRegisters.clear();
  coincidencePositionToId.clear();
  coincidenceIdToRegisters.clear();
}


TYPELOOKUP_DATA_REG(DAQInformationRP);

