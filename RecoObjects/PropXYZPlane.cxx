///////////////////////////////////////////////////////////////////////
///
/// \file   PropXYZPlane.cxx
///
/// \brief  Propagate between two SurfXYZPlane or SurfYZPlane surfaces.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "RecoObjects/PropXYZPlane.h"
#include "RecoObjects/SurfYZPlane.h"
#include "RecoObjects/SurfXYZPlane.h"
#include "RecoObjects/InteractPlane.h"
#include "cetlib/exception.h"

namespace trkf {

  /// Constructor.
  ///
  /// Arguments.
  ///
  /// tcut   - Delta ray energy cutoff for calculating dE/dx.
  /// doDedx - dE/dx enable flag.
  ///
  PropXYZPlane::PropXYZPlane(double tcut, bool doDedx) :
    Propagator(tcut, doDedx, std::shared_ptr<const Interactor>(new InteractPlane(tcut)))
  {}

  /// Destructor.
  PropXYZPlane::~PropXYZPlane()
  {}

  /// Propagate without error.
  /// Optionally return propagation matrix and noise matrix.
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
  PropXYZPlane::short_vec_prop(KTrack& trk,
			       const std::shared_ptr<const Surface>& psurf, 
			       Propagator::PropDirection dir,
			       bool doDedx,
			       TrackMatrix* prop_matrix,
			       TrackError* noise_matrix) const
  {
    // Set the default return value to be unitialized with value 0.

    boost::optional<double> result(false, 0.);

    // Get destination surface and surface parameters.
    // Return failure if wrong surface type.

    const SurfXYZPlane* to = dynamic_cast<const SurfXYZPlane*>(&*psurf);
    if(to == 0)
      return result;
    double x02 = to->x0();
    double y02 = to->y0();
    double z02 = to->z0();
    double theta2 = to->theta();
    double phi2 = to->phi();

    // Generate an intermediate surface that coincides with track position, but
    // is parallel to destination surface.

    double xyz[3];
    trk.getPosition(xyz);
    double x01 = xyz[0];
    double y01 = xyz[1];
    double z01 = xyz[2];
    const std::shared_ptr<const Surface> psurf1(new SurfXYZPlane(x01, y01, z01, phi2, theta2));

    // Do zero-distance propagation to intermediate surface.

    TrackMatrix local_prop_matrix;
    TrackMatrix* plocal_prop_matrix = (prop_matrix==0 ? 0 : &local_prop_matrix);
    boost::optional<double> result1 = fPropZero.short_vec_prop(trk, psurf1, dir, false,
							       plocal_prop_matrix, 0);
    if(!result1)
      return result1;

    // Get the intermediate track state vector and track parameters.

    const TrackVector& vec = trk.getVector();
    if(vec.size() != 5)
      throw cet::exception("PropXYZPlane") 
	<< "Track state vector has wrong size" << vec.size() << "\n";
    double u1 = vec(0);
    double v1 = vec(1);
    double dudw1 = vec(2);
    double dvdw1 = vec(3);
    double pinv = vec(4);
    Surface::TrackDirection dir1 = trk.getDirection();

    // Make sure intermediate track has a valid direction.

    if(dir1 == Surface::UNKNOWN)
      return result;

    // Calculate transcendental functions.

    double sinth2 = std::sin(theta2);
    double costh2 = std::cos(theta2);
    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);

    // Calculate elements of rotation matrix from global coordinate
    // system to destination coordinate system.

    double rux = costh2;
    double ruy = sinth2*sinphi2;
    double ruz = -sinth2*cosphi2;

    double rvx = 0.;
    double rvy = cosphi2;
    double rvz = sinphi2;

    double rwx = sinth2;
    double rwy = -costh2*sinphi2;
    double rwz = costh2*cosphi2;

    // Calculate the initial position in the destination coordinate
    // system.

