#ifndef TRACKSTATE_H
#define TRACKSTATE_H

#include "lardataobj/RecoBase/TrackingTypes.h"
#include "lardataobj/RecoBase/TrackingPlane.h"
#include "larreco/TrackFinder/TrackingPlaneHelper.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h"

namespace trkf {

  using SVector5     = recob::tracking::SVector5;
  using SVector6     = recob::tracking::SVector6;
  using SMatrix55    = recob::tracking::SMatrix55;
  using SMatrixSym55 = recob::tracking::SMatrixSym55;
  using SMatrixSym66 = recob::tracking::SMatrixSym66;
  using Plane        = recob::tracking::Plane;
  using Point_t      = recob::tracking::Point_t;
  using Vector_t     = recob::tracking::Vector_t;

  namespace {
   const double mumass = 0.105658367;  // Muon
   const double pimass = 0.13957;      // Charged pion
   const double kmass = 0.493677;      // Charged kaon
   const double pmass = 0.938272;      // Proton
  }

  class HitState {
  public:
  HitState(double hitMeas, double hitMeasErr2, geo::WireID& wireId, const geo::WireGeo& wgeom)
    : fHitMeas(hitMeas),fHitMeasErr2(hitMeasErr2), fWireId(wireId),fPlane(recob::tracking::makePlane(wgeom)) {}
  HitState(double hitMeas, double hitMeasErr2, geo::WireID&& wireId, const geo::WireGeo& wgeom)
    : fHitMeas(hitMeas),fHitMeasErr2(hitMeasErr2), fWireId(std::move(wireId)),fPlane(recob::tracking::makePlane(wgeom)) {}
    double             hitMeas()     const { return fHitMeas; }
    double             hitMeasErr2() const { return fHitMeasErr2; }
    const Plane&       plane()       const { return fPlane; }
    const geo::WireID& wireId()      const { return fWireId; }
  private:
    double            fHitMeas;
    double            fHitMeasErr2;
    const geo::WireID fWireId;
    Plane             fPlane;
  };

  class TrackState {
  public:
  TrackState(const SVector5& trackStatePar, const SMatrixSym55& trackStateCov, const Plane& plane, bool trackAlongPlaneDir, int pid)
      :fTrackStatePar(trackStatePar), fTrackStateCov(trackStateCov), fPlane(plane), fPid(pid)
    {
      SVector6 par6d = fPlane.Local5DToGlobal6DParameters(fTrackStatePar,trackAlongPlaneDir);
      fPos = Point_t(par6d[0],par6d[1],par6d[2]);
      fMom = Point_t(par6d[3],par6d[4],par6d[5]);
    }
    const SVector5&     parameters() const { return fTrackStatePar; }
    const SMatrixSym55& covariance() const { return fTrackStateCov; }
    const Plane&        plane()      const { return fPlane; }
    const Point_t&      position()   const { return fPos; }
    const Vector_t&     momentum()   const { return fMom; }
    int                 pID()        const { return fPid; }
    double              mass()       const {
      if (abs(fPid)==13) return mumass; if (abs(fPid)==211) return pimass;
      if (abs(fPid)==321) return kmass; if (abs(fPid)==2212) return pmass;
      return util::kBogusD;
    }
    SVector6     parameters6D() const { return SVector6(fPos.X(),fPos.Y(),fPos.Z(),fMom.X(),fMom.Y(),fMom.Z()); }
    SMatrixSym66 covariance6D() const { return fPlane.Local5DToGlobal6DCovariance(fTrackStateCov, true, fMom); }
    //
    bool isTrackAlongPlaneDir() const { return fMom.Dot(fPlane.direction())>0; }
    //
    inline double residual      (const HitState& hitstate) const { return hitstate.hitMeas()-fTrackStatePar(0); }
    inline double combinedError2(const HitState& hitstate) const { return hitstate.hitMeasErr2()+fTrackStateCov(0,0); }
    inline double combinedError (const HitState& hitstate) const { return sqrt(combinedError2(hitstate)); }
    inline double chi2          (const HitState& hitstate) const { return residual(hitstate)*residual(hitstate)/combinedError2(hitstate); }
    //
    void setCovariance(const SMatrixSym55& trackStateCov) { fTrackStateCov = trackStateCov; }
    void setParameters(const SVector5&     trackStatePar) {
      fTrackStatePar = trackStatePar;
      SVector6 par6d = fPlane.Local5DToGlobal6DParameters(trackStatePar,isTrackAlongPlaneDir());
      fPos = Point_t(par6d[0],par6d[1],par6d[2]);
      fMom = Vector_t(par6d[3],par6d[4],par6d[5]);
    }
    //
  private:
    SVector5     fTrackStatePar;
    SMatrixSym55 fTrackStateCov;
    Plane        fPlane;
    int          fPid;
    Point_t      fPos;
    Vector_t     fMom;
  };

}

#endif
