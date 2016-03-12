#include "TotemAlignment/RPTrackBased/interface/AlignmentTask.h"
#include "TotemAlignment/RPTrackBased/interface/AlignmentConstraint.h"
#include "TotemAlignment/RPTrackBased/interface/AlignmentGeometry.h"
#include "TotemAlignment/RPTrackBased/interface/LocalTrackFitter.h"
#include "TotemAlignment/RPTrackBased/interface/IdealResult.h"
#include "TotemAlignment/RPTrackBased/interface/JanAlignmentAlgorithm.h"
#include "TotemAlignment/RPTrackBased/interface/MillepedeAlgorithm.h"

#include <TVectorD.h>
#include <TGraph.h>
#include <TH1D.h>

namespace {
  namespace {
    AlignmentTask at;

    AlignmentConstraint ac;
	std::map<unsigned int, TVectorD> muitvd;

	std::vector<AlignmentConstraint> vac;

	DetGeometry dg;
    AlignmentGeometry ag;

	std::map<unsigned int, DetGeometry> muidg;
	std::set<unsigned int> sui;

    LocalTrackFitter ltf;
    IdealResult ir;
	
	SingularMode sm;
	std::vector<SingularMode> vsm;

    JanAlignmentAlgorithm jaa;
	JanAlignmentAlgorithm::ScatterPlot jaasp;
	JanAlignmentAlgorithm::DetStat jaads;
	std::map<unsigned int, JanAlignmentAlgorithm::DetStat> muids;
	std::vector<TH1D*> vth;
    std::vector<TGraph*> vtg;  
    std::map< std::set<unsigned int>, JanAlignmentAlgorithm::ScatterPlot> msuisp;

    MillepedeAlgorithm ma;
  }
}
