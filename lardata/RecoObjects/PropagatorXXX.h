#ifndef PROPAGATORXXX_H
#define PROPAGATORXXX_H

#include <memory>
#include "lardata/RecoObjects/Propagator.h"
#include "lardata/RecoObjects/Interactor.h"
#include "lardata/RecoObjects/TrackFitMeasurement.h"

namespace trkf {

  class PropagatorXXX
  {
  public:

    using Plane      = recob::tracking::Plane;
    using Point_t    = recob::tracking::Point_t;
    using Vector_t   = recob::tracking::Vector_t;
    using SVector6   = recob::tracking::SVector6;

    /// Propagation direction enum.
    enum PropDirection {FORWARD, BACKWARD, UNKNOWN};

    /// Constructor.
    PropagatorXXX(double maxStep, double tcut);

    /// Destructor.
    virtual ~PropagatorXXX();

    // Accessors.

    double getTcut() const {return fTcut;}

    const std::shared_ptr<const Interactor>& getInteractor() const {return fInteractor;}

    bool propagateToPlane(const TrackState& origin, const Plane& target, TrackState& result, bool dodedx, bool domcs, PropDirection dir = FORWARD) const;

    TrackState propagatedStateByPath(const TrackState& origin, const double s, const double sperp, bool dodedx, bool domcs, bool& success) const;

    inline Point_t propagatedPosByDistance(const Point_t& origpos, const Vector_t& origmom, double s) const { return origpos+s*origmom; }
    inline Point_t propagatedPosByDistance(const SVector6& orig, double s) const { return Point_t(orig(0)+s*orig(3),orig(1)+s*orig(4),orig(2)+s*orig(5)); }
    inline double distanceToPlane(const TrackState& origin, const Plane& target, PropDirection dir = FORWARD) const { return distanceToPlane(origin.position(), origin.momentum().Unit(), target, dir); }
    double distanceToPlane(const Point_t& origpos, const Vector_t& origdir, const Plane& target, PropDirection dir = FORWARD) const;
    inline std::pair<double, double> distancePairToPlane(const TrackState& origin, const Plane& target, PropDirection dir = FORWARD) const { return distancePairToPlane(origin.position(), origin.momentum().Unit(), target, dir); }
    std::pair<double, double> distancePairToPlane(const Point_t& origpos, const Vector_t& origdir, const Plane& target, PropDirection dir = FORWARD) const;

  private:

    double fMaxStep;                                     ///< Maximum propagation step length.
    double fTcut;                                        ///< Maximum delta ray energy for dE/dx.
    std::shared_ptr<const Interactor> fInteractor;  ///< Interactor (for calculating dedx and noise).
  };
}

#endif
