#include "lardata/RecoObjects/PropagatorToPlane.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "cetlib/exception.h"
#include "boost/optional.hpp"

using namespace recob::tracking;

namespace trkf {

  PropagatorToPlane::PropagatorToPlane(double minStep, double maxElossFrac, int maxNit, double tcut) :
    fMinStep(minStep),
    fMaxElossFrac(maxElossFrac),
    fMaxNit(maxNit),
    fTcut(tcut)
  {
    detprop = art::ServiceHandle<detinfo::DetectorPropertiesService>()->provider();
    larprop = lar::providerFrom<detinfo::LArPropertiesService>();
  }

  PropagatorToPlane::~PropagatorToPlane() {}

  using PropDirection = PropagatorToPlane::PropDirection;
  
  TrackState PropagatorToPlane::propagateToPlane(bool& success, const TrackState& origin, const Plane& target, bool dodedx, bool domcs, PropDirection dir) const {
    /*
      1- find distance to target plane
      2- propagate 3d position by distance
      3- translate global parameters to local on target plane
      4- propagate covariance (5d) by distance, still on origin reference frame
      5- rotate covariance to target plane
      6- apply material effects
    */
    //
    // find distance to target plane
    std::pair<double, double> distpair = distancePairToPlane(success, origin, target/*, dir*/);
    double distance = distpair.first;
    double path     = distance*origin.parameters()[4];
    double sperp    = distpair.second;
    //
    // std::cout << "distance=" << distance << " sperp=" << sperp << std::endl;
    if ((distance<-0.000001 && dir==FORWARD) || (distance>0.000001 && dir==BACKWARD)) {//fixme
      // std::cout << "wrong direction" << std::endl;
      success = false;
      return origin;
    }
    //
    // propagate 3d position by distance
    Point_t p = propagatedPosByDistance(origin.position(), origin.momentum(), path);//fixme meaning of distance
    //
    // translate global parameters to local on target plane
    SVector6 par6d(p.X(),p.Y(),p.Z(),origin.momentum().X(),origin.momentum().Y(),origin.momentum().Z());
    SVector5 par5d = target.Global6DToLocal5DParameters(par6d);
    //
    // propagate covariance (5d) by distance, still on origin reference frame
    SMatrixSym55 cov5d = origin.covariance();
    SMatrix55 pm = ROOT::Math::SMatrixIdentity();
    pm(0,2) = sperp;   // du2/d(dudw1);
    pm(1,3) = sperp;   // dv2/d(dvdw1);
    //
    // rotate covariance to target plane (fixme, move this feature in TrackingPlane and update end of TrackKalmanFitter.cxx)
    const double orpar2 = origin.parameters()[2];
    const double orpar3 = origin.parameters()[3];
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
    const double ruu = cosA1*cosA2 + sinA1*sinA2*cosdB;
    const double ruv = sinA2*sindB;
    const double ruw = sinA1*cosA2 - cosA1*sinA2*cosdB;
    const double rvu = -sinA1*sindB;
    const double rvv = cosdB;
    const double rvw = cosA1*sindB;
    const double rwu = cosA1*sinA2 - sinA1*cosA2*cosdB;
    const double rwv = -cosA2*sindB;
    const double rww = sinA1*sinA2 + cosA1*cosA2*cosdB;
    const double dw2dw1 = orpar2*rwu + orpar3*rwv + rww;
    if(dw2dw1 == 0.) {
      success = false;
      return origin;
    }
    const double dudw2 = (orpar2*ruu + orpar3*ruv + ruw) / dw2dw1;
    const double dvdw2 = (orpar2*rvu + orpar3*rvv + rvw) / dw2dw1;
    SMatrix55 rj = ROOT::Math::SMatrixIdentity();
    rj(0,0) = ruu - dudw2*rwu;    // du2/du1
    rj(1,0) = rvu - dvdw2*rwu;    // dv2/du1
    rj(0,1) = ruv - dudw2*rwv;    // du2/dv1
    rj(1,1) = rvv - dvdw2*rwv;    // dv2/dv1
    rj(2,2) = (ruu - dudw2*rwu) / dw2dw1;    // d(dudw2)/d(dudw1);
    rj(3,2) = (rvu - dvdw2*rwu) / dw2dw1;    // d(dvdw2)/d(dudw1);
    rj(2,3) = (ruv - dudw2*rwv) / dw2dw1;    // d(dudw2)/d(dvdw1);
    rj(3,3) = (rvv - dvdw2*rwv) / dw2dw1;    // d(dvdw2)/d(dvdw1);
    //
    // apply material effects
    bool arrived = false;
    int nit = 0;         // Iteration count.
    double deriv = 1.;
    SMatrixSym55 noise_matrix;
    while (!arrived) {
      //std::cout << "nit=" << nit << std::endl;
      ++nit;
      if(nit > fMaxNit) {
	// std::cout << "FAILED WITH NIT=" << nit << std::endl;
	success = false;
	return origin;
      }
      // Estimate maximum step distance, such that fMaxElossFrac of initial energy is lost by dedx
      const double mass = origin.mass();
      const double p = 1./par5d[4];
      const double e = std::hypot(p, mass);
      const double t = e - mass;
      const double dedx = 0.001 * detprop->Eloss(std::abs(p), mass, fTcut);
      const double range = t / dedx;
      const double smax = std::max(fMinStep,fMaxElossFrac*range);
      //double smax = -1.;
      double s = distance;//std::max(fabs(distance),0.);//fixme
      //if (distance<0.) s*=-1.;
      //std::cout << "distance=" << distance << " s=" << s << " smax=" << smax << std::endl;
      if (domcs && smax>0 && std::abs(s)>smax) {
	if (fMaxNit==1) {
	  success = false;
	  return origin;
	}
	s = (s>0 ? smax : -smax);
	distance-=smax;
      } else arrived = true;
      // now apply material effects
      if(domcs) {
	bool flip = false;
	if (origin.isTrackAlongPlaneDir()==true && dw2dw1<0.) flip = true;
	if (origin.isTrackAlongPlaneDir()==false && dw2dw1>0.) flip = true;
	bool ok = apply_mcs(par5d[2], par5d[3], par5d[4], origin.mass(), s, range, p, e*e, flip, noise_matrix);
	if(!ok) {
	  success = false;
	  return origin;
	}
      }
      if(dodedx) {
	apply_dedx(par5d(4), dedx, e, origin.mass(), s, deriv);
      }
    }
    pm(4,4)*=deriv;
    // std::cout << "rj=\n" << rj << std::endl;
    // std::cout << "pm=\n" << pm << std::endl;    
    // std::cout << "noise_matrix=\n" << noise_matrix << std::endl;    
    cov5d = ROOT::Math::Similarity(pm*rj,cov5d);
    cov5d = cov5d+noise_matrix;
    //
    TrackState trackState(par5d, cov5d, target, origin.momentum().Dot(target.direction())>0, origin.pID());
    // std::cout << "test propagated parameters=" << par5d << std::endl;
    // std::cout << "test propagated covariance=\n" << cov5d << std::endl;
    // if (nit > 100) std::cout << "NIT=" << nit << std::endl;
    return trackState;
  }
  
