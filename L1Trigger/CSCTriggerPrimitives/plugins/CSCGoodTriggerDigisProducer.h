#ifndef CSCTriggerPrimitives_CSCGoodTriggerDigisProducer_h
#define CSCTriggerPrimitives_CSCGoodTriggerDigisProducer_h

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/EDProducer.h>
#include <FWCore/Framework/interface/Event.h>
#include <FWCore/ParameterSet/interface/ParameterSet.h>
#include <FWCore/Utilities/interface/InputTag.h>
#include "CLHEP/Random/RandomEngine.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"

class CSCGoodTriggerDigisProducer : public edm::EDProducer
{
 public:
  explicit CSCGoodTriggerDigisProducer(const edm::ParameterSet&);
  ~CSCGoodTriggerDigisProducer();

  //virtual void beginRun(const edm::EventSetup& setup);
  virtual void produce(edm::Event&, const edm::EventSetup&);

 private:
  edm::InputTag compDigiProducer_;
  edm::InputTag wireDigiProducer_;
};

#endif
