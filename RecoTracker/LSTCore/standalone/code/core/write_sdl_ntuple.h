#ifndef write_sdl_ntuple_h
#define write_sdl_ntuple_h

#include <iostream>
#include <tuple>

#include "SDLMath.h"
#include "Event.h"

#include "AnalysisConfig.h"
#include "trkCore.h"
#include "AccessHelper.h"

using SDLEvent = SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Acc3D>;

// Common
void createOutputBranches();
void createRequiredOutputBranches();
void createOptionalOutputBranches();
void createGnnNtupleBranches();

void fillOutputBranches(SDLEvent* event);
void setOutputBranches(SDLEvent* event);
void setOptionalOutputBranches(SDLEvent* event);
void setPixelQuintupletOutputBranches(SDLEvent* event);
void setQuintupletOutputBranches(SDLEvent* event);
void setPixelTripletOutputBranches(SDLEvent* event);
void setGnnNtupleBranches(SDLEvent* event);
void setGnnNtupleMiniDoublet(SDLEvent* event, unsigned int MD);

std::tuple<int, float, float, float, int, std::vector<int>> parseTrackCandidate(
    SDLEvent* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepT5(
    SDLEvent* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepT3(
    SDLEvent* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parseT5(
    SDLEvent* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepLS(
    SDLEvent* event, unsigned int);

// Print multiplicities
void printMiniDoubletMultiplicities(SDLEvent* event);
void printHitMultiplicities(SDLEvent* event);

// Print objects (GPU)
void printAllObjects(SDLEvent* event);
void printpT4s(SDLEvent* event);
void printMDs(SDLEvent* event);
void printLSs(SDLEvent* event);
void printpLSs(SDLEvent* event);
void printT3s(SDLEvent* event);
void printT4s(SDLEvent* event);
void printTCs(SDLEvent* event);

// Print anomalous multiplicities
void debugPrintOutlierMultiplicities(SDLEvent* event);

#endif
