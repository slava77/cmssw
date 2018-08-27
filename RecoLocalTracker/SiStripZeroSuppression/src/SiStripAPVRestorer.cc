#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripAPVRestorer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <cmath>
#include <iostream>
#include <algorithm>


SiStripAPVRestorer::SiStripAPVRestorer(const edm::ParameterSet& conf):
  quality_cache_id(-1), noise_cache_id(-1), pedestal_cache_id(-1),
  ForceNoRestore_(conf.getParameter<bool>("ForceNoRestore")),
// SelfSelectRestoreAlgo_(conf.getParameter<bool>("SelfSelectRestoreAlgo")),
  InspectAlgo_(conf.getParameter<std::string>("APVInspectMode")),
  RestoreAlgo_(conf.getParameter<std::string>("APVRestoreMode")),
  // CM map settings
  useRealMeanCM_(conf.getParameter<bool>("useRealMeanCM")),
  MeanCM_(conf.getParameter<int32_t>("MeanCM")),
  DeltaCMThreshold_(conf.getParameter<uint32_t>("DeltaCMThreshold")),
  // abnormal baseline inspect
  fraction_(conf.getParameter<double>("Fraction")),
  deviation_(conf.getParameter<uint32_t>("Deviation")),
  // NullInspect
  restoreThreshold_(conf.getParameter<double>("restoreThreshold")),
  // baseline and saturation inspect
  nSaturatedStrip_(conf.getParameter<uint32_t>("nSaturatedStrip")),
  // FlatRegionsFinder
  nSigmaNoiseDerTh_(conf.getParameter<uint32_t>("nSigmaNoiseDerTh")),
  consecThreshold_(conf.getParameter<uint32_t>("consecThreshold")),
  nSmooth_(conf.getParameter<uint32_t>("nSmooth")),
  distortionThreshold_(conf.getParameter<uint32_t>("distortionThreshold")),
  ApplyBaselineCleaner_(conf.getParameter<bool>("ApplyBaselineCleaner")),
  CleaningSequence_(conf.getParameter<uint32_t>("CleaningSequence")),
  // cleaner_localmin
  slopeX_(conf.getParameter<int32_t>("slopeX")),
  slopeY_(conf.getParameter<int32_t>("slopeY")),
  // cleaner_monotony
  hitStripThreshold_(conf.getParameter<uint32_t>("hitStripThreshold")),
  // baseline follower (restore)
  minStripsToFit_(conf.getParameter<uint32_t>("minStripsToFit")),
  ApplyBaselineRejection_(conf.getParameter<bool>("ApplyBaselineRejection")),
  filteredBaselineMax_(conf.getParameter<double>("filteredBaselineMax")),
  filteredBaselineDerivativeSumSquare_(conf.getParameter<double>("filteredBaselineDerivativeSumSquare")),
  // derivative follower restorer
  gradient_threshold_(conf.getParameter<int>("discontinuityThreshold")),
  last_gradient_(conf.getParameter<int>("lastGradient")),
  size_window_(conf.getParameter<int>("sizeWindow")),
  width_cluster_(conf.getParameter<int>("widthCluster"))
{
  if ( RestoreAlgo_ == "BaselineFollower" && InspectAlgo_ != "BaselineFollower" )
    throw cms::Exception("Incompatible Algorithm") << "The BaselineFollower restore method requires the BaselineFollower inspect method";
}


void SiStripAPVRestorer::init(const edm::EventSetup& es)
{
  uint32_t n_cache_id = es.get<SiStripNoisesRcd>().cacheIdentifier();
  uint32_t q_cache_id = es.get<SiStripQualityRcd>().cacheIdentifier();
  uint32_t p_cache_id = es.get<SiStripPedestalsRcd>().cacheIdentifier();

  if(n_cache_id != noise_cache_id) {
    es.get<SiStripNoisesRcd>().get( noiseHandle );
    noise_cache_id = n_cache_id;
  } else {
    noise_cache_id = n_cache_id;
  }
  if(q_cache_id != quality_cache_id) {
    es.get<SiStripQualityRcd>().get( qualityHandle );
    quality_cache_id = q_cache_id;
  }else {
    quality_cache_id = q_cache_id;
  }

  if(p_cache_id != pedestal_cache_id) {
		es.get<SiStripPedestalsRcd>().get( pedestalHandle );
		pedestal_cache_id = p_cache_id;
  }else {
    pedestal_cache_id = p_cache_id;
  }
}

uint16_t SiStripAPVRestorer::InspectAndRestore(uint32_t detId, uint16_t firstAPV, const digivector_t& rawDigisPedSubtracted, digivector_t& processedRawDigi, const medians_t& vmedians )
{
  auto nAPVFlagged = inspect(detId, firstAPV, rawDigisPedSubtracted, vmedians);
  restore(firstAPV, processedRawDigi);
  return nAPVFlagged;
}

uint16_t SiStripAPVRestorer::inspect(uint32_t detId, uint16_t firstAPV, const digivector_t& digis, const medians_t& vmedians)
{
  detId_ = detId;

  apvFlags_.assign(6, "");
  apvFlagsBoolOverride_.assign(6, false);
  apvFlagsBool_.clear();
  median_.assign(6, -999);
  badAPVs_.assign(6, false);
  SmoothedMaps_.clear();
  BaselineMap_.clear();
  for ( const auto med : vmedians ) {
    auto iAPV = med.first;
    median_[iAPV] = med.second;
    badAPVs_[iAPV] = qualityHandle->IsApvBad(detId_, iAPV);
  }

  uint16_t nAPVFlagged = 0;
  if      ( InspectAlgo_ == "Hybrid" )
    nAPVFlagged = HybridFormatInspect(firstAPV, digis);
  else if ( InspectAlgo_ == "HybridEmulation" )
    nAPVFlagged = HybridEmulationInspect(firstAPV, digis);
  else if ( InspectAlgo_ == "BaselineFollower" )
    nAPVFlagged = BaselineFollowerInspect(firstAPV, digis);
  else if ( InspectAlgo_ == "AbnormalBaseline" )
    nAPVFlagged = AbnormalBaselineInspect(firstAPV, digis);
  else if ( InspectAlgo_ == "Null" )
    nAPVFlagged = NullInspect(firstAPV, digis);
  else if ( InspectAlgo_ == "BaselineAndSaturation" )
    nAPVFlagged = BaselineAndSaturationInspect(firstAPV, digis);
  else if ( InspectAlgo_ == "DerivativeFollower" )
    nAPVFlagged = ForceRestoreInspect(firstAPV, digis);
  else
    throw cms::Exception("Unregistered Inspect Algorithm") << "SiStripAPVRestorer possibilities: (Null), (AbnormalBaseline), (BaselineFollower), (BaselineAndSaturation), (DerivativeFollower), (Hybrid), (HybridEmulation)";

  // update boolean version of APV flags
  for ( size_t i = 0; i < apvFlags_.size(); ++i ) {
    apvFlagsBool_.push_back((!apvFlags_[i].empty()) && ( ! apvFlagsBoolOverride_[i] ));
  }

  return nAPVFlagged;
}

void SiStripAPVRestorer::restore(uint16_t firstAPV, digivector_t& digis)
{
  if ( ForceNoRestore_ ) return;

  for ( uint16_t iAPV=firstAPV; iAPV < digis.size()/128 + firstAPV; ++iAPV ) {
    auto algoToUse = apvFlags_[iAPV];
    if ( !algoToUse.empty()) {
      // if(!SelfSelectRestoreAlgo_) algoToUse = RestoreAlgo_;
      if        ( algoToUse == "Flat" ) {
        FlatRestore(iAPV, firstAPV, digis);
      } else if ( algoToUse == "BaselineFollower" ) {
        BaselineFollowerRestore(iAPV, firstAPV, median_[iAPV], digis);
      } else if ( algoToUse == "DerivativeFollower" ) {
        DerivativeFollowerRestore(iAPV, firstAPV, digis);
      } else {
        throw cms::Exception("Unregistered Restore Algorithm") << "SiStripAPVRestorer possibilities: (Flat), (BaselineFollower), (DerivativeFollower)";
      }
    }
  }
}

