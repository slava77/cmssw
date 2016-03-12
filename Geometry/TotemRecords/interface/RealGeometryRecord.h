/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*	Jan Kaspar (jan.kaspar@gmail.com) 
*
****************************************************************************/

#ifndef RECORDS_REALGEOMETRYRECORD_H
#define RECORDS_REALGEOMETRYRECORD_H

#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "Geometry/TotemRecords/interface/MeasuredGeometryRecord.h"

#include "boost/mpl/vector.hpp"

#include "TotemAlignment/RPRecords/interface/RPRealAlignmentRecord.h"

/**
 * \ingroup TotemRPGeometry
 * \brief Event setup record containing the real (actual) geometry information.
 **/
class RealGeometryRecord : public edm::eventsetup::DependentRecordImplementation
						   <RealGeometryRecord, boost::mpl::vector<MeasuredGeometryRecord, RPRealAlignmentRecord /*, ... */> >
{
};

#endif

