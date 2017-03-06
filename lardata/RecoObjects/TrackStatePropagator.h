#ifndef PROPAGATORTOPLANE_H
#define PROPAGATORTOPLACE_H

#include <memory>

#include "lardata/RecoObjects/TrackState.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"

namespace detinfo {
  class DetectorProperties;
  class LArProperties;
}

namespace trkf {

  class TrackStatePropagator
  {
  public:

    using Plane      = recob::tracking::Plane;
    using Point_t    = recob::tracking::Point_t;
    using Vector_t   = recob::tracking::Vector_t;
    using SVector6   = recob::tracking::SVector6;

    struct Config {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;
      fhicl::Atom<double> minStep {
	Name("minStep"),
	Comment("Minimum propagation step length guaranteed.")
       };
      fhicl::Atom<double> maxElossFrac {
	Name("maxElossFrac"),
	Comment("Maximum propagation step length based on fraction of energy loss.")
       };
      fhicl::Atom<int> maxNit {
	Name("maxNit"),
	Comment("Maximum number of iterations.")
       };
      fhicl::Atom<double> tcut {
	Name("tcut"),
	Comment("Maximum delta ray energy for dE/dx.")
       };
    };
    using Parameters = fhicl::Table<Config>;
    
    /// Propagation direction enum.
    enum PropDirection {FORWARD=0, BACKWARD=1, UNKNOWN=2};
    
    /// Constructor.
    TrackStatePropagator(double minStep, double maxElossFrac, int maxNit, double tcut);
    explicit TrackStatePropagator(Parameters const & p) : TrackStatePropagator(p().minStep(),p().maxElossFrac(),p().maxNit(),p().tcut()) {}

    /// Destructor.
    virtual ~TrackStatePropagator();

    // Accessors.

    double getTcut() const {return fTcut;}


    TrackState propagateToPlane(bool& success, const TrackState& origin, const Plane& target, bool dodedx, bool domcs, PropDirection dir = FORWARD) const;

    TrackState rotateToPlane(bool& success, const TrackState& origin, const Plane& target) const;

    inline Point_t propagatedPosByDistance(const Point_t& origpos, const Vector_t& origmom, double s) const { return origpos+s*origmom; }
    inline Point_t propagatedPosByDistance(const SVector6& orig, double s) const { return Point_t(orig(0)+s*orig(3),orig(1)+s*orig(4),orig(2)+s*orig(5)); }

    double distanceToPlane(bool& success, const Point_t& origpos, const Vector_t& origdir, const Plane& target) const;
    inline double distanceToPlane(bool& success, const TrackState& origin, const Plane& target) const {
      return distanceToPlane(success, origin.position(), origin.momentum().Unit(), target);
    }

    std::pair<double, double> distancePairToPlane(bool& success, const Point_t& origpos, const Vector_t& origdir, const Plane& target) const;
    inline std::pair<double, double> distancePairToPlane(bool& success, const TrackState& origin, const Plane& target) const {
      return distancePairToPlane(success, origin.position(), origin.momentum().Unit(), target);
    }

    void apply_dedx(double& pinv, double dedx, double e1, double mass, double s, double& deriv) const;
    bool apply_mcs(double dudw, double dvdw, double pinv, double mass, double s, double range, double p, double e2, bool flipSign, SMatrixSym55& noise_matrix) const;

  private:

    double fMinStep;                                ///< Minimum propagation step length guaranteed.
    double fMaxElossFrac;                           ///< Maximum propagation step length based on fraction of energy loss.
    int    fMaxNit;                                 ///< Maximum number of iterations.
    double fTcut;                                   ///< Maximum delta ray energy for dE/dx.
    const detinfo::DetectorProperties* detprop;
    const detinfo::LArProperties* larprop;
  };
}

#endif