// Inspect method implementations

inline uint16_t SiStripAPVRestorer::HybridFormatInspect(uint16_t firstAPV, const digivector_t& digis)
{
  digivector_t singleAPVdigi; singleAPVdigi.reserve(128);
  uint16_t nAPVflagged = 0;
  for ( uint16_t iAPV = firstAPV; iAPV < digis.size()/128 + firstAPV; ++iAPV ) {
    digimap_t smoothedmap;
    if ( ! badAPVs_[iAPV] ) {
      uint16_t stripsPerAPV = 0;
      singleAPVdigi.clear();
      for ( int16_t strip = (iAPV-firstAPV)*128; strip < (iAPV-firstAPV+1)*128; ++strip ) {
        const uint16_t adc = digis[strip];
        singleAPVdigi.push_back(adc);
        if ( adc > 0 ) ++stripsPerAPV;
      }

      if ( stripsPerAPV > 64 ) {
        FlatRegionsFinder(singleAPVdigi, smoothedmap, iAPV);
        apvFlags_[iAPV]= RestoreAlgo_;    //specify any algo to make the restore
        nAPVflagged++;
      }
    }
    SmoothedMaps_.emplace_hint(SmoothedMaps_.end(), iAPV, std::move(smoothedmap));
  }
  return nAPVflagged;
}

inline uint16_t SiStripAPVRestorer::BaselineFollowerInspect(uint16_t firstAPV, const digivector_t& digis)
{
  digivector_t singleAPVdigi; singleAPVdigi.reserve(128);
  uint16_t nAPVflagged = 0;

  auto itCMMap = std::end(MeanCMmap_);
  if (useRealMeanCM_) itCMMap = MeanCMmap_.find(detId_);

  for ( uint16_t iAPV = firstAPV; iAPV < firstAPV + digis.size()/128; ++iAPV ) {
    digimap_t smoothedmap;
    if ( ! badAPVs_[iAPV] ) {
      float MeanAPVCM = MeanCM_;
      if ( useRealMeanCM_ && ( std::end(MeanCMmap_) != itCMMap ) )
        MeanAPVCM = itCMMap->second[iAPV];

      singleAPVdigi.clear();
      std::copy_n(std::begin(digis)+128*(iAPV-firstAPV), 128, std::back_inserter(singleAPVdigi));

      const float DeltaCM = median_[iAPV] - MeanAPVCM;
      //std::cout << "Delta CM: " << DeltaCM << " CM: " << median_[APV] << " detId " << (uint32_t) detId_ << std::endl;
      if ( ( DeltaCM < 0 ) && ( std::abs(DeltaCM) > DeltaCMThreshold_ ) ) {
        if ( ! FlatRegionsFinder(singleAPVdigi, smoothedmap, iAPV) ) {
          apvFlags_[iAPV]= RestoreAlgo_;    //specify any algo to make the restore
          ++nAPVflagged;
        }
      }
    }
    SmoothedMaps_.emplace_hint(SmoothedMaps_.end(), iAPV, std::move(smoothedmap));
  }
  return nAPVflagged;
}

inline uint16_t SiStripAPVRestorer::ForceRestoreInspect(uint16_t firstAPV, const digivector_t& digis)
{
  uint16_t nAPVflagged = 0;
  for ( uint16_t iAPV = firstAPV; iAPV < firstAPV + digis.size()/128; ++iAPV ) {
    if ( ! badAPVs_[iAPV] ) {
      apvFlags_[iAPV] = RestoreAlgo_;    //specify any algo to make the restore
      nAPVflagged++;
    }
  }
  return nAPVflagged;
}

inline uint16_t SiStripAPVRestorer::BaselineAndSaturationInspect(uint16_t firstAPV, const digivector_t& digis)
{
  digivector_t singleAPVdigi; singleAPVdigi.reserve(128);
  uint16_t nAPVflagged = 0;

  auto itCMMap = std::end(MeanCMmap_);
  if (useRealMeanCM_) itCMMap = MeanCMmap_.find(detId_);

  for ( uint16_t iAPV = firstAPV; iAPV < firstAPV + digis.size()/128; ++iAPV ) {
    if ( ! badAPVs_[iAPV] ) {
      float MeanAPVCM = MeanCM_;
      if ( useRealMeanCM_ && ( std::end(MeanCMmap_) != itCMMap ) )
        MeanAPVCM = itCMMap->second[iAPV];

      singleAPVdigi.clear();
      uint16_t nSatStrip = 0;
      for ( int16_t strip = (iAPV-firstAPV)*128; strip < (iAPV-firstAPV+1)*128; ++strip ) {
        const uint16_t digi = digis[strip];
        singleAPVdigi.push_back(digi);
        if ( digi >= 1023 ) ++nSatStrip;
      }

      const float DeltaCM = median_[iAPV] - MeanAPVCM;
      if ( ( DeltaCM < 0 ) && ( std::abs(DeltaCM) > DeltaCMThreshold_ ) && ( nSatStrip >= nSaturatedStrip_ ) ) {
        apvFlags_[iAPV] = RestoreAlgo_;    //specify any algo to make the restore
        ++nAPVflagged;
      }
    }
  }
  return nAPVflagged;
}

inline uint16_t SiStripAPVRestorer::AbnormalBaselineInspect(uint16_t firstAPV, const digivector_t& digis)
{
  SiStripQuality::Range detQualityRange = qualityHandle->getRange(detId_);

  uint16_t nAPVflagged = 0;

  auto itCMMap = std::end(MeanCMmap_);
  if (useRealMeanCM_) itCMMap = MeanCMmap_.find(detId_);

  int devCount = 0, qualityCount = 0, minstrip = 0;
  for ( uint16_t iAPV = firstAPV; iAPV < firstAPV + digis.size()/128; ++iAPV ) {
    if ( ! badAPVs_[iAPV] ) {
      float MeanAPVCM = MeanCM_;
      if ( useRealMeanCM_ && ( std::end(MeanCMmap_) != itCMMap ) )
        MeanAPVCM = itCMMap->second[iAPV];

      for (uint16_t istrip = iAPV*128; istrip < (iAPV+1)*128; ++istrip ) {
        const auto fs = static_cast<int>(digis[istrip-firstAPV*128]);
        if ( ! qualityHandle->IsStripBad(detQualityRange,istrip) ) {
          qualityCount++;
          if ( std::abs(fs-MeanAPVCM) > static_cast<int>(deviation_) ) {
            devCount++;
            minstrip = std::min(fs, minstrip);
          }
        }
      }
      if ( devCount > fraction_ * qualityCount ) {
        apvFlags_[iAPV] = RestoreAlgo_;      //specify any algo to make the restore
        nAPVflagged++;
      }
    }
  }
  return nAPVflagged;
}

inline uint16_t SiStripAPVRestorer::NullInspect(uint16_t firstAPV, const digivector_t& digis)
{
  SiStripQuality::Range detQualityRange = qualityHandle->getRange(detId_);

  uint16_t nAPVflagged = 0;
  for ( uint16_t iAPV = firstAPV; iAPV < firstAPV + digis.size()/128; ++iAPV ) {
    if ( ! badAPVs_[iAPV] ) {
      int zeroCount = 0, qualityCount = 0;
      for (uint16_t istrip = iAPV*128; istrip < (iAPV+1)*128; ++istrip ) {
        const auto fs = static_cast<int>(digis[istrip-firstAPV*128]);
        if ( ! qualityHandle->IsStripBad(detQualityRange, istrip) ) {
          qualityCount++;
          if ( fs < 1 ) zeroCount++;
        }
      }
      if ( zeroCount > restoreThreshold_ * qualityCount ) {
        apvFlags_[iAPV] = RestoreAlgo_;     //specify any algo to make the restore
        nAPVflagged++;
      }
    }
  }
  return nAPVflagged;
}

