/** Implementation of the GEM Geometry Builder from DDD
 *
 *  \author Port of: MuDDDGEMBuilder (ORCA)
 *  \author M. Maggi - INFN Bari
 */
#include "Geometry/GEMGeometryBuilder/src/GEMGeometryBuilderFromDDD.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"

#include <DetectorDescription/Core/interface/DDFilter.h>
#include <DetectorDescription/Core/interface/DDFilteredView.h>
#include <DetectorDescription/Core/interface/DDSolid.h>

#include "Geometry/MuonNumbering/interface/MuonDDDNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/GEMNumberingScheme.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include "DataFormats/GeometryVector/interface/Basic3DVector.h"

#include "CLHEP/Units/GlobalSystemOfUnits.h"

#include <iostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

GEMGeometryBuilderFromDDD::GEMGeometryBuilderFromDDD(bool comp11) : theComp11Flag(comp11)
{ }

GEMGeometryBuilderFromDDD::~GEMGeometryBuilderFromDDD() 
{ }

GEMGeometry* GEMGeometryBuilderFromDDD::build(const DDCompactView* cview, const MuonDDDConstants& muonConstants)
{
  std::string attribute = "ReadOutName"; // could come from .orcarc
  std::string value     = "MuonGEMHits";    // could come from .orcarc
  DDValue val(attribute, value, 0.0);

  // Asking only for the MuonGEM's
  DDSpecificsFilter filter;
  filter.setCriteria(val, // name & value of a variable 
		     DDSpecificsFilter::matches,
		     DDSpecificsFilter::AND, 
		     true, // compare strings otherwise doubles
		     true // use merged-specifics or simple-specifics
		     );
  DDFilteredView fview(*cview);
  fview.addFilter(filter);

  return this->buildGeometry(fview, muonConstants);
}

