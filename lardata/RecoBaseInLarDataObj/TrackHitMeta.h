////////////////////////////////////////////////////////////////////////////////////
///
/// \file   TrackHitMeta.h
///
/// \brief  Class to keep data related to recob::Hit associated with recob::Track.
///
/// \author R. Sulej 
///
/// The purpose is to collect several variables that do not work well alone inside
/// track class and are related to 2D hits along the 3D trajectory. So in the first
/// place it is the hit index along the trajectory.
/// There is aldo dx associated to hit to help dE/dx calculations.
///
/// Please, add other variables that may fit here. One candidate is 3D position.
///
/// PLEASE, remember to add ***errors on values*** whenever this is possible to calculate.
///
////////////////////////////////////////////////////////////////////////////////////

#ifndef TRACKHITMET_H
#define TRACKHITMET_H

#ifndef __GCCXML__
#include <iosfwd>
#endif

// #include "TVector3.h"

namespace recob {

class TrackHitMeta
{
public:
	/// Default needed by ROOT.
	TrackHitMeta(void) { fIndex = 0; fDx = 0.0; }

	/// Constructor with initialization.
	TrackHitMeta(unsigned int idx, double dx = 0.0);

	/// Hit index along the track tracjectory.
	unsigned int Index(void) const { return fIndex; }

	/// Track section length associated with the 2D hit;
	/// i.e. half-dist to the next hit in the same plane plus
	/// half-dist to the preceding hit in the same plane.
	double Dx(void) const { return fDx; }

	/// Candidate to keep 3D trajectory point here instead of inside recob::Track
	//TVector3 const & Position3D(void) const { return fPosition3D; }

#ifndef __GCCXML__
public:
    friend std::ostream&  operator << (std::ostream & o, const TrackHitMeta & a);
    friend bool           operator <  (const TrackHitMeta & a, const TrackHitMeta & b);
#endif

private:
	unsigned int fIndex;
	double fDx;

	//TVector3 fPosition3D;
};

}

#endif