inline uint16_t SiStripAPVRestorer::HybridEmulationInspect(uint16_t firstAPV, const digivector_t& digis)
{
  uint16_t nAPVflagged = 0;

  auto itCMMap = std::end(MeanCMmap_);
  if (useRealMeanCM_) itCMMap = MeanCMmap_.find(detId_);

  for ( uint16_t iAPV = firstAPV; iAPV < firstAPV + digis.size()/128; ++iAPV ) {
    if ( ! badAPVs_[iAPV] ) {
      float MeanAPVCM = MeanCM_;
      if ( useRealMeanCM_ && ( std::end(MeanCMmap_) != itCMMap ) )
        MeanAPVCM = itCMMap->second[iAPV];

      const float DeltaCM = median_[iAPV] - MeanAPVCM;
      //std::cout << "Delta CM: " << DeltaCM << " CM: " << median_[APV] << " detId " << (uint32_t) detId_ << std::endl;
      if ( ( DeltaCM < 0 ) && ( std::abs(DeltaCM) > DeltaCMThreshold_ ) ) {
        apvFlags_[iAPV] = "HybridEmulation";
        ++nAPVflagged;
        //std::cout << "and it is APV: " << APV << "Delta CM: " << DeltaCM << " median " << median_[APV] << " Mean APV " << MeanAPVCM << " marked bad " << markedVRAPVs[APV] << std::endl; //to be removed
      }
    }
  }
  return nAPVflagged;
}

// Restore method implementations

inline void SiStripAPVRestorer::BaselineFollowerRestore(uint16_t APVn, uint16_t firstAPV, float median, digivector_t& digis)
{
  digivector_t baseline(size_t(128), 0);

  //============================= Find Flat Regions & Interpolating the baseline & subtracting the baseline  =================
  if ( ! SmoothedMaps_.empty() ) {
    auto itSmootedMap = SmoothedMaps_.find(APVn);
    BaselineFollower(itSmootedMap->second, baseline, median);
  } else {
    //median=0;
    digivector_t singleAPVdigi; singleAPVdigi.reserve(128);
    std::copy_n(std::begin(digis)+128*(APVn-firstAPV), 128, std::back_inserter(singleAPVdigi));
    digimap_t smoothedpoints;
    FlatRegionsFinder(singleAPVdigi, smoothedpoints, APVn);
    BaselineFollower(smoothedpoints, baseline, median);
  }

  if (ApplyBaselineRejection_) {
    if ( CheckBaseline(baseline) ) apvFlagsBoolOverride_[APVn] = true;
  }

  //============================= subtracting the baseline =============================================
  for ( int16_t itStrip = 0 ; itStrip < 128; ++itStrip ) {
    digis[128*(APVn-firstAPV)+itStrip] -= baseline[itStrip] - median;
  }

  //============================= storing baseline to the map =============================================
  BaselineMap_.emplace_hint(BaselineMap_.end(), APVn, std::move(baseline));
}

inline void SiStripAPVRestorer::FlatRestore(uint16_t APVn, uint16_t firstAPV, digivector_t& digis)
{
  digivector_t baseline(size_t(128), 150);
  baseline[0] = 0; baseline[127] = 0;

  for ( int16_t itStrip = 0; itStrip < 128; ++itStrip ) {
    digis[128*(APVn-firstAPV)+itStrip] = baseline[itStrip];
  }
  BaselineMap_.emplace_hint(BaselineMap_.end(), APVn, std::move(baseline));
}

// Baseline calculation implementation =========================================
bool inline SiStripAPVRestorer::FlatRegionsFinder(const digivector_t& adcs, digimap_t& smoothedpoints, uint16_t APVn)
{
  smoothedpoints.clear();

  SiStripNoises::Range detNoiseRange = noiseHandle->getRange(detId_);

  //============================= Height above local minimum ===============================
  std::vector<float> adcsLocalMinSubtracted(size_t(128), 0);
  for ( uint32_t istrip = 0; istrip < 128; ++istrip ) {
    float localmin = 999.9;
    for ( uint16_t jstrip = std::max(0, static_cast<int>(istrip-nSmooth_/2)); jstrip < std::min(128, static_cast<int>(istrip+nSmooth_/2)); ++jstrip ) {
      const float nextvalue = adcs[jstrip];
      if (nextvalue < localmin) localmin = nextvalue;
    }
    adcsLocalMinSubtracted[istrip] = adcs[istrip] - localmin;
  }

  //============================= Find regions with stable slopes ========================
  digimap_t consecpoints;
  std::vector<uint16_t> nConsStrip;
  //Creating maps with all the neighborhood strip and putting in a nCosntStip vector how many we have
  uint16_t consecStrips = 0;
  for ( uint32_t istrip = 0; istrip < 128; ++istrip ) {
    const int16_t adc = adcs[istrip];
   //if( adcsLocalMinSubtracted[istrip] < nSigmaNoiseDerTh_ * (float)noiseHandle->getNoise(istrip+APVn*128,detNoiseRange) && (adc - median) < hitStripThreshold_){
    if ( adcsLocalMinSubtracted[istrip] < nSigmaNoiseDerTh_ * (float)noiseHandle->getNoiseFast(istrip+APVn*128,detNoiseRange) ) {
      consecpoints.emplace_hint(consecpoints.end(), istrip, adc);
      ++consecStrips;
    } else if ( consecStrips > 0 ) {
      nConsStrip.push_back(consecStrips);
      consecStrips = 0;
    }
  }
  //to cope with the last flat region of the APV
  if ( consecStrips > 0 ) nConsStrip.push_back(consecStrips);

  //removing from the map the fist and last points in wide flat regions and erasing from the map too small regions
  auto itConsecpoints = consecpoints.begin();
  float MinSmoothValue=20000., MaxSmoothValue=0.;
  for ( const auto consecStrips : nConsStrip ) {
    if ( consecStrips >= consecThreshold_ ) {
      ++itConsecpoints;  //skipping first point
      uint16_t nFirstStrip = itConsecpoints->first;
      uint16_t nLastStrip;
      float smoothValue = 0.0;
      float stripCount = 1;
      for ( uint16_t n = 0; n < consecStrips-2; ++n ) {
        smoothValue += itConsecpoints->second;
        if ( stripCount == consecThreshold_ ) {
          smoothValue /= (float)stripCount;
          nLastStrip = nFirstStrip + stripCount - 1;
          smoothedpoints.emplace_hint(smoothedpoints.end(), nFirstStrip, smoothValue);
          smoothedpoints.emplace_hint(smoothedpoints.end(), nLastStrip, smoothValue);
          if ( smoothValue > MaxSmoothValue ) MaxSmoothValue = smoothValue;
          if ( smoothValue < MinSmoothValue ) MinSmoothValue = smoothValue;
          nFirstStrip = nLastStrip+1;
          smoothValue = 0;
          stripCount = 0;
        }
        ++stripCount;
        ++itConsecpoints;
      }
      ++itConsecpoints;  //and putting the pointer to the new seies of point

      if ( stripCount > 1 ) {
      //if(smoothValue>0){
        --stripCount;
        smoothValue /= static_cast<float>(stripCount);
        nLastStrip = nFirstStrip + stripCount -1;
        smoothedpoints.emplace_hint(smoothedpoints.end(), nFirstStrip, smoothValue);
        smoothedpoints.emplace_hint(smoothedpoints.end(), nLastStrip, smoothValue);
        if ( smoothValue > MaxSmoothValue ) MaxSmoothValue = smoothValue;
        if ( smoothValue < MinSmoothValue ) MinSmoothValue = smoothValue;
      }
    } else {
      for(int n =0; n< consecStrips ; ++n) ++itConsecpoints;
    }
  }

  if ( (MaxSmoothValue-MinSmoothValue) > distortionThreshold_) {
    if (ApplyBaselineCleaner_)
      BaselineCleaner(adcs, smoothedpoints, APVn);
    return false;
  }
  return true;
}

