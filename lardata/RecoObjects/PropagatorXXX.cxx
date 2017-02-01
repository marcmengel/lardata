#include "lardata/RecoObjects/PropagatorXXX.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/RecoObjects/InteractPlane.h"
#include "cetlib/exception.h"
#include "boost/optional.hpp"

using namespace recob::tracking;

namespace trkf {

  PropagatorXXX::PropagatorXXX(double maxStep, double tcut) :
    fMaxStep(maxStep),
    fTcut(tcut)
  {
    fInteractor = (tcut >= 0. ? std::shared_ptr<const Interactor>(new InteractPlane(tcut)) : std::shared_ptr<const Interactor>());
  }

  PropagatorXXX::~PropagatorXXX() {}

  using PropDirection = PropagatorXXX::PropDirection;

  bool PropagatorXXX::propagateToPlane(const TrackState& origin, const Plane& target, TrackState& result, bool dodedx, bool domcs, PropDirection dir) const {
    //std::cout << "origin.position()=" << origin.position() << " origin.momentum().Unit()=" << origin.momentum().Unit() << std::endl;
    const bool isTrackAlongPlaneDir = origin.momentum().Dot(target.direction())>0;
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
    const double sindphi = sinph1*cosph2 - cosph1*sinph2;
    const double cosdphi = cosph1*cosph2 + sinph1*sinph2;
    //std::cout <<
    //"sinth1=" << sinth1 <<
    //" costh1=" << costh1 <<
    //" sinth2=" << sinth2 <<
    //" costh2=" << costh2 <<
    //" sindphi=" << sindphi <<
    //" cosdphi=" << cosdphi << std::endl;
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
    orpar5[0] = 0.;
    orpar5[1] = 0.;
    orpar5[2] = dudw2;
    orpar5[3] = dvdw2;
    //
    //std::cout << "origin rotation" << std::endl;
    //std::cout << pm << std::endl;    
    //
    //std::cout << "orpar5=" << orpar5 << std::endl;
    //std::cout << "dotp target.direction().Dot(origin.momentum().Unit())=" << target.direction().Dot(origin.momentum().Unit()) << std::endl;
    result = TrackState(orpar5,ROOT::Math::Similarity(pm,origin.covariance()),Plane(origin.position(),target.direction()),isTrackAlongPlaneDir,origin.mass());
    //std::cout << "result.position()=" << result.position() << " result.momentum().Unit()=" << result.momentum().Unit() << std::endl;
    //
    //now do the propagation
    bool arrived = false;
    while (!arrived) {
      //compute the distance to destination
      std::pair<double, double> distancePair = distancePairToPlane(result, target, dir);
      double s     = distancePair.first;
      double sperp = distancePair.second;
      // Estimate maximum step distance, such that 10% of initial energy is lost by dedx 
      double mass = origin.mass();
      double p = 1./orpar5[4];
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
      result = propagatedStateByPath(result, s, sperp, dodedx, domcs, stepDone);
      if (!stepDone) return false;
    }
    //now that we arrived on the target plane, get local coordinates with respect to the target origin
    auto arrival5d = target.Global6DToLocal5DParameters(result.parameters6D());//fixme, there must be a faster way to do this
    result = TrackState(arrival5d,result.covariance(),target,isTrackAlongPlaneDir,result.mass());
    return true;
  }

  TrackState PropagatorXXX::propagatedStateByPath(const TrackState& origin, const double s, const double sperp, bool dodedx, bool domcs, bool& success) const {
    const SVector5& orig5d = origin.parameters();
    // SVector5 dest5d(orig5d(0)+sperp*orig5d(2),orig5d(1)+sperp*orig5d(3),orig5d(2),orig5d(3),orig5d(4));
    SVector5 dest5d(0.,0.,orig5d(2),orig5d(3),orig5d(4));
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
    success = true;
    return TrackState(dest5d,ROOT::Math::Similarity(pm,origin.covariance())+noise_matrix,Plane(destpos,origin.plane().direction()),origin.isTrackAlongPlaneDir(),origin.mass());
  }

  double PropagatorXXX::distanceToPlane(const Point_t& origpos, const Vector_t& origmom, const Plane& target, PropDirection dir) const {
    //fixme check that origmom is not along the plane, i.e. targdir.Dot(origmom.Unit())=0
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    //std::cout << "origpos=" << origpos << " origmom=" << origmom << " targpos=" << targpos << " targdir=" << targdir << std::endl;
    double s = targdir.Dot(targpos-origpos)/targdir.Dot(origmom.Unit());
    if (dir==PropagatorXXX::BACKWARD) s*=-1.;
    return s;
  }

  std::pair<double,double> PropagatorXXX::distancePairToPlane(const Point_t& origpos, const Vector_t& origmom, const Plane& target, PropDirection dir) const {
    //fixme check that origmom is not along the plane, i.e. targdir.Dot(origmom.Unit())=0
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    //std::cout << "origpos=" << origpos << " origmom=" << origmom << " targpos=" << targpos << " targdir=" << targdir << std::endl;
    //point-plane distance divided by track direction component orthogonal to the plane
    double sperp = targdir.Dot(targpos-origpos);
    //3d distance, correct sign for track momentum
    double s = sperp/targdir.Dot(origmom.Unit());
    if (dir==PropagatorXXX::BACKWARD) s*=-1.;
    //std::cout << "sperp=" << sperp << " s=" << s << std::endl;
    return std::pair<double, double>(s,sperp);
  }

} // end namespace trkf
