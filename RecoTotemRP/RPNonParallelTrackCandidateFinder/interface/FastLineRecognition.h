/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Jan Kašpar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#ifndef _FastLineRecognition_h_
#define _FastLineRecognition_h_

#include "Geometry/TotemRPGeometryBuilder/interface/TotemRPGeometry.h"
#include "DataFormats/TotemRPDataTypes/interface/RPRecoHit.h"

//#define TUNE 1

#ifdef TUNE
  #include "TH1D.h"
#endif


/**
 * \brief Class performing optimized hough transform to recognize lines.
**/

class FastLineRecognition
{
  protected:
    /// the uncertainty of 1-hit cluster, in mm
    static const double sigma0;

    /// "typical" z
    double z0;

    /// cluster half widths in a and b
    double chw_a, chw_b;

    /// weight threshold for accepting pattern candidates (clusters)
    double threshold;

    /// pointer to the geometry
    const TotemRPGeometry* geometry;

    struct GeomData
    {
      double z;   ///< z position of a sensor (wrt. IP)
      double s;   ///< sensor's centre projected to its read-out direction
    };

    /// map: raw detector id --> GeomData
    std::map<unsigned int, GeomData> geometryMap;

    /// expects raw detector id
    GeomData GetGeomData(unsigned int id);

    struct Point
    {
      const RPRecoHit* hit; ///< pointer to original reco hit
      double h;             ///< hit position in global coordinate system
      double z;             ///< z position with respect to z0
      double w;             ///< weight
      bool usable;          ///< whether the point can still be used
      Point(const RPRecoHit* _hit=NULL, double _h=0., double _z=0., double _w=0.) :
        hit(_hit), h(_h), z(_z), w(_w), usable(true) {}
    };
    
    /// cluster of intersection points
    struct Cluster
    {
      double Saw, Sbw, Sw, S1;
      double weight;
      double min_a, max_a, min_b, max_b;

      std::vector<const Point *> contents;
      
      Cluster() : Saw(0.), Sbw(0.), Sw(0.), S1(0.), weight(0.)
#ifdef TUNE
                  , min_a(1E100), max_a(-1E100),
                  min_b(1E100), max_b(-1E100)
#endif
                      {}
      
      void Add(const Point *p1, const Point *p2, double a, double b, double w);

      bool operator<(const Cluster &c) const
      {
        return (this->Sw > c.Sw) ? true : false;
      }
    };

#ifdef TUNE
    double cas_max, cbs_max;
    TH1D *cas, *cbs;
#endif

    /// gets the most significant pattern in the (remaining) points
    /// returns true when a pattern was found
    bool GetOneLine(const std::vector<Point> &points, double threshold, Cluster &result);

  public:
    FastLineRecognition(double cw_a = 0., double cw_b = 0.);

    ~FastLineRecognition();

    void ResetGeometry(const TotemRPGeometry *_g)
    {
      geometry = _g;
      geometryMap.clear();
    }

    void GetLines(const std::vector<const RPRecoHit *> &input, double _z0, double threshold,
      std::vector<RPRecognizedPatterns::Line> &lines);
};

#endif