void inline SiStripAPVRestorer::BaselineCleaner(const digivector_t& adcs, digimap_t& smoothedpoints, uint16_t APVn)
{
  if        ( CleaningSequence_ == 0 ) {  //default sequence used up to now
    Cleaner_HighSlopeChecker(smoothedpoints);
    Cleaner_LocalMinimumAdder(adcs, smoothedpoints, APVn);
  } else if ( CleaningSequence_ == 1 ) {
    Cleaner_LocalMinimumAdder(adcs, smoothedpoints, APVn);
    Cleaner_HighSlopeChecker(smoothedpoints);
    Cleaner_MonotonyChecker(smoothedpoints);
  } else if ( CleaningSequence_ == 2 ) {
    Cleaner_HighSlopeChecker(smoothedpoints);
  } else if ( CleaningSequence_ == 3 ) {
    Cleaner_LocalMinimumAdder(adcs, smoothedpoints, APVn);
    Cleaner_HighSlopeChecker(smoothedpoints);
  } else {
    Cleaner_HighSlopeChecker(smoothedpoints);
    Cleaner_LocalMinimumAdder(adcs, smoothedpoints, APVn);
  }
}

void inline SiStripAPVRestorer::Cleaner_MonotonyChecker(digimap_t& smoothedpoints)
{
  //Removing points without monotony
  //--------------------------------------------------------------------------------------------------
  if ( smoothedpoints.size() < 3 ) return;

  auto it = smoothedpoints.begin();
  while ( smoothedpoints.size() > 3 && it != --(--smoothedpoints.end()) ) { //while we are not at the last point
    // get info about current and next points
    auto it2 = it;
    const float adc1 = it2->second;
    const float adc2 = (++it2)->second;
    const float adc3 = (++it2)->second;

    if ( (adc2-adc1) > hitStripThreshold_ && (adc2-adc3) > hitStripThreshold_ ) {
      smoothedpoints.erase(--it2);
    } else {
      ++it;
    }
  }
}

void inline SiStripAPVRestorer::Cleaner_LocalMinimumAdder(const digivector_t& adcs, digimap_t& smoothedpoints, uint16_t APVn)
{
  SiStripNoises::Range detNoiseRange = noiseHandle->getRange(detId_);
  //inserting extra point is case of local minimum
  //--------------------------------------------------------------------------------------------------
  // these should be reset now for the point-insertion that follows

  if ( smoothedpoints.size() >= 2 ) {
    for ( auto it = smoothedpoints.begin(); it != --smoothedpoints.end(); ++it ) {
      auto itNext = it; ++itNext;
      const float strip1 = it->first;
      const float strip2 = itNext->first;
      const float adc1 = it->second;
      const float adc2 = itNext->second;
      const float m = (adc2 -adc1)/(strip2 -strip1);

      //2,4
      if ( (strip2 - strip1) > slopeX_ && abs(adc1-adc2) > slopeY_ ) {
        float itStrip = 1;
        float strip = itStrip + strip1;
        while ( strip < strip2 ) {
          const float adc = adcs[strip];
          const auto noise = static_cast<float>(noiseHandle->getNoiseFast(strip+APVn*128,detNoiseRange));
          if ( adc < (adc1 + m * itStrip - 2 * noise) ) {
            //std::cout << "applying correction strip: " << strip + APVn*128 << " adc " << adc << " detId: " << detId_ << std::endl;
            smoothedpoints.emplace_hint(itNext, strip, adc);
            ++it;
          }
          ++itStrip;
          ++strip;
        }
      }
    }
  }

  const uint16_t firstStripFlat = smoothedpoints.begin()->first;
  const uint16_t lastStripFlat = smoothedpoints.begin()->first;
  const int16_t firstStripFlatADC = (--smoothedpoints.end())->second;
  const int16_t lastStripFlatADC = (--smoothedpoints.end())->second;
  if ( firstStripFlat > 3 ) {
    auto it = smoothedpoints.begin();
    float strip = 0;
    while ( strip < firstStripFlat ) {
      const float adc = adcs[strip];
      const auto noise = static_cast<float>(noiseHandle->getNoiseFast(strip+APVn*128,detNoiseRange));
      if ( adc < ( firstStripFlatADC - 2 * noise ) ) {
        smoothedpoints.emplace_hint(it, strip, adc);
        ++it;
      }
      ++strip;
    }
  }
  if ( lastStripFlat < 125 ) {
    float strip = lastStripFlat+1;
    while ( strip < 128 ) {
      const float adc = adcs[strip];
      const auto noise = static_cast<float>(noiseHandle->getNoiseFast(strip+APVn*128,detNoiseRange));
      if ( adc < ( lastStripFlatADC - 2 * noise ) ) {
        smoothedpoints.emplace_hint(smoothedpoints.end(), strip,adc);
      }
      ++strip;
    }
  }
}

void inline SiStripAPVRestorer::Cleaner_HighSlopeChecker(digimap_t& smoothedpoints)
{
  //Removing points in the slope is too high
  //----------------------------------------------------------------------------
  if ( smoothedpoints.size() < 4 ) return;

  auto it = smoothedpoints.begin();
  while ( smoothedpoints.size() > 2 && it != --smoothedpoints.end() ) { //while we are not at the last point
    //if(smoothedpoints.size() <2) break;
    // get info about current and next points
    auto itNext = it; ++itNext;
    const float strip1 = it->first;
    const float strip2 = itNext->first;
    const float adc1 = it->second;
    const float adc2 = itNext->second;
    const float m = (adc2 -adc1)/(strip2 -strip1);

    if ( m > 2 ) { // in case of large positive slope, remove next point and try again from same current point
      smoothedpoints.erase(itNext);
    } else if ( m < -2 ) { // in case of large negative slope, remove current point and either...
      // move to next point if we have reached the beginning (post-increment to avoid invalidating pointer during erase) or...
      if ( it == smoothedpoints.begin() )
        smoothedpoints.erase(it++);
      // try again from the previous point if we have not reached the beginning
      else smoothedpoints.erase(it--);
    } else { // in case of a flat enough slope, continue on to the next point
      ++it;
    }
  }
}

void inline SiStripAPVRestorer::BaselineFollower(const digimap_t& smoothedpoints, digivector_t& baseline, float median)
{
  //if not enough points
  if ( smoothedpoints.size() < minStripsToFit_ ) {
     baseline.insert(baseline.begin(),128, median);
  } else {
    baseline.assign(size_t(128), 0);

    const auto lastIt = --smoothedpoints.end();
    const uint16_t firstStripFlat = smoothedpoints.begin()->first;
    const uint16_t lastStripFlat = lastIt->first;
    const int16_t firstStripFlatADC= smoothedpoints.begin()->second;
    const int16_t lastStripFlatADC= lastIt->second;

    //adding here the costant line at the extremities
    baseline.erase(baseline.begin(), baseline.begin()+firstStripFlat);
    baseline.insert(baseline.begin(), firstStripFlat, firstStripFlatADC);

    baseline.erase(baseline.begin()+lastStripFlat, baseline.end());
    baseline.insert(baseline.end(), 128 - lastStripFlat, lastStripFlatADC);

    //IMPORTANT: the itSmoothedpointsEnd should be at least smaller than smoothedpoints.end() -1
    for ( auto itSmoothedpoints = smoothedpoints.begin(); itSmoothedpoints != (--smoothedpoints.end()); ++itSmoothedpoints ) {
      auto itNextSmoothedpoints = itSmoothedpoints;
      ++itNextSmoothedpoints;
      const float strip1 = itSmoothedpoints->first;
      const float strip2 = itNextSmoothedpoints->first;
      const float adc1 = itSmoothedpoints->second;
      const float adc2 = itNextSmoothedpoints->second;

      baseline[strip1] = adc1;
      baseline[strip2] = adc2;
      const float m = (adc2 -adc1)/(strip2 -strip1);
      uint16_t itStrip = strip1 +1;
      float stripadc = adc1 + m;
      while ( itStrip < strip2 ) {
        baseline[itStrip] = stripadc;
        ++itStrip;
        stripadc += m;
      }
    }
  }
}


