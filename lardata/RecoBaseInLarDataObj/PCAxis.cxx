////////////////////////////////////////////////////////////////////////////
// \version $Id: PCAxis.cxx,v 1.5 2010/02/15 20:32:46 usher Exp $
//
// \file  PCAxis.cxx
// \brief Definition of Principal Components Axis object for LArSoft
//
// \author usher@slac.stanford.edu
//
// \see   PCAxis.h
//
////////////////////////////////////////////////////////////////////////////

#include "lardataobj/RecoBase/PCAxis.h"

#include <iomanip>
#include <iostream>

namespace recob
{

//----------------------------------------------------------------------
PCAxis::PCAxis()
{
}

//----------------------------------------------------------------------
PCAxis::PCAxis(bool                               ok,
               int                                nHits,
               const double*                      eigenValues,
               const recob::PCAxis::EigenVectors& eigenVecs,
               const double*                      avePos,
               const double                       aveHitDoca,
               size_t                             id) :
  fSvdOK(ok),
  fNumHitsUsed(nHits),
  fEigenVectors(eigenVecs),
  fAveHitDoca(aveHitDoca),
  fID(id)
{
    fEigenValues[0]     = eigenValues[0];
    fEigenValues[1]     = eigenValues[1];
    fEigenValues[2]     = eigenValues[2];
    fAvePosition[0]     = avePos[0];
    fAvePosition[1]     = avePos[1];
    fAvePosition[2]     = avePos[2];
}
  
//----------------------------------------------------------------------
// ostream operator.
//
std::ostream& operator<< (std::ostream & o, const PCAxis & a)
{
    if (a.fSvdOK)
    {
        o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        o << " PCAxis ID " << a.fID << " run with " << a.fNumHitsUsed << " space points" << std::endl;
        o << "   - center position: " << std::setw(6) << a.fAvePosition[0] << ", " << a.fAvePosition[1] << ", " << a.fAvePosition[2] << std::endl;
        o << "   - eigen values: " << std::setw(8) << std::right << a.fEigenValues[0] << ", "
            << a.fEigenValues[1] << ", " << a.fEigenValues[2] << std::endl;
        o << "   - average doca: " << a.fAveHitDoca << std::endl;
        o << "   - Principle axis: " << std::setw(7) << std::setprecision(4) << a.fEigenVectors[0][0] << ", " << a.fEigenVectors[0][1] << ", " << a.fEigenVectors[0][2] << std::endl;
        o << "   - second axis:    " << std::setw(7) << std::setprecision(4) << a.fEigenVectors[1][0] << ", " << a.fEigenVectors[1][1] << ", " << a.fEigenVectors[1][2] << std::endl;
        o << "   - third axis:     " << std::setw(7) << std::setprecision(4) << a.fEigenVectors[2][0] << ", " << a.fEigenVectors[2][1] << ", " << a.fEigenVectors[2][2] << std::endl;
    }
    else
        o << " Principal Components Axis is not valid" << std::endl;
    
    return o;
}

//----------------------------------------------------------------------------
// comparison operator
//
bool operator < (const PCAxis& a, const PCAxis& b)
{
  if(a.getID() != b. getID())
    return a.getID() < b.getID();

  return false; //They are equal
}

} // End of namespace

