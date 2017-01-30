///////////////////////////////////////////////////////////////////////
///
/// \file   PropagatorXXX.cxx
///
/// \brief  Base class for Kalman filter propagator.
///
/// \author H. Greenlee
///
////////////////////////////////////////////////////////////////////////

#include "lardata/RecoObjects/PropagatorXXX.h"
#include "lardata/RecoObjects/SurfXYZPlane.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "cetlib/exception.h"
#include "boost/optional.hpp"

using namespace recob::tracking;

namespace trkf {

  /// Constructor.
  ///
  /// Arguments:
  ///
  /// tcut   - Maximum delta ray energy.
  /// doDedx - dE/dx enable flag.
  ///
  PropagatorXXX::PropagatorXXX(double maxStep, double tcut, const std::shared_ptr<const Interactor>& interactor) :
    fMaxStep(maxStep),fTcut(tcut),fDoDedx(true),fInteractor(interactor)
  {}

  /// Destructor.
  PropagatorXXX::~PropagatorXXX()
  {}

  using PropDirection = PropagatorXXX::PropDirection;

  bool PropagatorXXX::propagateToPlane(const TrackState& origin, const Plane& target, TrackState& result, PropDirection dir) const {
    SVector5 orpar5 = origin.parameters();
    //first, translation and rotation of origin plane to be centered on initial position and parallel to target
    const double sinth1 = origin.plane().sinAlpha();
    const double costh1 = origin.plane().cosAlpha();
    const double sinth2 = target.sinAlpha();
    const double costh2 = target.cosAlpha();
    const double sinph1 = origin.plane().sinBeta();
    const double cosph1 = origin.plane().cosBeta();
    const double sinph2 = target.sinBeta();
    const double cosph2 = target.cosBeta();
    const double sindphi = sinph2*cosph1 - cosph2*sinph1;
    const double cosdphi = cosph2*cosph1 + sinph2*sinph1;
    const double ruu = costh1*costh2 + sinth1*sinth2*cosdphi;
    const double ruv = sinth2*sindphi;
    const double ruw = sinth1*costh2 - costh1*sinth2*cosdphi;
    const double rvu = -sinth1*sindphi;
    const double rvv = cosdphi;
    const double rvw = costh1*sindphi;
    const double rwu = costh1*sinth2 - sinth1*costh2*cosdphi;
    const double rwv = -costh2*sindphi;
    const double rww = sinth1*sinth2 + costh1*costh2*cosdphi;
    const double dw2dw1 = orpar5[2]*rwu + orpar5[3]*rwv + rww;
    if(dw2dw1 == 0.) return false;
    const double dudw2 = (orpar5[2]*ruu + orpar5[3]*ruv + ruw) / dw2dw1;
    const double dvdw2 = (orpar5[2]*rvu + orpar5[3]*rvv + rvw) / dw2dw1;
    SMatrix55 pm;
    //
    pm(0,0) = ruu /*- dudw2*rwu*/;    // du2/du1 //fixme check second term
    pm(1,0) = rvu /*- dvdw2*rwu*/;    // dv2/du1
    pm(2,0) = 0.;                 // d(dudw2)/du1
    pm(3,0) = 0.;                 // d(dvdw2)/du1
    pm(4,0) = 0.;                 // d(pinv2)/du1
    //
    pm(0,1) = ruv /*- dudw2*rwv*/;    // du2/dv1
    pm(1,1) = rvv /*- dvdw2*rwv*/;    // dv2/dv1
    pm(2,1) = 0.;                 // d(dudw2)/dv1
    pm(3,1) = 0.;                 // d(dvdw2)/dv1
    pm(4,1) = 0.;                 // d(pinv2)/dv1
    //
    pm(0,2) = 0.;                            // du2/d(dudw1);
    pm(1,2) = 0.;                            // dv2/d(dudw1);
    pm(2,2) = (ruu - dudw2*rwu) / dw2dw1;    // d(dudw2)/d(dudw1);
    pm(3,2) = (rvu - dvdw2*rwu) / dw2dw1;    // d(dvdw2)/d(dudw1);
    pm(4,2) = 0.;                            // d(pinv2)/d(dudw1);
    //
    pm(0,3) = 0.;                            // du2/d(dvdw1);
    pm(1,3) = 0.;                            // dv2/d(dvdw1);
    pm(2,3) = (ruv - dudw2*rwv) / dw2dw1;    // d(dudw2)/d(dvdw1);
    pm(3,3) = (rvv - dvdw2*rwv) / dw2dw1;    // d(dvdw2)/d(dvdw1);
    pm(4,3) = 0.;                            // d(pinv2)/d(dvdw1);
    //
    pm(0,4) = 0.;      // du2/d(pinv1);
    pm(1,4) = 0.;      // dv2/d(pinv1);
    pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
    pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
    pm(4,4) = 1.;      // d(pinv2)/d(pinv1);
    //
    orpar5[0] = 0.;
    orpar5[1] = 0.;
    orpar5[2] = dudw2;
    orpar5[3] = dvdw2;
    //
    result = TrackState(orpar5,ROOT::Math::Similarity(pm,origin.covariance()),Plane(origin.position(),target.direction()),origin.mass());
    //
    //now do the propagation
    bool arrived = false;
    while (!arrived) {
      //compute the distance to destination
      float distance = distanceToPlane(result, target, dir);
      float s = fMaxStep;
      if (distance<s) {
	s = distance;
	arrived = true;
      }
      bool stepDone = false;
      result = propagatedStateByPath(result, s, stepDone);
      if (!stepDone) return false;
    }
    return true;
  }

