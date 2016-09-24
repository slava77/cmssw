#ifndef TkNavigation_SimpleNavigationSchoolNoFWD_H
#define TkNavigation_SimpleNavigationSchoolNoFWD_H

#include "SimpleNavigationSchool.h"

#include <vector>

/** Concrete navigation school for the Tracker
 */

class dso_hidden SimpleNavigationSchoolNoFWD : public SimpleNavigationSchool {
public:
  
  SimpleNavigationSchoolNoFWD(const GeometricSearchTracker* theTracker,
			      const MagneticField* field) : SimpleNavigationSchool(theTracker, field, false) {}
};

#endif // SimpleNavigationSchoolNoFWD_H
