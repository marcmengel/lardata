#ifndef KALMANFILTERTRACKMEASUREMENT
#define KALMANFILTERTRACKMEASUREMENT

//#include "lardataobj/RecoBase/TrackFitMeasurement.h"
#include "lardata/RecoObjects/TrackFitMeasurement.h"
#include "lardata/RecoObjects/SurfWireX.h"

//kalman filter calculations (updatedTrackState and updatedTrackStateCov) optimized for 1D measurement along the first component of the track state, i.e. assuming H=(1,0,0,0,0)

namespace trkf {

  using SVector5     = recob::tracking::SVector5;
  using SMatrix55    = recob::tracking::SMatrix55;
  using SMatrixSym55 = recob::tracking::SMatrixSym55;

  class KalmanFilterTrackMeasurement {
    
  public:
    
    KalmanFilterTrackMeasurement(unsigned int component, const recob::TrackFitMeasurement* measurement,const trkf::SurfWireX* surface)
    :comp_(component),meas_(measurement),surf_(surface) {}
    
    const SVector5&          predictedTrackStatePar() const { return meas_->trackStatePar(); }
    const SMatrixSym55&      predictedTrackStateCov() const { return meas_->trackStateCov(); }
    const recob::TrackState& predictedTrackState()    const { return meas_->trackState(); }
    
    SVector5          updatedTrackStatePar() const;
    SMatrixSym55      updatedTrackStateCov() const;
    recob::TrackState updatedTrackState()    const { return recob::TrackState(updatedTrackStatePar(),updatedTrackStateCov()); }
    
    recob::TrackState combinedTrackState(recob::TrackState state1, recob::TrackState state2, bool& success) const;
    
    const trkf::SurfWireX* surface() const {return surf_; }
    
  private:
    unsigned int comp_;
    const recob::TrackFitMeasurement* meas_;
    const trkf::SurfWireX* surf_;
  };
}

#endif
