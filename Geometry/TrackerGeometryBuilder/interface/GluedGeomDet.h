#ifndef Geometry_TrackerGeometryBuilder_GluedGeomDet_H
#define Geometry_TrackerGeometryBuilder_GluedGeomDet_H

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "DataFormats/DetId/interface/DetId.h"

class GluedGeomDet final : public GeomDet{
public:

  GluedGeomDet( BoundPlane* sp, const GeomDetUnit* monoDet,  const GeomDetUnit* stereoDet);
  
  virtual ~GluedGeomDet();

  virtual std::vector<const GeomDet*> components() const;

  // Which subdetector
  virtual SubDetector subDetector() const {return theMonoDet->subDetector();}

  const GeomDetUnit* monoDet() const { return theMonoDet;}
  const GeomDetUnit* stereoDet() const { return theStereoDet;}

private:
  const GeomDetUnit* theMonoDet;
  const GeomDetUnit* theStereoDet;  
};

#endif
