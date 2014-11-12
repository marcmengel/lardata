#ifndef MCSHOWER_CXX
#define MCSHOWER_CXX

#include "MCShower.h"

namespace sim {

  //-------------------------------------------------------------
  void MCShower::Clear()
  //-------------------------------------------------------------
  {
    TLorentzVector invalid(kINVALID_DOUBLE,
			   kINVALID_DOUBLE,
			   kINVALID_DOUBLE,
			   kINVALID_DOUBLE);
    MCStep invalid_step(invalid,invalid);

    fOrigin    = simb::kUnknown;

    fPDGCode   = kINVALID_INT;
    fG4TrackID = kINVALID_UINT;
    fProcess   = "";
    fG4Start   = invalid_step;
    fG4End     = invalid_step;

    fMotherPDGCode   = kINVALID_INT;
    fMotherG4TrackID = kINVALID_UINT;
    fMotherProcess   = "";
    fMotherG4Start   = invalid_step;
    fMotherG4End     = invalid_step;

    fAncestorPDGCode   = kINVALID_INT;
    fAncestorG4TrackID = kINVALID_UINT;
    fAncestorProcess   = "";
    fAncestorG4Start   = invalid_step;
    fAncestorG4End     = invalid_step;

    fDetProfile = invalid_step;
    
    fDaughterTrackID.clear();
    fPlaneCharge.clear();
  }

  //----------------------------------------------------
  double MCShower::Charge(const size_t plane) const
  //----------------------------------------------------
  {
    if(plane > fPlaneCharge.size()) {

      std::cerr<<"\033[93m"<<"No charge stored for plane: "<<plane<<"\033[00m"<<std::endl;
      return -1;

    }
      
    return fPlaneCharge[plane];
  }

}

#endif
