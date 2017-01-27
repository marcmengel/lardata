#ifndef TRACKFITMEASUREMENT_H
#define TRACKFITMEASUREMENT_H

#include "lardataobj/RecoBase/TrackingTypes.h"

namespace recob {

  using SVector5     = recob::tracking::SVector5;
  using SMatrixSym55 = recob::tracking::SMatrixSym55;
  
  class TrackState {
  public:
    TrackState(SVector5 trackStatePar, SMatrixSym55 trackStateCov)
      :fTrackStatePar(trackStatePar), fTrackStateCov(trackStateCov) {}
    const SVector5&     parameters() const { return fTrackStatePar; }
    const SMatrixSym55& covariance() const { return fTrackStateCov; }
    void setCovariance(SMatrixSym55 trackStateCov) { fTrackStateCov = trackStateCov; }
  private:
    SVector5     fTrackStatePar;
    SMatrixSym55 fTrackStateCov;
  };

  class TrackFitMeasurement {
  public:
    TrackFitMeasurement()
      :fHitMeas(0),fHitMeasErr2(0),fTrackState(SVector5(),SMatrixSym55()) {}
    TrackFitMeasurement(double aHitMeas, double aHitMeasErr2, SVector5 aTrackStatePar, SMatrixSym55 aTrackStateCov)
      :fHitMeas(aHitMeas),fHitMeasErr2(aHitMeasErr2),fTrackState(aTrackStatePar,aTrackStateCov) {}
    TrackFitMeasurement(double aHitMeas, double aHitMeasErr2, TrackState aTrackState)
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