  TrackState PropagatorToPlane::rotateToPlane(bool& success, const TrackState& origin, const Plane& target) const {
    const bool isTrackAlongPlaneDir = origin.momentum().Dot(target.direction())>0;
    //
    SVector5 orpar5 = origin.parameters();
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
    orpar5[0] = origin.position().X() - target.position().X();
    orpar5[1] = (origin.position().Y() - target.position().Y()) * cosB2 + (origin.position().Z() - target.position().Z()) * sinB2;
    orpar5[2] = dudw2;
    orpar5[3] = dvdw2;
    //
    // std::cout << "rj=\n" << pm << std::endl;
    // std::cout << "origin.covariance()=\n" << origin.covariance() << std::endl;
    //
    success = true;
    return TrackState(orpar5,ROOT::Math::Similarity(pm,origin.covariance()),Plane(origin.position(),target.direction()),isTrackAlongPlaneDir,origin.pID());
  }

  double PropagatorToPlane::distanceToPlane(bool& success, const Point_t& origpos, const Vector_t& origmom, const Plane& target/*, PropDirection dir*/) const {
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    //check that origmom is not along the plane, i.e. targdir.Dot(origmom.Unit())=0
    if (targdir.Dot(origmom.Unit())==0) {
      success = false;
      return DBL_MAX;
    }
    double s = targdir.Dot(targpos-origpos)/targdir.Dot(origmom.Unit());
    // if (dir==PropagatorToPlane::BACKWARD) s*=-1.;
    success = true;
    return s;
  }

