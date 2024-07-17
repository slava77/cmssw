#ifndef write_sdl_ntuple_h
#define write_sdl_ntuple_h

#include <iostream>
#include <tuple>

#include "SDLMath.h"
#include "Event.h"

#include "AnalysisConfig.h"
#include "trkCore.h"
#include "AccessHelper.h"

// Common
void createOutputBranches();
void createRequiredOutputBranches();
void createOptionalOutputBranches();
void createGnnNtupleBranches();

void fillOutputBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setOutputBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setOptionalOutputBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setPixelQuintupletOutputBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setQuintupletOutputBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setPixelTripletOutputBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setGnnNtupleBranches(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void setGnnNtupleMiniDoublet(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event, unsigned int MD);

std::tuple<int, float, float, float, int, std::vector<int>> parseTrackCandidate(
    SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepT5(
    SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepT3(
    SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parseT5(
    SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event, unsigned int);
std::tuple<float, float, float, std::vector<unsigned int>, std::vector<unsigned int>> parsepLS(
    SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event, unsigned int);

// Print multiplicities
void printMiniDoubletMultiplicities(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printHitMultiplicities(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);

// Print objects (GPU)
void printAllObjects(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printpT4s(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printMDs(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printLSs(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printpLSs(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printT3s(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printT4s(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);
void printTCs(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);

// Print anomalous multiplicities
void debugPrintOutlierMultiplicities(SDL::Event<ALPAKA_ACCELERATOR_NAMESPACE::Device>* event);

#endif
