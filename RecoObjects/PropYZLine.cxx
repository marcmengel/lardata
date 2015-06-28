///////////////////////////////////////////////////////////////////////
///
/// \file   PropYZLine.cxx
///
/// \brief  Propagate to SurfYZLine surface.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "RecoObjects/PropYZLine.h"
#include "RecoObjects/SurfYZLine.h"
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
  PropYZLine::PropYZLine(double tcut, bool doDedx) :
    Propagator(tcut, doDedx, std::shared_ptr<const Interactor>(new InteractPlane(tcut)))
  {}

  /// Destructor.
  PropYZLine::~PropYZLine()
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
  PropYZLine::short_vec_prop(KTrack& trk,
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

    const SurfYZLine* to = dynamic_cast<const SurfYZLine*>(&*psurf);
    if(to == 0)
      return result;
    double x02 = to->x0();
    double y02 = to->y0();
    double z02 = to->z0();
    double phi2 = to->phi();

    // Generate an intermediate surface that coincides with track position, but
    // is parallel to destination surface.

    double xyz[3];
    trk.getPosition(xyz);
    double x01 = xyz[0];
    double y01 = xyz[1];
    double z01 = xyz[2];
    const std::shared_ptr<const Surface> psurf1(new SurfYZLine(x01, y01, z01, phi2));

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
      throw cet::exception("PropYZLine") 
	<< "Track state vector has wrong size" << vec.size() << "\n";
    double r1 = vec(0);
    double v1 = vec(1);
    double phid1 = vec(2);
    double eta1 = vec(3);
    double pinv = vec(4);

    // Calculate transcendental functions.

    double sinphid1 = std::sin(phid1);
    double cosphid1 = std::cos(phid1);
    double sh1 = std::sinh(eta1);
    double ch1 = std::cosh(eta1);
    double sinphi2 = std::sin(phi2);
    double cosphi2 = std::cos(phi2);

    // Calculate the initial position in the intermediate coordinate system.

    double u1 = -r1 * sinphid1;
    double w1 = r1 * cosphid1;

    // Calculate initial position in the destination coordinate system.

    double u2 = x01 - x02 + u1;
    double v2 = (y01 - y02) * cosphi2 + (z01 - z02) * sinphi2 + v1;
    double w2 = -(y01 - y02) * sinphi2 + (z01 - z02) * cosphi2 + w1;

    // Calculate the impact parameter in the destination coordinate system.

    double r2 = w2 * cosphid1 - u2 * sinphid1;

    // Calculate the perpendicular propagation distance.

    double d2 = -(w2 * sinphid1 + u2 * cosphid1);

    // Calculate the final position in the destination coordinate system.

    //double u2p = -r2 * sinphid1;
    double v2p = v2 + d2 * sh1;
    //double w2p = r2 * cosphid1;

    // Calculate the signed propagation distance.

    double s = d2 * ch1;

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

      pm(0,0) = 1.;      // dr2/dr1
      pm(1,0) = 0.;      // dv2/dr1
      pm(2,0) = 0.;      // d(phi2)/dr1
      pm(3,0) = 0.;      // d(eta2)/dr1
      pm(4,0) = 0.;      // d(pinv2)/dr1

      pm(0,1) = 0.;      // dr2/dv1
      pm(1,1) = 1.;      // dv2/dv1
      pm(2,1) = 0.;      // d(phi2)/dv1
      pm(3,1) = 0.;      // d(eta2)/dv1
      pm(4,1) = 0.;      // d(pinv2)/dv1

      pm(0,2) = d2;      // dr2/d(phi1);
      pm(1,2) = -r2*sh1; // dv2/d(phi1);
      pm(2,2) = 1.;      // d(phi2)/d(phi1);
      pm(3,2) = 0.;      // d(eta2)/d(phi1);
      pm(4,2) = 0.;      // d(pinv2)/d(phi1);

      pm(0,3) = 0.;      // dr2/d(eta1);
      pm(1,3) = d2*ch1;  // dv2/d(eta1);
      pm(2,3) = 0.;      // d(phi2)/d(eta1);
      pm(3,3) = 1.;      // d(eta2)/d(eta1);
      pm(4,3) = 0.;      // d(pinv2)/d(eta1);

      pm(0,4) = 0.;      // dr2/d(pinv1);
      pm(1,4) = 0.;      // dv2/d(pinv1);
      pm(2,4) = 0.;      // d(phi2)/d(pinv1);
      pm(3,4) = 0.;      // d(eta2)/d(pinv1);
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
    vec2(0) = r2;
    vec2(1) = v2p;
    vec2(2) = phid1;
    vec2(3) = eta1;
    vec2(4) = *pinv2;

    // Update track.

    trk.setSurface(psurf);
    trk.setVector(vec2);

    // Done.

    return result;
  }

} // end namespace trkf
