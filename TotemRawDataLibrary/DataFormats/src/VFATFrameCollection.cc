/****************************************************************************
*
* This is a part of the TOTEM testbeam/monitoring software.
* This is a part of the TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*    
****************************************************************************/


#include "TotemRawDataLibrary/DataFormats/interface/VFATFrameCollection.h"

namespace Totem {
    
const VFATFrame* VFATFrameCollection::GetFrameByIndexID(FramePosition index, unsigned int ID)
{
  const VFATFrame* returnframe = GetFrameByIndex(index);
  if (returnframe == NULL)
    return NULL;
  return (returnframe->getChipID() == (ID & 0xFFF)) ? returnframe : NULL;
}

} // namespace

