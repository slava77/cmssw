#include "SimpleNavigationSchoolNoFWD.h"

#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"

 
#include "NavigationSchoolFactory.h"
#include "TrackingTools/DetLayers/interface/NavigationSchool.h"
DEFINE_EDM_PLUGIN(NavigationSchoolFactory, SimpleNavigationSchoolNoFWD, "SimpleNavigationSchoolNoFWD");

