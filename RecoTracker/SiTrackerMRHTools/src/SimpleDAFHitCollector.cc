#include "RecoTracker/SiTrackerMRHTools/interface/SimpleDAFHitCollector.h"
#include "RecoTracker/SiTrackerMRHTools/interface/SiTrackerMultiRecHitUpdator.h"
#include "TrackingTools/DetLayers/interface/MeasurementEstimator.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "DataFormats/TrackingRecHit/interface/InvalidTrackingRecHit.h"
#include "TrackingTools/MeasurementDet/interface/MeasurementDet.h"
#include "RecoTracker/MeasurementDet/interface/MeasurementTrackerEvent.h"

#include <vector>
#include <map>

#define _debug_SimpleDAFHitCollector_ 

using namespace std;

vector<TrajectoryMeasurement> SimpleDAFHitCollector::recHits(const Trajectory& traj, const MeasurementTrackerEvent *theMTE) const{

  LogTrace("MultiRecHitCollector") << " Calling SimpleDAFHitCollector::recHits" << std::endl;

  //WARNING: At the moment the trajectories has the measurements 
  //with reversed sorting after the track smoothing
  const vector<TrajectoryMeasurement>& meas = traj.measurements();

  if (meas.empty()) return vector<TrajectoryMeasurement>();

  //debug
  LogTrace("MultiRecHitCollector") << "Original measurements are:";
  for(vector<TrajectoryMeasurement>::const_iterator itrajmeas = meas.begin(); itrajmeas < meas.end();
      itrajmeas++) {
    if (itrajmeas->recHit()->isValid()){
      LogTrace("MultiRecHitCollector") << "  Valid Hit with DetId " << itrajmeas->recHit()->geographicalId().rawId()
                	<< " local position " << itrajmeas->recHit()->hit()->localPosition()
                	<< " global position " << itrajmeas->recHit()->hit()->globalPosition() ;
            } else {
              LogTrace("MultiRecHitCollector") << "   Invalid Hit with DetId " << itrajmeas->recHit()->geographicalId().rawId();
            }
  }

  //groups the TrajectoryMeasurements on a sensor by sensor
  //we have to sort the TrajectoryMeasurements in the opposite way in the fitting direction
  vector<TrajectoryMeasurement> result;
  for(vector<TrajectoryMeasurement>::const_reverse_iterator itrajmeas = meas.rbegin(); itrajmeas < meas.rend();
      itrajmeas++) {

      DetId id = itrajmeas->recHit()->geographicalId();
      MeasurementDetWithData measDet = theMTE->idToDet(id);
      tracking::TempMeasurements tmps;
      std::vector<const TrackingRecHit*> hits;

      TrajectoryStateOnSurface current = itrajmeas->updatedState();
      //the error is scaled in order to take more "compatible" hits
      if (current.isValid()) current.rescaleError(10);

      TrajectoryStateOnSurface state = itrajmeas->predictedState();
      if (!state.isValid()){
        LogTrace("MultiRecHitCollector") << "Something wrong! no valid TSOS found in current group ";
        continue;
      }
      //collected hits compatible with the itrajmeas
      if( measDet.measurements(current, *(getEstimator()), tmps)){
        LogTrace("MultiRecHitCollector") << "  Found " << tmps.size() << " compatible measurements";
        for (std::size_t i=0; i!=tmps.size(); ++i){
          DetId idtemps = tmps.hits[i]->geographicalId();

          if( idtemps == id && tmps.hits[i]->hit()->isValid() ) {
            hits.push_back(tmps.hits[i]->hit());
          }
        }

        //I will keep the Invalid hit, IF this is not the first one       
        if (hits.empty()){
          LogTrace("MultiRecHitCollector") << " -> but no valid hits found in current group.";

          if( result.empty() ) continue;

          result.push_back(TrajectoryMeasurement(state,
                                        std::make_shared<InvalidTrackingRecHit>(measDet.mdet().geomDet(), TrackingRecHit::missing)));
        } else {
          //measurements in groups are sorted with increating chi2
          //sort( *hits.begin(), *hits.end(), TrajMeasLessEstim());

          LogTrace("MultiRecHitCollector") << " -> " << hits.size() << " valid hits for this sensor.";

          //building a MultiRecHit out of each sensor group
          result.push_back(TrajectoryMeasurement(state,theUpdator->buildMultiRecHit(hits, state)));
        }
      } else {
          LogTrace("MultiRecHitCollector") << "  No measurements found in current group.";

          if( result.empty() ) continue;

          result.push_back(TrajectoryMeasurement(state,
                                        std::make_shared<InvalidTrackingRecHit>(measDet.mdet().geomDet(), TrackingRecHit::missing)));

      }
      
  }
  LogTrace("MultiRecHitCollector") << " Ending SimpleDAFHitCollector::recHits >> " << result.size();

  //adding a protection against too few hits and invalid hits 
  //(due to failed propagation on the same surface of the original hits)
  if (result.size()>2)
  {
    int hitcounter=0;
    //check if the vector result has more than 3 valid hits
    for (vector<TrajectoryMeasurement>::const_iterator iimeas = result.begin(); iimeas != result.end(); ++iimeas) {
      if(iimeas->recHit()->isValid()) hitcounter++;
    }

    if(hitcounter>2)
      return result;
    else return vector<TrajectoryMeasurement>();
  }

  else{return vector<TrajectoryMeasurement>();}

}

