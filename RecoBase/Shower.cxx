////////////////////////////////////////////////////////////////////////////
// \version $Id: Shower.cxx,v 1.2 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of shower object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#include "RecoBase/Shower.h"

#include <iomanip>
#include <iostream>

namespace recob{

  //----------------------------------------------------------------------
  Shower::Shower()
  {
  }

  //----------------------------------------------------------------------
  Shower::Shower(double *dcosVtx,
     double *dcosVtxErr,
     double *maxTransverseWidth,
     double  distanceMaxWidth,
     double  totalCharge,
     int     id)
    : fID(id)
    , fDCosStart(TVector3(dcosVtx[0], dcosVtx[1], dcosVtx[2]))
    , fSigmaDCosStart(TVector3(dcosVtxErr[0], dcosVtxErr[1], dcosVtxErr[2]))
    , fDistanceMaxWidth(distanceMaxWidth)
    , fTotalCharge(totalCharge)
  {
    for(int i = 0; i < 2; ++i) fMaxTransverseWidth[i] = maxTransverseWidth[i];
  }

  //----------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, Shower const& a)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(3);
    o << " Shower ID "        << std::setw(4)  << std::right << a.ID();
    o << " Charge    "        << std::setw(4)  << std::right << a.TotalCharge();

    return o;
  }

  //----------------------------------------------------------------------
  // < operator.
  //
  bool operator < (const Shower & a, const Shower & b)
  {
    if(a.ID() != b. ID())
      return a.ID() < b.ID();

    return false; //They are equal
  }


}