bool SiStripAPVRestorer::CheckBaseline(const digivector_t &baseline) const
{
	// The Savitzky-Golay (S-G) filter of any left length nL, right
	// length nR, and order m, and with an optional opt equals the
	// derivative order (0 for the magnitude, 1 for the first
	// derivative, etc.) can be calculated using the following
	// Mathematica code:
	//
	// SavitzkyGolay[m_?IntegerQ, {nL_?IntegerQ, nR_?IntegerQ},
	//   opt___] := Module[
	//   {a, d},
	//   d = If[opt === Null, 0, If[IntegerQ[opt], opt, 0]];
	//   a = Table[
	//     If[i == 0 && j == 0, 1, i^j], {i, -nL, nR}, {j, 0,
	//      m}]; (Inverse[Transpose[a].a].Transpose[a])[[d + 1]]];
	//
	// The following coefficients can be then calculated by:
	//
	// N[Join[Table[SavitzkyGolay[2, {k, 16}], {k, 0, 16}],
	//   Table[SavitzkyGolay[2, {16, k}], {k, 15, 0, -1}]]]

	// nLR = max(nL, nR)
	static const size_t savitzky_golay_n_l_r = 16;
	static const float savitzky_golay_coefficient
		[2 * savitzky_golay_n_l_r + 1][2 * savitzky_golay_n_l_r + 1] = {
		{ 0.422085, 0.325077, 0.23839, 0.162023, 0.0959752, 0.0402477,
		  -0.00515996, -0.0402477, -0.0650155, -0.0794634, -0.0835913,
		  -0.0773994, -0.0608875, -0.0340557, 0.00309598, 0.0505676,
		  0.108359 },
		{ 0.315789, 0.254902, 0.19969, 0.150155, 0.106295, 0.0681115,
		  0.0356037, 0.00877193, -0.0123839, -0.0278638, -0.0376677,
		  -0.0417957, -0.0402477, -0.0330237, -0.0201238, -0.00154799,
		  0.0227038, 0.0526316 },
		{ 0.234586, 0.198496, 0.165207, 0.134719, 0.107032, 0.0821465,
		  0.0600619, 0.0407784, 0.024296, 0.0106148, -0.000265369,
		  -0.00834439, -0.0136223, -0.0160991, -0.0157747, -0.0126493,
		  -0.00672269, 0.00200501, 0.0135338 },
		{ 0.172078, 0.153076, 0.135099, 0.118148, 0.102221, 0.0873206,
		  0.073445, 0.0605947, 0.0487697, 0.0379699, 0.0281955,
		  0.0194463, 0.0117225, 0.00502392, -0.000649351, -0.00529733,
		  -0.00892003, -0.0115174, -0.0130895, -0.0136364 },
		{ 0.123659, 0.116431, 0.109144, 0.101798, 0.0943921, 0.0869268,
		  0.0794021, 0.0718179, 0.0641743, 0.0564712, 0.0487087,
		  0.0408868, 0.0330054, 0.0250646, 0.0170644, 0.00900473,
		  0.000885613, -0.00729294, -0.0155309, -0.0238283,
		  -0.0321852 },
		{ 0.0859684, 0.0868154, 0.0869565, 0.0863919, 0.0851214,
		  0.0831451, 0.080463, 0.0770751, 0.0729814, 0.0681818,
		  0.0626765, 0.0564653, 0.0495483, 0.0419255, 0.0335968,
		  0.0245624, 0.0148221, 0.00437606, -0.00677583, -0.0186335,
		  -0.0311971, -0.0444664 },
		{ 0.0565217, 0.0628458, 0.0680971, 0.0722756, 0.0753811,
		  0.0774139, 0.0783738, 0.0782609, 0.0770751, 0.0748165,
		  0.071485, 0.0670807, 0.0616036, 0.0550536, 0.0474308,
		  0.0387352, 0.0289667, 0.0181254, 0.00621118, -0.00677583,
		  -0.0208357, -0.0359684, -0.0521739 },
		{ 0.0334615, 0.0434281, 0.0521329, 0.0595759, 0.0657571,
		  0.0706765, 0.0743341, 0.07673, 0.0778641, 0.0777364,
		  0.0763469, 0.0736957, 0.0697826, 0.0646078, 0.0581712,
		  0.0504728, 0.0415126, 0.0312907, 0.0198069, 0.00706142,
		  -0.00694588, -0.022215, -0.0387458, -0.0565385 },
		{ 0.0153846, 0.0276923, 0.0386622, 0.0482943, 0.0565886,
		  0.0635452, 0.0691639, 0.0734448, 0.076388, 0.0779933,
		  0.0782609, 0.0771906, 0.0747826, 0.0710368, 0.0659532,
		  0.0595318, 0.0517726, 0.0426756, 0.0322408, 0.0204682,
		  0.00735786, -0.0070903, -0.0228763, -0.04, -0.0584615 },
		{ 0.001221, 0.0149451, 0.027326, 0.0383639, 0.0480586,
		  0.0564103, 0.0634188, 0.0690842, 0.0734066, 0.0763858,
		  0.078022, 0.078315, 0.077265, 0.0748718, 0.0711355,
		  0.0660562, 0.0596337, 0.0518681, 0.0427595, 0.0323077,
		  0.0205128, 0.00737485, -0.00710623, -0.0229304, -0.0400977,
		  -0.0586081 },
		{ -0.00985222, 0.00463138, 0.0178098, 0.029683, 0.0402509,
		  0.0495137, 0.0574713, 0.0641236, 0.0694708, 0.0735127,
		  0.0762494, 0.0776809, 0.0778073, 0.0766284, 0.0741442,
		  0.0703549, 0.0652604, 0.0588607, 0.0511557, 0.0421456,
		  0.0318302, 0.0202097, 0.0072839, -0.00694708, -0.0224833,
		  -0.0393247, -0.0574713 },
		{ -0.0184729, -0.00369458, 0.00984169, 0.0221359, 0.0331881,
		  0.0429982, 0.0515662, 0.0588923, 0.0649762, 0.0698181,
		  0.073418, 0.0757758, 0.0768915, 0.0767652, 0.0753968,
		  0.0727864, 0.0689339, 0.0638394, 0.0575028, 0.0499242,
		  0.0411035, 0.0310408, 0.019736, 0.00718917, -0.00659972,
		  -0.0216307, -0.0379037, -0.0554187 },
		{ -0.025139, -0.0103925, 0.00318873, 0.0156046, 0.0268552,
		  0.0369405, 0.0458605, 0.0536151, 0.0602045, 0.0656285,
		  0.0698872, 0.0729806, 0.0749086, 0.0756714, 0.0752688,
		  0.0737009, 0.0709677, 0.0670692, 0.0620054, 0.0557763,
		  0.0483818, 0.039822, 0.0300969, 0.0192065, 0.0071508,
		  -0.00607024, -0.0204566, -0.0360083, -0.0527253 },
		{ -0.0302419, -0.0157536, -0.00234785, 0.00997537, 0.021216,
		  0.0313741, 0.0404497, 0.0484427, 0.0553532, 0.0611811,
		  0.0659264, 0.0695892, 0.0721695, 0.0736672, 0.0740823,
		  0.0734149, 0.0716649, 0.0688324, 0.0649174, 0.0599198,
		  0.0538396, 0.0466769, 0.0384316, 0.0291038, 0.0186934,
		  0.00720046, -0.00537502, -0.0190331, -0.0337736,
		  -0.0495968 },
		{ -0.0340909, -0.0200147, -0.006937, 0.00514208, 0.0162226,
		  0.0263045, 0.0353878, 0.0434725, 0.0505587, 0.0566463,
		  0.0617353, 0.0658257, 0.0689175, 0.0710107, 0.0721054,
		  0.0722014, 0.0712989, 0.0693978, 0.0664981, 0.0625999,
		  0.057703, 0.0518076, 0.0449135, 0.0370209, 0.0281297,
		  0.01824, 0.0073516, -0.00453534, -0.0174209, -0.031305,
		  -0.0461877 },
		{ -0.0369318, -0.0233688, -0.0107221, 0.00100806, 0.0118218,
		  0.0217192, 0.0307001, 0.0387647, 0.0459128, 0.0521444,
		  0.0574597, 0.0618585, 0.0653409, 0.0679069, 0.0695565,
		  0.0702896, 0.0701063, 0.0690066, 0.0669905, 0.0640579,
		  0.0602089, 0.0554435, 0.0497617, 0.0431635, 0.0356488,
		  0.0272177, 0.0178702, 0.0076063, -0.00357405, -0.0156708,
		  -0.028684, -0.0426136 },
		{ -0.038961, -0.025974, -0.0138249, -0.00251362, 0.00795978,
		  0.0175953, 0.026393, 0.0343527, 0.0414747, 0.0477587,
		  0.0532049, 0.0578132, 0.0615836, 0.0645161, 0.0666108,
		  0.0678676, 0.0682866, 0.0678676, 0.0666108, 0.0645161,
		  0.0615836, 0.0578132, 0.0532049, 0.0477587, 0.0414747,
		  0.0343527, 0.026393, 0.0175953, 0.00795978, -0.00251362,
		  -0.0138249, -0.025974, -0.038961 },
		{ -0.0426136, -0.028684, -0.0156708, -0.00357405, 0.0076063,
		  0.0178702, 0.0272177, 0.0356488, 0.0431635, 0.0497617,
		  0.0554435, 0.0602089, 0.0640579, 0.0669905, 0.0690066,
		  0.0701063, 0.0702896, 0.0695565, 0.0679069, 0.0653409,
		  0.0618585, 0.0574597, 0.0521444, 0.0459128, 0.0387647,
		  0.0307001, 0.0217192, 0.0118218, 0.00100806, -0.0107221,
		  -0.0233688, -0.0369318 },
		{ -0.0461877, -0.031305, -0.0174209, -0.00453534, 0.0073516,
		  0.01824, 0.0281297, 0.0370209, 0.0449135, 0.0518076,
		  0.057703, 0.0625999, 0.0664981, 0.0693978, 0.0712989,
		  0.0722014, 0.0721054, 0.0710107, 0.0689175, 0.0658257,
		  0.0617353, 0.0566463, 0.0505587, 0.0434725, 0.0353878,
		  0.0263045, 0.0162226, 0.00514208, -0.006937, -0.0200147,
		  -0.0340909 },
		{ -0.0495968, -0.0337736, -0.0190331, -0.00537502, 0.00720046,
		  0.0186934, 0.0291038, 0.0384316, 0.0466769, 0.0538396,
		  0.0599198, 0.0649174, 0.0688324, 0.0716649, 0.0734149,
		  0.0740823, 0.0736672, 0.0721695, 0.0695892, 0.0659264,
		  0.0611811, 0.0553532, 0.0484427, 0.0404497, 0.0313741,
		  0.021216, 0.00997537, -0.00234785, -0.0157536, -0.0302419 },
		{ -0.0527253, -0.0360083, -0.0204566, -0.00607024, 0.0071508,
		  0.0192065, 0.0300969, 0.039822, 0.0483818, 0.0557763,
		  0.0620054, 0.0670692, 0.0709677, 0.0737009, 0.0752688,
		  0.0756714, 0.0749086, 0.0729806, 0.0698872, 0.0656285,
		  0.0602045, 0.0536151, 0.0458605, 0.0369405, 0.0268552,
		  0.0156046, 0.00318873, -0.0103925, -0.025139 },
		{ -0.0554187, -0.0379037, -0.0216307, -0.00659972, 0.00718917,
		  0.019736, 0.0310408, 0.0411035, 0.0499242, 0.0575028,
		  0.0638394, 0.0689339, 0.0727864, 0.0753968, 0.0767652,
		  0.0768915, 0.0757758, 0.073418, 0.0698181, 0.0649762,
		  0.0588923, 0.0515662, 0.0429982, 0.0331881, 0.0221359,
		  0.00984169, -0.00369458, -0.0184729 },
		{ -0.0574713, -0.0393247, -0.0224833, -0.00694708, 0.0072839,
		  0.0202097, 0.0318302, 0.0421456, 0.0511557, 0.0588607,
		  0.0652604, 0.0703549, 0.0741442, 0.0766284, 0.0778073,
		  0.0776809, 0.0762494, 0.0735127, 0.0694708, 0.0641236,
		  0.0574713, 0.0495137, 0.0402509, 0.029683, 0.0178098,
		  0.00463138, -0.00985222 },
		{ -0.0586081, -0.0400977, -0.0229304, -0.00710623, 0.00737485,
		  0.0205128, 0.0323077, 0.0427595, 0.0518681, 0.0596337,
		  0.0660562, 0.0711355, 0.0748718, 0.077265, 0.078315,
		  0.078022, 0.0763858, 0.0734066, 0.0690842, 0.0634188,
		  0.0564103, 0.0480586, 0.0383639, 0.027326, 0.0149451,
		  0.001221 },
		{ -0.0584615, -0.04, -0.0228763, -0.0070903, 0.00735786,
		  0.0204682, 0.0322408, 0.0426756, 0.0517726, 0.0595318,
		  0.0659532, 0.0710368, 0.0747826, 0.0771906, 0.0782609,
		  0.0779933, 0.076388, 0.0734448, 0.0691639, 0.0635452,
		  0.0565886, 0.0482943, 0.0386622, 0.0276923, 0.0153846 },
		{ -0.0565385, -0.0387458, -0.022215, -0.00694588, 0.00706142,
		  0.0198069, 0.0312907, 0.0415126, 0.0504728, 0.0581712,
		  0.0646078, 0.0697826, 0.0736957, 0.0763469, 0.0777364,
		  0.0778641, 0.07673, 0.0743341, 0.0706765, 0.0657571,
		  0.0595759, 0.0521329, 0.0434281, 0.0334615 },
		{ -0.0521739, -0.0359684, -0.0208357, -0.00677583, 0.00621118,
		  0.0181254, 0.0289667, 0.0387352, 0.0474308, 0.0550536,
		  0.0616036, 0.0670807, 0.071485, 0.0748165, 0.0770751,
		  0.0782609, 0.0783738, 0.0774139, 0.0753811, 0.0722756,
		  0.0680971, 0.0628458, 0.0565217 },
		{ -0.0444664, -0.0311971, -0.0186335, -0.00677583, 0.00437606,
		  0.0148221, 0.0245624, 0.0335968, 0.0419255, 0.0495483,
		  0.0564653, 0.0626765, 0.0681818, 0.0729814, 0.0770751,
		  0.080463, 0.0831451, 0.0851214, 0.0863919, 0.0869565,
		  0.0868154, 0.0859684 },
		{ -0.0321852, -0.0238283, -0.0155309, -0.00729294, 0.000885613,
		  0.00900473, 0.0170644, 0.0250646, 0.0330054, 0.0408868,
		  0.0487087, 0.0564712, 0.0641743, 0.0718179, 0.0794021,
		  0.0869268, 0.0943921, 0.101798, 0.109144, 0.116431,
		  0.123659 },
		{ -0.0136364, -0.0130895, -0.0115174, -0.00892003, -0.00529733,
		  -0.000649351, 0.00502392, 0.0117225, 0.0194463, 0.0281955,
		  0.0379699, 0.0487697, 0.0605947, 0.073445, 0.0873206,
		  0.102221, 0.118148, 0.135099, 0.153076, 0.172078 },
		{ 0.0135338, 0.00200501, -0.00672269, -0.0126493, -0.0157747,
		  -0.0160991, -0.0136223, -0.00834439, -0.000265369, 0.0106148,
		  0.024296, 0.0407784, 0.0600619, 0.0821465, 0.107032,
		  0.134719, 0.165207, 0.198496, 0.234586 },
		{ 0.0526316, 0.0227038, -0.00154799, -0.0201238, -0.0330237,
		  -0.0402477, -0.0417957, -0.0376677, -0.0278638, -0.0123839,
		  0.00877193, 0.0356037, 0.0681115, 0.106295, 0.150155,
		  0.19969, 0.254902, 0.315789 },
		{ 0.108359, 0.0505676, 0.00309598, -0.0340557, -0.0608875,
		  -0.0773994, -0.0835913, -0.0794634, -0.0650155, -0.0402477,
		  -0.00515996, 0.0402477, 0.0959752, 0.162023, 0.23839,
		  0.325077, 0.422085 }
	};

	float filtered_baseline[128];
	float filtered_baseline_derivative[127];

	// Zero filtered_baseline
	memset(filtered_baseline, 0, 128 * sizeof(float));
	// Filter the left edge using (nL, nR) = (0, 16) .. (15, 16) S-G
	// filters
	for (size_t i = 0; i < savitzky_golay_n_l_r; i++) {
		for (size_t j = 0; j < savitzky_golay_n_l_r + 1 + i; j++) {
			filtered_baseline[i] +=
				savitzky_golay_coefficient[i][j] * baseline[j];
		}
	}
	// Filter the middle section using the (nL, nR) = (16, 16) S-G
	// filter, while taking advantage of the symmetry to save 16
	// multiplications.
	for (size_t i = savitzky_golay_n_l_r;
		 i < 128 - savitzky_golay_n_l_r; i++) {
		filtered_baseline[i] =
			savitzky_golay_coefficient
			[savitzky_golay_n_l_r][savitzky_golay_n_l_r] * baseline[i];
		for (size_t j = 0; j < savitzky_golay_n_l_r; j++) {
			filtered_baseline[i] +=
				savitzky_golay_coefficient[savitzky_golay_n_l_r][j] *
				(baseline[i + j - savitzky_golay_n_l_r] +
				 baseline[i - j + savitzky_golay_n_l_r]);
		}
#if 0
		// Test that the indexing above is correct
		float test = 0;
		for (size_t j = 0; j < 2 * savitzky_golay_n_l_r + 1; j++) {
			test +=
				savitzky_golay_coefficient[savitzky_golay_n_l_r][j] *
				baseline[i + j - savitzky_golay_n_l_r];
		}
		// test == filtered_baseline[i] should hold now
#endif
	}
	// Filter the right edge using (nL, nR) = (16, 15) .. (16, 0) S-G
	// filters
	for (size_t i = 128 - savitzky_golay_n_l_r; i < 128; i++) {
		for (size_t j = 0; j < 128 - i + savitzky_golay_n_l_r; j++) {
			filtered_baseline[i] +=
				savitzky_golay_coefficient
				[2 * savitzky_golay_n_l_r + i + 1 - 128][j] *
				baseline[i + j - savitzky_golay_n_l_r];
		}
	}
	// In lieu of a spearate S-G derivative filter, the finite
	// difference is used here (since the output is sufficiently
	// smooth).
	for (size_t i = 0; i < 127; i++) {
		filtered_baseline_derivative[i] =
			filtered_baseline[i + 1] - filtered_baseline[i];
	}

	// Calculate the maximum deviation between filtered and unfiltered
	// baseline, plus the sum square of the derivative.

	double filtered_baseline_max = 0;
	double filtered_baseline_derivative_sum_square = 0;

	for (size_t i = 0; i < 128; i++) {
		const double d = filtered_baseline[i] - baseline[i];

		filtered_baseline_max =
			std::max(filtered_baseline_max,
					 static_cast<double>(fabs(d)));
	}
	for (size_t i = 0; i < 127; i++) {
		filtered_baseline_derivative_sum_square +=
			filtered_baseline_derivative[i] *
			filtered_baseline_derivative[i];
	}

#if 0
	std::cerr << __FILE__ << ':' << __LINE__ << ": "
			  << filtered_baseline_max << ' '
			  << filtered_baseline_derivative_sum_square << std::endl;
#endif

	// Apply the cut
	return !(filtered_baseline_max >= filteredBaselineMax_ ||
 			 filtered_baseline_derivative_sum_square >= filteredBaselineDerivativeSumSquare_);
}