  std::pair<double,double> PropagatorToPlane::distancePairToPlane(bool& success, const Point_t& origpos, const Vector_t& origmom, const Plane& target/*, PropDirection dir*/) const {
    const Point_t& targpos = target.position();
    const Vector_t& targdir = target.direction();
    //check that origmom is not along the plane, i.e. targdir.Dot(origmom.Unit())=0
    if (targdir.Dot(origmom.Unit())==0) {
      success = false;
      return std::pair<double, double>(DBL_MAX,DBL_MAX);
    }
    //point-plane distance divided by track direction component orthogonal to the plane
    double sperp = targdir.Dot(targpos-origpos);
    //3d distance, correct sign for track momentum
    double s = sperp/targdir.Dot(origmom.Unit());
    // if (dir==PropagatorToPlane::BACKWARD) s*=-1.;
    success = true;
    return std::pair<double, double>(s,sperp);
  }

  void PropagatorToPlane::apply_dedx(double& pinv, double dedx, double e1, double mass, double s, double& deriv) const
  {
    // For infinite initial momentum, return with infinite momentum.
    if (pinv == 0.) return;
    //
    const double emid = e1 - 0.5 * s * dedx;
    if(emid > mass) {
      const double pmid = std::sqrt(emid*emid - mass*mass);
      const double e2 = e1 - 0.001 * s * detprop->Eloss(pmid, mass, fTcut);
      if(e2 > mass) {
	const double p2 = std::sqrt(e2*e2 - mass*mass);
	double pinv2 = 1./p2;
	if(pinv < 0.) pinv2 = -pinv2;	
	// derivative
	deriv = pinv2*pinv2*pinv2 * e2 / (pinv*pinv*pinv * e1);
	// update result.
	pinv = pinv2;
      }
    }
    return;
  }
  
  bool PropagatorToPlane::apply_mcs(double dudw, double dvdw, double pinv, double mass, double s, double range, double p, double e2, bool flipSign, SMatrixSym55& noise_matrix) const {
    // If distance is zero, or momentum is infinite, return zero noise.

    if(pinv == 0. || s == 0.)
      return true;

    // Use crude estimate of the range of the track.
    if(range > 100.) range = 100.;
    const double p2 = p*p;

    // Calculate the radiation length in cm.
    const double x0 = larprop->RadiationLength() / detprop->Density();

    // Calculate projected rms scattering angle.
    // Use the estimted range in the logarithm factor.
    // Use the incremental propagation distance in the square root factor.
    const double betainv = std::sqrt(1. + pinv*pinv * mass*mass);
    const double theta_fact = (0.0136 * pinv * betainv) * (1. + 0.038 * std::log(range/x0));
    // const double theta_fact = (0.0136 * pinv * betainv) * (1. + 0.038 * std::log(std::abs(s/x0)));//fixme
    const double theta02 = theta_fact*theta_fact * std::abs(s/x0);

    // Calculate some common factors needed for multiple scattering.
    const double ufact2 = 1. + dudw*dudw;
    const double vfact2 = 1. + dvdw*dvdw;
    const double uvfact2 = 1. + dudw*dudw + dvdw*dvdw;
    const double uvfact = std::sqrt(uvfact2);
    const double uv = dudw * dvdw;
    const double dist2_3 = s*s / 3.;
    double dist_2 = std::abs(s) / 2.;
    if(flipSign) dist_2 = -dist_2;

    // Calculate energy loss fluctuations.

    const double evar = 1.e-6 * detprop->ElossVar(p, mass) * std::abs(s); // E variance (GeV^2).
    const double pinvvar = evar * e2 / (p2*p2*p2);                        // Inv. p variance (1/GeV^2)

    // Update elements of noise matrix.

    // Position submatrix.
    noise_matrix(0,0) += dist2_3 * theta02 * ufact2;           // sigma^2(u,u)
    noise_matrix(1,0) += dist2_3 * theta02 * uv;               // sigma^2(u,v)
    noise_matrix(1,1) += dist2_3 * theta02 * vfact2;           // sigma^2(v,v)

    // Slope submatrix.
    noise_matrix(2,2) += theta02 * uvfact2 * ufact2;           // sigma^2(u', u')
    noise_matrix(3,2) += theta02 * uvfact2 * uv;               // sigma^2(v', u')
    noise_matrix(3,3) += theta02 * uvfact2 * vfact2;           // sigma^2(v', v')

    // Same-view position-slope correlations.
    noise_matrix(2,0) += dist_2 * theta02 * uvfact * ufact2;   // sigma^2(u', u)
    noise_matrix(3,1) += dist_2 * theta02 * uvfact * vfact2;   // sigma^2(v', v)

    // Opposite-view position-slope correlations.
    noise_matrix(2,1) += dist_2 * theta02 * uvfact * uv;       // sigma^2(u', v)
    noise_matrix(3,0) += dist_2 * theta02 * uvfact * uv;       // sigma^2(v', u)

    // Momentum correlations (zero).
    // noise_matrix(4,0) += 0.;                                   // sigma^2(pinv, u)
    // noise_matrix(4,1) += 0.;                                   // sigma^2(pinv, v)
    // noise_matrix(4,2) += 0.;                                   // sigma^2(pinv, u')
    // noise_matrix(4,3) += 0.;                                   // sigma^2(pinv, v')

    // Energy loss fluctuations.
    noise_matrix(4,4) += pinvvar;                              // sigma^2(pinv, pinv)

    // Done (success).
    return true;
  }
  
} // end namespace trkf

