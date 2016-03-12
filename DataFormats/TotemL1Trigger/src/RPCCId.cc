/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *	Leszek Grzanka (braciszek@gmail.com)
 *
 ****************************************************************************/

#include "DataFormats/TotemL1Trigger/interface/RPCCId.h"
#include "FWCore/Utilities/interface/Exception.h"

RPCCId::RPCCId():DetId(DetId::Totem,totem_rp_subdet_id)
{}


RPCCId::RPCCId(RPCCIdRaw id):DetId(id)
{
  if (det()!=DetId::Totem || subdetId()!=totem_rp_subdet_id)
    {
      throw cms::Exception("InvalidDetId") << "RPCCId:"
					   << " det: " << det()
					   << " subdet: " << subdetId()
					   << " is not a valid Totem RP id";
    }
}


void RPCCId::init(unsigned int Arm, unsigned int Station,
		  unsigned int RomanPot, unsigned int Direction)
{
  if( Arm>=2 || Station>=3 || RomanPot>=6 || Direction>=2)
    {
      throw cms::Exception("InvalidDetId") << "RPCCId ctor:"
					   << " Invalid parameters: "
					   << " Arm "<<Arm
					   << " Station "<<Station
					   << " RomanPot "<<RomanPot
					   << " Direction "<<Direction
					   << std::endl;
    }

  uint32_t ok=0xfe000000;
  id_ &= ok;

  id_ |= ((Arm&0x1) << startArmBit);
  id_ |= ((Station&0x3) << startStationBit);
  id_ |= ((RomanPot&0x7) << startRPBit);
  id_ |= ((Direction&0xf) << startDirBit);
}


RPCCId::RPCCId(unsigned int Arm, unsigned int Station,
	       unsigned int RomanPot, unsigned int Direction):
  DetId(DetId::Totem,totem_rp_subdet_id)
{
  this->init(Arm,Station,RomanPot,Direction);
}


std::ostream& operator<<( std::ostream& os, const RPCCId& id )
{
  os << " Arm "<<id.Arm()
     << " Station "<<id.Station()
     << " RomanPot "<<id.RomanPot()
     << " Direction "<<id.Direction();

  return os;
}
