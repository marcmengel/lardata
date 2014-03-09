////////////////////////////////////////////////////////////////////////////
// \version $Id: Shower.h,v 1.2 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of shower object for LArSoft
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////////
#ifndef SHOWER_H
#define SHOWER_H

#ifndef __GCCXML__
#include <iosfwd>
#include "SimpleTypesAndConstants/PhysicalConstants.h"
#endif

#include "TVector3.h"

namespace recob {

  class Shower {

  public:

    Shower();  ///Default constructor

    private:

    int      fID;
    TVector3 fDCosStart;             ///< direction cosines at start of shower
    TVector3 fSigmaDCosStart;        ///< uncertainting on initial direction cosines
    double   fMaxTransverseWidth[2]; ///< maximum width of the prong in the x(0) and y(0) directions
    double   fDistanceMaxWidth;      ///< distance from the start of the prong to its maximum width
    double   fTotalCharge;           ///< total charge of hits in the shower

#ifndef __GCCXML__

  public:

    Shower(double *dcosVtx,
	   double *dcosVtxErr,
	   double *maxTransWidth,
	   double  distanceMaxWidth,
	   double  totalCharge,
	   int     id=util::kBogusI);

    double          TotalCharge()  const;
    int             ID()           const;
    TVector3 const& Direction()    const;
    TVector3 const& DirectionErr() const;
    double          MaxTransverseX();
    double          MaxTransverseY();
    double          DistanceMaxWidth() const;

    friend std::ostream& operator << (std::ostream& stream, Shower const& a);

    friend bool          operator <   (const Shower & a, const Shower & b);

#endif

  };
}

#ifndef __GCCXML__

inline double          recob::Shower::TotalCharge()  const { return fTotalCharge;    }
inline int             recob::Shower::ID()           const { return fID;             }
inline TVector3 const& recob::Shower::Direction()    const { return fDCosStart;      }
inline TVector3 const& recob::Shower::DirectionErr() const { return fSigmaDCosStart; }
inline double          recob::Shower::MaxTransverseX()     { return fMaxTransverseWidth[0]; }
inline double          recob::Shower::MaxTransverseY()     { return fMaxTransverseWidth[1]; }
inline double          recob::Shower::DistanceMaxWidth() const { return fDistanceMaxWidth;      }

#endif

#endif // SHOWER_H