/*vector<TrajectoryMeasurement> SimpleDAFHitCollector::recHits(const Trajectory& traj, const MeasurementTrackerEvent *theMTE) const{


	//it assumes that the measurements are sorted in the smoothing direction
	vector<TrajectoryMeasurement> meas = traj.measurements();
	if (meas.empty()) //return TransientTrackingRecHit::ConstRecHitContainer();	
		return vector<TrajectoryMeasurement>();


	//TransientTrackingRecHit::ConstRecHitContainer result;
	vector<TrajectoryMeasurement> result;

	for(vector<TrajectoryMeasurement>::reverse_iterator imeas = meas.rbegin(); imeas != meas.rend(); imeas++) {
		//if the rechit is associated to a valid detId  
		if(imeas->recHit()->geographicalId().rawId()){
			MeasurementDetWithData md = theMTE->idToDet(imeas->recHit()->geographicalId());
			vector<TrajectoryMeasurement> currentLayerMeas = md.fastMeasurements(imeas->updatedState(), imeas->updatedState(), *thePropagator, *(getEstimator()));
	        	buildMultiRecHits(currentLayerMeas, result);
		} else {
			result.push_back(*imeas);
		}
	}
	LogTrace("MultiRecHitCollector") << "Original Measurement size "  << meas.size() << " SimpleDAFHitCollector returned " << result.size() << " rechits";
	//results are sorted in the fitting direction
	return result;	
}

void SimpleDAFHitCollector::buildMultiRecHits(const vector<TrajectoryMeasurement>& vmeas, vector<TrajectoryMeasurement>& result) const {

	if (vmeas.empty()) {
		LogTrace("MultiRecHitCollector") << "fastMeasurements returned an empty vector, should not happen " ; 
		//should we do something?
		//result.push_back(InvalidTransientRecHit::build(0,TrackingRecHit::missing));
		return;
	} 

	TrajectoryStateOnSurface state = vmeas.front().predictedState();

	if (state.isValid()==false){
		LogTrace("MultiRecHitCollector") << "first state is invalid; skipping ";
		return;
	}

	vector<const TrackingRecHit*> hits;
	//	TransientTrackingRecHit::ConstRecHitContainer hits;

	for (vector<TrajectoryMeasurement>::const_iterator imeas = vmeas.begin(); imeas != vmeas.end(); imeas++){
		if (imeas->recHit()->getType() != TrackingRecHit::missing) {
                                LogTrace("MultiRecHitCollector") << "This hit is valid ";
                                hits.push_back(imeas->recHit()->hit());
                }
	}

	if (hits.empty()){
		LogTrace("MultiRecHitCollector") << "No valid hits found ";
		return;
	}

	result.push_back(TrajectoryMeasurement(state,theUpdator->buildMultiRecHit(hits, state)));	
	//	result.push_back(TrajectoryMeasurement(state,theUpdator->update(hits, state)));	

}

*/
