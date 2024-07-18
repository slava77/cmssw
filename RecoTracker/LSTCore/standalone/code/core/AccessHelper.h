#ifndef AccessHelper_h
#define AccessHelper_h

#include <vector>
#include <tuple>
#include "Event.h"

using SDLEvent = SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Acc3D>;

enum { kpT5 = 7, kpT3 = 5, kT5 = 4, kpLS = 8 };

// ----* Hit *----
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> convertHitsToHitIdxsAndHitTypes(
    SDLEvent* event, std::vector<unsigned int> hits);

// ----* pLS *----
std::vector<unsigned int> getPixelHitsFrompLS(SDLEvent* event,
                                              unsigned int pLS);
std::vector<unsigned int> getPixelHitIdxsFrompLS(SDLEvent* event,
                                                 unsigned int pLS);
std::vector<unsigned int> getPixelHitTypesFrompLS(SDLEvent* event,
                                                  unsigned int pLS);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFrompLS(
    SDLEvent* event, unsigned pLS);

// ----* MD *----
std::vector<unsigned int> getHitsFromMD(SDLEvent* event, unsigned int MD);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromMD(
    SDLEvent* event, unsigned MD);

// ----* LS *----
std::vector<unsigned int> getMDsFromLS(SDLEvent* event, unsigned int LS);
std::vector<unsigned int> getHitsFromLS(SDLEvent* event, unsigned int LS);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromLS(
    SDLEvent* event, unsigned LS);

// ----* T3 *----
std::vector<unsigned int> getLSsFromT3(SDLEvent* event, unsigned int T3);
std::vector<unsigned int> getMDsFromT3(SDLEvent* event, unsigned int T3);
std::vector<unsigned int> getHitsFromT3(SDLEvent* event, unsigned int T3);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromT3(
    SDLEvent* event, unsigned T3);

// ----* T5 *----
std::vector<unsigned int> getT3sFromT5(SDLEvent* event, unsigned int T5);
std::vector<unsigned int> getLSsFromT5(SDLEvent* event, unsigned int T5);
std::vector<unsigned int> getMDsFromT5(SDLEvent* event, unsigned int T5);
std::vector<unsigned int> getHitsFromT5(SDLEvent* event, unsigned int T5);
std::vector<unsigned int> getHitIdxsFromT5(SDLEvent* event, unsigned int T5);
std::vector<unsigned int> getHitTypesFromT5(SDLEvent* event, unsigned int T5);
std::vector<unsigned int> getModuleIdxsFromT5(SDLEvent* event, unsigned int T5);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromT5(
    SDLEvent* event, unsigned T5);

// ----* pT3 *----
unsigned int getPixelLSFrompT3(SDLEvent* event, unsigned int pT3);
unsigned int getT3FrompT3(SDLEvent* event, unsigned int pT3);
std::vector<unsigned int> getLSsFrompT3(SDLEvent* event, unsigned int pT3);
std::vector<unsigned int> getMDsFrompT3(SDLEvent* event, unsigned int pT3);
std::vector<unsigned int> getOuterTrackerHitsFrompT3(SDLEvent* event,
                                                     unsigned int pT3);
std::vector<unsigned int> getPixelHitsFrompT3(SDLEvent* event,
                                              unsigned int pT3);
std::vector<unsigned int> getHitsFrompT3(SDLEvent* event, unsigned int pT3);
std::vector<unsigned int> getHitIdxsFrompT3(SDLEvent* event, unsigned int pT3);
std::vector<unsigned int> getHitTypesFrompT3(SDLEvent* event, unsigned int pT3);
std::vector<unsigned int> getModuleIdxsFrompT3(SDLEvent* event,
                                               unsigned int pT3);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFrompT3(
    SDLEvent* event, unsigned pT3);

// ----* pT5 *----
unsigned int getPixelLSFrompT5(SDLEvent* event, unsigned int pT5);
unsigned int getT5FrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getT3sFrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getLSsFrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getMDsFrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getOuterTrackerHitsFrompT5(SDLEvent* event,
                                                     unsigned int pT5);
std::vector<unsigned int> getPixelHitsFrompT5(SDLEvent* event,
                                              unsigned int pT5);
std::vector<unsigned int> getHitsFrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getHitIdxsFrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getHitTypesFrompT5(SDLEvent* event, unsigned int pT5);
std::vector<unsigned int> getModuleIdxsFrompT5(SDLEvent* event,
                                               unsigned int pT5);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFrompT5(
    SDLEvent* event, unsigned pT5);

// ----* TC *----
std::vector<unsigned int> getLSsFromTC(SDLEvent* event, unsigned int TC);
std::vector<unsigned int> getHitsFromTC(SDLEvent* event, unsigned int TC);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromTC(
    SDLEvent* event, unsigned int TC);

#endif
