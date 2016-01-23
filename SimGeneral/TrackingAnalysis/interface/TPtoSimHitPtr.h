#ifndef TrackingAnalysis_TPtoSimHitPtr_h
#define TrackingAnalysis_TPtoSimHitPtr_h

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

struct TPtoSimHitPtr {
  TPtoSimHitPtr() {hit = nullptr; tp = TrackingParticleRef();}
  TPtoSimHitPtr(const TrackingParticleRef& p, const PSimHit* h) {hit = h; tp = p;}
  static bool greater(const TPtoSimHitPtr& i, const TPtoSimHitPtr& j){return i.tp.key() > j.tp.key();}
  TrackingParticleRef tp;
  const PSimHit* hit;
};

#endif
