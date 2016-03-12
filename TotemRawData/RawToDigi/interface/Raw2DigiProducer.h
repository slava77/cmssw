/****************************************************************************
*
* This is a part of TOTEM offline software.
* Authors: 
*   Maciej Wr??bel (wroblisko@gmail.com)
*   Jan Ka??par (jan.kaspar@gmail.com)
*   Marcin Borratynski (mborratynski@gmail.com)
*
****************************************************************************/

#ifndef Raw2DigiProducer_h
#define Raw2DigiProducer_h

#include <memory>
#include <string>

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/DetSetVector.h"

#include "TotemCondFormats/DAQInformation/interface/AnalysisMask.h"
#include "TotemCondFormats/DAQInformation/interface/DAQMapping.h"
#include "TotemRawDataLibrary/DataFormats/interface/VFATFrameCollection.h"
#include "TotemRawDataLibrary/DataFormats/interface/RawEvent.h"
#include "DataFormats/TotemRPDataTypes/interface/RPStripDigi.h"


// t1
#define DIS_CONN 56

namespace edm {
  class ParameterSet;
  class EventSetup;
  class Event;
  class EventID;
}

namespace Totem {
  class FramePosition;
}

using namespace std;


/**
 *\brief Producer module, which uses RawEvent class to produce all (all which are still in use) digi outputs (RP Data/RP CC/T1 DATA/T2 Data).
 *  Module also produces the information about conversion - Raw2DigiStatus.
 *
 *  Module takes following untracked parameters:
 *  - uint verbosity - describes the level of module verbosity. Values from 0-3.
 *    0 - no logging informations
 *    1 - only error information per event
 *    2 - error information per event and information about which frames couldn't have been processed
 *    3 - all the previous + information why certain frame couldn't have been processed
 *  - string rpDataProductLabel/rpCCProductLabel/t1DataProductLabel/t2DataProductLabel - name of output products, bu default "rpDataOutput" etc.
 *  - string conversionStatusLabel - label of Raw2DigiStatus - by default "conversionStatus"
 *  - float ECThreshold and BCThreshold, which indicate what fraction of all frames in data should be the frames with most frequent EC/BC number. So, the following condition should be satisfied mostFrequentCounterFrames/TotalFrames >= Threshold.
 * 
 *  Module produces digi outputs and conversion status (described in different document). To perform the conversion it needs AnalysisMask and DAQMapping from EventSetup which is produced by DAQMappingSourceXML.
 */

class Raw2DigiProducer : public edm::EDProducer
{
 public:

  Raw2DigiProducer(const edm::ParameterSet& conf);
  virtual  ~Raw2DigiProducer();

  virtual void beginRun(const edm::Run&,const edm::EventSetup&);
  virtual void endJob();
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  // RP DATA
  void rpDataBeginJob();
  void rpDataEndJob(edm::Event& e);
  void rpDataProduce(edm::Event& e, VFATFrameCollection::Iterator &iter, const VFATInfo &info, const VFATAnalysisMask &analysisMask);



 private:
  edm::InputTag rawEventLabel;
  //Handle< RawEvent >
  edm::EDGetTokenT<RawEvent>rawEventLabelToken_;



  unsigned char verbosity;

  unsigned int printErrorSummary;
  unsigned int printUnknownFrameSummary;

  enum TestFlag { tfNoTest, tfWarn, tfErr };

  unsigned int testFootprint;
  unsigned int testCRC;
  unsigned int testID;
  unsigned int testECRaw;
  unsigned int testECDAQ;
  unsigned int testECMostFrequent;
  unsigned int testBCMostFrequent;

  // product labels
  string rpDataProductLabel; 
  string conversionStatusLabel; 

  /// the minimal required number of frames to determine the most frequent counter value
  unsigned int EC_min, BC_min;

  /// the minimal required (relative) occupancy of the most frequent counter value to be accepted
  double EC_fraction, BC_fraction;


  // products
  std::auto_ptr<edm::DetSetVector<RPStripDigi> > rpDataOutput;  

  // error summary
  std::map<Totem::FramePosition, std::map<VFATStatus, unsigned int> > errorSummary;
  std::map<Totem::FramePosition, unsigned int> unknownSummary;
};

#endif
