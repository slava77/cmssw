/****************************************************************************
* This module is directly copied from 
* RPRecoDataFormats/RPTrackCandidateCollection.h and expanded to hold 
* multiple track candidates.
*   
* Original Authors:
*   Hubert Niewiadomski (Hubert.Niewiadomski@cern.ch)
* Secondary Authors:
*   Zhang Zhengkui (zhang.zhengkui.fin@gmail.com)
*
****************************************************************************/


#ifndef RecoTotemRP_RPRecoDataFormats_RPMulTrackCandidateCollection_h
#define RecoTotemRP_RPRecoDataFormats_RPMulTrackCandidateCollection_h
 
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "DataFormats/TotemRPDataTypes/interface/RPTypes.h"

#include <vector>
#include <map>

 
class RPMulTrackCandidateCollection : public std::map<RPId, std::vector<RPTrackCandidate> >
{
};

class RPMulTrackCandidateSetsCollection : public std::map<RPId, std::vector<std::vector<RPTrackCandidate> > >
{
};

#endif