  bool PropagatorXXX::propagateToPlaneNoMaterial(const TrackState& origin, const Plane& target, TrackState& result, PropDirection dir) const {
    return true;
  }

  TrackState PropagatorXXX::propagatedStateByPath(const TrackState& origin, const float s, bool& success) const {
    const SVector5& orig5d = origin.parameters();
    SVector5 dest5d(orig5d(0)+s*orig5d(2),orig5d(1)+s*orig5d(3),orig5d(2),orig5d(3),orig5d(4));
    Point_t destpos = propagatedPosByDistance(origin.position(), origin.momentum(), s);
    //add material effects, assume they are applied at destination (ok for short distance propagation)
    double deriv = 1.;
    boost::optional<double> pinv_new(true, 1./origin.momentum().R());
    if(true/*doDedx*/) {//fixme
      pinv_new = getInteractor()->dedx_prop(*pinv_new, origin.mass(), s, &deriv);
      if (pinv_new) dest5d(4) = *pinv_new;
      else {
	success = false;
	return origin;
      }
    }
    // now update the errors
    SMatrixSym55 noise_matrix;
    if(getInteractor().get() != 0) {
      bool ok = getInteractor()->noise(origin, s, noise_matrix);
      if(!ok) {
	success = false;
	return origin;
      }
    }
    //
    recob::tracking::SMatrix55 pm;
    //
    pm(0,0) = 1.;      // du2/du1
    pm(1,0) = 0.;      // dv2/du1
    pm(2,0) = 0.;      // d(dudw2)/du1
    pm(3,0) = 0.;      // d(dvdw2)/du1
    pm(4,0) = 0.;      // d(pinv2)/du1
    //
    pm(0,1) = 0.;      // du2/dv1
    pm(1,1) = 1.;      // dv2/dv1
    pm(2,1) = 0.;      // d(dudw2)/dv1
    pm(3,1) = 0.;      // d(dvdw2)/dv1
    pm(4,1) = 0.;      // d(pinv2)/dv1
    //
    pm(0,2) = s;//fixme check sign    // du2/d(dudw1);
    pm(1,2) = 0.;      // dv2/d(dudw1);
    pm(2,2) = 1.;      // d(dudw2)/d(dudw1);
    pm(3,2) = 0.;      // d(dvdw2)/d(dudw1);
    pm(4,2) = 0.;      // d(pinv2)/d(dudw1);
    //
    pm(0,3) = 0.;      // du2/d(dvdw1);
    pm(1,3) = s;//fixme check sign     // dv2/d(dvdw1);
    pm(2,3) = 0.;      // d(dudw2)/d(dvdw1);
    pm(3,3) = 1.;      // d(dvdw2)/d(dvdw1);
    pm(4,3) = 0.;      // d(pinv2)/d(dvdw1);
    //
    pm(0,4) = 0.;      // du2/d(pinv1);
    pm(1,4) = 0.;      // dv2/d(pinv1);
    pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
    pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
    pm(4,4) = deriv;   // d(pinv2)/d(pinv1);
    //
    success = true;
    return TrackState(dest5d,ROOT::Math::Similarity(pm,origin.covariance())+noise_matrix,Plane(destpos,origin.plane().direction()),origin.mass());
  }

