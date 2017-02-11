#ifndef PROPAGATORTOPLANE_H
#define PROPAGATORTOPLACE_H

#include <memory>
#include "lardata/RecoObjects/Interactor.h"
#include "lardata/RecoObjects/TrackState.h"

namespace trkf {

  class PropagatorToPlane
  {
  public:

    using Plane      = recob::tracking::Plane;
    using Point_t    = recob::tracking::Point_t;
    using Vector_t   = recob::tracking::Vector_t;
    using SVector6   = recob::tracking::SVector6;

    /// Propagation direction enum.
    enum PropDirection {FORWARD, BACKWARD, UNKNOWN};

    /// Constructor.
    PropagatorToPlane(double maxStep, double tcut);

    /// Destructor.
    virtual ~PropagatorToPlane();

    // Accessors.

    double getTcut() const {return fTcut;}

    const std::shared_ptr<const Interactor>& getInteractor() const {return fInteractor;}

    TrackState propagateToPlane(bool& success, const TrackState& origin, const Plane& target, bool dodedx, bool domcs, PropDirection dir = FORWARD) const;

    TrackState rotateToPlane(bool& success, const TrackState& origin, const Plane& target) const;

    TrackState propagatedStateByPath(bool& success, const TrackState& origin, const double s, const double sperp, bool dodedx, bool domcs) const;

    inline Point_t propagatedPosByDistance(const Point_t& origpos, const Vector_t& origmom, double s) const { return origpos+s*origmom; }
    inline Point_t propagatedPosByDistance(const SVector6& orig, double s) const { return Point_t(orig(0)+s*orig(3),orig(1)+s*orig(4),orig(2)+s*orig(5)); }

    double distanceToPlane(bool& success, const Point_t& origpos, const Vector_t& origdir, const Plane& target, PropDirection dir = FORWARD) const;
    inline double distanceToPlane(bool& success, const TrackState& origin, const Plane& target, PropDirection dir = FORWARD) const {
      return distanceToPlane(success, origin.position(), origin.momentum().Unit(), target, dir);
    }

    std::pair<double, double> distancePairToPlane(bool& success, const Point_t& origpos, const Vector_t& origdir, const Plane& target, PropDirection dir = FORWARD) const;
    inline std::pair<double, double> distancePairToPlane(bool& success, const TrackState& origin, const Plane& target, PropDirection dir = FORWARD) const {
      return distancePairToPlane(success, origin.position(), origin.momentum().Unit(), target, dir);
    }

  private:

    double fMaxStep;                                ///< Maximum propagation step length.
    double fTcut;                                   ///< Maximum delta ray energy for dE/dx.
    std::shared_ptr<const Interactor> fInteractor;  ///< Interactor (for calculating dedx and noise).
  };
}

#endif