/*
    // par6d[0] = planePos.X() + p[0]*cosa;
    // par6d[1] = planePos.Y() + p[0]*sina*sinb + p[1]*cosb;
    // par6d[2] = planePos.Z() - p[0]*sina*cosb + p[1]*sinb;

    // par6d[0] = planePos.X() + P[0]*cosA;
    // par6d[1] = planePos.Y() + P[0]*sinA*sinB + P[1]*cosB;
    // par6d[2] = planePos.Z() - P[0]*sinA*cosB + P[1]*sinB;


    p[0]*cosa = P[0]*cosA
    p[0]*sina*sinb + p[1]*cosb = P[0]*sinA*sinB + P[1]*cosB

    => P[0] = p[0]*cosa/cosA;
    => P[1] = (p[0]*sina*sinb + p[1]*cosb - P[0]*sinA*sinB)/cosB
 
    p[0]*sina*sinb + p[1]*cosb = P[0]*sinA*sinB + P[1]*cosB
    -p[0]*sina*cosb + p[1]*sinb = -P[0]*sinA*cosB + P[1]*sinB

    P[1]*cosB = p[0]*sina*sinb + p[1]*cosb - P[0]*sinA*sinB => P[1] = (p[0]*sina*sinb + p[1]*cosb - P[0]*sinA*sinB)/cosB

    -p[0]*sina*cosb*cosB + p[1]*sinb*cosB = -P[0]*sinA*cosB*cosB + p[0]*sina*sinb*sinB + p[1]*cosb*sinB - P[0]*sinA*sinB*sinB
    -p[0]*sina*(cosb*cosB+sinb*sinB) + p[1]*(sinb*cosB-cosb*sinB) = -P[0]*sinA
    p[0]*sina*(cosb*cosB+sinb*sinB) - p[1]*(sinb*cosB-cosb*sinB) = P[0]*sinA
*/

  // TrackState PropagatorToPlane::propagateToPlane(bool& success, const TrackState& origin, const Plane& target, bool dodedx, bool domcs, PropDirection dir) const {
  //   //first, translation and rotation of origin plane to be centered on initial position and parallel to target
  //   TrackState trackState = rotateToPlane(success, origin, target);
  //   if (!success) return origin;
  //   //
  //   // std::cout << "origin covariance=\n" << origin.covariance() << std::endl;
  //   // std::cout << "rotate covariance=\n" << trackState.covariance() << std::endl;
  //   //
  //   //now do the propagation
  //   int nitmax = 10000;  // Maximum number of iterations.
  //   int nit = 0;         // Iteration count.
  //   bool arrived = false;
  //   while (!arrived) {
  //     ++nit;
  //     // std::cout << "step=" << nit << std::endl;
  //     if(nit > nitmax) {
  // 	success = false;
  // 	return origin;
  //     }
  //     //compute the distance to destination
  //     std::pair<double, double> distancePair = distancePairToPlane(success, trackState, target/*, dir*/);
  //     if (!success) return origin;
  //     double s     = distancePair.first;
  //     double sperp = distancePair.second;
  //     // Estimate maximum step distance, such that 10% of initial energy is lost by dedx
  //     double mass = origin.mass();
  //     double p = 1./trackState.parameters()[4];
  //     double e = std::hypot(p, mass);
  //     double t = p*p / (e + mass);
  //     double dedx = 0.001 * detprop->Eloss(std::abs(p), mass, fTcut);
  //     double smax = std::max(0.3,0.1 * t / dedx);
  //     // std::cout << "s=" << s << " sperp=" << sperp << " smax=" << smax << std::endl;
  //     //
  //     if (domcs && std::abs(s)>smax) {
  // 	sperp = smax*sperp/std::abs(s);
  // 	s     = (s>0 ? smax : -smax);
  //     } else arrived = true;
  //     bool stepDone = false;
  //     // std::cout << "s=" << s << " sperp=" << sperp << " smax=" << smax << std::endl;
  //     trackState = propagatedStateByPath(stepDone, trackState, s, sperp, dodedx, domcs);
  //     // std::cout << "intermediate position="   << trackState.position() << std::endl;
  //     // std::cout << "intermediate momentum="   << trackState.momentum() << std::endl;
  //     // std::cout << "intermediate parameters=" << trackState.parameters() << std::endl;
  //     // std::cout << "intermediate planarity test=" << target.direction().Dot(target.position()-trackState.position()) << std::endl;
  //     // std::cout << "intermediate position2="  << trackState.plane().Local5DToGlobal6DParameters(trackState.parameters(), true) << std::endl;
  //     // std::cout << "intermediate plane position="   << trackState.plane().position() << std::endl;
  //     // std::cout << "intermediate plane direction="   << trackState.plane().direction() << std::endl;
  //     if (!stepDone) {
  // 	success = false;
  // 	return origin;
  //     }
  //   }
  //   // if (nit > 1) std::cout << "OLD NIT=" << nit << std::endl;
  //   success = true;
  //   return TrackState(trackState.parameters(), trackState.covariance(), target, trackState.isTrackAlongPlaneDir(), trackState.pID());
  // }

  // TrackState PropagatorToPlane::propagatedStateByPath(bool& success, const TrackState& origin, const double s, const double sperp, bool dodedx, bool domcs) const {
  //   const SVector5& orig5d = origin.parameters();
  //   SVector5 dest5d(orig5d(0)+sperp*orig5d(2),orig5d(1)+sperp*orig5d(3),orig5d(2),orig5d(3),orig5d(4));
  //   Point_t destpos = propagatedPosByDistance(origin.position(), origin.momentum().Unit(), s);
  //   //
  //   // SVector5 dest5dtmp(orig5d(0)+sperp*orig5d(2),orig5d(1)+sperp*orig5d(3),orig5d(2),orig5d(3),orig5d(4));
  //   // Point_t destplanepos = propagatedPosByDistance(origin.plane().position(), origin.plane().direction(), s);
  //   //
  //   // std::cout << "origin.position()=" << origin.position() << std::endl;
  //   // std::cout << "origin.momentum().Unit()=" << origin.momentum().Unit() << std::endl;
  //   // std::cout << "origin.plane().position()=" << origin.plane().position() << std::endl;
  //   // std::cout << "origin.plane().direction()=" << origin.plane().direction() << std::endl;
  //   // // std::cout << "=" <<  << std::endl;
  //   // // std::cout << "=" <<  << std::endl;
  //   //
  //   // // std::cout << "orig5d=" << orig5d << std::endl;
  //   // std::cout << "dest5dtmp=" << dest5dtmp << std::endl;
  //   // // std::cout << "destpos=" << destpos << std::endl;
  //   // // std::cout << "destplane=" << destplanepos << std::endl;
  //   // // std::cout << "origin.plane().direction()=" << origin.plane().direction() << std::endl;
  //   // Plane destplane(destplanepos,origin.plane().direction());
  //   // auto p1 = destplane.Local5DToGlobal6DParameters(dest5dtmp, true);
  //   // auto p2 = destplane.Local5DToGlobal6DParameters(dest5dtmp, false);
  //   // std::cout << "p1=" << p1 << std::endl;
  //   // std::cout << "p2=" << p2 << std::endl;
  //   // // SVector5 dest5d2(orig5d(0)-sperp*orig5d(2),orig5d(1)-sperp*orig5d(3),orig5d(2),orig5d(3),orig5d(4));
  //   // // SVector6 v6(destpos.X(),destpos.Y(),destpos.Z(),origin.momentum().X(),origin.momentum().Y(),origin.momentum().Z());
  //   // // std::cout << "locpos=" << destplane.Global6DToLocal5DParameters(v6) << std::endl;
  //   // SVector5 dest5d(0.,0.,orig5d(2),orig5d(3),orig5d(4));
  //   //add material effects, assume they are applied at destination (ok for short distance propagation)
  //   double deriv = 1.;
  //   boost::optional<double> pinv_new(true, orig5d(4));
  //   if(dodedx) {
  //     pinv_new = getInteractor()->dedx_prop(*pinv_new, origin.mass(), s, &deriv);
  //     if (pinv_new) dest5d(4) = *pinv_new;
  //   }
  //   // now update the errors
  //   SMatrixSym55 noise_matrix;
  //   if(domcs) {
  //     if (getInteractor().get() != 0) {
  // 	bool ok = getInteractor()->noise(origin.parameters(), origin.mass(), s, s>0, noise_matrix);//fixme s>0
  // 	if(!ok) {
  // 	  success = false;
  // 	  return origin;
  // 	}
  //     } else {
  // 	success = false;
  // 	return origin;
  //     }
  //   }
  //   //
  //   SMatrix55 pm;
  //   //
  //   pm(0,0) = 1.;      // du2/du1
  //   pm(1,0) = 0.;      // dv2/du1
  //   pm(2,0) = 0.;      // d(dudw2)/du1
  //   pm(3,0) = 0.;      // d(dvdw2)/du1
  //   pm(4,0) = 0.;      // d(pinv2)/du1
  //   //
  //   pm(0,1) = 0.;      // du2/dv1
  //   pm(1,1) = 1.;      // dv2/dv1
  //   pm(2,1) = 0.;      // d(dudw2)/dv1
  //   pm(3,1) = 0.;      // d(dvdw2)/dv1
  //   pm(4,1) = 0.;      // d(pinv2)/dv1
  //   //
  //   pm(0,2) = sperp;   // du2/d(dudw1);
  //   pm(1,2) = 0.;      // dv2/d(dudw1);
  //   pm(2,2) = 1.;      // d(dudw2)/d(dudw1);
  //   pm(3,2) = 0.;      // d(dvdw2)/d(dudw1);
  //   pm(4,2) = 0.;      // d(pinv2)/d(dudw1);
  //   //
  //   pm(0,3) = 0.;      // du2/d(dvdw1);
  //   pm(1,3) = sperp;   // dv2/d(dvdw1);
  //   pm(2,3) = 0.;      // d(dudw2)/d(dvdw1);
  //   pm(3,3) = 1.;      // d(dvdw2)/d(dvdw1);
  //   pm(4,3) = 0.;      // d(pinv2)/d(dvdw1);
  //   //
  //   pm(0,4) = 0.;      // du2/d(pinv1);
  //   pm(1,4) = 0.;      // dv2/d(pinv1);
  //   pm(2,4) = 0.;      // d(dudw2)/d(pinv1);
  //   pm(3,4) = 0.;      // d(dvdw2)/d(pinv1);
  //   pm(4,4) = deriv;   // d(pinv2)/d(pinv1);
  //   //
  //   std::cout << "pm=\n" << pm << std::endl;
  //   std::cout << "noise_matrix=\n" << noise_matrix << std::endl;    
  //   success = true;
  //   // return TrackState(dest5d,ROOT::Math::Similarity(pm,origin.covariance())+noise_matrix,Plane(destplanepos,origin.plane().direction()),origin.isTrackAlongPlaneDir(),origin.pID());
  //   return TrackState(dest5d,ROOT::Math::Similarity(pm,origin.covariance())+noise_matrix,Plane(destpos,origin.plane().direction()),origin.isTrackAlongPlaneDir(),origin.pID());
  // }