  float PropagatorXXX::distanceToPlane(const Point_t& origpos, const Vector_t& origmom, const Plane& target, PropDirection dir) const {
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    double sign = ( origmom.Dot(targpos-origpos)>0 ? 1. : -1.);
    if (dir==PropagatorXXX::BACKWARD) sign*=-1.;
    return sign*fabs(targdir.Dot(targpos-origpos));
  }

  /*
  
  /// Propagate without error (long distance).
  ///
  /// Arguments:
  ///
  /// trk          - Track to propagate.
  /// psurf        - Destination surface.
  /// dir          - Propagation direction (FORWARD, BACKWARD, or UNKNOWN).
  /// doDedx       - dE/dx enable/disable flag.
  /// prop_matrix  - Return propagation matrix if not null.
  /// noise_matrix - Return noise matrix if not null.
  ///
  /// Returned value: Propagation distance & success flag.
  ///
  /// This method calls virtual method short_vec_prop in steps of some
  /// maximum size.
  ///
  boost::optional<double> PropagatorXXX::vec_prop(KTrack& trk,
					       const std::shared_ptr<const Surface>& psurf, 
					       PropDirection dir,
					       bool doDedx,
					       TrackMatrix* prop_matrix,
					       TrackError* noise_matrix) const
  {
    // Default result.
    
    auto result = boost::make_optional<double>(false, 0.);

    // Get the inverse momentum (assumed to be track parameter four).

    double pinv = trk.getVector()(4);

    // If dE/dx is not requested, or if inverse momentum is zero, then
    // it is safe to propagate in one step.  In this case, just pass
    // the call to short_vec_prop with unlimited distance.

    bool dedx = getDoDedx() && doDedx;
    if(!dedx || pinv == 0.)
      result = short_vec_prop(trk, psurf, dir, dedx, prop_matrix, noise_matrix);

    else {

      // dE/dx is requested.  In this case we limit the maximum
      // propagation distance such that the kinetic energy of the
      // particle should not change by more thatn 10%.

      // Get LAr service.

      auto const * detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

      // Initialize propagation matrix to unit matrix (if specified).

      int nvec = trk.getVector().size();
      if(prop_matrix)
	*prop_matrix = ublas::identity_matrix<TrackVector::value_type>(nvec);

      // Initialize noise matrix to zero matrix (if specified).

      if(noise_matrix) {
	noise_matrix->resize(nvec, nvec, false);
	noise_matrix->clear();
      }

      // Remember the starting track.

      KTrack trk0(trk);

      // Make pointer variables pointing to local versions of the
      // propagation and noise matrices, or null if not specified.

      TrackMatrix local_prop_matrix;
      TrackMatrix* plocal_prop_matrix = (prop_matrix==0 ? 0 : &local_prop_matrix);
      TrackError local_noise_matrix;
      TrackError* plocal_noise_matrix = (noise_matrix==0 ? 0 : &local_noise_matrix);

      // Cumulative propagation distance.

      double s = 0.;
	
      // Begin stepping loop.
      // We put a maximum iteration count to prevent infinite loops caused by
      // floating point pathologies.  The iteration count is large enough to reach
      // any point in the tpc using the minimum step size (for a reasonable tpc).

      bool done = false;
      int nitmax = 10000;  // Maximum number of iterations.
      int nit = 0;         // Iteration count.
      while(!done) {

	// If the iteration count exceeds the maximum, return failure.

	++nit;
	if(nit > nitmax) {
	  trk = trk0;
	  result = boost::optional<double>(false, 0.);
	  return result;
	}

	// Estimate maximum step distance according to the above
	// stated principle.

	pinv = trk.getVector()(4);
	double mass = trk.Mass();
	double p = 1./std::abs(pinv);
	double e = std::hypot(p, mass);
	double t = p*p / (e + mass);
	double dedx = 0.001 * detprop->Eloss(p, mass, fTcut);
	double smax = 0.1 * t / dedx;
	if (smax <= 0.)
	  throw cet::exception("PropagatorXXX") << __func__ << ": maximum step " << smax << "\n";

	// Always allow a step of at least 0.3 cm (about one wire spacing).

	if(smax < 0.3)
	  smax = 0.3;

	// First do a test propagation (without dE/dx and errors) to
	// find the distance to the destination surface.

	KTrack trktest(trk);
	boost::optional<double> dist = short_vec_prop(trktest, psurf, dir, false, 0, 0);

	// If the test propagation failed, return failure.

	if(!dist) {
	  trk = trk0;
	  return dist;
	}

	// Generate destionation surface for this step (either final
	// destination, or some intermediate surface).

	std::shared_ptr<const Surface> pstep;
	if(std::abs(*dist) <= smax) {
	  done = true;
	  pstep = psurf;
	}
	else {

	  // Generate intermediate surface.
	  // First get point where track will intersect intermediate surface.

	  double xyz0[3];                                // Starting point.
	  trk.getPosition(xyz0);
	  double xyz1[3];                                // Destination point.
	  trktest.getPosition(xyz1);
	  double frac = smax / std::abs(*dist);
	  double xyz[3];                                 // Intermediate point.
	  xyz[0] = xyz0[0] + frac * (xyz1[0] - xyz0[0]);
	  xyz[1] = xyz0[1] + frac * (xyz1[1] - xyz0[1]);
	  xyz[2] = xyz0[2] + frac * (xyz1[2] - xyz0[2]);

	  // Choose orientation of intermediate surface perpendicular
	  // to track.

	  double mom[3];
	  trk.getMomentum(mom);

	  // Make intermediate surface object.

	  pstep = std::shared_ptr<const Surface>(new SurfXYZPlane(xyz[0], xyz[1], xyz[2],
								  mom[0], mom[1], mom[2]));
	}

	// Do the actual step propagation.

	dist = short_vec_prop(trk, pstep, dir, doDedx, 
			      plocal_prop_matrix, plocal_noise_matrix);

	// If the step propagation failed, return failure.

	if(!dist) {
	  trk = trk0;
	  return dist;
	}

	// Update cumulative propagation distance.

	s += *dist;

	// Update cumulative propagation matrix (left-multiply).

	if(prop_matrix != 0) {
	  TrackMatrix temp = prod(*plocal_prop_matrix, *prop_matrix);
	  *prop_matrix = temp;
	}

	// Update cumulative noise matrix.

	if(noise_matrix != 0) {
	  TrackMatrix temp = prod(*noise_matrix, trans(*plocal_prop_matrix));
	  TrackMatrix temp2 = prod(*plocal_prop_matrix, temp);
	  *noise_matrix = ublas::symmetric_adaptor<TrackMatrix>(temp2);
	  *noise_matrix += *plocal_noise_matrix;
	}
      }

      // Set the final result (distance + success).

      result = boost::optional<double>(true, s);
    }

    // Done.

    return result;
  }

  /// Linearized propagate without error.
  ///
  /// Arguments:
  ///
  /// trk          - Track to propagate.
  /// psurf        - Destination surface.
  /// dir          - Propagation direction (FORWARD, BACKWARD, or UNKNOWN).
  /// doDedx       - dE/dx enable/disable flag.
  /// ref          - Reference track (for linearized propagation).  Can be null.
  /// prop_matrix  - Return propagation matrix if not null.
  /// noise_matrix - Return noise matrix if not null.
  ///
  /// Returned value: Propagation distance & success flag.
  ///
  /// If the reference track is null, this method simply calls vec_prop.
  ///
  boost::optional<double> PropagatorXXX::lin_prop(KTrack& trk,
					       const std::shared_ptr<const Surface>& psurf, 
					       PropDirection dir,
					       bool doDedx,
					       KTrack* ref,
					       TrackMatrix* prop_matrix,
					       TrackError* noise_matrix) const
  {
    // Default result.

    boost::optional<double> result;

    if(ref == 0)
      result = vec_prop(trk, psurf, dir, doDedx, prop_matrix, noise_matrix);
    else {

      // A reference track has been provided.

      // It is an error (throw exception) if the reference track and
      // the track to be propagted are not on the same surface.

      if(!trk.getSurface()->isEqual(*(ref->getSurface())))
	throw cet::exception("PropagatorXXX") << 
	  "Input track and reference track not on same surface.\n";

      // Remember the starting track and reference track.

      KTrack trk0(trk);
      KTrack ref0(*ref);

      // Propagate the reference track.  Make sure we calculate the
      // propagation matrix.

      TrackMatrix prop_temp;
      if(prop_matrix == 0)
	prop_matrix = &prop_temp;

      // Do the propgation.  The returned result will be the result of
      // this propagatrion.

      result = vec_prop(*ref, psurf, dir, doDedx, prop_matrix, noise_matrix);
      if(!!result) {

	// Propagation of reference track succeeded.  Update the track
	// state vector and surface of the track to be propagated.

	TrackVector diff = trk.getSurface()->getDiff(trk.getVector(), ref0.getVector());
	TrackVector newvec = ref->getVector() + prod(*prop_matrix, diff);

	// Store updated state vector and surface.

	trk.setVector(newvec);
	trk.setSurface(psurf);
	trk.setDirection(ref->getDirection());
	if (!trk.getSurface()->isEqual(*(ref->getSurface())))
	  throw cet::exception("PropagatorXXX") << __func__ << ": surface mismatch";

	// Final validity check.  In case of failure, restore the track
	// and reference track to their starting values.

	if(!trk.isValid()) {
	  result = boost::optional<double>(false, 0.);
	  trk = trk0;
	  *ref = ref0;
	}
      }
      else {

	// Propagation failed.
	// Restore the reference track to its starting value, so that we ensure 
	// the reference track and the actual track remain on the same surface.

	trk = trk0;
	*ref = ref0;
      }
    }

    // Done.

    return result;
  }


  /// Propagate with error, but without noise (i.e. reversibly).
  ///
  /// Arguments:
  ///
  /// tre         - Track to propagate.
  /// psurf       - Destination surface.
  /// dir         - Propagation direction (FORWARD, BACKWARD, or UNKNOWN).
  /// doDedx      - dE/dx enable/disable flag.
  /// ref         - Reference track (for linearized propagation).  Can be null.
  /// prop_matrix - Return propagation matrix if not null.
  ///
  /// Returned value: propagation distance + success flag.
  ///
  boost::optional<double> PropagatorXXX::err_prop(KETrack& tre,
					       const std::shared_ptr<const Surface>& psurf, 
					       PropDirection dir,
					       bool doDedx,
					       KTrack* ref,
					       TrackMatrix* prop_matrix) const
  {
    // Propagate without error, get propagation matrix.

    TrackMatrix prop_temp;
    if(prop_matrix == 0)
      prop_matrix = &prop_temp;
    boost::optional<double> result = lin_prop(tre, psurf, dir, doDedx, ref, prop_matrix, 0);

    // If propagation succeeded, update track error matrix.

    if(!!result) {
      TrackMatrix temp = prod(tre.getError(), trans(*prop_matrix));
      TrackMatrix temp2 = prod(*prop_matrix, temp);
      TrackError newerr = ublas::symmetric_adaptor<TrackMatrix>(temp2);
      tre.setError(newerr);
    }

    // Done.

    return result;
  }

  /// Propagate with error and noise.
  ///
  /// Arguments:
  ///
  /// tre    - Track to propagate.
  /// psurf  - Destination surface.
  /// dir    - Propagation direction (FORWARD, BACKWARD, or UNKNOWN).
  /// doDedx - dE/dx enable/disable flag.
  /// ref    - Reference track (for linearized propagation).  Can be null.
  ///
  /// Returned value: propagation distance + success flag.
  ///
  boost::optional<double> PropagatorXXX::noise_prop(KETrack& tre,
						 const std::shared_ptr<const Surface>& psurf, 
						 PropDirection dir,
						 bool doDedx,
						 KTrack* ref) const
  {
    // Propagate without error, get propagation matrix and noise matrix.

    TrackMatrix prop_matrix;
    TrackError noise_matrix;
    boost::optional<double> result = lin_prop(tre, psurf, dir, doDedx, ref,
					      &prop_matrix, &noise_matrix);

    // If propagation succeeded, update track error matrix.

    if(!!result) {
      TrackMatrix temp = prod(tre.getError(), trans(prop_matrix));
      TrackMatrix temp2 = prod(prop_matrix, temp);
      TrackError newerr = ublas::symmetric_adaptor<TrackMatrix>(temp2);
      newerr += noise_matrix;
      tre.setError(newerr);
    }

    // Done.

    return result;
  }

  /// Method to calculate updated momentum due to dE/dx.
  ///
  /// Arguments:
  ///
  /// pinv  - Initial inverse momentum (units c/GeV).
  /// mass  - Particle mass (GeV/c^2).
  /// s     - Path distance.
  /// deriv - Pointer to store derivative d(pinv2)/d(pinv1) if nonzero.
  ///
  /// Returns: Final inverse momentum (pinv2) + success flag.
  ///
  /// Failure is returned in case of range out.
  ///
  /// Inverse momentum can be signed (q/p).  Returned inverse momentum
  /// has the same sign as the input.
  ///
  /// In this method, we are solving the differential equation in
  /// terms of energy.
  ///
  /// dE/dx = -f(E)
  ///
  /// where f(E) is the stopping power returned by method
  /// LArProperties::Eloss.
  ///
  /// We expect that this method will be called exclusively for short
  /// distance propagation.  The differential equation is solved using
  /// the midpoint method using a single step, which requires two
  /// evaluations of f(E).
  ///
  /// dE = -s*f(E1)
  /// E2 = E1 - s*f(E1 + 0.5*dE)
  ///
  /// The derivative is calculated assuming E2 = E1 + constant, giving
  ///
  /// d(pinv2)/d(pinv1) = pinv2^3 E2 / (pinv1^3 E1).
  /// 
  ///
  boost::optional<double> PropagatorXXX::dedx_prop(double pinv, double mass,
						double s, double* deriv) const
  {
    // For infinite initial momentum, return with success status,
    // still infinite momentum.

    if(pinv == 0.)
      return boost::optional<double>(true, 0.);

    // Set the default return value to be uninitialized with value 0.

    boost::optional<double> result(false, 0.);

    // Get LAr service.

    auto const * detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

    // Calculate final energy.

    double p1 = 1./std::abs(pinv);
    double e1 = std::hypot(p1, mass);
    double de = -0.001 * s * detprop->Eloss(p1, mass, fTcut);
    double emid = e1 + 0.5 * de;
    if(emid > mass) {
      double pmid = std::sqrt(emid*emid - mass*mass);
      double e2 = e1 - 0.001 * s * detprop->Eloss(pmid, mass, fTcut);
      if(e2 > mass) {
	double p2 = std::sqrt(e2*e2 - mass*mass);
	double pinv2 = 1./p2;
	if(pinv < 0.)
	  pinv2 = -pinv2;

	// Calculation was successful, update result.

	result = boost::optional<double>(true, pinv2);

	// Also calculate derivative, if requested.

	if(deriv != 0)
	  *deriv = pinv2*pinv2*pinv2 * e2 / (pinv*pinv*pinv * e1);
      }
    }

    // Done.

    return result;
  }
  */
} // end namespace trkf
