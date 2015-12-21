#include "lardata/MCBase/MCShower.h"

#include <stdexcept> // std::out_of_range
#include <string> // std::out_of_range

namespace sim {

  //-------------------------------------------------------------
  MCShower::~MCShower() = default;

  //-------------------------------------------------------------
  void MCShower::ClearData()
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
    fdQdx.clear();
  }

  //----------------------------------------------------
  double MCShower::Charge(size_t plane) const
  //----------------------------------------------------
  {
    if (plane >= fPlaneCharge.size())
      throw std::out_of_range("No charge stored for plane " + std::to_string(plane));
    return fPlaneCharge[plane];
  } // MCShower::Charge()

  //----------------------------------------------------
  double MCShower::dQdx(size_t plane) const
  //----------------------------------------------------
  {
    if (plane >= fdQdx.size())
      throw std::out_of_range("No dQ/dx stored for plane " + std::to_string(plane));
    return fdQdx[plane];
  } // MCShower::dQdx()

}