GEMGeometry* GEMGeometryBuilderFromDDD::buildGeometry(DDFilteredView& fview, const MuonDDDConstants& muonConstants)
{
  LogDebug("GEMGeometryBuilderFromDDD") <<"Building the geometry service";
  GEMGeometry* geometry = new GEMGeometry();

  LogDebug("GEMGeometryBuilderFromDDD") << "About to run through the GEM structure\n" 
					<<" First logical part "
					<<fview.logicalPart().name().name();
  bool doSubDets = fview.firstChild();
  LogDebug("GEMGeometryBuilderFromDDD") << "doSubDets = " << doSubDets;

  LogDebug("GEMGeometryBuilderFromDDD") <<"start the loop"; 
  int nChambers(0);
  while (doSubDets)
  {
    // Get the Base Muon Number
    MuonDDDNumbering mdddnum(muonConstants);
    LogDebug("GEMGeometryBuilderFromDDD") <<"Getting the Muon base Number";
    MuonBaseNumber mbn = mdddnum.geoHistoryToBaseNumber(fview.geoHistory());

    LogDebug("GEMGeometryBuilderFromDDD") <<"Start the GEM Numbering Schema";
    GEMNumberingScheme gemnum(muonConstants);

    GEMDetId rollDetId(gemnum.baseNumberToUnitNumber(mbn));
    LogDebug("GEMGeometryBuilderFromDDD") << "GEM eta partition rawId: " << rollDetId.rawId() << ", detId: " << rollDetId;
    if (rollDetId.roll()==1) ++nChambers;

    DDValue numbOfStrips("nStrips");
    DDValue numbOfPads("nPads");

    std::vector<const DDsvalues_type* > specs(fview.specifics());
    std::vector<const DDsvalues_type* >::iterator is = specs.begin();
    double nStrips = 0., nPads = 0.;
    for (;is != specs.end(); is++)
    {
      if (DDfetch( *is, numbOfStrips)) nStrips = numbOfStrips.doubles()[0];
      if (DDfetch( *is, numbOfPads))   nPads = numbOfPads.doubles()[0];
    }
    LogDebug("GEMGeometryBuilderFromDDD") 
      << ((nStrips == 0. ) ? ("No nStrips found!!") : ("Number of strips: " + boost::lexical_cast<std::string>(nStrips))); 
    LogDebug("GEMGeometryBuilderFromDDD") 
      << ((nPads == 0. ) ? ("No nPads found!!") : ("Number of pads: " + boost::lexical_cast<std::string>(nPads)));

    std::vector<double> dpar=fview.logicalPart().solid().parameters();
    std::string name = fview.logicalPart().name().name();
    DDTranslation tran = fview.translation();
   //removed .Inverse after comparing to DT...
    DDRotationMatrix rota = fview.rotation();//.Inverse();
    Surface::PositionType pos(tran.x()/cm, tran.y()/cm, tran.z()/cm);
    // CLHEP way
    // Surface::RotationType rot(rota.xx(),rota.xy(),rota.xz(),
    //           	      rota.yx(),rota.yy(),rota.yz(),
    // 			      rota.zx(),rota.zy(),rota.zz());

    //ROOT::Math way
    DD3Vector x, y, z;
    rota.GetComponents(x,y,z);
    // doesn't this just re-inverse???
    Surface::RotationType rot(float(x.X()), float(x.Y()), float(x.Z()),
			      float(y.X()), float(y.Y()), float(y.Z()),
			      float(z.X()), float(z.Y()), float(z.Z())); 
    
    float be = dpar[4]/cm; // half bottom edge
    float te = dpar[8]/cm; // half top edge
    float ap = dpar[0]/cm; // half apothem
    float ti = 0.4/cm;     // half thickness

    //  TrapezoidalPlaneBounds* 
    Bounds* bounds = new TrapezoidalPlaneBounds(be, te, ap, ti);

    std::vector<float> pars;
    pars.push_back(be); 
    pars.push_back(te); 
    pars.push_back(ap); 
    pars.push_back(nStrips);
    pars.push_back(nPads);

    LogDebug("GEMGeometryBuilderFromDDD") 
      << "GEM " << name << " par " << be << " " << te << " " << ap << " " << dpar[0];
    
    GEMEtaPartitionSpecs* e_p_specs = new GEMEtaPartitionSpecs(GeomDetEnumerators::GEM, name, pars);

      //Change of axes for the forward
    Basic3DVector<float> newX(1.,0.,0.);
    Basic3DVector<float> newY(0.,0.,1.);
    //      if (tran.z() > 0. )
    newY *= -1;
    Basic3DVector<float> newZ(0.,1.,0.);
    rot.rotateAxes (newX, newY, newZ);
    
    BoundPlane* bp = new BoundPlane(pos, rot, bounds);
    ReferenceCountingPointer<BoundPlane> surf(bp);
    GEMEtaPartition* gep = new GEMEtaPartition(rollDetId, surf, e_p_specs);

    // Add the eta partition to the geometry
    geometry->add(gep);
    // go to next layer
    doSubDets = fview.nextSibling(); 
  }
  
  auto& partitions(geometry->etaPartitions());
  int nEtaPartitions((int)partitions.size()/nChambers);

  std::vector<GEMDetId> vDetId;
  vDetId.clear();
  for (unsigned i=1; i<=partitions.size(); ++i){
    GEMDetId detId(partitions.at(i-1)->id());
    vDetId.push_back(detId);
    if (i%nEtaPartitions==0){
      GEMDetId fId(vDetId.front());
      GEMDetId chamberId(fId.region(),fId.ring(),fId.station(),fId.layer(),fId.chamber(),0);

      // compute the overall boundplane using the first eta partition
      const GEMEtaPartition* p(geometry->etaPartition(fId));
      const BoundPlane& bps = p->surface();
      BoundPlane* bp = const_cast<BoundPlane*>(&bps);
      ReferenceCountingPointer<BoundPlane> surf(bp);
    
      GEMChamber* ch = new GEMChamber(chamberId, surf); 
      LogDebug("GEMGeometryBuilderFromDDD") << "Creating chamber " << chamberId << " with " << vDetId.size() << " eta partitions";

      for(auto id : vDetId){
	LogDebug("GEMGeometryBuilderFromDDD") << "Adding eta partition " << id << " to GEM chamber";
	ch->add(const_cast<GEMEtaPartition*>(geometry->etaPartition(id)));
      }

      LogDebug("GEMGeometryBuilderFromDDD") << "Adding the chamber to the geometry";
      geometry->add(ch);
      vDetId.clear();
    }
  }
  
  auto& chambers(geometry->chambers());
  for (unsigned i=1; i<=chambers.size(); ++i){
    GEMDetId detIdL1(chambers.at(i-1)->id());
    if (detIdL1.layer()==2) continue;
    LogDebug("GEMGeometryBuilderFromDDD") << "First chamber for super chamber" << detIdL1;
    GEMDetId detIdL2(detIdL1.region(),detIdL1.ring(),detIdL1.station(),2,detIdL1.chamber(),0);
    LogDebug("GEMGeometryBuilderFromDDD") << "Second chamber for super chamber" << detIdL2;
    GEMDetId schId(detIdL1.region(),detIdL1.ring(),detIdL1.station(),0,detIdL1.chamber(),0);
    LogDebug("GEMGeometryBuilderFromDDD") << "Proposed detId for the super chamber" << schId;
    
    // Bound plane for a super chambers is the bound plane for the first chamber
    auto chL1(geometry->chamber(detIdL1));
    const BoundPlane& bps = chL1->specificSurface();
    BoundPlane* bp = const_cast<BoundPlane*>(&bps);
    ReferenceCountingPointer<BoundPlane> surf(bp);
    
    GEMSuperChamber* sch = new GEMSuperChamber(schId,surf); 
    sch->add(const_cast<GEMChamber*>(geometry->chamber(detIdL1)));
    sch->add(const_cast<GEMChamber*>(geometry->chamber(detIdL2)));
    geometry->add(sch);
  }
  
  return geometry;
}
