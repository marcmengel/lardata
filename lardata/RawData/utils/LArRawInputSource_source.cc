// ======================================================================
//
// LArRawInputSource_source.cc
//
// ======================================================================

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "lardata/RawData/utils/LArRawInputDriver.h"

DEFINE_ART_INPUT_SOURCE(art::Source<lris::LArRawInputDriver>)
