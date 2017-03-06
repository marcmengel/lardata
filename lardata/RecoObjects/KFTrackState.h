#ifndef KFTRACKSTATE_H
#define KFTRACKSTATE_H

#include "lardata/RecoObjects/TrackState.h"

namespace trkf {

  class KFTrackState {
  public:
    //constructors
  KFTrackState(const SVector5& trackStatePar, const SMatrixSym55& trackStateCov, const Plane& plane, bool trackAlongPlaneDir, int pid)
    : fTrackState( std::move(TrackState(trackStatePar, trackStateCov, plane, trackAlongPlaneDir, pid)) ) {}
  KFTrackState(TrackState&& trackState)
    : fTrackState(std::move(trackState)) { }
    //
    bool updateWithHitState(const HitState& hitstate);
    bool updateWithHitState(const HitState& hitstate, const double slopevar);
    bool combineWithTrackState(const TrackState& trackstate);
    const TrackState& trackState() { return fTrackState; }
    void setTrackState(TrackState&& s) { fTrackState = std::move(s); }
    //
    const SVector5&     parameters()           const { return fTrackState.parameters(); }
    const SMatrixSym55& covariance()           const { return fTrackState.covariance(); }
    const Plane&        plane()                const { return fTrackState.plane(); }
    const Point_t&      position()             const { return fTrackState.position(); }
    const Vector_t&     momentum()             const { return fTrackState.momentum(); }
    int                 pID()                  const { return fTrackState.pID(); }
    double              mass()                 const { return fTrackState.mass(); }
    const SVector6      parameters6D()         const { return fTrackState.parameters6D(); }
    bool                isTrackAlongPlaneDir() const { return fTrackState.isTrackAlongPlaneDir(); }
    //
    double              residual      (const HitState& hitstate) const { return fTrackState.residual(hitstate); }
    double              combinedError2(const HitState& hitstate) const { return fTrackState.combinedError2(hitstate); }
    double              combinedError (const HitState& hitstate) const { return fTrackState.combinedError(hitstate); }
    double              chi2          (const HitState& hitstate) const { return fTrackState.chi2(hitstate); }
    //
    void                setCovariance(const SMatrixSym55& trackStateCov) { fTrackState.setCovariance(trackStateCov); }
    void                setParameters(const SVector5&     trackStatePar) { fTrackState.setParameters(trackStatePar); }
    //
  private:
    TrackState fTrackState;
  };

}
#endif
