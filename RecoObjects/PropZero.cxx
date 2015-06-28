///////////////////////////////////////////////////////////////////////
///
/// \file   PropZero.cxx
///
/// \brief  Propagate zero distance between to dissimilar surfaces.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "RecoObjects/PropZero.h"
#include "RecoObjects/SurfYZLine.h"
#include "RecoObjects/SurfYZPlane.h"
#include "RecoObjects/SurfXYZPlane.h"
#include "cetlib/exception.h"

namespace trkf {

  /// Constructor.
  ///
  /// Arguments.
  ///
  /// max_dist - Maximum perpendicular distance from initial position to destination.
  ///
  PropZero::PropZero(double max_dist) :
    Propagator(0., false, std::shared_ptr<const Interactor>()),
    fMaxDist(max_dist)
  {}

  /// Destructor.
  PropZero::~PropZero()
  {}

  /// Propagate without error.
  /// Optionally return propagation matrix and noise matrix.
  /// Noise is always returned as zero.
  /// Propagation direction and dedx flags are ignored.
  /// In case of successful propagation, the propagation distance is always returned as zero.
  ///
  /// Arguments:
  ///
  /// trk   - Track to propagate.
  /// psurf - Destination surface.
  /// dir   - Propagation direction (FORWARD, BACKWARD, or UNKNOWN).
  /// doDedx - dE/dx enable/disable flag.
  /// prop_matrix - Pointer to optional propagation matrix.
  /// noise_matrix - Pointer to optional noise matrix.
  ///
  /// Returned value: propagation distance + success flag.
  ///
  boost::optional<double>
  PropZero::short_vec_prop(KTrack& trk,
			   const std::shared_ptr<const Surface>& psurf, 
			   Propagator::PropDirection propdir,
			   bool doDedx,
			   TrackMatrix* prop_matrix,
			   TrackError* noise_matrix) const
  {
    // Set the default return value to be unitialized with value 0.

    boost::optional<double> result(false, 0.);

    // Get track parameters and direction.

    TrackVector vec = trk.getVector();    // Modifiable copy.
    if(vec.size() != 5)
      throw cet::exception("PropYZPlane") 
	<< "Track state vector has wrong size" << vec.size() << "\n";
    Surface::TrackDirection dir = trk.getDirection();

    // Test initial and final surface types.

    if(const SurfYZLine* from = dynamic_cast<const SurfYZLine*>(&*trk.getSurface())) {

      // Initial surface is SurfYZLine.
      // Get surface paramters.

      double x01 = from->x0();
      double y01 = from->y0();
      double z01 = from->z0();
      double phi1 = from->phi();

      if(const SurfYZLine* to = dynamic_cast<const SurfYZLine*>(&*psurf)) {

	// SurfYZLine -> SurfYZLine.
	// Get destination surface parameters.

	double x02 = to->x0();
	double y02 = to->y0();
	double z02 = to->z0();
	double phi2 = to->phi();

	// Transform track to destination surface.

	bool ok = transformYZLine_YZLine(x01, y01, z01, phi1,
					 x02, y02, z02, phi2,
					 vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
      else if(const SurfYZPlane* to = dynamic_cast<const SurfYZPlane*>(&*psurf)) {

	// SurfYZLine -> SurfYZPlane.
	// Get destination surface parameters.

	double y02 = to->y0();
	double z02 = to->z0();
	double phi2 = to->phi();

	// Transform track to destination surface.

	bool ok = transformYZLine_YZPlane(x01, y01, z01, phi1,
					  y02, z02, phi2,
					  vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
      else if(const SurfXYZPlane* to = dynamic_cast<const SurfXYZPlane*>(&*psurf)) {

	// SurfXYZLine -> SurfXYZPlane.
	// Get destination surface parameters.

	double x02 = to->x0();
	double y02 = to->y0();
	double z02 = to->z0();
	double theta2 = to->theta();
	double phi2 = to->phi();

	// Transform track to destination surface.

	bool ok = transformYZLine_XYZPlane(x01, y01, z01, phi1,
					   x02, y02, z02, theta2, phi2,
					   vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
    }
    else if(const SurfYZPlane* from = dynamic_cast<const SurfYZPlane*>(&*trk.getSurface())) {

      // Initial surface is SurfYZPlane.
      // Get surface paramters.

      double y01 = from->y0();
      double z01 = from->z0();
      double phi1 = from->phi();

      if(const SurfYZLine* to = dynamic_cast<const SurfYZLine*>(&*psurf)) {

	// SurfYZPlane -> SurfYZLine.
	// Get destination surface parameters.

	double x02 = to->x0();
	double y02 = to->y0();
	double z02 = to->z0();
	double phi2 = to->phi();

	// Transform track to destination surface.

	bool ok = transformYZPlane_YZLine(y01, z01, phi1,
					  x02, y02, z02, phi2,
					  vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
      else if(const SurfYZPlane* to = dynamic_cast<const SurfYZPlane*>(&*psurf)) {

	// SurfYZPlane -> SurfYZPlane.
	// Get destination surface parameters.

	double y02 = to->y0();
	double z02 = to->z0();
	double phi2 = to->phi();

	// Transform track to destination surface.

	bool ok = transformYZPlane_YZPlane(y01, z01, phi1,
					   y02, z02, phi2,
					   vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
      else if(const SurfXYZPlane* to = dynamic_cast<const SurfXYZPlane*>(&*psurf)) {

	// SurfYZPlane -> SurfXYZPlane.
	// Get destination surface parameters.

	double x02 = to->x0();
	double y02 = to->y0();
	double z02 = to->z0();
	double theta2 = to->theta();
	double phi2 = to->phi();
	bool ok = transformYZPlane_XYZPlane(y01, z01, phi1,
					    x02, y02, z02, theta2, phi2,
					    vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
    }
    else if(const SurfXYZPlane* from = dynamic_cast<const SurfXYZPlane*>(&*trk.getSurface())) {

      // Initial surface is SurfXYZPlane.
      // Get surface paramters.

      double x01 = from->x0();
      double y01 = from->y0();
      double z01 = from->z0();
      double theta1 = from->theta();
      double phi1 = from->phi();

      if(const SurfYZLine* to = dynamic_cast<const SurfYZLine*>(&*psurf)) {

	// SurfXYZPlane -> SurfYZLine.
	// Get destination surface parameters.

	double x02 = to->x0();
	double y02 = to->y0();
	double z02 = to->z0();
	double phi2 = to->phi();
	bool ok = transformXYZPlane_YZLine(x01, y01, z01, theta1, phi1,
					   x02, y02, z02, phi2,
					   vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
      else if(const SurfYZPlane* to = dynamic_cast<const SurfYZPlane*>(&*psurf)) {

	// SurfXYZPlane -> SurfYZPlane.
	// Get destination surface parameters.

	double y02 = to->y0();
	double z02 = to->z0();
	double phi2 = to->phi();
	bool ok = transformXYZPlane_YZPlane(x01, y01, z01, theta1, phi1,
					    y02, z02, phi2,
					    vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
      else if(const SurfXYZPlane* to = dynamic_cast<const SurfXYZPlane*>(&*psurf)) {

	// SurfXYZPlane -> SurfXYZPlane.
	// Get destination surface parameters.

	double x02 = to->x0();
	double y02 = to->y0();
	double z02 = to->z0();
	double theta2 = to->theta();
	double phi2 = to->phi();
	bool ok = transformXYZPlane_XYZPlane(x01, y01, z01, theta1, phi1,
					     x02, y02, z02, theta2, phi2,
					     vec, dir, prop_matrix);
	result = boost::optional<double>(ok, 0.);
	if(!ok)
	  return result;
      }
    }

    // Propagation was successful if we reach here.
    // Update track.
    // Track paraeters, direction, and propagation matrix were updated in above if-block.
    // Destination surface is what was passed in as argument.

    trk.setSurface(psurf);
    trk.setVector(vec);
    trk.setDirection(dir);

    // Update noise matrix (if requested).

    if(noise_matrix != 0) {
      noise_matrix->resize(vec.size(), vec.size(), false);
      noise_matrix->clear();
    }

    // Done.

    return result;
  }

  // Transform from SurfYZLine to SurfYZLine.

  bool PropZero::transformYZLine_YZLine(double x01, double y01, double z01, double phi1,
					double x02, double y02, double z02, double phi2,
					TrackVector& vec, Surface::TrackDirection& dir,
					TrackMatrix* prop_matrix) const
  {
    // Calculate surface transcendental functions.

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track parameters.

    double r1 = vec(0);
    double v1 = vec(1);
    double phid1 = vec(2);
    double eta1 = vec(3);

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = 1.;
    double ruv = 0.;
    double ruw = 0.;

    double rvu = 0.;
    double rvv = cosdphi;
    double rvw = sindphi;

    double rwu = 0.;
    double rwv = -sindphi;
    double rww = cosdphi;

    // Calculate track transcendental functions.

    double sinphid1 = std::sin(phid1);
    double cosphid1 = std::cos(phid1);
    double sh1 = 1. / std::cosh(eta1);   // sech(eta1)
    double th1 = std::tanh(eta1);

    // Calculate initial position in Cartesian coordinates.

    double u1 = -r1 * sinphid1;
    double w1 = r1 * cosphid1;

    // Calculate position in the destination coordinate system.

    double u2 = x01 - x02 + u1;
    double v2 = (y01 - y02) * cosphi2 + (z01 - z02) * sinphi2 + v1 * cosdphi + w1 * sindphi;
    double w2 = -(y01 - y02) * sinphi2 + (z01 - z02) * cosphi2 - v1 * sindphi + w1 * cosdphi;

    // Calculate direction in destination coordinate system.

    double du2 = sh1*cosphid1;
    double dv2 = th1*cosdphi + sh1*sinphid1*sindphi;
    double dw2 = -th1*sindphi + sh1*sinphid1*cosdphi;
    double duw2 = std::hypot(du2, dw2);

    // Calculate final direction track parameters.

    double phid2 = atan2(dw2, du2);
    double eta2 = std::asinh(dv2 / duw2);

    // Calculate the impact parameter in the destination coordinate system.

    double r2 = (w2*du2 - u2*dw2) / duw2;   // w2*cos(phid2) - u2*sin(phid2)

    // Calculate the perpendicular propagation distance.
    // Should be zero if track is at surface.

    double st = -(w2*dw2 + u2*du2) / duw2;  // -(w2*sin(phid2) + u2*cos(phid2))

    // Maximum distance cut.

    if(std::abs(st) > fMaxDist)
      return false;

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      // Partials of initial positions and directions wrt initial t.p.'s.

      double du1dr1 = -sinphid1;
      double du1dv1 = 0.;
      double du1dphi1 = -w1;
      double du1deta1 = 0.;

      double dv1dr1 = 0.;
      double dv1dv1 = 1.;
      double dv1dphi1 = 0.;
      double dv1deta1 = 0.;

      double dw1dr1 = cosphid1;
      double dw1dv1 = 0.;
      double dw1dphi1 = u1;
      double dw1deta1 = 0.;

      double ddu1dr1 = 0.;
      double ddu1dv1 = 0.;
      double ddu1dphi1 = -sinphid1*sh1;
      double ddu1deta1 = -cosphid1*sh1*th1;

      double ddv1dr1 = 0.;
      double ddv1dv1 = 0.;
      double ddv1dphi1 = 0.;
      double ddv1deta1 = sh1*sh1;

      double ddw1dr1 = 0.;
      double ddw1dv1 = 0.;
      double ddw1dphi1 = cosphid1*sh1;
      double ddw1deta1 = -sinphid1*sh1*th1;

      // Rotate partials to destination coordinate system.

      double du2dr1 = ruu*du1dr1 + ruv*dv1dr1 + ruw*dw1dr1;
      double dv2dr1 = rvu*du1dr1 + rvv*dv1dr1 + rvw*dw1dr1;
      double dw2dr1 = rwu*du1dr1 + rwv*dv1dr1 + rww*dw1dr1;

      double du2dv1 = ruu*du1dv1 + ruv*dv1dv1 + ruw*dw1dv1;
      double dv2dv1 = rvu*du1dv1 + rvv*dv1dv1 + rvw*dw1dv1;
      double dw2dv1 = rwu*du1dv1 + rwv*dv1dv1 + rww*dw1dv1;

      double du2dphi1 = ruu*du1dphi1 + ruv*dv1dphi1 + ruw*dw1dphi1;
      double dv2dphi1 = rvu*du1dphi1 + rvv*dv1dphi1 + rvw*dw1dphi1;
      double dw2dphi1 = rwu*du1dphi1 + rwv*dv1dphi1 + rww*dw1dphi1;
      
      double du2deta1 = ruu*du1deta1 + ruv*dv1deta1 + ruw*dw1deta1;
      double dv2deta1 = rvu*du1deta1 + rvv*dv1deta1 + rvw*dw1deta1;
      double dw2deta1 = rwu*du1deta1 + rwv*dv1deta1 + rww*dw1deta1;

      double ddu2dr1 = ruu*ddu1dr1 + ruv*ddv1dr1 + ruw*ddw1dr1;
      double ddv2dr1 = rvu*ddu1dr1 + rvv*ddv1dr1 + rvw*ddw1dr1;
      double ddw2dr1 = rwu*ddu1dr1 + rwv*ddv1dr1 + rww*ddw1dr1;

      double ddu2dv1 = ruu*ddu1dv1 + ruv*ddv1dv1 + ruw*ddw1dv1;
      double ddv2dv1 = rvu*ddu1dv1 + rvv*ddv1dv1 + rvw*ddw1dv1;
      double ddw2dv1 = rwu*ddu1dv1 + rwv*ddv1dv1 + rww*ddw1dv1;

      double ddu2dphi1 = ruu*ddu1dphi1 + ruv*ddv1dphi1 + ruw*ddw1dphi1;
      double ddv2dphi1 = rvu*ddu1dphi1 + rvv*ddv1dphi1 + rvw*ddw1dphi1;
      double ddw2dphi1 = rwu*ddu1dphi1 + rwv*ddv1dphi1 + rww*ddw1dphi1;

      double ddu2deta1 = ruu*ddu1deta1 + ruv*ddv1deta1 + ruw*ddw1deta1;
      double ddv2deta1 = rvu*ddu1deta1 + rvv*ddv1deta1 + rvw*ddw1deta1;
      double ddw2deta1 = rwu*ddu1deta1 + rwv*ddv1deta1 + rww*ddw1deta1;

      // Partials of final t.p. wrt final position and direction.

      double dr2du2 = -dw2/duw2;
      double dr2dv2 = 0.;
      double dr2dw2 = du2/duw2;
      double dr2ddu2 = w2/duw2;
      double dr2ddv2 = r2*dv2/(duw2*duw2);
      double dr2ddw2 = -u2/duw2;

      double dphi2du2 = 0.;
      double dphi2dv2 = 0.;
      double dphi2dw2 = 0.;
      double dphi2ddu2 = -dw2/(duw2*duw2);
      double dphi2ddv2 = 0.;
      double dphi2ddw2 = du2/(duw2*duw2);

      double deta2du2 = 0.;
      double deta2dv2 = 0.;
      double deta2dw2 = 0.;
      double deta2ddu2 = 0.;
      double deta2ddv2 = 1./(duw2*duw2);
      double deta2ddw2 = 0.;

      // Partials of final t.p. wrt initial t.p.

      double dr2dr1 =    dr2du2*du2dr1 +   dr2dv2*dv2dr1 +   dr2dw2*dw2dr1
                     + dr2ddu2*ddu2dr1 + dr2ddv2*ddv2dr1 + dr2ddw2*ddw2dr1;
      double dr2dv1 =    dr2du2*du2dv1 +   dr2dv2*dv2dv1 +   dr2dw2*dw2dv1
                     + dr2ddu2*ddu2dv1 + dr2ddv2*ddv2dv1 + dr2ddw2*ddw2dv1;
      double dr2dphi1 =    dr2du2*du2dphi1 +   dr2dv2*dv2dphi1 +   dr2dw2*dw2dphi1
                       + dr2ddu2*ddu2dphi1 + dr2ddv2*ddv2dphi1 + dr2ddw2*ddw2dphi1;
      double dr2deta1 =    dr2du2*du2deta1 +   dr2dv2*dv2deta1 +   dr2dw2*dw2deta1
                       + dr2ddu2*ddu2deta1 + dr2ddv2*ddv2deta1 + dr2ddw2*ddw2deta1;

      double dphi2dr1 =    dphi2du2*du2dr1 +   dphi2dv2*dv2dr1 +   dphi2dw2*dw2dr1
                       + dphi2ddu2*ddu2dr1 + dphi2ddv2*ddv2dr1 + dphi2ddw2*ddw2dr1;
      double dphi2dv1 =    dphi2du2*du2dv1 +   dphi2dv2*dv2dv1 +   dphi2dw2*dw2dv1
                       + dphi2ddu2*ddu2dv1 + dphi2ddv2*ddv2dv1 + dphi2ddw2*ddw2dv1;
      double dphi2dphi1 =    dphi2du2*du2dphi1 +   dphi2dv2*dv2dphi1 +   dphi2dw2*dw2dphi1
                         + dphi2ddu2*ddu2dphi1 + dphi2ddv2*ddv2dphi1 + dphi2ddw2*ddw2dphi1;
      double dphi2deta1 =    dphi2du2*du2deta1 +   dphi2dv2*dv2deta1 +   dphi2dw2*dw2deta1
                         + dphi2ddu2*ddu2deta1 + dphi2ddv2*ddv2deta1 + dphi2ddw2*ddw2deta1;

      double deta2dr1 =    deta2du2*du2dr1 +   deta2dv2*dv2dr1 +   deta2dw2*dw2dr1
                       + deta2ddu2*ddu2dr1 + deta2ddv2*ddv2dr1 + deta2ddw2*ddw2dr1;
      double deta2dv1 =    deta2du2*du2dv1 +   deta2dv2*dv2dv1 +   deta2dw2*dw2dv1
                       + deta2ddu2*ddu2dv1 + deta2ddv2*ddv2dv1 + deta2ddw2*ddw2dv1;
      double deta2dphi1 =    deta2du2*du2dphi1 +   deta2dv2*dv2dphi1 +   deta2dw2*dw2dphi1
                         + deta2ddu2*ddu2dphi1 + deta2ddv2*ddv2dphi1 + deta2ddw2*ddw2dphi1;
      double deta2deta1 =    deta2du2*du2deta1 +   deta2dv2*dv2deta1 +   deta2dw2*dw2deta1
                         + deta2ddu2*ddu2deta1 + deta2ddv2*ddv2deta1 + deta2ddw2*ddw2deta1;

      // We still need to calculate the corretion due to the dependence of the
      // propagation distance on the initial track parameters.  This correction is
      // needed even though the actual propagation distance is zero.

      // This correction only effects the v track parameter, since the v parameter
      // the only parameter that actually dependes on the propagation distance.

      // Partials of propagation distance wrt position and direction in the destination
      // coordinate system.

      double dsdu2 = -du2/(duw2*duw2);
      double dsdv2 = 0.;
      double dsdw2 = -dw2/(duw2*duw2);
      double dsddu2 = -u2/(duw2*duw2);
      double dsddv2 = st*dv2/(duw2*duw2*duw2);
      double dsddw2 = -w2/(duw2*duw2);

      // Partials of propagation distance wrt initial t.p.

      double dsdr1 =    dsdu2*du2dr1 +   dsdv2*dv2dr1 +   dsdw2*dw2dr1
                    + dsddu2*ddu2dr1 + dsddv2*ddv2dr1 + dsddw2*ddw2dr1;
      double dsdv1 =    dsdu2*du2dv1 +   dsdv2*dv2dv1 +   dsdw2*dw2dv1
                    + dsddu2*ddu2dv1 + dsddv2*ddv2dv1 + dsddw2*ddw2dv1;
      double dsdphi1 =    dsdu2*du2dphi1 +   dsdv2*dv2dphi1 +   dsdw2*dw2dphi1
                      + dsddu2*ddu2dphi1 + dsddv2*ddv2dphi1 + dsddw2*ddw2dphi1;
      double dsdeta1 =    dsdu2*du2deta1 +   dsdv2*dv2deta1 +   dsdw2*dw2deta1
                      + dsddu2*ddu2deta1 + dsddv2*ddv2deta1 + dsddw2*ddw2deta1;

      // Calculate correction to v parameter partials wrt initial t.p. due to path length.

      dv2dr1 += dv2*dsdr1;
      dv2dv1 += dv2*dsdv1;
      dv2dphi1 += dv2*dsdphi1;
      dv2deta1 += dv2*dsdeta1;

      // Fill derivative matrix.

      pm(0,0) = dr2dr1;     // dr2/dr1
      pm(1,0) = dv2dr1;     // dv2/dr1
      pm(2,0) = dphi2dr1;   // d(phi2)/dr1
      pm(3,0) = deta2dr1;   // d(eta2)/dr1
      pm(4,0) = 0.;         // d(pinv2)/dr1

      pm(0,1) = dr2dv1;     // dr2/dv1
      pm(1,1) = dv2dv1;     // dv2/dv1
      pm(2,1) = dphi2dv1;   // d(phi2)/dv1
      pm(3,1) = deta2dv1;   // d(eta2)/dv1
      pm(4,1) = 0.;         // d(pinv2)/dv1

      pm(0,2) = dr2dphi1;     // dr2/d(phi1);
      pm(1,2) = dv2dphi1;     // dv2/d(phi1);
      pm(2,2) = dphi2dphi1;   // d(phi2)/d(phi1);
      pm(3,2) = deta2dphi1;   // d(eta2)/d(phi1);
      pm(4,2) = 0.;           // d(pinv2)/d(phi1);

      pm(0,3) = dr2deta1;     // dr2/d(eta1);
      pm(1,3) = dv2deta1;     // dv2/d(eta1);
      pm(2,3) = dphi2deta1;   // d(phi2)/d(eta1);
      pm(3,3) = deta2deta1;   // d(eta2)/d(eta1);
      pm(4,3) = 0.;           // d(pinv2)/d(eta1);

      pm(0,4) = 0.;      // dr2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(phi2)/d(pinv1);
      pm(3,4) = 0.;      // d(eta2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = r2;
    vec(1) = v2;
    vec(2) = phid2;
    vec(3) = eta2;

    // Done (success).

    return true;
  }

  // Transform from SurfYZLine to SurfYZPlane.

  bool PropZero::transformYZLine_YZPlane(double x01, double y01, double z01, double phi1,
					 double y02, double z02, double phi2,
					 TrackVector& vec, Surface::TrackDirection& dir,
					 TrackMatrix* prop_matrix) const
  {
    // Calculate surface transcendental functions.

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track parameters.

    double r1 = vec(0);
    double v1 = vec(1);
    double phid1 = vec(2);
    double eta1 = vec(3);

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = 1.;
    double ruv = 0.;
    double ruw = 0.;

    double rvu = 0.;
    double rvv = cosdphi;
    double rvw = sindphi;

    double rwu = 0.;
    double rwv = -sindphi;
    double rww = cosdphi;

    // Calculate track transcendental functions.

    double sinphid1 = std::sin(phid1);
    double cosphid1 = std::cos(phid1);
    double sh1 = 1. / std::cosh(eta1);   // sech(eta1)
    double th1 = std::tanh(eta1);

    // Calculate initial position in Cartesian coordinates.

    double u1 = -r1 * sinphid1;
    double w1 = r1 * cosphid1;

    // Calculate position in the destination coordinate system.

    double u2 = x01 + u1;
    double v2 = (y01 - y02) * cosphi2 + (z01 - z02) * sinphi2 + v1 * cosdphi + w1 * sindphi;
    double w2 = -(y01 - y02) * sinphi2 + (z01 - z02) * cosphi2 - v1 * sindphi + w1 * cosdphi;

    // Maximum distance cut.

    if(std::abs(w2) > fMaxDist)
      return false;

    // Calculate direction in destination coordinate system.

    double du2 = sh1*cosphid1;
    double dv2 = th1*cosdphi + sh1*sinphid1*sindphi;
    double dw2 = -th1*sindphi + sh1*sinphid1*cosdphi;
    //double duw2 = std::hypot(du2, dw2);

    // Calculate the track direction relative to the destination surface.
    // The track direction comes from the sign of dw2 (=dw/ds).
    // If dw2 is zero, the destionation surface is unreachable, return failure.

    if(dw2 > 0.)
      dir = Surface::TrackDirection::FORWARD;
    else if(dw2 < 0.)
      dir = Surface::TrackDirection::BACKWARD;
    else
      return false;

    // Calculate final track slope track parameters.

    double dudw2 = du2 / dw2;
    double dvdw2 = dv2 / dw2;

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      // Partials of initial positions and directions wrt initial t.p.'s.

      double du1dr1 = -sinphid1;
      double du1dv1 = 0.;
      double du1dphi1 = -w1;
      double du1deta1 = 0.;

      double dv1dr1 = 0.;
      double dv1dv1 = 1.;
      double dv1dphi1 = 0.;
      double dv1deta1 = 0.;

      double dw1dr1 = cosphid1;
      double dw1dv1 = 0.;
      double dw1dphi1 = u1;
      double dw1deta1 = 0.;

      double ddu1dr1 = 0.;
      double ddu1dv1 = 0.;
      double ddu1dphi1 = -sinphid1*sh1;
      double ddu1deta1 = -cosphid1*sh1*th1;

      double ddv1dr1 = 0.;
      double ddv1dv1 = 0.;
      double ddv1dphi1 = 0.;
      double ddv1deta1 = sh1*sh1;

      double ddw1dr1 = 0.;
      double ddw1dv1 = 0.;
      double ddw1dphi1 = cosphid1*sh1;
      double ddw1deta1 = -sinphid1*sh1*th1;

      // Rotate partials to destination coordinate system.

      double du2dr1 = ruu*du1dr1 + ruv*dv1dr1 + ruw*dw1dr1;
      double dv2dr1 = rvu*du1dr1 + rvv*dv1dr1 + rvw*dw1dr1;
      double dw2dr1 = rwu*du1dr1 + rwv*dv1dr1 + rww*dw1dr1;

      double du2dv1 = ruu*du1dv1 + ruv*dv1dv1 + ruw*dw1dv1;
      double dv2dv1 = rvu*du1dv1 + rvv*dv1dv1 + rvw*dw1dv1;
      double dw2dv1 = rwu*du1dv1 + rwv*dv1dv1 + rww*dw1dv1;

      double du2dphi1 = ruu*du1dphi1 + ruv*dv1dphi1 + ruw*dw1dphi1;
      double dv2dphi1 = rvu*du1dphi1 + rvv*dv1dphi1 + rvw*dw1dphi1;
      double dw2dphi1 = rwu*du1dphi1 + rwv*dv1dphi1 + rww*dw1dphi1;
      
      double du2deta1 = ruu*du1deta1 + ruv*dv1deta1 + ruw*dw1deta1;
      double dv2deta1 = rvu*du1deta1 + rvv*dv1deta1 + rvw*dw1deta1;
      double dw2deta1 = rwu*du1deta1 + rwv*dv1deta1 + rww*dw1deta1;

      double ddu2dr1 = ruu*ddu1dr1 + ruv*ddv1dr1 + ruw*ddw1dr1;
      double ddv2dr1 = rvu*ddu1dr1 + rvv*ddv1dr1 + rvw*ddw1dr1;
      double ddw2dr1 = rwu*ddu1dr1 + rwv*ddv1dr1 + rww*ddw1dr1;

      double ddu2dv1 = ruu*ddu1dv1 + ruv*ddv1dv1 + ruw*ddw1dv1;
      double ddv2dv1 = rvu*ddu1dv1 + rvv*ddv1dv1 + rvw*ddw1dv1;
      double ddw2dv1 = rwu*ddu1dv1 + rwv*ddv1dv1 + rww*ddw1dv1;

      double ddu2dphi1 = ruu*ddu1dphi1 + ruv*ddv1dphi1 + ruw*ddw1dphi1;
      double ddv2dphi1 = rvu*ddu1dphi1 + rvv*ddv1dphi1 + rvw*ddw1dphi1;
      double ddw2dphi1 = rwu*ddu1dphi1 + rwv*ddv1dphi1 + rww*ddw1dphi1;

      double ddu2deta1 = ruu*ddu1deta1 + ruv*ddv1deta1 + ruw*ddw1deta1;
      double ddv2deta1 = rvu*ddu1deta1 + rvv*ddv1deta1 + rvw*ddw1deta1;
      double ddw2deta1 = rwu*ddu1deta1 + rwv*ddv1deta1 + rww*ddw1deta1;

      // Partials of final slope t.p. wrt final position and direction.

      double ddudw2du2 = 0.;
      double ddudw2dv2 = 0.;
      double ddudw2dw2 = 0.;
      double ddudw2ddu2 = 1. / dw2;
      double ddudw2ddv2 = 0.;
      double ddudw2ddw2 = -dudw2 / dw2;

      double ddvdw2du2 = 0.;
      double ddvdw2dv2 = 0.;
      double ddvdw2dw2 = 0.;
      double ddvdw2ddu2 = 0.;
      double ddvdw2ddv2 = 1. / dw2;
      double ddvdw2ddw2 = -dvdw2 / dw2;

      // Partials of final slope t.p. wrt initial t.p.

      double ddudw2dr1 =   ddudw2du2*du2dr1 +   ddudw2dv2*dv2dr1 +   ddudw2dw2*dw2dr1
                       + ddudw2ddu2*ddu2dr1 + ddudw2ddv2*ddv2dr1 + ddudw2ddw2*ddw2dr1;
      double ddudw2dv1 =   ddudw2du2*du2dv1 +   ddudw2dv2*dv2dv1 +   ddudw2dw2*dw2dv1
                       + ddudw2ddu2*ddu2dv1 + ddudw2ddv2*ddv2dv1 + ddudw2ddw2*ddw2dv1;
      double ddudw2dphi1 =   ddudw2du2*du2dphi1 +   ddudw2dv2*dv2dphi1 +   ddudw2dw2*dw2dphi1
                         + ddudw2ddu2*ddu2dphi1 + ddudw2ddv2*ddv2dphi1 + ddudw2ddw2*ddw2dphi1;
      double ddudw2deta1 =   ddudw2du2*du2deta1 +   ddudw2dv2*dv2deta1 +   ddudw2dw2*dw2deta1
                         + ddudw2ddu2*ddu2deta1 + ddudw2ddv2*ddv2deta1 + ddudw2ddw2*ddw2deta1;

      double ddvdw2dr1 =   ddvdw2du2*du2dr1 +   ddvdw2dv2*dv2dr1 +   ddvdw2dw2*dw2dr1
                       + ddvdw2ddu2*ddu2dr1 + ddvdw2ddv2*ddv2dr1 + ddvdw2ddw2*ddw2dr1;
      double ddvdw2dv1 =   ddvdw2du2*du2dv1 +   ddvdw2dv2*dv2dv1 +   ddvdw2dw2*dw2dv1
                       + ddvdw2ddu2*ddu2dv1 + ddvdw2ddv2*ddv2dv1 + ddvdw2ddw2*ddw2dv1;
      double ddvdw2dphi1 =   ddvdw2du2*du2dphi1 +   ddvdw2dv2*dv2dphi1 +   ddvdw2dw2*dw2dphi1
                         + ddvdw2ddu2*ddu2dphi1 + ddvdw2ddv2*ddv2dphi1 + ddvdw2ddw2*ddw2dphi1;
      double ddvdw2deta1 =   ddvdw2du2*du2deta1 +   ddvdw2dv2*dv2deta1 +   ddvdw2dw2*dw2deta1
                         + ddvdw2ddu2*ddu2deta1 + ddvdw2ddv2*ddv2deta1 + ddvdw2ddw2*ddw2deta1;

      // We still need to calculate the corretion due to the dependence of the
      // propagation distance on the initial track parameters.  This correction is
      // needed even though the actual propagation distance is zero.

      // This correction effects the u and v track parameters.

      // Partials of perpendicular propagation distance wrt position and direction 
      // in the destination coordinate system.

      double dstdu2 = 0.;
      double dstdv2 = 0.;
      double dstdw2 = -1.;
      double dstddu2 = 0.;
      double dstddv2 = 0.;
      double dstddw2 = 0.;

      // Partials of propagation distance wrt initial t.p.

      double dstdr1 =    dstdu2*du2dr1 +   dstdv2*dv2dr1 +   dstdw2*dw2dr1
                     + dstddu2*ddu2dr1 + dstddv2*ddv2dr1 + dstddw2*ddw2dr1;
      double dstdv1 =    dstdu2*du2dv1 +   dstdv2*dv2dv1 +   dstdw2*dw2dv1
                     + dstddu2*ddu2dv1 + dstddv2*ddv2dv1 + dstddw2*ddw2dv1;
      double dstdphi1 =    dstdu2*du2dphi1 +   dstdv2*dv2dphi1 +   dstdw2*dw2dphi1
                       + dstddu2*ddu2dphi1 + dstddv2*ddv2dphi1 + dstddw2*ddw2dphi1;
      double dstdeta1 =    dstdu2*du2deta1 +   dstdv2*dv2deta1 +   dstdw2*dw2deta1
                       + dstddu2*ddu2deta1 + dstddv2*ddv2deta1 + dstddw2*ddw2deta1;

      // Calculate correction to u and v parameter partials wrt initial t.p. due to path length.

      du2dr1 += dstdr1 * dudw2;
      du2dv1 += dstdv1 * dudw2;
      du2dphi1 += dstdphi1 * dudw2;
      du2deta1 += dstdeta1 * dudw2;

      dv2dr1 += dstdr1 * dvdw2;
      dv2dv1 += dstdv1 * dvdw2;
      dv2dphi1 += dstdphi1 * dvdw2;
      dv2deta1 += dstdeta1 * dvdw2;

      // Fill derivative matrix.

      pm(0,0) = du2dr1;     // du2/dr1
      pm(1,0) = dv2dr1;     // dv2/dr1
      pm(2,0) = ddudw2dr1;  // d(dudw2)/dr1
      pm(3,0) = ddvdw2dr1;  // d(dvdw2)/dr1
      pm(4,0) = 0.;         // d(pinv2)/dr1

      pm(0,1) = du2dv1;     // du2/dv1
      pm(1,1) = dv2dv1;     // dv2/dv1
      pm(2,1) = ddudw2dv1;  // d(dudw2)/dv1
      pm(3,1) = ddvdw2dv1;  // d(dvdw2)/dv1
      pm(4,1) = 0.;         // d(pinv2)/dv1

      pm(0,2) = du2dphi1;     // du2/d(phi1);
      pm(1,2) = dv2dphi1;     // dv2/d(phi1);
      pm(2,2) = ddudw2dphi1;  // d(dudw2)/d(phi1);
      pm(3,2) = ddvdw2dphi1;  // d(dvdw2)/d(phi1);
      pm(4,2) = 0.;           // d(pinv2)/d(phi1);

      pm(0,3) = du2deta1;     // du2/d(eta1);
      pm(1,3) = dv2deta1;     // dv2/d(eta1);
      pm(2,3) = ddudw2deta1;  // d(dudw2)/d(eta1);
      pm(3,3) = ddvdw2deta1;  // d(dvdw2)/d(eta1);
      pm(4,3) = 0.;           // d(pinv2)/d(eta1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = u2;
    vec(1) = v2;
    vec(2) = dudw2;
    vec(3) = dvdw2;

    // Done (success).

    return true;
  }

  // Transform from SurfYZLine to SurfXYZPlane.

  bool PropZero::transformYZLine_XYZPlane(double x01, double y01, double z01, double phi1,
					  double x02, double y02, double z02,
					  double theta2, double phi2,
					  TrackVector& vec, Surface::TrackDirection& dir,
					  TrackMatrix* prop_matrix) const
  {
    // Calculate surface transcendental functions.

    double sinth1 = 0.;
    double costh1 = 1.;
    double sinth2 = std::sin(theta2);
    double costh2 = std::cos(theta2);

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track parameters.

    double r1 = vec(0);
    double v1 = vec(1);
    double phid1 = vec(2);
    double eta1 = vec(3);

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = costh1*costh2 + sinth1*sinth2*cosdphi;
    double ruv = sinth2*sindphi;
    double ruw = sinth1*costh2 - costh1*sinth2*cosdphi;

    double rvu = -sinth1*sindphi;
    double rvv = cosdphi;
    double rvw = costh1*sindphi;

    double rwu = costh1*sinth2 - sinth1*costh2*cosdphi;
    double rwv = -costh2*sindphi;
    double rww = sinth1*sinth2 + costh1*costh2*cosdphi;

    // Calculate elements of rotation matrix from global coordinate
    // system to destination coordinate system.

    double rux = costh2;
    double ruy = sinth2*sinphi2;
    double ruz = -sinth2*cosphi2;

    double rvy = cosphi2;
    double rvz = sinphi2;

    double rwx = sinth2;
    double rwy = -costh2*sinphi2;
    double rwz = costh2*cosphi2;

    // Calculate track transcendental functions.

    double sinphid1 = std::sin(phid1);
    double cosphid1 = std::cos(phid1);
    double sh1 = 1. / std::cosh(eta1);   // sech(eta1)
    double th1 = std::tanh(eta1);

    // Calculate initial position in Cartesian coordinates.

    double u1 = -r1 * sinphid1;
    double w1 = r1 * cosphid1;

    // Calculate the initial position in the destination coordinate
    // system.

    double u2 = (x01-x02)*rux + (y01-y02)*ruy + (z01-z02)*ruz + u1*ruu + v1*ruv + w1*ruw;
    double v2 =                 (y01-y02)*rvy + (z01-z02)*rvz + u1*rvu + v1*rvv + w1*rvw;
    double w2 = (x01-x02)*rwx + (y01-y02)*rwy + (z01-z02)*rwz + u1*rwu + v1*rwv + w1*rww;

    // Maximum distance cut.

    if(std::abs(w2) > fMaxDist)
      return false;

    // Calculate direction in source coordinate system.

    double du1 = sh1*cosphid1;
    double dv1 = th1;
    double dw1 = sh1*sinphid1;
    //double duw2 = std::hypot(du2, dw2);

    // Rotate direction to destination coordinate system.

    double du2 = ruu*du1 + ruv*dv1 + ruw*dw1;
    double dv2 = rvu*du1 + rvv*dv1 + rvw*dw1;
    double dw2 = rwu*du1 + rwv*dv1 + rww*dw1;
    //double duw2 = std::hypot(du2, dw2);

    // Calculate the track direction relative to the destination surface.
    // The track direction comes from the sign of dw2 (=dw/ds).
    // If dw2 is zero, the destionation surface is unreachable, return failure.

    if(dw2 > 0.)
      dir = Surface::TrackDirection::FORWARD;
    else if(dw2 < 0.)
      dir = Surface::TrackDirection::BACKWARD;
    else
      return false;

    // Calculate final track slope track parameters.

    double dudw2 = du2 / dw2;
    double dvdw2 = dv2 / dw2;

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      // Partials of initial positions and directions wrt initial t.p.'s.

      double du1dr1 = -sinphid1;
      double du1dv1 = 0.;
      double du1dphi1 = -w1;
      double du1deta1 = 0.;

      double dv1dr1 = 0.;
      double dv1dv1 = 1.;
      double dv1dphi1 = 0.;
      double dv1deta1 = 0.;

      double dw1dr1 = cosphid1;
      double dw1dv1 = 0.;
      double dw1dphi1 = u1;
      double dw1deta1 = 0.;

      double ddu1dr1 = 0.;
      double ddu1dv1 = 0.;
      double ddu1dphi1 = -sinphid1*sh1;
      double ddu1deta1 = -cosphid1*sh1*th1;

      double ddv1dr1 = 0.;
      double ddv1dv1 = 0.;
      double ddv1dphi1 = 0.;
      double ddv1deta1 = sh1*sh1;

      double ddw1dr1 = 0.;
      double ddw1dv1 = 0.;
      double ddw1dphi1 = cosphid1*sh1;
      double ddw1deta1 = -sinphid1*sh1*th1;

      // Rotate partials to destination coordinate system.

      double du2dr1 = ruu*du1dr1 + ruv*dv1dr1 + ruw*dw1dr1;
      double dv2dr1 = rvu*du1dr1 + rvv*dv1dr1 + rvw*dw1dr1;
      double dw2dr1 = rwu*du1dr1 + rwv*dv1dr1 + rww*dw1dr1;

      double du2dv1 = ruu*du1dv1 + ruv*dv1dv1 + ruw*dw1dv1;
      double dv2dv1 = rvu*du1dv1 + rvv*dv1dv1 + rvw*dw1dv1;
      double dw2dv1 = rwu*du1dv1 + rwv*dv1dv1 + rww*dw1dv1;

      double du2dphi1 = ruu*du1dphi1 + ruv*dv1dphi1 + ruw*dw1dphi1;
      double dv2dphi1 = rvu*du1dphi1 + rvv*dv1dphi1 + rvw*dw1dphi1;
      double dw2dphi1 = rwu*du1dphi1 + rwv*dv1dphi1 + rww*dw1dphi1;
      
      double du2deta1 = ruu*du1deta1 + ruv*dv1deta1 + ruw*dw1deta1;
      double dv2deta1 = rvu*du1deta1 + rvv*dv1deta1 + rvw*dw1deta1;
      double dw2deta1 = rwu*du1deta1 + rwv*dv1deta1 + rww*dw1deta1;

      double ddu2dr1 = ruu*ddu1dr1 + ruv*ddv1dr1 + ruw*ddw1dr1;
      double ddv2dr1 = rvu*ddu1dr1 + rvv*ddv1dr1 + rvw*ddw1dr1;
      double ddw2dr1 = rwu*ddu1dr1 + rwv*ddv1dr1 + rww*ddw1dr1;

      double ddu2dv1 = ruu*ddu1dv1 + ruv*ddv1dv1 + ruw*ddw1dv1;
      double ddv2dv1 = rvu*ddu1dv1 + rvv*ddv1dv1 + rvw*ddw1dv1;
      double ddw2dv1 = rwu*ddu1dv1 + rwv*ddv1dv1 + rww*ddw1dv1;

      double ddu2dphi1 = ruu*ddu1dphi1 + ruv*ddv1dphi1 + ruw*ddw1dphi1;
      double ddv2dphi1 = rvu*ddu1dphi1 + rvv*ddv1dphi1 + rvw*ddw1dphi1;
      double ddw2dphi1 = rwu*ddu1dphi1 + rwv*ddv1dphi1 + rww*ddw1dphi1;

      double ddu2deta1 = ruu*ddu1deta1 + ruv*ddv1deta1 + ruw*ddw1deta1;
      double ddv2deta1 = rvu*ddu1deta1 + rvv*ddv1deta1 + rvw*ddw1deta1;
      double ddw2deta1 = rwu*ddu1deta1 + rwv*ddv1deta1 + rww*ddw1deta1;

      // Partials of final slope t.p. wrt final position and direction.

      double ddudw2du2 = 0.;
      double ddudw2dv2 = 0.;
      double ddudw2dw2 = 0.;
      double ddudw2ddu2 = 1. / dw2;
      double ddudw2ddv2 = 0.;
      double ddudw2ddw2 = -dudw2 / dw2;

      double ddvdw2du2 = 0.;
      double ddvdw2dv2 = 0.;
      double ddvdw2dw2 = 0.;
      double ddvdw2ddu2 = 0.;
      double ddvdw2ddv2 = 1. / dw2;
      double ddvdw2ddw2 = -dvdw2 / dw2;

      // Partials of final slope t.p. wrt initial t.p.

      double ddudw2dr1 =   ddudw2du2*du2dr1 +   ddudw2dv2*dv2dr1 +   ddudw2dw2*dw2dr1
                       + ddudw2ddu2*ddu2dr1 + ddudw2ddv2*ddv2dr1 + ddudw2ddw2*ddw2dr1;
      double ddudw2dv1 =   ddudw2du2*du2dv1 +   ddudw2dv2*dv2dv1 +   ddudw2dw2*dw2dv1
                       + ddudw2ddu2*ddu2dv1 + ddudw2ddv2*ddv2dv1 + ddudw2ddw2*ddw2dv1;
      double ddudw2dphi1 =   ddudw2du2*du2dphi1 +   ddudw2dv2*dv2dphi1 +   ddudw2dw2*dw2dphi1
                         + ddudw2ddu2*ddu2dphi1 + ddudw2ddv2*ddv2dphi1 + ddudw2ddw2*ddw2dphi1;
      double ddudw2deta1 =   ddudw2du2*du2deta1 +   ddudw2dv2*dv2deta1 +   ddudw2dw2*dw2deta1
                         + ddudw2ddu2*ddu2deta1 + ddudw2ddv2*ddv2deta1 + ddudw2ddw2*ddw2deta1;

      double ddvdw2dr1 =   ddvdw2du2*du2dr1 +   ddvdw2dv2*dv2dr1 +   ddvdw2dw2*dw2dr1
                       + ddvdw2ddu2*ddu2dr1 + ddvdw2ddv2*ddv2dr1 + ddvdw2ddw2*ddw2dr1;
      double ddvdw2dv1 =   ddvdw2du2*du2dv1 +   ddvdw2dv2*dv2dv1 +   ddvdw2dw2*dw2dv1
                       + ddvdw2ddu2*ddu2dv1 + ddvdw2ddv2*ddv2dv1 + ddvdw2ddw2*ddw2dv1;
      double ddvdw2dphi1 =   ddvdw2du2*du2dphi1 +   ddvdw2dv2*dv2dphi1 +   ddvdw2dw2*dw2dphi1
                         + ddvdw2ddu2*ddu2dphi1 + ddvdw2ddv2*ddv2dphi1 + ddvdw2ddw2*ddw2dphi1;
      double ddvdw2deta1 =   ddvdw2du2*du2deta1 +   ddvdw2dv2*dv2deta1 +   ddvdw2dw2*dw2deta1
                         + ddvdw2ddu2*ddu2deta1 + ddvdw2ddv2*ddv2deta1 + ddvdw2ddw2*ddw2deta1;

      // We still need to calculate the corretion due to the dependence of the
      // propagation distance on the initial track parameters.  This correction is
      // needed even though the actual propagation distance is zero.

      // This correction effects the u and v track parameters.

      // Partials of perpendicular propagation distance wrt position and direction 
      // in the destination coordinate system.

      double dstdu2 = 0.;
      double dstdv2 = 0.;
      double dstdw2 = -1.;
      double dstddu2 = 0.;
      double dstddv2 = 0.;
      double dstddw2 = 0.;

      // Partials of propagation distance wrt initial t.p.

      double dstdr1 =    dstdu2*du2dr1 +   dstdv2*dv2dr1 +   dstdw2*dw2dr1
                     + dstddu2*ddu2dr1 + dstddv2*ddv2dr1 + dstddw2*ddw2dr1;
      double dstdv1 =    dstdu2*du2dv1 +   dstdv2*dv2dv1 +   dstdw2*dw2dv1
                     + dstddu2*ddu2dv1 + dstddv2*ddv2dv1 + dstddw2*ddw2dv1;
      double dstdphi1 =    dstdu2*du2dphi1 +   dstdv2*dv2dphi1 +   dstdw2*dw2dphi1
                       + dstddu2*ddu2dphi1 + dstddv2*ddv2dphi1 + dstddw2*ddw2dphi1;
      double dstdeta1 =    dstdu2*du2deta1 +   dstdv2*dv2deta1 +   dstdw2*dw2deta1
                       + dstddu2*ddu2deta1 + dstddv2*ddv2deta1 + dstddw2*ddw2deta1;

      // Calculate correction to u and v parameter partials wrt initial t.p. due to path length.

      du2dr1 += dstdr1 * dudw2;
      du2dv1 += dstdv1 * dudw2;
      du2dphi1 += dstdphi1 * dudw2;
      du2deta1 += dstdeta1 * dudw2;

      dv2dr1 += dstdr1 * dvdw2;
      dv2dv1 += dstdv1 * dvdw2;
      dv2dphi1 += dstdphi1 * dvdw2;
      dv2deta1 += dstdeta1 * dvdw2;

      // Fill derivative matrix.

      pm(0,0) = du2dr1;     // du2/dr1
      pm(1,0) = dv2dr1;     // dv2/dr1
      pm(2,0) = ddudw2dr1;  // d(dudw2)/dr1
      pm(3,0) = ddvdw2dr1;  // d(dvdw2)/dr1
      pm(4,0) = 0.;         // d(pinv2)/dr1

      pm(0,1) = du2dv1;     // du2/dv1
      pm(1,1) = dv2dv1;     // dv2/dv1
      pm(2,1) = ddudw2dv1;  // d(dudw2)/dv1
      pm(3,1) = ddvdw2dv1;  // d(dvdw2)/dv1
      pm(4,1) = 0.;         // d(pinv2)/dv1

      pm(0,2) = du2dphi1;     // du2/d(phi1);
      pm(1,2) = dv2dphi1;     // dv2/d(phi1);
      pm(2,2) = ddudw2dphi1;  // d(dudw2)/d(phi1);
      pm(3,2) = ddvdw2dphi1;  // d(dvdw2)/d(phi1);
      pm(4,2) = 0.;           // d(pinv2)/d(phi1);

      pm(0,3) = du2deta1;     // du2/d(eta1);
      pm(1,3) = dv2deta1;     // dv2/d(eta1);
      pm(2,3) = ddudw2deta1;  // d(dudw2)/d(eta1);
      pm(3,3) = ddvdw2deta1;  // d(dvdw2)/d(eta1);
      pm(4,3) = 0.;           // d(pinv2)/d(eta1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = u2;
    vec(1) = v2;
    vec(2) = dudw2;
    vec(3) = dvdw2;

    // Done (success).

    return true;
  }

  // Transform from SurfYZPlane to SurfYZLine.

  bool PropZero::transformYZPlane_YZLine(double y01, double z01, double phi1,
					 double x02, double y02, double z02, double phi2,
					 TrackVector& vec, Surface::TrackDirection& dir,
					 TrackMatrix* prop_matrix) const
  {
    // Calculate surface transcendental functions.

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track parameters.

    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);

    // Make sure initial track has a valid direction.

    double dirf = 1.;
    if(dir == Surface::BACKWARD)
      dirf = -1.;
    else if(dir != Surface::FORWARD)
      return false;

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = 1.;
    double ruv = 0.;
    double ruw = 0.;

    double rvu = 0.;
    double rvv = cosdphi;
    double rvw = sindphi;

    double rwu = 0.;
    double rwv = -sindphi;
    double rww = cosdphi;

    // Calculate position in the destination coordinate system.

    double u2 = -x02 + u1;
    double v2 = (y01 - y02) * cosphi2 + (z01 - z02) * sinphi2 + v1 * cosdphi;
    double w2 = -(y01 - y02) * sinphi2 + (z01 - z02) * cosphi2 - v1 * sindphi;

    // Calculate direction in the starting coordinate system.

    double dw1 = dirf / std::sqrt(1. + dudw1*dudw1 + dvdw1*dvdw1);
    double du1 = dudw1 * dw1;
    double dv1 = dvdw1 * dw1;

    // Rotate direction vector into destination coordinate system.

    double du2 = ruu*du1 + ruv*dv1 + ruw*dw1;
    double dv2 = rvu*du1 + rvv*dv1 + rvw*dw1;
    double dw2 = rwu*du1 + rwv*dv1 + rww*dw1;
    double duw2 = std::hypot(du2, dw2);

    // Calculate final direction track parameters.

    double phid2 = atan2(dw2, du2);
    double eta2 = std::asinh(dv2 / duw2);

    // Calculate the impact parameter in the destination coordinate system.

    double r2 = (w2*du2 - u2*dw2) / duw2;   // w2*cos(phid2) - u2*sin(phid2)

    // Calculate the perpendicular propagation distance.
    // Should be zero if track is at surface.

    double st = -(w2*dw2 + u2*du2) / duw2;  // -(w2*sin(phid2) + u2*cos(phid2))

    // Maximum distance cut.

    if(std::abs(st) > fMaxDist)
      return false;

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      // Partials of initial positions and directions wrt initial t.p.'s.

      double du1du1 = 1.;
      double du1dv1 = 0.;
      double du1ddudw1 = 0.;
      double du1ddvdw1 = 0.;

      double dv1du1 = 0.;
      double dv1dv1 = 1.;
      double dv1ddudw1 = 0.;
      double dv1ddvdw1 = 0.;

      double dw1du1 = 0.;
      double dw1dv1 = 0.;
      double dw1ddudw1 = 0.;
      double dw1ddvdw1 = 0.; 

      double ddu1du1 = 0.;
      double ddu1dv1 = 0.;
      double ddu1ddudw1 = (1. + dvdw1*dvdw1) * dw1*dw1*dw1;
      double ddu1ddvdw1 = -dudw1 * dvdw1 * dw1*dw1*dw1;

      double ddv1du1 = 0.;
      double ddv1dv1 = 0.;
      double ddv1ddudw1 = -dudw1 * dvdw1 * dw1*dw1*dw1;
      double ddv1ddvdw1 = (1. + dudw1*dudw1) * dw1*dw1*dw1;

      double ddw1du1 = 0.;
      double ddw1dv1 = 0.;
      double ddw1ddudw1 = -dudw1 * dw1*dw1*dw1;
      double ddw1ddvdw1 = -dvdw1 * dw1*dw1*dw1;

      // Rotate partials to destination coordinate system.

      double du2du1 = ruu*du1du1 + ruv*dv1du1 + ruw*dw1du1;
      double dv2du1 = rvu*du1du1 + rvv*dv1du1 + rvw*dw1du1;
      double dw2du1 = rwu*du1du1 + rwv*dv1du1 + rww*dw1du1;

      double du2dv1 = ruu*du1dv1 + ruv*dv1dv1 + ruw*dw1dv1;
      double dv2dv1 = rvu*du1dv1 + rvv*dv1dv1 + rvw*dw1dv1;
      double dw2dv1 = rwu*du1dv1 + rwv*dv1dv1 + rww*dw1dv1;

      double du2ddudw1 = ruu*du1ddudw1 + ruv*dv1ddudw1 + ruw*dw1ddudw1;
      double dv2ddudw1 = rvu*du1ddudw1 + rvv*dv1ddudw1 + rvw*dw1ddudw1;
      double dw2ddudw1 = rwu*du1ddudw1 + rwv*dv1ddudw1 + rww*dw1ddudw1;
      
      double du2ddvdw1 = ruu*du1ddvdw1 + ruv*dv1ddvdw1 + ruw*dw1ddvdw1;
      double dv2ddvdw1 = rvu*du1ddvdw1 + rvv*dv1ddvdw1 + rvw*dw1ddvdw1;
      double dw2ddvdw1 = rwu*du1ddvdw1 + rwv*dv1ddvdw1 + rww*dw1ddvdw1;

      double ddu2du1 = ruu*ddu1du1 + ruv*ddv1du1 + ruw*ddw1du1;
      double ddv2du1 = rvu*ddu1du1 + rvv*ddv1du1 + rvw*ddw1du1;
      double ddw2du1 = rwu*ddu1du1 + rwv*ddv1du1 + rww*ddw1du1;

      double ddu2dv1 = ruu*ddu1dv1 + ruv*ddv1dv1 + ruw*ddw1dv1;
      double ddv2dv1 = rvu*ddu1dv1 + rvv*ddv1dv1 + rvw*ddw1dv1;
      double ddw2dv1 = rwu*ddu1dv1 + rwv*ddv1dv1 + rww*ddw1dv1;

      double ddu2ddudw1 = ruu*ddu1ddudw1 + ruv*ddv1ddudw1 + ruw*ddw1ddudw1;
      double ddv2ddudw1 = rvu*ddu1ddudw1 + rvv*ddv1ddudw1 + rvw*ddw1ddudw1;
      double ddw2ddudw1 = rwu*ddu1ddudw1 + rwv*ddv1ddudw1 + rww*ddw1ddudw1;

      double ddu2ddvdw1 = ruu*ddu1ddvdw1 + ruv*ddv1ddvdw1 + ruw*ddw1ddvdw1;
      double ddv2ddvdw1 = rvu*ddu1ddvdw1 + rvv*ddv1ddvdw1 + rvw*ddw1ddvdw1;
      double ddw2ddvdw1 = rwu*ddu1ddvdw1 + rwv*ddv1ddvdw1 + rww*ddw1ddvdw1;

      // Partials of final t.p. wrt final position and direction.

      double dr2du2 = -dw2/duw2;
      double dr2dv2 = 0.;
      double dr2dw2 = du2/duw2;
      double dr2ddu2 = w2/duw2;
      double dr2ddv2 = r2*dv2/(duw2*duw2);
      double dr2ddw2 = -u2/duw2;

      double dphi2du2 = 0.;
      double dphi2dv2 = 0.;
      double dphi2dw2 = 0.;
      double dphi2ddu2 = -dw2/(duw2*duw2);
      double dphi2ddv2 = 0.;
      double dphi2ddw2 = du2/(duw2*duw2);

      double deta2du2 = 0.;
      double deta2dv2 = 0.;
      double deta2dw2 = 0.;
      double deta2ddu2 = 0.;
      double deta2ddv2 = 1./(duw2*duw2);
      double deta2ddw2 = 0.;

      // Partials of final t.p. wrt initial t.p.

      double dr2du1 =    dr2du2*du2du1 +   dr2dv2*dv2du1 +   dr2dw2*dw2du1
                     + dr2ddu2*ddu2du1 + dr2ddv2*ddv2du1 + dr2ddw2*ddw2du1;
      double dr2dv1 =    dr2du2*du2dv1 +   dr2dv2*dv2dv1 +   dr2dw2*dw2dv1
                     + dr2ddu2*ddu2dv1 + dr2ddv2*ddv2dv1 + dr2ddw2*ddw2dv1;
      double dr2ddudw1 =   dr2du2*du2ddudw1 +   dr2dv2*dv2ddudw1 +   dr2dw2*dw2ddudw1
                       + dr2ddu2*ddu2ddudw1 + dr2ddv2*ddv2ddudw1 + dr2ddw2*ddw2ddudw1;
      double dr2ddvdw1 =    dr2du2*du2ddvdw1 +   dr2dv2*dv2ddvdw1 +   dr2dw2*dw2ddvdw1
                       + dr2ddu2*ddu2ddvdw1 + dr2ddv2*ddv2ddvdw1 + dr2ddw2*ddw2ddvdw1;

      double dphi2du1 =    dphi2du2*du2du1 +   dphi2dv2*dv2du1 +   dphi2dw2*dw2du1
                       + dphi2ddu2*ddu2du1 + dphi2ddv2*ddv2du1 + dphi2ddw2*ddw2du1;
      double dphi2dv1 =    dphi2du2*du2dv1 +   dphi2dv2*dv2dv1 +   dphi2dw2*dw2dv1
                       + dphi2ddu2*ddu2dv1 + dphi2ddv2*ddv2dv1 + dphi2ddw2*ddw2dv1;
      double dphi2ddudw1 =   dphi2du2*du2ddudw1 +   dphi2dv2*dv2ddudw1 +   dphi2dw2*dw2ddudw1
                         + dphi2ddu2*ddu2ddudw1 + dphi2ddv2*ddv2ddudw1 + dphi2ddw2*ddw2ddudw1;
      double dphi2ddvdw1 =   dphi2du2*du2ddvdw1 +   dphi2dv2*dv2ddvdw1 +   dphi2dw2*dw2ddvdw1
                         + dphi2ddu2*ddu2ddvdw1 + dphi2ddv2*ddv2ddvdw1 + dphi2ddw2*ddw2ddvdw1;

      double deta2du1 =    deta2du2*du2du1 +   deta2dv2*dv2du1 +   deta2dw2*dw2du1
                       + deta2ddu2*ddu2du1 + deta2ddv2*ddv2du1 + deta2ddw2*ddw2du1;
      double deta2dv1 =    deta2du2*du2dv1 +   deta2dv2*dv2dv1 +   deta2dw2*dw2dv1
                       + deta2ddu2*ddu2dv1 + deta2ddv2*ddv2dv1 + deta2ddw2*ddw2dv1;
      double deta2ddudw1 =   deta2du2*du2ddudw1 +   deta2dv2*dv2ddudw1 +   deta2dw2*dw2ddudw1
                         + deta2ddu2*ddu2ddudw1 + deta2ddv2*ddv2ddudw1 + deta2ddw2*ddw2ddudw1;
      double deta2ddvdw1 =   deta2du2*du2ddvdw1 +   deta2dv2*dv2ddvdw1 +   deta2dw2*dw2ddvdw1
                         + deta2ddu2*ddu2ddvdw1 + deta2ddv2*ddv2ddvdw1 + deta2ddw2*ddw2ddvdw1;

      // We still need to calculate the corretion due to the dependence of the
      // propagation distance on the initial track parameters.  This correction is
      // needed even though the actual propagation distance is zero.

      // This correction only effects the v track parameter, since the v parameter
      // the only parameter that actually dependes on the propagation distance.

      // Partials of propagation distance wrt position and direction in the destination
      // coordinate system.

      double dsdu2 = -du2/(duw2*duw2);
      double dsdv2 = 0.;
      double dsdw2 = -dw2/(duw2*duw2);
      double dsddu2 = -u2/(duw2*duw2);
      double dsddv2 = st*dv2/(duw2*duw2*duw2);
      double dsddw2 = -w2/(duw2*duw2);

      // Partials of propagation distance wrt initial t.p.

      double dsdu1 =    dsdu2*du2du1 +   dsdv2*dv2du1 +   dsdw2*dw2du1
                    + dsddu2*ddu2du1 + dsddv2*ddv2du1 + dsddw2*ddw2du1;
      double dsdv1 =    dsdu2*du2dv1 +   dsdv2*dv2dv1 +   dsdw2*dw2dv1
                    + dsddu2*ddu2dv1 + dsddv2*ddv2dv1 + dsddw2*ddw2dv1;
      double dsddudw1 =   dsdu2*du2ddudw1 +   dsdv2*dv2ddudw1 +   dsdw2*dw2ddudw1
                      + dsddu2*ddu2ddudw1 + dsddv2*ddv2ddudw1 + dsddw2*ddw2ddudw1;
      double dsddvdw1 =   dsdu2*du2ddvdw1 +   dsdv2*dv2ddvdw1 +   dsdw2*dw2ddvdw1
                      + dsddu2*ddu2ddvdw1 + dsddv2*ddv2ddvdw1 + dsddw2*ddw2ddvdw1;

      // Calculate correction to v parameter partials wrt initial t.p. due to path length.

      dv2du1 += dv2*dsdu1;
      dv2dv1 += dv2*dsdv1;
      dv2ddudw1 += dv2*dsddudw1;
      dv2ddvdw1 += dv2*dsddvdw1;

      // Fill matrix.

      pm(0,0) = dr2du1;     // dr2/du1
      pm(1,0) = dv2du1;     // dv2/du1
      pm(2,0) = dphi2du1;   // d(phi2)/du1
      pm(3,0) = deta2du1;   // d(eta2)/du1
      pm(4,0) = 0.;         // d(pinv2)/du1

      pm(0,1) = dr2dv1;     // dr2/dv1
      pm(1,1) = dv2dv1;     // dv2/dv1
      pm(2,1) = dphi2dv1;   // d(phi2)/dv1
      pm(3,1) = deta2dv1;   // d(eta2)/dv1
      pm(4,1) = 0.;         // d(pinv2)/dv1

      pm(0,2) = dr2ddudw1;     // dr2/d(dudw1);
      pm(1,2) = dv2ddudw1;     // dv2/d(dudw1);
      pm(2,2) = dphi2ddudw1;   // d(dudw2)/d(dudw1);
      pm(3,2) = deta2ddudw1;   // d(eta2)/d(dudw1);
      pm(4,2) = 0.;            // d(pinv2)/d(dudw1);

      pm(0,3) = dr2ddvdw1;     // dr2/d(dvdw1);
      pm(1,3) = dv2ddvdw1;     // dv2/d(dvdw1);
      pm(2,3) = dphi2ddvdw1;   // d(phi2)/d(dvdw1);
      pm(3,3) = deta2ddvdw1;   // d(eta2)/d(dvdw1);
      pm(4,3) = 0.;            // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // dr2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(phi2)/d(pinv1);
      pm(3,4) = 0.;      // d(eta2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = r2;
    vec(1) = v2;
    vec(2) = phid2;
    vec(3) = eta2;

    // Done (success).

    return true;
  }

  // Transform from SurfYZPlane to SurfYZPlane.

  bool PropZero::transformYZPlane_YZPlane(double y01, double z01, double phi1,
					  double y02, double z02, double phi2,
					  TrackVector& vec, Surface::TrackDirection& dir,
					  TrackMatrix* prop_matrix) const
  {
    // Calculate transcendental functions.

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track parameters.

    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);

    // Make sure initial track has a valid direction.

    if(dir == Surface::UNKNOWN)
      return false;

    // Calculate initial position in the destination coordinate system.

    double u2 = u1;
    double v2 = (y01 - y02) * cosphi2 + (z01 - z02) * sinphi2 + v1 * cosdphi;
    double w2 = -(y01 - y02) * sinphi2 + (z01 - z02) * cosphi2 - v1 * sindphi;

    // Maximum distance cut.

    if(std::abs(w2) > fMaxDist)
      return false;

    // Calculate derivative dw2/dw1.
    // If dw2/dw1 == 0., that means the track is moving parallel
    // to destination plane.
    // In this case return propagation failure.

    double dw2dw1 = cosdphi - dvdw1 * sindphi;
    if(dw2dw1 == 0.)
      return false;

    // Calculate slope in destrination coordiante system.

    double dudw2 = dudw1 / dw2dw1;
    double dvdw2 = (sindphi + dvdw1 * cosdphi) / dw2dw1;

    // Calculate direction parameter at destination surface.
    // Direction will flip if dw2dw1 < 0.;

    switch (dir) {
    case Surface::FORWARD:
      dir = (dw2dw1 > 0.)? Surface::FORWARD: Surface::BACKWARD;
      break;
    case Surface::BACKWARD:
      dir = (dw2dw1 > 0.)? Surface::BACKWARD: Surface::FORWARD;
      break;
    default:
      throw cet::exception("PropYZPlane")
	<< "unexpected direction #" << ((int) dir) << "\n";
    } // switch

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      pm(0,0) = 1.;   // du2/du1
      pm(1,0) = 0.;   // dv2/du1
      pm(2,0) = 0.;   // d(dudw2)/du1
      pm(3,0) = 0.;   // d(dvdw2)/du1
      pm(4,0) = 0.;   // d(pinv2)/du1

      pm(0,1) = dudw2 * sindphi;             // du2/dv1
      pm(1,1) = cosdphi + dvdw2 * sindphi;   // dv2/dv1
      pm(2,1) = 0.;                          // d(dudw2)/dv1
      pm(3,1) = 0.;                          // d(dvdw2)/dv1
      pm(4,1) = 0.;                          // d(pinv2)/dv1

      pm(0,2) = 0.;             // du2/d(dudw1);
      pm(1,2) = 0.;             // dv2/d(dudw1);
      pm(2,2) = 1. / dw2dw1;    // d(dudw2)/d(dudw1);
      pm(3,2) = 0.;             // d(dvdw2)/d(dudw1);
      pm(4,2) = 0.;             // d(pinv2)/d(dudw1);

      pm(0,3) = 0.;                                  // du2/d(dvdw1);
      pm(1,3) = 0.;                                  // dv2/d(dvdw1);
      pm(2,3) = dudw1 * sindphi / (dw2dw1*dw2dw1);   // d(dudw2)/d(dvdw1);
      pm(3,3) = 1. / (dw2dw1*dw2dw1);                // d(dvdw2)/d(dvdw1);
      pm(4,3) = 0.;                                  // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = u2;
    vec(1) = v2;
    vec(2) = dudw2;
    vec(3) = dvdw2;

    // Done (success).

    return true;
  }

  // Transform from SurfYZPlane to SurfXYZPlane.

  bool PropZero::transformYZPlane_XYZPlane(double y01, double z01, double phi1,
					   double x02, double y02, double z02,
					   double theta2, double phi2,
					   TrackVector& vec, Surface::TrackDirection& dir,
					   TrackMatrix* prop_matrix) const
  {
    // Calculate transcendental functions.

    double sinth2 = std::sin(theta2);
    double costh2 = std::cos(theta2);

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track state vector and track parameters.

    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);

    // Make sure initial track has a valid direction.

    if(dir == Surface::UNKNOWN)
      return false;

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = costh2;
    double ruv = sinth2*sindphi;
    double ruw = -sinth2*cosdphi;

    double rvv = cosdphi;
    double rvw = sindphi;

    double rwu = sinth2;
    double rwv = -costh2*sindphi;
    double rww = costh2*cosdphi;

    // Calculate elements of rotation matrix from global coordinate
    // system to destination coordinate system.

    double rux = costh2;
    double ruy = sinth2*sinphi2;
    double ruz = -sinth2*cosphi2;

    double rvy = cosphi2;
    double rvz = sinphi2;

    double rwx = sinth2;
    double rwy = -costh2*sinphi2;
    double rwz = costh2*cosphi2;

    // Calculate the initial position in the destination coordinate
    // system.

    double u2 = -x02*rux + (y01-y02)*ruy + (z01-z02)*ruz + u1*ruu + v1*ruv;
    double v2 =            (y01-y02)*rvy + (z01-z02)*rvz          + v1*rvv;
    double w2 = -x02*rwx + (y01-y02)*rwy + (z01-z02)*rwz + u1*rwu + v1*rwv;

    // Maximum distance cut.

    if(std::abs(w2) > fMaxDist)
      return false;

    // Calculate the derivative dw2/dw1;
    // If dw2/dw1 == 0., that means the track is moving parallel
    // to destination plane.
    // In this case return propagation failure.

    double dw2dw1 = dudw1*rwu + dvdw1*rwv + rww;
    if(dw2dw1 == 0.)
      return false;

    // Calculate slope in destination plane coordinates.

    double dudw2 = (dudw1*ruu + dvdw1*ruv + ruw) / dw2dw1;
    double dvdw2 = (dvdw1*rvv + rvw) / dw2dw1;

    // Calculate direction parameter at destination surface.
    // Direction will flip if dw2dw1 < 0.;

    switch (dir) {
      case Surface::FORWARD:
        dir = (dw2dw1 > 0.)? Surface::FORWARD: Surface::BACKWARD;
        break;
      case Surface::BACKWARD:
        dir = (dw2dw1 > 0.)? Surface::BACKWARD: Surface::FORWARD;
        break;
      default:
        throw cet::exception("PropXYZPlane") 
          << __func__ << ": unexpected direction #" << ((int) dir) << "\n";
    } // switch

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      pm(0,0) = ruu - dudw2*rwu;    // du2/du1
      pm(1,0) = -dvdw2*rwu;         // dv2/du1
      pm(2,0) = 0.;                 // d(dudw2)/du1
      pm(3,0) = 0.;                 // d(dvdw2)/du1
      pm(4,0) = 0.;                 // d(pinv2)/du1

      pm(0,1) = ruv - dudw2*rwv;    // du2/dv1
      pm(1,1) = rvv - dvdw2*rwv;    // dv2/dv1
      pm(2,1) = 0.;                 // d(dudw2)/dv1
      pm(3,1) = 0.;                 // d(dvdw2)/dv1
      pm(4,1) = 0.;                 // d(pinv2)/dv1

      pm(0,2) = 0.;                            // du2/d(dudw1);
      pm(1,2) = 0.;                            // dv2/d(dudw1);
      pm(2,2) = (ruu - dudw2*rwu) / dw2dw1;    // d(dudw2)/d(dudw1);
      pm(3,2) = -dvdw2*rwu / dw2dw1;           // d(dvdw2)/d(dudw1);
      pm(4,2) = 0.;                            // d(pinv2)/d(dudw1);

      pm(0,3) = 0.;                            // du2/d(dvdw1);
      pm(1,3) = 0.;                            // dv2/d(dvdw1);
      pm(2,3) = (ruv - dudw2*rwv) / dw2dw1;    // d(dudw2)/d(dvdw1);
      pm(3,3) = (rvv - dvdw2*rwv) / dw2dw1;    // d(dvdw2)/d(dvdw1);
      pm(4,3) = 0.;                            // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = u2;
    vec(1) = v2;
    vec(2) = dudw2;
    vec(3) = dvdw2;

    // Done (success).

    return true;
  }

  // Transform from SurfXYZPlane to SurfYZLine.

  bool PropZero::transformXYZPlane_YZLine(double x01, double y01, double z01,
					  double theta1, double phi1,
					  double x02, double y02, double z02, double phi2,
					  TrackVector& vec, Surface::TrackDirection& dir,
					  TrackMatrix* prop_matrix) const
  {
    // Calculate surface transcendental functions.

    double sinth1 = std::sin(theta1);
    double costh1 = std::cos(theta1);
    double sinth2 = 0.;
    double costh2 = 1.;

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track parameters.

    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);

    // Make sure initial track has a valid direction.

    double dirf = 1.;
    if(dir == Surface::BACKWARD)
      dirf = -1.;
    else if(dir != Surface::FORWARD)
      return false;

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = costh1*costh2 + sinth1*sinth2*cosdphi;
    double ruv = sinth2*sindphi;
    double ruw = sinth1*costh2 - costh1*sinth2*cosdphi;

    double rvu = -sinth1*sindphi;
    double rvv = cosdphi;
    double rvw = costh1*sindphi;

    double rwu = costh1*sinth2 - sinth1*costh2*cosdphi;
    double rwv = -costh2*sindphi;
    double rww = sinth1*sinth2 + costh1*costh2*cosdphi;

    // Calculate elements of rotation matrix from global coordinate
    // system to destination coordinate system.

    double rux = costh2;
    double ruy = sinth2*sinphi2;
    double ruz = -sinth2*cosphi2;

    double rvy = cosphi2;
    double rvz = sinphi2;

    double rwx = sinth2;
    double rwy = -costh2*sinphi2;
    double rwz = costh2*cosphi2;

    // Calculate the initial position in the destination coordinate
    // system.

    double u2 = (x01-x02)*rux + (y01-y02)*ruy + (z01-z02)*ruz + u1*ruu + v1*ruv;
    double v2 =                 (y01-y02)*rvy + (z01-z02)*rvz + u1*rvu + v1*rvv;
    double w2 = (x01-x02)*rwx + (y01-y02)*rwy + (z01-z02)*rwz + u1*rwu + v1*rwv;

    // Calculate direction in the starting coordinate system.

    double dw1 = dirf / std::sqrt(1. + dudw1*dudw1 + dvdw1*dvdw1);
    double du1 = dudw1 * dw1;
    double dv1 = dvdw1 * dw1;

    // Rotate direction vector into destination coordinate system.

    double du2 = ruu*du1 + ruv*dv1 + ruw*dw1;
    double dv2 = rvu*du1 + rvv*dv1 + rvw*dw1;
    double dw2 = rwu*du1 + rwv*dv1 + rww*dw1;
    double duw2 = std::hypot(du2, dw2);

    // Calculate final direction track parameters.

    double phid2 = atan2(dw2, du2);
    double eta2 = std::asinh(dv2 / duw2);

    // Calculate the impact parameter in the destination coordinate system.

    double r2 = (w2*du2 - u2*dw2) / duw2;   // w2*cos(phid2) - u2*sin(phid2)

    // Calculate the perpendicular propagation distance.
    // Should be zero if track is at surface.

    double st = -(w2*dw2 + u2*du2) / duw2;  // -(w2*sin(phid2) + u2*cos(phid2))

    // Maximum distance cut.

    if(std::abs(st) > fMaxDist)
      return false;

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      // Partials of initial positions and directions wrt initial t.p.'s.

      double du1du1 = 1.;
      double du1dv1 = 0.;
      double du1ddudw1 = 0.;
      double du1ddvdw1 = 0.;

      double dv1du1 = 0.;
      double dv1dv1 = 1.;
      double dv1ddudw1 = 0.;
      double dv1ddvdw1 = 0.;

      double dw1du1 = 0.;
      double dw1dv1 = 0.;
      double dw1ddudw1 = 0.;
      double dw1ddvdw1 = 0.; 

      double ddu1du1 = 0.;
      double ddu1dv1 = 0.;
      double ddu1ddudw1 = (1. + dvdw1*dvdw1) * dw1*dw1*dw1;
      double ddu1ddvdw1 = -dudw1 * dvdw1 * dw1*dw1*dw1;

      double ddv1du1 = 0.;
      double ddv1dv1 = 0.;
      double ddv1ddudw1 = -dudw1 * dvdw1 * dw1*dw1*dw1;
      double ddv1ddvdw1 = (1. + dudw1*dudw1) * dw1*dw1*dw1;

      double ddw1du1 = 0.;
      double ddw1dv1 = 0.;
      double ddw1ddudw1 = -dudw1 * dw1*dw1*dw1;
      double ddw1ddvdw1 = -dvdw1 * dw1*dw1*dw1;

      // Rotate partials to destination coordinate system.

      double du2du1 = ruu*du1du1 + ruv*dv1du1 + ruw*dw1du1;
      double dv2du1 = rvu*du1du1 + rvv*dv1du1 + rvw*dw1du1;
      double dw2du1 = rwu*du1du1 + rwv*dv1du1 + rww*dw1du1;

      double du2dv1 = ruu*du1dv1 + ruv*dv1dv1 + ruw*dw1dv1;
      double dv2dv1 = rvu*du1dv1 + rvv*dv1dv1 + rvw*dw1dv1;
      double dw2dv1 = rwu*du1dv1 + rwv*dv1dv1 + rww*dw1dv1;

      double du2ddudw1 = ruu*du1ddudw1 + ruv*dv1ddudw1 + ruw*dw1ddudw1;
      double dv2ddudw1 = rvu*du1ddudw1 + rvv*dv1ddudw1 + rvw*dw1ddudw1;
      double dw2ddudw1 = rwu*du1ddudw1 + rwv*dv1ddudw1 + rww*dw1ddudw1;
      
      double du2ddvdw1 = ruu*du1ddvdw1 + ruv*dv1ddvdw1 + ruw*dw1ddvdw1;
      double dv2ddvdw1 = rvu*du1ddvdw1 + rvv*dv1ddvdw1 + rvw*dw1ddvdw1;
      double dw2ddvdw1 = rwu*du1ddvdw1 + rwv*dv1ddvdw1 + rww*dw1ddvdw1;

      double ddu2du1 = ruu*ddu1du1 + ruv*ddv1du1 + ruw*ddw1du1;
      double ddv2du1 = rvu*ddu1du1 + rvv*ddv1du1 + rvw*ddw1du1;
      double ddw2du1 = rwu*ddu1du1 + rwv*ddv1du1 + rww*ddw1du1;

      double ddu2dv1 = ruu*ddu1dv1 + ruv*ddv1dv1 + ruw*ddw1dv1;
      double ddv2dv1 = rvu*ddu1dv1 + rvv*ddv1dv1 + rvw*ddw1dv1;
      double ddw2dv1 = rwu*ddu1dv1 + rwv*ddv1dv1 + rww*ddw1dv1;

      double ddu2ddudw1 = ruu*ddu1ddudw1 + ruv*ddv1ddudw1 + ruw*ddw1ddudw1;
      double ddv2ddudw1 = rvu*ddu1ddudw1 + rvv*ddv1ddudw1 + rvw*ddw1ddudw1;
      double ddw2ddudw1 = rwu*ddu1ddudw1 + rwv*ddv1ddudw1 + rww*ddw1ddudw1;

      double ddu2ddvdw1 = ruu*ddu1ddvdw1 + ruv*ddv1ddvdw1 + ruw*ddw1ddvdw1;
      double ddv2ddvdw1 = rvu*ddu1ddvdw1 + rvv*ddv1ddvdw1 + rvw*ddw1ddvdw1;
      double ddw2ddvdw1 = rwu*ddu1ddvdw1 + rwv*ddv1ddvdw1 + rww*ddw1ddvdw1;

      // Partials of final t.p. wrt final position and direction.

      double dr2du2 = -dw2/duw2;
      double dr2dv2 = 0.;
      double dr2dw2 = du2/duw2;
      double dr2ddu2 = w2/duw2;
      double dr2ddv2 = r2*dv2/(duw2*duw2);
      double dr2ddw2 = -u2/duw2;

      double dphi2du2 = 0.;
      double dphi2dv2 = 0.;
      double dphi2dw2 = 0.;
      double dphi2ddu2 = -dw2/(duw2*duw2);
      double dphi2ddv2 = 0.;
      double dphi2ddw2 = du2/(duw2*duw2);

      double deta2du2 = 0.;
      double deta2dv2 = 0.;
      double deta2dw2 = 0.;
      double deta2ddu2 = 0.;
      double deta2ddv2 = 1./(duw2*duw2);
      double deta2ddw2 = 0.;

      // Partials of final t.p. wrt initial t.p.

      double dr2du1 =    dr2du2*du2du1 +   dr2dv2*dv2du1 +   dr2dw2*dw2du1
                     + dr2ddu2*ddu2du1 + dr2ddv2*ddv2du1 + dr2ddw2*ddw2du1;
      double dr2dv1 =    dr2du2*du2dv1 +   dr2dv2*dv2dv1 +   dr2dw2*dw2dv1
                     + dr2ddu2*ddu2dv1 + dr2ddv2*ddv2dv1 + dr2ddw2*ddw2dv1;
      double dr2ddudw1 =   dr2du2*du2ddudw1 +   dr2dv2*dv2ddudw1 +   dr2dw2*dw2ddudw1
                       + dr2ddu2*ddu2ddudw1 + dr2ddv2*ddv2ddudw1 + dr2ddw2*ddw2ddudw1;
      double dr2ddvdw1 =    dr2du2*du2ddvdw1 +   dr2dv2*dv2ddvdw1 +   dr2dw2*dw2ddvdw1
                       + dr2ddu2*ddu2ddvdw1 + dr2ddv2*ddv2ddvdw1 + dr2ddw2*ddw2ddvdw1;

      double dphi2du1 =    dphi2du2*du2du1 +   dphi2dv2*dv2du1 +   dphi2dw2*dw2du1
                       + dphi2ddu2*ddu2du1 + dphi2ddv2*ddv2du1 + dphi2ddw2*ddw2du1;
      double dphi2dv1 =    dphi2du2*du2dv1 +   dphi2dv2*dv2dv1 +   dphi2dw2*dw2dv1
                       + dphi2ddu2*ddu2dv1 + dphi2ddv2*ddv2dv1 + dphi2ddw2*ddw2dv1;
      double dphi2ddudw1 =   dphi2du2*du2ddudw1 +   dphi2dv2*dv2ddudw1 +   dphi2dw2*dw2ddudw1
                         + dphi2ddu2*ddu2ddudw1 + dphi2ddv2*ddv2ddudw1 + dphi2ddw2*ddw2ddudw1;
      double dphi2ddvdw1 =   dphi2du2*du2ddvdw1 +   dphi2dv2*dv2ddvdw1 +   dphi2dw2*dw2ddvdw1
                         + dphi2ddu2*ddu2ddvdw1 + dphi2ddv2*ddv2ddvdw1 + dphi2ddw2*ddw2ddvdw1;

      double deta2du1 =    deta2du2*du2du1 +   deta2dv2*dv2du1 +   deta2dw2*dw2du1
                       + deta2ddu2*ddu2du1 + deta2ddv2*ddv2du1 + deta2ddw2*ddw2du1;
      double deta2dv1 =    deta2du2*du2dv1 +   deta2dv2*dv2dv1 +   deta2dw2*dw2dv1
                       + deta2ddu2*ddu2dv1 + deta2ddv2*ddv2dv1 + deta2ddw2*ddw2dv1;
      double deta2ddudw1 =   deta2du2*du2ddudw1 +   deta2dv2*dv2ddudw1 +   deta2dw2*dw2ddudw1
                         + deta2ddu2*ddu2ddudw1 + deta2ddv2*ddv2ddudw1 + deta2ddw2*ddw2ddudw1;
      double deta2ddvdw1 =   deta2du2*du2ddvdw1 +   deta2dv2*dv2ddvdw1 +   deta2dw2*dw2ddvdw1
                         + deta2ddu2*ddu2ddvdw1 + deta2ddv2*ddv2ddvdw1 + deta2ddw2*ddw2ddvdw1;

      // We still need to calculate the corretion due to the dependence of the
      // propagation distance on the initial track parameters.  This correction is
      // needed even though the actual propagation distance is zero.

      // This correction only effects the v track parameter, since the v parameter
      // the only parameter that actually dependes on the propagation distance.

      // Partials of propagation distance wrt position and direction in the destination
      // coordinate system.

      double dsdu2 = -du2/(duw2*duw2);
      double dsdv2 = 0.;
      double dsdw2 = -dw2/(duw2*duw2);
      double dsddu2 = -u2/(duw2*duw2);
      double dsddv2 = st*dv2/(duw2*duw2*duw2);
      double dsddw2 = -w2/(duw2*duw2);

      // Partials of propagation distance wrt initial t.p.

      double dsdu1 =    dsdu2*du2du1 +   dsdv2*dv2du1 +   dsdw2*dw2du1
                    + dsddu2*ddu2du1 + dsddv2*ddv2du1 + dsddw2*ddw2du1;
      double dsdv1 =    dsdu2*du2dv1 +   dsdv2*dv2dv1 +   dsdw2*dw2dv1
                    + dsddu2*ddu2dv1 + dsddv2*ddv2dv1 + dsddw2*ddw2dv1;
      double dsddudw1 =   dsdu2*du2ddudw1 +   dsdv2*dv2ddudw1 +   dsdw2*dw2ddudw1
                      + dsddu2*ddu2ddudw1 + dsddv2*ddv2ddudw1 + dsddw2*ddw2ddudw1;
      double dsddvdw1 =   dsdu2*du2ddvdw1 +   dsdv2*dv2ddvdw1 +   dsdw2*dw2ddvdw1
                      + dsddu2*ddu2ddvdw1 + dsddv2*ddv2ddvdw1 + dsddw2*ddw2ddvdw1;

      // Calculate correction to v parameter partials wrt initial t.p. due to path length.

      dv2du1 += dv2*dsdu1;
      dv2dv1 += dv2*dsdv1;
      dv2ddudw1 += dv2*dsddudw1;
      dv2ddvdw1 += dv2*dsddvdw1;

      // Fill matrix.

      pm(0,0) = dr2du1;     // dr2/du1
      pm(1,0) = dv2du1;     // dv2/du1
      pm(2,0) = dphi2du1;   // d(phi2)/du1
      pm(3,0) = deta2du1;   // d(eta2)/du1
      pm(4,0) = 0.;         // d(pinv2)/du1

      pm(0,1) = dr2dv1;     // dr2/dv1
      pm(1,1) = dv2dv1;     // dv2/dv1
      pm(2,1) = dphi2dv1;   // d(phi2)/dv1
      pm(3,1) = deta2dv1;   // d(eta2)/dv1
      pm(4,1) = 0.;         // d(pinv2)/dv1

      pm(0,2) = dr2ddudw1;     // dr2/d(dudw1);
      pm(1,2) = dv2ddudw1;     // dv2/d(dudw1);
      pm(2,2) = dphi2ddudw1;   // d(dudw2)/d(dudw1);
      pm(3,2) = deta2ddudw1;   // d(eta2)/d(dudw1);
      pm(4,2) = 0.;            // d(pinv2)/d(dudw1);

      pm(0,3) = dr2ddvdw1;     // dr2/d(dvdw1);
      pm(1,3) = dv2ddvdw1;     // dv2/d(dvdw1);
      pm(2,3) = dphi2ddvdw1;   // d(phi2)/d(dvdw1);
      pm(3,3) = deta2ddvdw1;   // d(eta2)/d(dvdw1);
      pm(4,3) = 0.;            // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // dr2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(phi2)/d(pinv1);
      pm(3,4) = 0.;      // d(eta2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = r2;
    vec(1) = v2;
    vec(2) = phid2;
    vec(3) = eta2;

    // Done (success).

    return true;
  }

  // Transform from SurfXYZPlane to SurfYZPlane.

  bool PropZero::transformXYZPlane_YZPlane(double x01, double y01, double z01,
					   double theta1, double phi1,
					   double y02, double z02, double phi2,
					   TrackVector& vec, Surface::TrackDirection& dir,
					   TrackMatrix* prop_matrix) const
  {
    // Calculate transcendental functions.

    double sinth1 = std::sin(theta1);
    double costh1 = std::cos(theta1);

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track state vector and track parameters.

    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);

    // Make sure initial track has a valid direction.

    if(dir == Surface::UNKNOWN)
      return false;

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = costh1;
    double ruw = sinth1;

    double rvu = -sinth1*sindphi;
    double rvv = cosdphi;
    double rvw = costh1*sindphi;

    double rwu = -sinth1*cosdphi;
    double rwv = -sindphi;
    double rww = costh1*cosdphi;

    // Calculate elements of rotation matrix from global coordinate
    // system to destination coordinate system.

    double rvy = cosphi2;
    double rvz = sinphi2;

    double rwy = -sinphi2;
    double rwz = cosphi2;

    // Calculate the initial position in the destination coordinate
    // system.

    double u2 = x01 + u1*ruu;
    double v2 = (y01-y02)*rvy + (z01-z02)*rvz + u1*rvu + v1*rvv;
    double w2 = (y01-y02)*rwy + (z01-z02)*rwz + u1*rwu + v1*rwv;

    // Maximum distance cut.

    if(std::abs(w2) > fMaxDist)
      return false;

    // Calculate the derivative dw2/dw1;
    // If dw2/dw1 == 0., that means the track is moving parallel
    // to destination plane.
    // In this case return propagation failure.

    double dw2dw1 = dudw1*rwu + dvdw1*rwv + rww;
    if(dw2dw1 == 0.)
      return false;

    // Calculate slope in destination plane coordinates.

    double dudw2 = (dudw1*ruu + ruw) / dw2dw1;
    double dvdw2 = (dudw1*rvu + dvdw1*rvv + rvw) / dw2dw1;

    // Calculate direction parameter at destination surface.
    // Direction will flip if dw2dw1 < 0.;

    switch (dir) {
      case Surface::FORWARD:
        dir = (dw2dw1 > 0.)? Surface::FORWARD: Surface::BACKWARD;
        break;
      case Surface::BACKWARD:
        dir = (dw2dw1 > 0.)? Surface::BACKWARD: Surface::FORWARD;
        break;
      default:
        throw cet::exception("PropXYZPlane") 
          << __func__ << ": unexpected direction #" << ((int) dir) << "\n";
    } // switch

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      pm(0,0) = ruu - dudw2*rwu;    // du2/du1
      pm(1,0) = rvu - dvdw2*rwu;    // dv2/du1
      pm(2,0) = 0.;                 // d(dudw2)/du1
      pm(3,0) = 0.;                 // d(dvdw2)/du1
      pm(4,0) = 0.;                 // d(pinv2)/du1

      pm(0,1) = -dudw2*rwv;         // du2/dv1
      pm(1,1) = rvv - dvdw2*rwv;    // dv2/dv1
      pm(2,1) = 0.;                 // d(dudw2)/dv1
      pm(3,1) = 0.;                 // d(dvdw2)/dv1
      pm(4,1) = 0.;                 // d(pinv2)/dv1

      pm(0,2) = 0.;                            // du2/d(dudw1);
      pm(1,2) = 0.;                            // dv2/d(dudw1);
      pm(2,2) = (ruu - dudw2*rwu) / dw2dw1;    // d(dudw2)/d(dudw1);
      pm(3,2) = (rvu - dvdw2*rwu) / dw2dw1;    // d(dvdw2)/d(dudw1);
      pm(4,2) = 0.;                            // d(pinv2)/d(dudw1);

      pm(0,3) = 0.;                            // du2/d(dvdw1);
      pm(1,3) = 0.;                            // dv2/d(dvdw1);
      pm(2,3) = -dudw2*rwv / dw2dw1;           // d(dudw2)/d(dvdw1);
      pm(3,3) = (rvv - dvdw2*rwv) / dw2dw1;    // d(dvdw2)/d(dvdw1);
      pm(4,3) = 0.;                            // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = u2;
    vec(1) = v2;
    vec(2) = dudw2;
    vec(3) = dvdw2;

    // Done (success).

    return true;
  }

  // Transform from SurfXYZPlane to SurfXYZPlane.

  bool PropZero::transformXYZPlane_XYZPlane(double x01, double y01, double z01,
					    double theta1, double phi1,
					    double x02, double y02, double z02,
					    double theta2, double phi2,
					    TrackVector& vec, Surface::TrackDirection& dir,
					    TrackMatrix* prop_matrix) const
  {
    // Calculate transcendental functions.

    double sinth1 = std::sin(theta1);
    double costh1 = std::cos(theta1);
    double sinth2 = std::sin(theta2);
    double costh2 = std::cos(theta2);

    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);
    double sindphi = std::sin(phi2 - phi1);
    double cosdphi = std::cos(phi2 - phi1);

    // Get the initial track state vector and track parameters.

    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);

    // Make sure initial track has a valid direction.

    if(dir == Surface::UNKNOWN)
      return false;

    // Calculate elements of rotation matrix from initial coordinate
    // system to destination coordinte system.

    double ruu = costh1*costh2 + sinth1*sinth2*cosdphi;
    double ruv = sinth2*sindphi;
    double ruw = sinth1*costh2 - costh1*sinth2*cosdphi;

    double rvu = -sinth1*sindphi;
    double rvv = cosdphi;
    double rvw = costh1*sindphi;

    double rwu = costh1*sinth2 - sinth1*costh2*cosdphi;
    double rwv = -costh2*sindphi;
    double rww = sinth1*sinth2 + costh1*costh2*cosdphi;

    // Calculate elements of rotation matrix from global coordinate
    // system to destination coordinate system.

    double rux = costh2;
    double ruy = sinth2*sinphi2;
    double ruz = -sinth2*cosphi2;

    double rvy = cosphi2;
    double rvz = sinphi2;

    double rwx = sinth2;
    double rwy = -costh2*sinphi2;
    double rwz = costh2*cosphi2;

    // Calculate the initial position in the destination coordinate
    // system.

    double u2 = (x01-x02)*rux + (y01-y02)*ruy + (z01-z02)*ruz + u1*ruu + v1*ruv;
    double v2 =                 (y01-y02)*rvy + (z01-z02)*rvz + u1*rvu + v1*rvv;
    double w2 = (x01-x02)*rwx + (y01-y02)*rwy + (z01-z02)*rwz + u1*rwu + v1*rwv;

    // Maximum distance cut.

    if(std::abs(w2) > fMaxDist)
      return false;

    // Calculate the derivative dw2/dw1;
    // If dw2/dw1 == 0., that means the track is moving parallel
    // to destination plane.
    // In this case return propagation failure.

    double dw2dw1 = dudw1*rwu + dvdw1*rwv + rww;
    if(dw2dw1 == 0.)
      return false;

    // Calculate slope in destination plane coordinates.

    double dudw2 = (dudw1*ruu + dvdw1*ruv + ruw) / dw2dw1;
    double dvdw2 = (dudw1*rvu + dvdw1*rvv + rvw) / dw2dw1;

    // Calculate direction parameter at destination surface.
    // Direction will flip if dw2dw1 < 0.;

    switch (dir) {
      case Surface::FORWARD:
        dir = (dw2dw1 > 0.)? Surface::FORWARD: Surface::BACKWARD;
        break;
      case Surface::BACKWARD:
        dir = (dw2dw1 > 0.)? Surface::BACKWARD: Surface::FORWARD;
        break;
      default:
        throw cet::exception("PropXYZPlane") 
          << __func__ << ": unexpected direction #" << ((int) dir) << "\n";
    } // switch

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix& pm = *prop_matrix;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      pm(0,0) = ruu - dudw2*rwu;    // du2/du1
      pm(1,0) = rvu - dvdw2*rwu;    // dv2/du1
      pm(2,0) = 0.;                 // d(dudw2)/du1
      pm(3,0) = 0.;                 // d(dvdw2)/du1
      pm(4,0) = 0.;                 // d(pinv2)/du1

      pm(0,1) = ruv - dudw2*rwv;    // du2/dv1
      pm(1,1) = rvv - dvdw2*rwv;    // dv2/dv1
      pm(2,1) = 0.;                 // d(dudw2)/dv1
      pm(3,1) = 0.;                 // d(dvdw2)/dv1
      pm(4,1) = 0.;                 // d(pinv2)/dv1

      pm(0,2) = 0.;                            // du2/d(dudw1);
      pm(1,2) = 0.;                            // dv2/d(dudw1);
      pm(2,2) = (ruu - dudw2*rwu) / dw2dw1;    // d(dudw2)/d(dudw1);
      pm(3,2) = (rvu - dvdw2*rwu) / dw2dw1;    // d(dvdw2)/d(dudw1);
      pm(4,2) = 0.;                            // d(pinv2)/d(dudw1);

      pm(0,3) = 0.;                            // du2/d(dvdw1);
      pm(1,3) = 0.;                            // dv2/d(dvdw1);
      pm(2,3) = (ruv - dudw2*rwv) / dw2dw1;    // d(dudw2)/d(dvdw1);
      pm(3,3) = (rvv - dvdw2*rwv) / dw2dw1;    // d(dvdw2)/d(dvdw1);
      pm(4,3) = 0.;                            // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    }

    // Update track vector.

    vec(0) = u2;
    vec(1) = v2;
    vec(2) = dudw2;
    vec(3) = dvdw2;

    // Done (success).

    return true;
  }

} // end namespace trkf
