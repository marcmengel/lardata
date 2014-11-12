#ifndef MCTRACK_CXX
#define MCTRACK_CXX

#include "MCTrack.h"

namespace sim {

  void MCTrack::Clear()
  {
    std::vector<MCStep>::clear();

    fOrigin  = simb::kUnknown;
    fProcess = "";
    fPDGCode         = kINVALID_INT;
    fG4TrackID       = kINVALID_UINT;

    fMotherPDGCode   = kINVALID_INT;
    fMotherG4TrackID = kINVALID_UINT;
    fMotherProcess   = "";

    fAncestorPDGCode   = kINVALID_INT;
    fAncestorG4TrackID = kINVALID_UINT;
    fAncestorProcess   = "";

    TLorentzVector invalid(kINVALID_DOUBLE,
			   kINVALID_DOUBLE,
			   kINVALID_DOUBLE,
			   kINVALID_DOUBLE);

    MCStep invalid_step(invalid,invalid);

    fG4Start = invalid_step;
    fG4End   = invalid_step;

    fMotherG4Start = invalid_step;
    fMotherG4End   = invalid_step;
    
    fAncestorG4Start = invalid_step;
    fAncestorG4End   = invalid_step;

  }
}
#endif
  
