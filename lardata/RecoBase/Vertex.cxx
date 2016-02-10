////////////////////////////////////////////////////////////////////////////
// \version $Id: Vertex.cxx,v 1.4 2010/06/10 16:21:31 antonm Exp $
//
// \brief Definition of vertex object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "RecoBase/Vertex.h"

#include <iomanip>

namespace recob{

  //----------------------------------------------------------------------
  Vertex::Vertex()
  {

  }

  //----------------------------------------------------------------------
  Vertex::Vertex(double *xyz,
		 int id)
    : fID(id)
  {
    fXYZ[0] = xyz[0];
    fXYZ[1] = xyz[1];
    fXYZ[2] = xyz[2];
  }

  //----------------------------------------------------------------------
  void Vertex::XYZ(double *xyz) const
  {
    xyz[0] = fXYZ[0];
    xyz[1] = fXYZ[1];
    xyz[2] = fXYZ[2];

    return;
  }

  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& o, const Vertex & a)
  {
    o << std::setprecision(5);
    o << "Vertex ID    "  << a.fID << std::setw(5)
      << " (x,y,z)  = ("  << a.fXYZ[0] << "," << a.fXYZ[1] << ","
      << a.fXYZ[2] << ")";

    return o;
  }

  //----------------------------------------------------------------------
  // < operator.
  //
  bool operator < (const Vertex & a, const Vertex & b)
  {
    double xyza[3] = {0.};
    double xyzb[3] = {0.};
    a.XYZ(xyza);
    b.XYZ(xyzb);

    return xyza[2] < xyzb[2];

  }


}
