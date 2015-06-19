///////////////////////////////////////////////////////////////////////
///
/// \file   SurfLine.cxx
///
/// \brief  Base class for Kalman filter planar surfaces.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "RecoObjects/SurfLine.h"

namespace trkf {

  /// Default constructor.
  SurfLine::SurfLine()
  {}

  /// Destructor.
  SurfLine::~SurfLine()
  {}

  /// Get pointing error of track.
  ///
  /// Arguments:
  ///
  /// vec - Track parameters.
  /// err - Track error matrix.
  ///
  /// Returns: Pointing error.
  ///
  /// This method calculates the track pointing error based on the
  /// slope track paramers and errors (parameters 2 and 3).
  ///
  double SurfLine::PointingError(const TrackVector& vec, const TrackError& err) const
  { 
    // Get slope parameters and error matrix.

    double phi = vec(2);
    double eta = vec(3);
    double epp = err(2, 2);
    double ehh = err(3, 3);
    double ehp = err(3, 2);

    // Calculate error matrix of pointing unit vector in some coordinate system.

    double sh = 1./std::cosh(eta);  // sech(eta)
    double sh2 = sh*sh;
    double sh3 = sh*sh2;
    double sh4 = sh*sh3;

    double th = std::tanh(eta);
    double th2 = th*th;

    double cphi = std::cos(phi);
    double cphi2 = cphi*cphi;

    double sphi = std::sin(phi);
    double sphi2 = sphi*sphi;

    double vxx = sh2*th2*cphi2 * ehh + sh2*sphi2 * epp + 2.*sh2*th*sphi*cphi * ehp;
    double vyy = sh2*th2*sphi2 * ehh + sh2*cphi2 * epp - 2.*sh2*th*sphi*cphi * ehp;
    double vzz = sh4 * epp;

    double vxy = sh2*th2*sphi*cphi * ehh - 
    double vyz = ( xp*xp*yp * exx - yp*(1. + xp*xp) * eyy - xp*(1. + xp*xp - yp*yp) * exy ) / den3;
    double vxz = ( -xp*(1. + yp*yp) * exx + xp*yp*yp * eyy - yp*(1. - xp*xp + yp*yp) * exy ) / den3;

    // Calculate square root of the largest eigenvalue of error matrix.

    double ddd = sqrt(vxx*vxx + vyy*vyy + vzz*vzz
		      - 2.*vxx*vyy - 2.*vxx*vzz - 2.*vyy*vzz
		      + 4.*vxy*vxy + 4.*vyz*vyz + 4.*vxz*vxz);
    double lambda = sqrt(0.5 * ( vxx + vyy + vzz + ddd));

    return lambda;
  }

  /// Get starting error matrix for Kalman filter.
  ///
  /// Arguments:
  ///
  /// err - Error matrix.
  ///
  void SurfLine::getStartingError(TrackError& err) const {
    err.resize(5, false);
    err.clear();
    err(0, 0) = 1000.;
    err(1, 1) = 1000.;
    err(2, 2) = 1000.;
    err(3, 3) = 1.;
    err(4, 4) = 10.;
  }

} // end namespace trkf
