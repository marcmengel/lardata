#ifndef KALMANFILTERTRACKMEASUREMENT
#define KALMANFILTERTRACKMEASUREMENT

//#include "lardataobj/RecoBase/TrackFitMeasurement.h"
#include "lardata/RecoObjects/TrackFitMeasurement.h"

// wrapper around recob::TrackFitMeasurement to make KalmanFilter calculations
// optimized for 1D measurement along a component of the track state, i.e. assuming H=(1,0,0,0,0) if component=0.

namespace trkf {

  using SVector5     = recob::tracking::SVector5;
  using SMatrix55    = recob::tracking::SMatrix55;
  using SMatrixSym55 = recob::tracking::SMatrixSym55;

  class KalmanFilterTrackMeasurement {
    
  public:
    
    KalmanFilterTrackMeasurement(const TrackFitMeasurement* measurement, unsigned int component = 0)
      : meas_(measurement),comp_(component) {}
    
    const SVector5&     predictedTrackStatePar() const { return meas_->trackStatePar(); }
    const SMatrixSym55& predictedTrackStateCov() const { return meas_->trackStateCov(); }
    const TrackState&   predictedTrackState()    const { return meas_->trackState(); }
    
    SVector5     updatedTrackStatePar() const;
    SMatrixSym55 updatedTrackStateCov() const;
    TrackState   updatedTrackState()    const { return TrackState(updatedTrackStatePar(),updatedTrackStateCov(),meas_->trackState().plane(),meas_->trackState().isTrackAlongPlaneDir(),meas_->trackState().mass()); }
    
    bool combineWithState(const TrackState& state, TrackState& result) const;
    
  private:
    const TrackFitMeasurement* meas_;
    unsigned int comp_;
  };
}

#endif
