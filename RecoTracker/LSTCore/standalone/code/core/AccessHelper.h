#ifndef AccessHelper_h
#define AccessHelper_h

#include <vector>
#include <tuple>
#include "Event.h"

using lstEvent = lst::Event<ALPAKA_ACCELERATOR_NAMESPACE::Acc3D>;

enum { kpT5 = 7, kpT3 = 5, kT5 = 4, kpLS = 8 };

// ----* Hit *----
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> convertHitsToHitIdxsAndHitTypes(
    lstEvent* event, std::vector<unsigned int> hits);

// ----* pLS *----
std::vector<unsigned int> getPixelHitsFrompLS(lstEvent* event, unsigned int pLS);
std::vector<unsigned int> getPixelHitIdxsFrompLS(lstEvent* event, unsigned int pLS);
std::vector<unsigned int> getPixelHitTypesFrompLS(lstEvent* event, unsigned int pLS);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFrompLS(lstEvent* event,
                                                                                              unsigned pLS);

// ----* MD *----
std::vector<unsigned int> getHitsFromMD(lstEvent* event, unsigned int MD);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromMD(lstEvent* event,
                                                                                             unsigned MD);

// ----* LS *----
std::vector<unsigned int> getMDsFromLS(lstEvent* event, unsigned int LS);
std::vector<unsigned int> getHitsFromLS(lstEvent* event, unsigned int LS);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromLS(lstEvent* event,
                                                                                             unsigned LS);

// ----* T3 *----
std::vector<unsigned int> getLSsFromT3(lstEvent* event, unsigned int T3);
std::vector<unsigned int> getMDsFromT3(lstEvent* event, unsigned int T3);
std::vector<unsigned int> getHitsFromT3(lstEvent* event, unsigned int T3);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromT3(lstEvent* event,
                                                                                             unsigned T3);

// ----* T5 *----
std::vector<unsigned int> getT3sFromT5(lstEvent* event, unsigned int T5);
std::vector<unsigned int> getLSsFromT5(lstEvent* event, unsigned int T5);
std::vector<unsigned int> getMDsFromT5(lstEvent* event, unsigned int T5);
std::vector<unsigned int> getHitsFromT5(lstEvent* event, unsigned int T5);
std::vector<unsigned int> getHitIdxsFromT5(lstEvent* event, unsigned int T5);
std::vector<unsigned int> getHitTypesFromT5(lstEvent* event, unsigned int T5);
std::vector<unsigned int> getModuleIdxsFromT5(lstEvent* event, unsigned int T5);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromT5(lstEvent* event,
                                                                                             unsigned T5);

// ----* pT3 *----
unsigned int getPixelLSFrompT3(lstEvent* event, unsigned int pT3);
unsigned int getT3FrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getLSsFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getMDsFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getOuterTrackerHitsFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getPixelHitsFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getHitsFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getHitIdxsFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getHitTypesFrompT3(lstEvent* event, unsigned int pT3);
std::vector<unsigned int> getModuleIdxsFrompT3(lstEvent* event, unsigned int pT3);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFrompT3(lstEvent* event,
                                                                                              unsigned pT3);

// ----* pT5 *----
unsigned int getPixelLSFrompT5(lstEvent* event, unsigned int pT5);
unsigned int getT5FrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getT3sFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getLSsFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getMDsFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getOuterTrackerHitsFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getPixelHitsFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getHitsFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getHitIdxsFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getHitTypesFrompT5(lstEvent* event, unsigned int pT5);
std::vector<unsigned int> getModuleIdxsFrompT5(lstEvent* event, unsigned int pT5);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFrompT5(lstEvent* event,
                                                                                              unsigned pT5);

// ----* TC *----
std::vector<unsigned int> getLSsFromTC(lstEvent* event, unsigned int TC);
std::vector<unsigned int> getHitsFromTC(lstEvent* event, unsigned int TC);
std::tuple<std::vector<unsigned int>, std::vector<unsigned int>> getHitIdxsAndHitTypesFromTC(lstEvent* event,
                                                                                             unsigned int TC);

#endif
