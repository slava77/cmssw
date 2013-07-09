/** Derived from DTGeometryAnalyzer by Nicola Amapane
 *
 *  \author M. Maggi - INFN Bari
 */

#include <memory>
#include <fstream>
#include <FWCore/Framework/interface/Frameworkfwd.h>

#include <FWCore/Framework/interface/EDAnalyzer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/EventSetup.h>
#include <FWCore/Framework/interface/ESHandle.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
#include <Geometry/Records/interface/MuonGeometryRecord.h>
#include <Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h>

#include "DataFormats/Math/interface/deltaPhi.h"

#include <string>
#include <cmath>
#include <vector>
#include <iomanip>
#include <set>

class GEMGeometryAnalyzer : public edm::EDAnalyzer {

public: 
  GEMGeometryAnalyzer( const edm::ParameterSet& pset);

  ~GEMGeometryAnalyzer();

  virtual void analyze( const edm::Event&, const edm::EventSetup& );
  
  const std::string& myName() { return myName_;}
  
private: 

  const int dashedLineWidth_;
  const std::string dashedLine_;
  const std::string myName_;
  std::ofstream ofos;
};

GEMGeometryAnalyzer::GEMGeometryAnalyzer( const edm::ParameterSet& /*iConfig*/ )
  : dashedLineWidth_(104), dashedLine_( std::string(dashedLineWidth_, '-') ), 
    myName_( "GEMGeometryAnalyzer" ) 
{ 
  ofos.open("MytestOutput.out"); 
  std::cout <<"======================== Opening output file"<< std::endl;
}


GEMGeometryAnalyzer::~GEMGeometryAnalyzer() 
{
  ofos.close();
  std::cout <<"======================== Closing output file"<< std::endl;
}

