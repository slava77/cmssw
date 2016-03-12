/****************************************************************************
 *
 * This is a part of TOTEM offline software.
 * Authors:
 *  Jan Kašpar (jan.kaspar@gmail.com)
 *
 ****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPStationTrackFitCollection_h
#define RecoTotemRP_RPRecoDataFormats_RPStationTrackFitCollection_h


#include "RecoTotemRP/RPRecoDataFormats/interface/RPStationTrackFit.h"

#include <map>
#include <vector>

/**
 *\brief A collection of track fits (through multiple RPs = station)
 * map: arm id --> list of track fits
 **/
class RPStationTrackFitCollection : public std::map<unsigned int, std::vector<RPStationTrackFit> >
{
};

#endif

