/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*
****************************************************************************/

#ifndef RecoTotemRP_RPRecoDataFormats_RPRecognizedPatterns_h
#define RecoTotemRP_RPRecoDataFormats_RPRecognizedPatterns_h

#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"

#include <vector>

/**
 *\brief A result of pattern recognition.
 **/
class RPRecognizedPatterns
{
 public:

  /**
   *\brief A recognized linear pattern.
   * The intercept b is taken at the middle of a RP:
   *     (geometry->GetRPDevice(RPId)->translation().z())
   * The global coordinate system is used (wrt. the beam). This is the same convention
   * as for the 1-RP track fits.
   *
   * These modules yet need adapting to this new scheme:
   *      TotemDQM/Modules/plugins/TotemDQMModuleRP.cc
   **/
  struct Line {
    typedef std::vector<RPRecoHit> HitCollection;

    double a;                       ///< slope in rad
    double b;                       ///< intercept in mm
    double w;                       ///< weight
    HitCollection hits;             ///< corresponding hits

    Line(double _a = 0., double _b = 0., double _w = 1.) : a(_a), b(_b), w(_w) {}
  };

  /// identifies the algorithm that produced this record
  enum SourceType { sParallel, sNonParallel } source;

  std::vector<Line> uLines, vLines; ///< the recognized lines

  void Clear()
    { uLines.clear(); vLines.clear(); }
};


#endif

