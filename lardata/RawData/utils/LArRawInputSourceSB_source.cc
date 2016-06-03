// ======================================================================
//
// LArRawInputSource_plugin.cc
//
// ======================================================================

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "lardata/RawData/utils/LArRawInputDriverShortBo.h"

namespace lris {
  typedef art::Source<LArRawInputDriverShortBo> LArRawInputSourceSB;
}

DEFINE_ART_INPUT_SOURCE(lris::LArRawInputSourceSB)
