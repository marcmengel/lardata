#ifndef PROPAGATORXXX_H
#define PROPAGATORXXX_H

#include <memory>
#include "lardata/RecoObjects/Propagator.h"
#include "lardata/RecoObjects/Interactor.h"
#include "lardata/RecoObjects/TrackFitMeasurement.h"

namespace trkf {

  class PropagatorXXX //: public Propagator
  {
  public:

    using Plane      = recob::tracking::Plane;
    using Point_t    = recob::tracking::Point_t;
    using Vector_t   = recob::tracking::Vector_t;
    using SVector6   = recob::tracking::SVector6;

    /// Propagation direction enum.
    enum PropDirection {FORWARD, BACKWARD, UNKNOWN};

    /// Constructor.
    PropagatorXXX(double maxStep, double tcut, const std::shared_ptr<const Interactor>& interactor);

    /// Destructor.
    virtual ~PropagatorXXX();

    // Accessors.

    double getTcut() const {return fTcut;}

    const std::shared_ptr<const Interactor>& getInteractor() const {return fInteractor;}

    bool propagateToPlane(const TrackState& origin, const Plane& target, TrackState& result, PropDirection dir = FORWARD) const;

    bool propagateToPlaneNoMaterial(const TrackState& origin, const Plane& target, TrackState& result, PropDirection dir = FORWARD) const;

    TrackState propagatedStateByPath(const TrackState& origin, const float s, bool& success) const;

    inline Point_t propagatedPosByDistance(const Point_t& origpos, const Vector_t& origmom, float s) const { return origpos+s*origmom; }
    inline Point_t propagatedPosByDistance(const SVector6& orig, float s) const { return Point_t(orig(0)+s*orig(3),orig(1)+s*orig(4),orig(2)+s*orig(5)); }
    inline float distanceToPlane(const TrackState& origin, const Plane& target, PropDirection dir = FORWARD) const { return distanceToPlane(origin.position(), origin.momentum().Unit(), target, dir); }
    float distanceToPlane(const Point_t& origpos, const Vector_t& origdir, const Plane& target, PropDirection dir = FORWARD) const;

  private:

    // Attributes.
    double fMaxStep;                                ///< Maximum propagation step length.
    double fTcut;                                   ///< Maximum delta ray energy for dE/dx.
    bool fDoDedx;                                   ///< Energy loss enable flag.
    std::shared_ptr<const Interactor> fInteractor;  ///< Interactor (for calculating noise).
  };
}

#endif