//Other methods implementation ==============================================
//==========================================================================

void SiStripAPVRestorer::LoadMeanCMMap(const edm::Event& iEvent)
{
  if ( useRealMeanCM_ ) {
    edm::Handle<edm::DetSetVector<SiStripRawDigi>> input;
    iEvent.getByLabel("siStripDigis","VirginRaw", input);
    CreateCMMapRealPed(*input);
  } else {
    edm::Handle<edm::DetSetVector<SiStripProcessedRawDigi>> inputCM;
    iEvent.getByLabel("MEANAPVCM",inputCM);
    CreateCMMapCMstored(*inputCM);
  }
}
void SiStripAPVRestorer::CreateCMMapRealPed(const edm::DetSetVector<SiStripRawDigi>& input)
{
  MeanCMmap_.clear();
  for ( const auto& rawDigis : input ) {
    const auto detPedestalRange = pedestalHandle->getRange(rawDigis.id);
    const size_t nAPVs = rawDigis.size()/128;
    std::vector<float> MeanCMDetSet; MeanCMDetSet.reserve(nAPVs);
    for ( uint16_t iAPV = 0; iAPV < nAPVs; ++iAPV ) {
      uint16_t minPed = 128;
      for ( uint16_t strip = 128*iAPV; strip < 128*(iAPV+1); ++strip ) {
        uint16_t ped = static_cast<uint16_t>(pedestalHandle->getPed(strip, detPedestalRange));
        if (ped < minPed) minPed = ped;
      }
      MeanCMDetSet.push_back(minPed);
    }
    MeanCMmap_.emplace(rawDigis.id, std::move(MeanCMDetSet));
  }
}
void SiStripAPVRestorer::CreateCMMapCMstored(const edm::DetSetVector<SiStripProcessedRawDigi>& input)
{
  MeanCMmap_.clear();
  for ( const auto& rawDigis : input ) {
    std::vector<float> meanCMNValue;
    meanCMNValue.reserve(rawDigis.size());
    std::transform(std::begin(rawDigis), std::end(rawDigis), std::back_inserter(meanCMNValue),
        [] ( SiStripProcessedRawDigi cm ) { return cm.adc(); });
    MeanCMmap_.emplace(rawDigis.id, std::move(meanCMNValue));
  }
}

