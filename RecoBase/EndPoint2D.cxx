////////////////////////////////////////////////////////////////////////////
// \version $Id: EndPoint2D.cxx,v 1.2 2010/06/19 22:20:12 spitz7 Exp $
//
// \brief Definition of vertex object for LArSoft
//
// \author spitz
//
////////////////////////////////////////////////////////////////////////////

#include "RecoBase/EndPoint2D.h"

#include <iomanip>
#include <iostream>

namespace recob{

  //----------------------------------------------------------------------
  EndPoint2D::EndPoint2D()
    : fDriftTime(-1.)
    , fWireID()
    , fID(-1)
    , fStrength(-1.)
    , fView(geo::kUnknown)
    , fTotalCharge(-999.)
  {
  }

  //----------------------------------------------------------------------
  EndPoint2D::EndPoint2D(double driftTime,
			 geo::WireID wireID,
			 double strength,
			 int id,
			 geo::View_t view,
			 double totalQ)
    : fDriftTime(driftTime)
    , fWireID(wireID)
    , fID(id)
    , fStrength(strength)
    , fView(view)
    , fTotalCharge(totalQ)
  {
  }

  //----------------------------------------------------------------------
  // ostream operator.
  //

  std::ostream& operator<< (std::ostream& o, const EndPoint2D& ep)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << "EndPoint2D ID "    << std::setw(5)  << std::right << ep.ID()
      << " WireID : \n"
      << "\tCryostat = "     << std::setw(7)  << std::right << ep.WireID().Cryostat
      << "\tTPC = "          << std::setw(7)  << std::right << ep.WireID().TPC
      << "\tPlane = "        << std::setw(7)  << std::right << ep.WireID().Plane
      << "\tWire Number = "  << std::setw(7)  << std::right << ep.WireID().Wire
      << " View = "          << std::setw(9)  << std::right << ep.View()
      << " Time = "          << std::setw(9)  << std::right << ep.DriftTime();

    return o;
  }


}
