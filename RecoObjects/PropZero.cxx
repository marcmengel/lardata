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

    if(const SurfYZPlane* from = dynamic_cast<const SurfYZPlane*>(&*trk.getSurface())) {

      // Initial surface is SurfYZPlane.
      // Get surface paramters.

      double y01 = from->y0();
      double z01 = from->z0();
      double phi1 = from->phi();

      if(const SurfYZPlane* to = dynamic_cast<const SurfYZPlane*>(&*psurf)) {

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

      if(const SurfYZPlane* to = dynamic_cast<const SurfYZPlane*>(&*psurf)) {

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
