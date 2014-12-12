#ifndef MCTRACK_CXX
#define MCTRACK_CXX

#include "MCTrack.h"

namespace sim {

  void MCTrack::Clear()
  {
    std::vector<MCStep>::clear();

    fOrigin  = simb::kUnknown;
    fProcess = "";
    fPDGCode = kINVALID_INT;
    fTrackID = kINVALID_UINT;

    fMotherPDGCode = kINVALID_INT;
    fMotherTrackID = kINVALID_UINT;
    fMotherProcess = "";

    fAncestorPDGCode = kINVALID_INT;
    fAncestorTrackID = kINVALID_UINT;
    fAncestorProcess = "";

    TLorentzVector invalid(kINVALID_DOUBLE,
			   kINVALID_DOUBLE,
			   kINVALID_DOUBLE,
			   kINVALID_DOUBLE);

    MCStep invalid_step(invalid,invalid);

    fStart = invalid_step;
    fEnd   = invalid_step;

    fMotherStart = invalid_step;
    fMotherEnd   = invalid_step;
    
    fAncestorStart = invalid_step;
    fAncestorEnd   = invalid_step;

  }
}
#endif
  
