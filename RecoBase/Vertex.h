////////////////////////////////////////////////////////////////////////////
// \version $Id: Vertex.h,v 1.4 2010/06/10 16:21:31 antonm Exp $
//
// \brief Definition of vertex object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////

#ifndef RB_VERTEX_H
#define RB_VERTEX_H

#ifndef __GCCXML__
#include <iosfwd>

#include "SimpleTypesAndConstants/PhysicalConstants.h"
#endif

namespace recob {

  class Vertex  {

  public:

    Vertex();  // Default constructor

  private:

    double fXYZ[3];    ///< location of vertex
    int    fID;        ///< id number for vertex

#ifndef __GCCXML__
  public:

    explicit  Vertex(double *xyz,
		     int     id=util::kBogusI);
    void      XYZ(double *xyz) const;
    int ID()                   const;

    friend bool          operator <   (const Vertex & a, const Vertex & b);
    friend std::ostream& operator <<  (std::ostream& o,  const Vertex & a);

#endif

  };
}

#ifndef __GCCXML__

inline int recob::Vertex::ID() const { return fID; }

#endif 
#endif // RB_VERTEX_H
