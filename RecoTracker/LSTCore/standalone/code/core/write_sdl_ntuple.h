#ifndef write_sdl_ntuple_h
#define write_sdl_ntuple_h

#include <iostream>
#include <tuple>

#include "lstMath.h"
#include "Event.h"

#include "AnalysisConfig.h"
#include "trkCore.h"
#include "AccessHelper.h"

using lstEvent = lst::Event<ALPAKA_ACCELERATOR_NAMESPACE::Acc3D>;

// Common
void createOutputBranches();
void createRequiredOutputBranches();
void createOptionalOutputBranches();
void createGnnNtupleBranches();

void fillOutputBranches(lstEvent* event);
void setOutputBranches(lstEvent* event);
void setOptionalOutputBranches(lstEvent* event);
void setPixelQuintupletOutputBranches(lstEvent* event);
void setQuintupletOutputBranches(lstEvent* event);
void setPixelTripletOutputBranches(lstEvent* event);
void setGnnNtupleBranches(lstEvent* event);
void setGnnNtupleMiniDoublet(lstEvent* event, unsigned int MD);

std::tuple<int, float, float, float, int, std::vector<int>> parseTrackCandidate(lstEvent* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepT5(lstEvent* event,
                                                                                               unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepT3(lstEvent* event,
                                                                                               unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parseT5(lstEvent* event,
                                                                                              unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepLS(lstEvent* event,
                                                                                               unsigned int);

// Print multiplicities
void printMiniDoubletMultiplicities(lstEvent* event);
void printHitMultiplicities(lstEvent* event);

// Print objects (GPU)
void printAllObjects(lstEvent* event);
void printpT4s(lstEvent* event);
void printMDs(lstEvent* event);
void printLSs(lstEvent* event);
void printpLSs(lstEvent* event);
void printT3s(lstEvent* event);
void printT4s(lstEvent* event);
void printTCs(lstEvent* event);

// Print anomalous multiplicities
void debugPrintOutlierMultiplicities(lstEvent* event);

#endif
