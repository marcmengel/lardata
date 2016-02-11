////////////////////////////////////////////////////////////////////////////
// \version $Id: Shower.cxx,v 1.2 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of shower object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#include "lardata/RecoBase/Shower.h"

#include <iomanip>
#include <iostream>

namespace recob{

  //----------------------------------------------------------------------
  Shower::Shower()
  {
  }

  //----------------------------------------------------------------------
  Shower::Shower(TVector3& dcosVtx,
	   TVector3& dcosVtxErr,
	   TVector3& xyz,
	   TVector3& xyzErr,
	   std::vector< double >  TotalEnergy,
	   std::vector< double >  TotalEnergyErr,
	   std::vector< double >  dEdx,
 	   std::vector< double >  dEdxErr,
	   int bestplane,
	   int     id)
    : fID(id)
    , fDCosStart(dcosVtx)
    , fSigmaDCosStart(dcosVtxErr)
    , fXYZstart(xyz)
    , fSigmaXYZstart(xyzErr)
    , fTotalEnergy(TotalEnergy)
    , fSigmaTotalEnergy(TotalEnergyErr)
    , fdEdx(dEdx)
    , fSigmadEdx(dEdxErr)
    , fBestPlane(bestplane)
  {
   
  }

  //----------------------------------------------------------------------
  std::ostream& operator<< (std::ostream& o, Shower const& a)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(3);
    o << " Shower ID "        << std::setw(4)  << std::right << a.ID();
    o << " Energy    "        << std::setw(4)  << std::right << a.Energy()[a.best_plane()];
    o << " dEdx    "         << std::setw(4)  << std::right << a.dEdx()[a.best_plane()];
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
