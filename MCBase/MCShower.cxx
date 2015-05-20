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

    fOrigin  = simb::kUnknown;

    fPDGCode = kINVALID_INT;
    fTrackID = kINVALID_UINT;
    fProcess = "";
    fStart   = invalid_step;
    fEnd     = invalid_step;

    fMotherPDGCode = kINVALID_INT;
    fMotherTrackID = kINVALID_UINT;
    fMotherProcess = "";
    fMotherStart   = invalid_step;
    fMotherEnd     = invalid_step;

    fAncestorPDGCode = kINVALID_INT;
    fAncestorTrackID = kINVALID_UINT;
    fAncestorProcess = "";
    fAncestorStart   = invalid_step;
    fAncestorEnd     = invalid_step;

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
