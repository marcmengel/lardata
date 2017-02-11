#include "lardata/RecoObjects/PropagatorToPlane.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/RecoObjects/InteractPlane.h"
#include "cetlib/exception.h"
#include "boost/optional.hpp"

using namespace recob::tracking;

namespace trkf {

  PropagatorToPlane::PropagatorToPlane(double maxStep, double tcut) :
    fMaxStep(maxStep),
    fTcut(tcut)
  {
    fInteractor = (tcut >= 0. ? std::shared_ptr<const Interactor>(new InteractPlane(tcut)) : std::shared_ptr<const Interactor>());
  }

  PropagatorToPlane::~PropagatorToPlane() {}

  using PropDirection = PropagatorToPlane::PropDirection;

  TrackState PropagatorToPlane::propagateToPlane(bool& success, const TrackState& origin, const Plane& target, bool dodedx, bool domcs, PropDirection dir) const {
    //std::cout << "origin.position()=" << origin.position() << " origin.momentum().Unit()=" << origin.momentum().Unit() << std::endl;
    //std::cout << "origin.covariance()=\n" << origin.covariance() << std::endl;
    TrackState trackState = rotateToPlane(success, origin, target);
    if (!success) return origin;
    //
    //now do the propagation
    int nitmax = 10000;  // Maximum number of iterations.
    int nit = 0;         // Iteration count.
    bool arrived = false;
    // std::cout << "begin while" << std::endl;
    while (!arrived) {
      ++nit;
      if(nit > nitmax) {
	success = false;
	return origin;
      }
      //compute the distance to destination
      std::pair<double, double> distancePair = distancePairToPlane(success, trackState, target, dir);
      if (!success) return origin;
      double s     = distancePair.first;
      double sperp = distancePair.second;
      // Estimate maximum step distance, such that 10% of initial energy is lost by dedx
      double mass = origin.mass();
      double p = 1./origin.parameters()[4];
      double e = std::hypot(p, mass);
      double t = p*p / (e + mass);
      auto const * detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();//fixme
      double dedx = 0.001 * detprop->Eloss(std::abs(p), mass, fTcut);
      double smax = std::max(0.3,0.1 * t / dedx);
      //
      if (domcs && std::abs(s)>smax) {
	sperp = smax*std::abs(sperp)/s;
	s     = (s>0 ? smax : -smax);
      } else arrived = true;
      bool stepDone = false;
      trackState = propagatedStateByPath(stepDone, trackState, s, sperp, dodedx, domcs);
      if (!stepDone) {
	success = false;
	return origin;
      }
    }
    // std::cout << "end while" << std::endl;
    success = true;
    return TrackState(trackState.parameters(), trackState.covariance(), target, trackState.isTrackAlongPlaneDir(), trackState.pID());
  }

  TrackState PropagatorToPlane::rotateToPlane(bool& success, const TrackState& origin, const Plane& target) const {
    const bool isTrackAlongPlaneDir = origin.momentum().Dot(target.direction())>0;
    //
    SVector5 orpar5 = origin.parameters();
    //std::cout << "orig par5=" << orpar5 << std::endl;
    //first, translation and rotation of origin plane to be centered on initial position and parallel to target
    const double sinA1 = origin.plane().sinAlpha();
    const double cosA1 = origin.plane().cosAlpha();
    const double sinA2 = target.sinAlpha();
    const double cosA2 = target.cosAlpha();
    const double sinB1 = origin.plane().sinBeta();
    const double cosB1 = origin.plane().cosBeta();
    const double sinB2 = target.sinBeta();
    const double cosB2 = target.cosBeta();
    const double sindB = -sinB1*cosB2 + cosB1*sinB2;
    const double cosdB = cosB1*cosB2 + sinB1*sinB2;
    //std::cout << "sinA1=" << sinA1 << " cosA1=" << cosA1 << " sinA2=" << sinA2 << " cosA2=" << cosA2 << " sindB=" << sindB << " cosdB=" << cosdB << std::endl;
    const double ruu = cosA1*cosA2 + sinA1*sinA2*cosdB;
    const double ruv = sinA2*sindB;
    const double ruw = sinA1*cosA2 - cosA1*sinA2*cosdB;
    const double rvu = -sinA1*sindB;
    const double rvv = cosdB;
    const double rvw = cosA1*sindB;
    const double rwu = cosA1*sinA2 - sinA1*cosA2*cosdB;
    const double rwv = -cosA2*sindB;
    const double rww = sinA1*sinA2 + cosA1*cosA2*cosdB;
    const double dw2dw1 = orpar5[2]*rwu + orpar5[3]*rwv + rww;
    if(dw2dw1 == 0.) {
      success = false;
      return origin;
    }
    const double dudw2 = (orpar5[2]*ruu + orpar5[3]*ruv + ruw) / dw2dw1;
    const double dvdw2 = (orpar5[2]*rvu + orpar5[3]*rvv + rvw) / dw2dw1;
    SMatrix55 pm;
    //
    pm(0,0) = ruu - dudw2*rwu;    // du2/du1 //fixme check second term
    pm(1,0) = rvu - dvdw2*rwu;    // dv2/du1
    pm(2,0) = 0.;                 // d(dudw2)/du1
    pm(3,0) = 0.;                 // d(dvdw2)/du1
    pm(4,0) = 0.;                 // d(pinv2)/du1
    //
    pm(0,1) = ruv - dudw2*rwv;    // du2/dv1
    pm(1,1) = rvv - dvdw2*rwv;    // dv2/dv1
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
    orpar5[0] = origin.position().X() - target.position().X();
    orpar5[1] = (origin.position().Y() - target.position().Y()) * cosB2 + (origin.position().Z() - target.position().Z()) * sinB2;
    orpar5[2] = dudw2;
    orpar5[3] = dvdw2;
    //
    //std::cout << "orpar5=" << orpar5 << std::endl;
    //std::cout << "origin rotation" << std::endl;
    //std::cout << pm << std::endl;
    //
    //std::cout << "dotp target.direction().Dot(origin.momentum().Unit())=" << target.direction().Dot(origin.momentum().Unit()) << std::endl;
    //std::cout << "tmp par5=" << orpar5[0] << ", " << orpar5[1] << std::endl;
    success = true;
    //std::cout << "trackState.position()=" << trackState.position() << " trackState.momentum().Unit()=" << trackState.momentum().Unit() << std::endl;
    return TrackState(orpar5,ROOT::Math::Similarity(pm,origin.covariance()),Plane(origin.position(),target.direction()),isTrackAlongPlaneDir,origin.pID());
  }

