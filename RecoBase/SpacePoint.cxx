////////////////////////////////////////////////////////////////////////////
//
// Implementation of SpacePoint class for LArSoft
//
// msoderbe@syr.edu
//
////////////////////////////////////////////////////////////////////////////

#include "RecoBase/SpacePoint.h"

#include <iomanip>
#include <iostream>

namespace recob{

  //----------------------------------------------------------------------
  SpacePoint::SpacePoint() :
    fID(-1),
    fChisq(0.)
  {
    for(int i=0; i<6; ++i)
      fErrXYZ[i] = 0.;
  }

  //----------------------------------------------------------------------
  SpacePoint::SpacePoint(double *xyz,
       double *err,
       double  chisq,
       int    id)
    : fID(id)
    , fChisq(chisq)
  {
    for(int i = 0; i < 3; ++i) fXYZ[i]    = xyz[i];
    for(int i = 0; i < 6; ++i) fErrXYZ[i] = err[i];
  }

  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& o, const SpacePoint & a)
  {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << " SpacePoint ID " << std::setw(5) << std::right << a.ID()
      << " (X,Y,Z) = ("    << std::setw(5) << std::right << a.XYZ()[0]
      << " , "             << std::setw(5) << std::right << a.XYZ()[1]
      << " , "             << std::setw(5) << std::right << a.XYZ()[2]
      << ")" ;

    return o;
  }


  //----------------------------------------------------------------------
  // < operator.
  //
  bool operator < (const SpacePoint & a, const SpacePoint & b)
  {
    if(a.ID() != b. ID())
      return a.ID()<b.ID();

    return false; //They are equal

  }

}

