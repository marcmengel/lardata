// ======================================================================
//
// LArRawInputSource_plugin.cc
//
// ======================================================================

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "lardata/RawData/utils/LArRawInputDriverLongBo.h"

namespace lris {
  typedef art::Source<LArRawInputDriverLongBo> LArRawInputSourceLB;
}

DEFINE_ART_INPUT_SOURCE(lris::LArRawInputSourceLB)