  TrackState PropagatorToPlane::propagatedStateByPath(bool& success, const TrackState& origin, const double s, const double sperp, bool dodedx, bool domcs) const {
    const SVector5& orig5d = origin.parameters();
    SVector5 dest5d(orig5d(0)+sperp*orig5d(2),orig5d(1)+sperp*orig5d(3),orig5d(2),orig5d(3),orig5d(4));
    Point_t destpos = propagatedPosByDistance(origin.position(), origin.momentum(), s);
    //add material effects, assume they are applied at destination (ok for short distance propagation)
    double deriv = 1.;
    boost::optional<double> pinv_new(true, 1./origin.momentum().R());
    if(dodedx) {
      //std::cout << "Test my s=" << s << std::endl;
      pinv_new = getInteractor()->dedx_prop(*pinv_new, origin.mass(), s, &deriv);
      if (pinv_new) dest5d(4) = *pinv_new;
      else {
	success = false;
	return origin;
      }
    }
    // now update the errors
    SMatrixSym55 noise_matrix;
    if(domcs) {
      if (getInteractor().get() != 0) {
	bool ok = getInteractor()->noise(origin, s, noise_matrix);
	if(!ok) {
	  success = false;
	  return origin;
	}
      } else {
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
    pm(0,2) = sperp;   // du2/d(dudw1);
    pm(1,2) = 0.;      // dv2/d(dudw1);
    pm(2,2) = 1.;      // d(dudw2)/d(dudw1);
    pm(3,2) = 0.;      // d(dvdw2)/d(dudw1);
    pm(4,2) = 0.;      // d(pinv2)/d(dudw1);
    //
    pm(0,3) = 0.;      // du2/d(dvdw1);
    pm(1,3) = sperp;   // dv2/d(dvdw1);
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
    //std::cout << "propagation matrix" << std::endl;
    //std::cout << pm << std::endl;
    //
    //std::cout << "vec2=" << dest5d << std::endl;
    //std::cout << "noise_matrix=\n" << noise_matrix << std::endl;
    //
    success = true;
    return TrackState(dest5d,ROOT::Math::Similarity(pm,origin.covariance())+noise_matrix,Plane(destpos,origin.plane().direction()),origin.isTrackAlongPlaneDir(),origin.pID());
  }

  double PropagatorToPlane::distanceToPlane(bool& success, const Point_t& origpos, const Vector_t& origmom, const Plane& target, PropDirection dir) const {
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    //check that origmom is not along the plane, i.e. targdir.Dot(origmom.Unit())=0
    if (targdir.Dot(origmom.Unit())==0) {
      success = false;
      return DBL_MAX;
    }
    //std::cout << "origpos=" << origpos << " origmom=" << origmom << " targpos=" << targpos << " targdir=" << targdir << std::endl;
    double s = targdir.Dot(targpos-origpos)/targdir.Dot(origmom.Unit());
    if (dir==PropagatorToPlane::BACKWARD) s*=-1.;
    success = true;
    return s;
  }

  std::pair<double,double> PropagatorToPlane::distancePairToPlane(bool& success, const Point_t& origpos, const Vector_t& origmom, const Plane& target, PropDirection dir) const {
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    //check that origmom is not along the plane, i.e. targdir.Dot(origmom.Unit())=0
    if (targdir.Dot(origmom.Unit())==0) {
      success = false;
      return std::pair<double, double>(DBL_MAX,DBL_MAX);
    }
    //std::cout << "origpos=" << origpos << " origmom=" << origmom << " targpos=" << targpos << " targdir=" << targdir << std::endl;
    //point-plane distance divided by track direction component orthogonal to the plane
    double sperp = targdir.Dot(targpos-origpos);
    //3d distance, correct sign for track momentum
    double s = sperp/targdir.Dot(origmom.Unit());
    if (dir==PropagatorToPlane::BACKWARD) s*=-1.;
    //std::cout << "sperp=" << sperp << " s=" << s << std::endl;
    success = true;
    return std::pair<double, double>(s,sperp);
  }

} // end namespace trkf
