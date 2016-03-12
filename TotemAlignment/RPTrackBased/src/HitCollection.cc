/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*  Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"
#include "DataFormats/TotemRPDetId/interface/TotRPDetId.h"
#include "TotemAlignment/RPTrackBased/interface/HitCollection.h"

#include <cmath>

Hit::Hit(const RPRecoHit &rh) : 
  id(TotRPDetId::RawToDecId(rh.DetId())),
  position(rh.Position()),
  sigma(rh.Sigma())
{
  // prevents null uncertainty, this would make certain algorithms crash
  if (sigma < 1E-6)
    sigma = 66E-3/sqrt(12.);
}

