#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ModuleFactory.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "RecoTracker/LST/interface/LSTModulesDev.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"

// LST includes
#include <SDL/ModuleMethods.h>
#include <SDL/LST.h>

namespace ALPAKA_ACCELERATOR_NAMESPACE {
  
class LSTModulesDevESProducer : public ESProducer {
public:
  LSTModulesDevESProducer(const edm::ParameterSet &iConfig);

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

  std::optional<SDL::modulesBuffer<alpaka_common::DevHost>> produce(const TrackerRecoGeometryRecord &iRecord);

private:
  const std::string txtFile_;
};

LSTModulesDevESProducer::LSTModulesDevESProducer(const edm::ParameterSet &iConfig)
  : ESProducer(iConfig), txtFile_{iConfig.getParameter<edm::FileInPath>("txt").fullPath()}
{
  setWhatProduced(this, iConfig.getParameter<std::string>("ComponentName"));
}

void LSTModulesDevESProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("ComponentName", "")->setComment("Product label");
  desc.add<edm::FileInPath>("txt", edm::FileInPath())
      ->setComment("Path to the txt file for the module map parameters");
  descriptions.addWithDefaultLabel(desc);
}

  std::optional<SDL::modulesBuffer<alpaka_common::DevHost>> LSTModulesDevESProducer::produce(const TrackerRecoGeometryRecord &iRecord) {
    // write directly to SDL : FIXME : SHOULD NOT HAPPEN HERE
    SDL::modulesBuffer<alpaka_common::DevHost> modules(cms::alpakatools::host());
    alpaka::QueueCpuBlocking queue(cms::alpakatools::host());
    SDL::LST::loadMaps();
    SDL::loadModulesFromFile(&modules, SDL::nModules, SDL::nLowerModules, *SDL::pixelMapping, queue, txtFile_.c_str());
    return modules;
}

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(LSTModulesDevESProducer);
