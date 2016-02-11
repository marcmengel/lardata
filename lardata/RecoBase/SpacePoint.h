////////////////////////////////////////////////////////////////////////////
//
// Definition of SpacePoint class for LArSoft
//
// SpacePoints are 3D objects that contain pointers to Hits from multiple
// wireplanes that have been identified as matching.
//
// msoderbe@syr.edu
//
////////////////////////////////////////////////////////////////////////////

#ifndef SPACEPOINT_H
#define SPACEPOINT_H

#ifndef __GCCXML__
#include <iosfwd>

#include "larcore/SimpleTypesAndConstants/PhysicalConstants.h"
#endif

namespace recob {

  class SpacePoint {

  public:

    SpacePoint();  ///Default constructor

  private:
    int                        fID;        ///< SpacePoint ID
    double                     fXYZ[3];    ///< position of SpacePoint in xyz
    double                     fErrXYZ[6]; ///< Error matrix (triangular).
    double                     fChisq;     ///< Chisquare.

#ifndef __GCCXML__
  public:
    SpacePoint(double const*xyz,
	       double const*err,
	       double  chisq,
	       int     id=util::kBogusI);

    int                        ID()      const;
    const double*              XYZ()     const;
    const double*              ErrXYZ()  const;
    double                     Chisq()   const;

    friend std::ostream& operator << (std::ostream& o, const SpacePoint & a);
    friend bool          operator <  (const SpacePoint & a, const SpacePoint & b);

#endif

  };
}

#ifndef __GCCXML__

inline int           recob::SpacePoint::ID()      const { return fID;     }
inline const double* recob::SpacePoint::XYZ()     const { return fXYZ;    }
inline const double* recob::SpacePoint::ErrXYZ()  const { return fErrXYZ; }
inline double        recob::SpacePoint::Chisq()   const { return fChisq;  }

#endif

#endif //SPACEPOINT_H
