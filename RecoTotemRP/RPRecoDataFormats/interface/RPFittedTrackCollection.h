#ifndef RecoTotemRP_RPRecoDataFormats_RPFittedTrackCollection_h
#define RecoTotemRP_RPRecoDataFormats_RPFittedTrackCollection_h


#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPFittedTrack.h"

#include <map>


class RPFittedTrackCollection : public std::map<RPId, RPFittedTrack>
{
};


#endif