void
GEMGeometryAnalyzer::analyze( const edm::Event& /*iEvent*/, const edm::EventSetup& iSetup )
{
  edm::ESHandle<GEMGeometry> pDD;
  iSetup.get<MuonGeometryRecord>().get(pDD);     
  
  std::cout << myName() << ": Analyzer..." << std::endl;
  std::cout << "start " << dashedLine_ << std::endl;

  std::cout << " Geometry node for GEMGeom is  " << &(*pDD) << std::endl;   
  std::cout << " I have "<<pDD->detTypes().size()      << " detTypes" << std::endl;
  std::cout << " I have "<<pDD->detUnits().size()      << " detUnits" << std::endl;
  std::cout << " I have "<<pDD->dets().size()          << " dets" << std::endl;
  std::cout << " I have "<<pDD->etaPartitions().size() << " eta partitions" << std::endl;
  std::cout << " I have "<<pDD->chambers().size()      << " chambers" << std::endl;
  std::cout << " I have "<<pDD->superChambers().size() << " super chambers" << std::endl;

  std::cout << myName() << ": Begin iteration over geometry..." << std::endl;
  std::cout << "iter " << dashedLine_ << std::endl;

  
  //----------------------- Global GEMGeometry TEST -------------------------------------------------------
  std::cout << myName() << "Begin GEMGeometry structure TEST" << std::endl;
  
  auto& superChambers(pDD->superChambers());
  int i=1;
  for (auto sch : superChambers){
    GEMDetId schId(sch->id());
    auto& chambers(sch->chambers());
    int nChambers(sch->nChambers());
    std::cout << "GEMSuperChamber " << i << ", GEMDetId = " << schId.rawId() << ", " << schId << " has " << nChambers << " chambers." << std::endl;
    int j = 1;
    for (auto ch : chambers){
      GEMDetId chId(ch->id());
      auto& rolls(ch->etaPartitions());
      int nRolls(ch->nEtaPartitions());
      std::cout << "  GEMChamber " << j << ", GEMDetId = " << chId.rawId() << ", " << chId << " has " << nRolls << " eta partitions." << std::endl;
      int k = 1;

      /*
       * Checklist for an eta partition:
       *   base_bottom, base_top, height, strips, pads
       *   cx, cy, cz, ceta, cphi
       *   tx, ty, tz, teta, tphi
       *   bx, by, bz, beta, bphi
       *   pitch center, pitch bottom, pitch top
       *   deta, dphi
       *   gap thickness
       *   sum of all dx + gap = chamber height
       */      
      for (auto roll : rolls){
	GEMDetId rId(roll->id());
	std::cout<<"    GEMEtaPartition " << k << ", GEMDetId = " << rId.rawId() << ", " << rId << std::endl;

	const BoundPlane& bSurface(roll->surface());
 	const StripTopology* topology(dynamic_cast<const StripTopology*>(&(roll->topology())));

	// base_bottom, base_top, height, strips, pads (all half length)
	auto& parameters(roll->specs()->parameters());
 	float bottomEdge(parameters[0]);
	float topEdge(parameters[1]);
	float height(parameters[2]);
	float nStrips(parameters[3]);
	float nPads(parameters[4]);

 	LocalPoint  lCentre( 0., 0., 0. );
 	GlobalPoint gCentre(bSurface.toGlobal(lCentre));

 	LocalPoint  lTop( 0., height, 0.);
 	GlobalPoint gTop(bSurface.toGlobal(lTop));

 	LocalPoint  lBottom( 0., -height, 0.);
 	GlobalPoint gBottom(bSurface.toGlobal(lBottom));

	//   gx, gy, gz, geta, gphi (center)
	double cx(gCentre.x());
	double cy(gCentre.y());
	double cz(gCentre.z());
	double ceta(gCentre.eta());
	int cphi(static_cast<int>(gCentre.phi().degrees()));
	if (cphi < 0) cphi += 360;

	double tx(gTop.x());
	double ty(gTop.y());
	double tz(gTop.z());
	double teta(gTop.eta());
	int tphi(static_cast<int>(gTop.phi().degrees()));
	if (tphi < 0) tphi += 360;

	double bx(gBottom.x());
	double by(gBottom.y());
	double bz(gBottom.z());
	double beta(gBottom.eta());
	int bphi(static_cast<int>(gBottom.phi().degrees()));
	if (bphi < 0) bphi += 360;

	// pitch bottom, pitch top, pitch centre
	float pitch(roll->pitch());
	float topPitch(roll->localPitch(lTop));
	float bottomPitch(roll->localPitch(lBottom));

	// Type - should be GHA[1-nRolls]
	std::string type(roll->type().name());

	// print info about edges
	LocalPoint lEdge1(topology->localPosition(0.));
	LocalPoint lEdgeN(topology->localPosition((float)nStrips));
	
	double cstrip1(roll->toGlobal(lEdge1).phi().degrees());
	double cstripN(roll->toGlobal(lEdgeN).phi().degrees());
	double dphi(cstripN - cstrip1);
	if (dphi < 0.) dphi += 360.;
	double deta(TMath::Abs(beta - teta));

	std::cout << "    \tType: " << type << std::endl
		  << "    \tDimensions[cm]: b = " << bottomEdge << ", B = " << topEdge << ", h  = " << height << std::endl
		  << "    \tnStrips = " << nStrips << ", nPads =  " << nPads << std::endl
		  << "    \tcenter(x,y,z) = " << cx << " " << cy << " " << cz << ", center(eta,phi) = " << ceta << " " << cphi << std::endl
		  << "    \ttop(x,y,z) = " << tx << " " << ty << " " << tz << ", top(eta,phi) = " << teta << " " << tphi << std::endl
		  << "    \tbottom(x,y,z) = " << bx << " " << by << " " << bz << ", bottom(eta,phi) = " << beta << " " << bphi << std::endl
		  << "    \tpith (top,center,bottom) = " << topPitch << " " << pitch << " " << bottomPitch << ", dEta = " << deta << ", dPhi = " << dphi << std::endl;
	++k;
      }
      ++j;
    }
    ++i;
  }
   std::cout << dashedLine_ << " end" << std::endl;
}

//define this as a plug-in
#include <FWCore/Framework/interface/MakerMacros.h>
DEFINE_FWK_MODULE(GEMGeometryAnalyzer);