//NEW algorithm designed for implementation in FW=============================
//============================================================================
void SiStripAPVRestorer::DerivativeFollowerRestore(uint16_t APVn, uint16_t firstAPV, digivector_t& digis)
{
//std::cout << "++++++++" << "SiStripAPVRestorer::DerivativeFollowe" << std::endl;
//	int gradient_threshold_ =12;
//	std::cout <<"DT: "<< gradient_threshold_ << std::endl;
//	int last_gradient_ = 10;
//	int width_cluster_ = 64;
//	int size_window_ = 1;
//	std::cout << "LG: " << last_gradient_ << std::endl;
//	std::cout << "SW: " << size_window_ << std::endl;
//	std::cout << "WC: " << width_cluster_ << std::endl;
//	std::cout << "detId: " << detId_ << std::endl;

  digivector_t singleAPVdigi;
  singleAPVdigi.clear();
  for(int16_t strip = (APVn-firstAPV)*128; strip < (APVn-firstAPV+1)*128; ++strip) singleAPVdigi.push_back(digis[strip]+1024);

  digimap_t discontinuities;   // it will contain the start and the end of each region in which a greadient is present
  discontinuities.clear();

  digimap_t::iterator itdiscontinuities;

  //----Variables of the first part---//

  bool isMinimumAndNoMax = false;
  bool isFirstStrip = false;

  //---Variables of the second part---//

  int actualStripADC = 0;
  int previousStripADC = 0;


  int greadient = 0;
  int maximum_value=0;
  int high_maximun_cluster = 1025 + 1024;
  int number_good_minimum = 0;
  int first_gradient = 0;
  int strip_first_gradient = 0;
  int ADC_start_point_cluster_pw = 0;
  int auxiliary_end_cluster = 0;
  int first_start_cluster_strip = 0;
  int first_start_cluster_ADC = 0;
  bool isAuxiliary_Minimum = false;
  bool isPossible_wrong_minimum = false;
  bool isMax =false;

  //----------SECOND PART: CLUSTER FINDING--------//

  for(uint16_t strip=0; strip < singleAPVdigi.size(); ++strip){
    if (strip == 0) {
      actualStripADC = singleAPVdigi[strip];
      if (abs(singleAPVdigi[strip]-singleAPVdigi[strip+1])>gradient_threshold_){
        isFirstStrip=true;
        isMinimumAndNoMax=true;
        discontinuities.insert(discontinuities.end(), std::pair<int, int >(strip, actualStripADC));
      } else if (actualStripADC > (gradient_threshold_+1024)) {
          discontinuities.insert(discontinuities.end(), std::pair<uint16_t, int16_t >(strip, 0+1024));
          isMinimumAndNoMax=true;
          first_start_cluster_strip=strip;
          first_start_cluster_ADC=1024;
      }

    } else if (strip>0) {
      previousStripADC = actualStripADC;
      actualStripADC = singleAPVdigi[strip];
      greadient = actualStripADC - previousStripADC;

      if (((greadient> gradient_threshold_)&&(isMax==false)&&(isMinimumAndNoMax==false))){
        isMax=false;
        if (((abs(maximum_value - previousStripADC) < (2*gradient_threshold_))&&discontinuities.size()>1)){ // agregar el || del tamano de cluster                                      //para verificar que el ruido no interfiera y se detecten falsos hits
                isPossible_wrong_minimum=true;
                itdiscontinuities=discontinuities.end();
                --itdiscontinuities;
                if(discontinuities.size()>1){
                        --itdiscontinuities;
                }
                strip_first_gradient= itdiscontinuities->first;
                ADC_start_point_cluster_pw= itdiscontinuities->second;
                first_gradient = abs(ADC_start_point_cluster_pw - singleAPVdigi[strip_first_gradient+1]);
                ++itdiscontinuities;
                discontinuities.erase(itdiscontinuities);
                itdiscontinuities=discontinuities.end();
                --itdiscontinuities;
                discontinuities.erase(itdiscontinuities);
        }

        if ((discontinuities.size()%2==1)&&(!discontinuities.empty())){ //&&(no_minimo == 0)
                itdiscontinuities=discontinuities.end();
                --itdiscontinuities;
                discontinuities.erase(itdiscontinuities);
        }

        discontinuities.insert(discontinuities.end(), std::pair<uint16_t, int16_t >(strip-1, previousStripADC));
        isMinimumAndNoMax=true;
        maximum_value = 0;
        first_start_cluster_strip=strip -1;
        first_start_cluster_ADC=previousStripADC;

      } else if ((isMax==false)&&((actualStripADC-previousStripADC<0)&&isMinimumAndNoMax==true)){
              isMax=true;
              isMinimumAndNoMax=false;
              high_maximun_cluster = 1025 + 1024;
              if ((previousStripADC > maximum_value)&&(discontinuities.size()%2==1)) maximum_value = previousStripADC;
      }

      if ((isMax==true)&&(strip<126)){
        if (high_maximun_cluster>(abs(singleAPVdigi[strip+1]- actualStripADC))){
          high_maximun_cluster = singleAPVdigi[strip+1]- actualStripADC;
          auxiliary_end_cluster = strip+2;
        } else {
          auxiliary_end_cluster = 127;
        }
      }

      if ((isMax==true)&&((actualStripADC-previousStripADC)>=0)&&(size_window_>0)&&(strip<=(127-size_window_-1))) {
        number_good_minimum = 0;
        for (uint16_t wintry=0; wintry <= size_window_; wintry++){
          if (abs(singleAPVdigi[strip+wintry] - singleAPVdigi[strip+wintry+1])<=last_gradient_) ++number_good_minimum;
        }
        --number_good_minimum;

        if (size_window_!= number_good_minimum) {
          isMax=false;                // not Valid end Point
          isMinimumAndNoMax=true;
        }

      } else if ((isMax==true)&&(strip > (127-size_window_-1))) {
        isMax=true;
        isAuxiliary_Minimum=true;//for minimums after strip 127-SW-1
      }


      if (!discontinuities.empty()){
        itdiscontinuities=discontinuities.end();
        --itdiscontinuities;
      }

      if ((isMax==true)&&(actualStripADC<=first_start_cluster_ADC)) {

        if ((abs(first_start_cluster_ADC - singleAPVdigi[strip+2])>first_gradient)&&(isPossible_wrong_minimum==true)) {
          discontinuities.erase(itdiscontinuities);
          discontinuities.insert(discontinuities.end(), std::pair<int, int >(strip_first_gradient, ADC_start_point_cluster_pw));
          discontinuities.insert(discontinuities.end(), std::pair<int, int >(strip, ADC_start_point_cluster_pw));//????
        } else {
          if ((discontinuities.size()%2==0)&&(discontinuities.size()>1)){
            itdiscontinuities=discontinuities.end();
            --itdiscontinuities;
            discontinuities.erase(itdiscontinuities);
          }
          discontinuities.insert(discontinuities.end(), std::pair<int, int >(strip, actualStripADC));
        }
        isMax=false;
        ADC_start_point_cluster_pw=0;
        isPossible_wrong_minimum=false;
        strip_first_gradient=0;
        first_start_cluster_strip=0;
        first_start_cluster_ADC=0;
      }

      if ((isMax==true)&&((actualStripADC-previousStripADC)>=0)&&(isAuxiliary_Minimum==false)){     //For the end Poit when strip >127-SW-1
        if ((abs(first_start_cluster_ADC - singleAPVdigi[strip+1])>first_gradient)&&(isPossible_wrong_minimum==true)) {
          discontinuities.erase(itdiscontinuities);
          discontinuities.insert(discontinuities.end(), std::pair<int, int >(strip_first_gradient, ADC_start_point_cluster_pw));
        }

        if ((discontinuities.size()%2==0)&&(discontinuities.size()>1)){
          itdiscontinuities=discontinuities.end();
          --itdiscontinuities;
          discontinuities.erase(itdiscontinuities);
        }
        discontinuities.insert(discontinuities.end(), std::pair<int, int >(strip-1, previousStripADC));
        isMax=false;
        ADC_start_point_cluster_pw=0;
        isPossible_wrong_minimum=false;
        strip_first_gradient=0;
        first_start_cluster_strip=0;
        first_start_cluster_ADC=0;
      }
    }
  }

  //----------THIRD PART reconstruction of the event without baseline-------//

  if(!discontinuities.empty()){
    if ((first_start_cluster_strip)==127-1) discontinuities.insert(discontinuities.end(), std::pair<int, int >(127, first_start_cluster_ADC));
    if ((isMax==true)&&(isAuxiliary_Minimum==true)) discontinuities.insert(discontinuities.end(), std::pair<int, int >(auxiliary_end_cluster, first_start_cluster_ADC));
  }

  if (isFirstStrip==true){
    itdiscontinuities=discontinuities.begin();
    ++itdiscontinuities;
    ++itdiscontinuities;
    //int firstStrip= itdiscontinuities->first;
    int firstADC= itdiscontinuities->second;
    --itdiscontinuities;
    itdiscontinuities->second=firstADC;
    --itdiscontinuities;
    itdiscontinuities->second=firstADC;
    isFirstStrip=false;
  }

  if(!discontinuities.empty()){
    itdiscontinuities=discontinuities.begin();
    uint16_t firstStrip  = itdiscontinuities->first;
    int16_t firstADC = itdiscontinuities->second;
    ++itdiscontinuities;
    uint16_t lastStrip  = itdiscontinuities->first;
    int16_t secondADC = itdiscontinuities->second;
    ++itdiscontinuities;

    for(uint16_t strip=0; strip < 128; ++strip){
      if (strip > lastStrip&&itdiscontinuities!=discontinuities.end()){
        firstStrip  = itdiscontinuities->first;
        firstADC = itdiscontinuities->second;
        ++itdiscontinuities;
        lastStrip  = itdiscontinuities->first;
        secondADC = itdiscontinuities->second;
        ++itdiscontinuities;
      }

      if ((firstStrip <= strip) && (strip <= lastStrip) && (0 < (singleAPVdigi[strip]- firstADC - ((secondADC-firstADC)/(lastStrip-firstStrip))*(strip-firstStrip)-gradient_threshold_))){
        digis[(APVn-firstAPV)*128+strip]= singleAPVdigi[strip]- firstADC - (((secondADC-firstADC)/(lastStrip-firstStrip))*(strip-firstStrip));
std::cout << "no baseline " << digis[(APVn-firstAPV)*128+strip] << std::endl;
      } else {
        digis[(APVn-firstAPV)*128+strip]=0;
      }
    }
  }
}
