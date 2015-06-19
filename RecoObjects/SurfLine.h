////////////////////////////////////////////////////////////////////////
///
/// \file   SurfLine.h
///
/// \brief  Base class for Kalman filter line surfaces.
///
/// \author H. Greenlee 
///
/// This class acts as an intermediate layer between abstract surfaces
/// (base class Surface), and concrete line surfaces (like SurfYZLine).
/// It does not include any data members of its own.  It guarantees the
/// existence of a right-handed local Cartesian coordinate system (u,v,w),
/// in which this line surface corresponds to the v-axis.  A track is 
/// considered to be at this type of surface when it is at its closest
/// approach to the v-axis.
///
/// The track parameters for a line surface are (r, v, phi, eta, q/p).
///
/// r = Signed impoact parameter.  Absolute value of r is the perpendicular
///     distance of the track to the v-axis at the point of closest 
///     approach to v-axis.  Sign of r matches sign of L_v (v projection
///     of angular momentum).
/// v = V-coordinate of track at point of closest approach to v-axis.
/// phi = Direction of track in u-w plane (phi = arctan(w/u)).
/// eta = Pseudorapidity with respect to v-axis.
/// q/p or 1/p = Inverse momentum.
///
/// In terms of these parameters, the point of closest approach to the
/// v-axis is
///
/// u = -r sin(phi)
/// v = v
/// w = r cos(phi)
///
/// The unit direction vector is
///
/// du/ds = cos(phi) sech(eta)
/// dv/ds = tanh(eta)
/// dw/ds = sin(phi) sech(eta)
///
///
////////////////////////////////////////////////////////////////////////

#ifndef SURFLINE_H
#define SURFLINE_H

#include <ostream>
#include "RecoObjects/Surface.h"

namespace trkf {

  class SurfLine : public Surface
  {
  public:

    /// Default constructor.
    SurfLine();

    /// Destructor.
    virtual ~SurfLine();

    // Overrides.

    /// Get pointing error of track.
    double PointingError(const TrackVector& vec, const TrackError& err) const;

    /// Get starting error matrix for Kalman filter.
    void getStartingError(TrackError& err) const;
  };
}

#endif