    double u2 = (x01-x02)*rux + (y01-y02)*ruy + (z01-z02)*ruz + u1;
    double v2 = (x01-x02)*rvx + (y01-y02)*rvy + (z01-z02)*rvz + v1;
    double w2 = (x01-x02)*rwx + (y01-y02)*rwy + (z01-z02)*rwz;

    // Calculate position at destination surface (propagate distance -w2).

    double u2p = u2 - w2 * dudw1;
    double v2p = v2 - w2 * dvdw1;

    // Calculate the signed propagation distance.

    double s = -w2 * std::sqrt(1. + dudw1*dudw1 + dvdw1*dvdw1);
    if(dir1 == Surface::BACKWARD)
      s = -s;

    // Check if propagation was in the right direction.
    // (Compare sign of s with requested direction).

    bool sok = (dir == Propagator::UNKNOWN ||
		(dir == Propagator::FORWARD && s >= 0.) ||
		(dir == Propagator::BACKWARD && s <= 0.));

    // If wrong direction, return failure without updating the track
    // or propagation matrix.

    if(!sok)
      return result;

    // Find final momentum.

    double deriv = 1.;
    boost::optional<double> pinv2(true, pinv);
    if(getDoDedx() && doDedx && s != 0.) {
      double* pderiv = (prop_matrix != 0 ? &deriv : 0);
      pinv2 = dedx_prop(pinv, trk.Mass(), s, pderiv);
    }

    // Return failure in case of range out.

    if(!pinv2)
      return result;

    // Update result object (success guaranteed).

    result = boost::optional<double>(true, s);		

    // Update propagation matrix (if requested).

    if(prop_matrix != 0) {
      TrackMatrix pm;
      pm.resize(vec.size(), vec.size(), false);

      // Calculate partial derivatives.

      pm(0,0) = 1.;      // du2/du1
      pm(1,0) = 0.;      // dv2/du1
      pm(2,0) = 0.;      // d(dudw2)/du1
      pm(3,0) = 0.;      // d(dvdw2)/du1
      pm(4,0) = 0.;      // d(pinv2)/du1

      pm(0,1) = 0.;      // du2/dv1
      pm(1,1) = 1.;      // dv2/dv1
      pm(2,1) = 0.;      // d(dudw2)/dv1
      pm(3,1) = 0.;      // d(dvdw2)/dv1
      pm(4,1) = 0.;      // d(pinv2)/dv1

      pm(0,2) = -w2;     // du2/d(dudw1);
      pm(1,2) = 0.;      // dv2/d(dudw1);
      pm(2,2) = 1.;      // d(dudw2)/d(dudw1);
      pm(3,2) = 0.;      // d(dvdw2)/d(dudw1);
      pm(4,2) = 0.;      // d(pinv2)/d(dudw1);

      pm(0,3) = 0.;      // du2/d(dvdw1);
      pm(1,3) = -w2;     // dv2/d(dvdw1);
      pm(2,3) = 0.;      // d(dudw2)/d(dvdw1);
      pm(3,3) = 1.;      // d(dvdw2)/d(dvdw1);
      pm(4,3) = 0.;      // d(pinv2)/d(dvdw1);

      pm(0,4) = 0.;      // du2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
      pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
      pm(4,4) = deriv;   // d(pinv2)/d(pinv1);

      // Compose the final propagation matrix from zero-distance propagation and
      // parallel surface propagation.

      *prop_matrix = prod(pm, *plocal_prop_matrix);
    }

    // Update noise matrix (if requested).

    if(noise_matrix != 0) {
      noise_matrix->resize(vec.size(), vec.size(), false);
      if(getInteractor().get() != 0)
	getInteractor()->noise(trk, s, *noise_matrix);
      else
	noise_matrix->clear();
    }

    // Construct track vector at destination surface.

    TrackVector vec2(vec.size());
    vec2(0) = u2p;
    vec2(1) = v2p;
    vec2(2) = dudw1;
    vec2(3) = dvdw1;
    vec2(4) = *pinv2;

    // Update track.

    trk.setSurface(psurf);
    trk.setVector(vec2);

    // Done.

    return result;
  }

} // end namespace trkf
