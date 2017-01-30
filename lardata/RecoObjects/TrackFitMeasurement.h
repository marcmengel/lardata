#ifndef TRACKFITMEASUREMENT_H
#define TRACKFITMEASUREMENT_H

#include "lardataobj/RecoBase/TrackingTypes.h"
#include "lardataobj/RecoBase/TrackingPlane.h"

namespace trkf {

  using SVector5     = recob::tracking::SVector5;
  using SVector6     = recob::tracking::SVector6;
  using SMatrixSym55 = recob::tracking::SMatrixSym55;
  using Plane        = recob::tracking::Plane;
  using Point_t      = recob::tracking::Point_t;
  using Vector_t     = recob::tracking::Vector_t;

  class TrackState {//add also mass, global point and global momentum (make it trkf, there should be another version in recob that becomes persistent)
  public:
    TrackState(const SVector5& trackStatePar, const SMatrixSym55& trackStateCov, const Plane& plane, float mass)
      :fTrackStatePar(trackStatePar), fTrackStateCov(trackStateCov), fPlane(plane), fMass(mass)
    {
      SVector6 par6d = fPlane.Local5DToGlobal6DParameters(fTrackStatePar);
      fPos = Point_t(par6d[0],par6d[1],par6d[2]);
      fMom = Point_t(par6d[3],par6d[4],par6d[5]);
    }
    const SVector5&     parameters() const { return fTrackStatePar; }
    const SMatrixSym55& covariance() const { return fTrackStateCov; }
    const Plane&        plane()      const { return fPlane; }
    const Point_t&      position()   const { return fPos; }
    const Vector_t&     momentum()   const { return fMom; }
          float         mass()       const { return fMass; }               
    const SVector6      parameters6D() const { return SVector6(fPos.X(),fPos.Y(),fPos.Z(),fMom.X(),fMom.Y(),fMom.Z()); }
    void setCovariance(SMatrixSym55 trackStateCov) { fTrackStateCov = trackStateCov; }
  private:
    SVector5     fTrackStatePar;
    SMatrixSym55 fTrackStateCov;
    Plane        fPlane;
    float        fMass;
    Point_t      fPos;
    Vector_t     fMom;
  };

  class TrackFitMeasurement {
  public:
    /* TrackFitMeasurement() */
    /*   :fHitMeas(0),fHitMeasErr2(0),fTrackState(SVector5(),SMatrixSym55(),Plane()) {} */
    TrackFitMeasurement(double aHitMeas, double aHitMeasErr2, const SVector5& aTrackStatePar, const SMatrixSym55& aTrackStateCov, const Plane& aPlane, float aMass)
      :fHitMeas(aHitMeas),fHitMeasErr2(aHitMeasErr2),fTrackState(aTrackStatePar,aTrackStateCov,aPlane,aMass) {}
    TrackFitMeasurement(double aHitMeas, double aHitMeasErr2, const TrackState& aTrackState)
      :fHitMeas(aHitMeas),fHitMeasErr2(aHitMeasErr2),fTrackState(aTrackState) {}

    double hitMeas()    const { return fHitMeas; }
    double hitMeasErr2() const { return fHitMeasErr2; }
    const SVector5&     trackStatePar() const { return fTrackState.parameters(); }
    const SMatrixSym55& trackStateCov() const { return fTrackState.covariance(); }
    const TrackState& trackState() const { return fTrackState; }
    
    inline double residual() const { return fHitMeas-fTrackState.parameters()[0]; }
    inline double combinedError2() const { return fHitMeasErr2+fTrackState.covariance().At(0,0); }
    inline double combinedError() const { return sqrt(combinedError2()); }
    inline double chi2() const { return residual()*residual()/combinedError2(); }

  private:
    double       fHitMeas;
    double       fHitMeasErr2;
    TrackState   fTrackState;
  };

}

#endif
