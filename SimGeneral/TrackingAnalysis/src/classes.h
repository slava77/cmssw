#include "DataFormats/Common/interface/Wrapper.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimGeneral/TrackingAnalysis/interface/TPtoSimHitPtr.h"

namespace {
  struct dictionary {
    std::pair<TrackingParticleRef, TrackPSimHitRef> dummy13;
    edm::Wrapper<std::pair<TrackingParticleRef, TrackPSimHitRef> > dummy14;
    std::vector<std::pair<TrackingParticleRef, TrackPSimHitRef> > dummy07;
    edm::Wrapper<std::vector<std::pair<TrackingParticleRef, TrackPSimHitRef> > > dummy08;

    TPtoSimHitPtr dummy06p;
    std::vector<TPtoSimHitPtr> dummy07p;
    edm::Wrapper<std::vector<TPtoSimHitPtr> > dummy08p;
  };
}
