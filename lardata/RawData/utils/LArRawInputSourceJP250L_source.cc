// ======================================================================
//
// LArRawInputSourceJP250L_source.cc
//
// ======================================================================

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "lardata/RawData/utils/LArRawInputDriverJP250L.h"

namespace lris {
  typedef art::Source<LArRawInputDriverJP250L> LArRawInputSourceJP250L;
}

DEFINE_ART_INPUT_SOURCE(lris::LArRawInputSourceJP250L)
