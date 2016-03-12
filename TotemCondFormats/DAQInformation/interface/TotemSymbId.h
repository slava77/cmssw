/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*	Tuomas Tajakka
*   Jan Kašpar (jan.kaspar@cern.ch)
*
****************************************************************************/

#ifndef _TotemSymbId_
#define _TotemSymbId_

#include <iostream>

/**
 *\brief Symbolic ID describing an entity of a TOTEM subdetector.
 **/
class TotemSymbID
{
  public:
    /// identifies the TOTEM subsystem
    enum {RP, T1, T2} subSystem;

    /// integer-encoded symbolic ID
    unsigned int symbolicID;

    bool operator < (const TotemSymbID &sid) const
    {
      if (subSystem == sid.subSystem)
		  return (symbolicID < sid.symbolicID);
      return (subSystem < sid.subSystem);
    }

    bool operator == (const TotemSymbID &sid) const
      { return ((subSystem==sid.subSystem)&&(symbolicID==sid.symbolicID)); }
    
    friend std::ostream& operator << (std::ostream& s, const TotemSymbID &sid);
};

#endif

