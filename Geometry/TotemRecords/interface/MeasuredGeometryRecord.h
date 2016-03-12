/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*	Dominik Mierzejewski (dmierzej@cern.ch)
*
****************************************************************************/

#ifndef RECORDS_MEASUREDGEOMETRYRECORD_H
#define RECORDS_MEASUREDGEOMETRYRECORD_H

#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"

#include "boost/mpl/vector.hpp"

#include "TotemAlignment/RPRecords/interface/RPMeasuredAlignmentRecord.h"

/**
 * \ingroup TotemRPGeometry
 * \brief Event setup record containing the Measured (measured) geometry information.
 **/
class MeasuredGeometryRecord : public edm::eventsetup::DependentRecordImplementation
						   <MeasuredGeometryRecord, boost::mpl::vector<IdealGeometryRecord, RPMeasuredAlignmentRecord /*, ... */> >
{
};

#endif

