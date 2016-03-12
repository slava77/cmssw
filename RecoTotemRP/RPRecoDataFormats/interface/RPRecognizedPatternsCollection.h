/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPRecognizedPatternsCollection_h
#define RecoTotemRP_RPRecoDataFormats_RPRecognizedPatternsCollection_h

#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPRecognizedPatterns.h"

#include <map>

/**
 *\brief A collection (per RP) of pattern-recognition results.
 **/
class RPRecognizedPatternsCollection : public std::map<RPId, RPRecognizedPatterns>
{
};


#endif

