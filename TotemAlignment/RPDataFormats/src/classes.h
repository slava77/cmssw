#include "DataFormats/Common/interface/Wrapper.h"

#include "TotemAlignment/RPDataFormats/interface/LocalTrackFit.h"
#include "TotemAlignment/RPDataFormats/interface/RPAlignmentCorrection.h"
#include "TotemAlignment/RPDataFormats/interface/RPAlignmentCorrections.h"

namespace {
  namespace {
	LocalTrackFit ltf;
	edm::Wrapper<LocalTrackFit> wltf;

	RPAlignmentCorrection ac;
	RPAlignmentCorrections acs;
  }
}
