/** ****************************************************************************
 * @file   Cluster.h
 * @brief  Declaration of cluster object.
 * @author mitchell.soderberg@yale.edu
 * @see    Cluster.cxx
 * 
 * Changes:
 * 20141212 Gianluca Petrillo (petrillo@fnal.gov)
 *   data architecture revision changes
 * 
 * ****************************************************************************/

#ifndef CLUSTER_H
#define CLUSTER_H

// C/C++ standard library
#ifndef __GCCXML__
# include <iosfwd> // std::ostream
#endif

// LArSoft libraries
#include "SimpleTypesAndConstants/geo_types.h" // geo::PlaneID, geo::View_t


namespace recob {
  
  
  /**
   * @brief Set of hits with a 2D structure
   *
   * A cluster is a set of reconstructed hits supposed to originate from the
   * same physical entity.
   * A cluster lies in a single plane (of a single TPC).
   * 
   * Clusters provide the base of reconstructed 3D objects: tracks and showers.
   * The cluster class contains information that helps characterizing the
   * originating particle and discriminating its signature as track-like
   * or shower-like.
   * 
   * A cluster is supposed to describe the reconstruction of a transiting
   * particle, and can therefore be thought as having a start, the location
   * where it is first seen in time, and an end, the location where it is seen
   * last. In practice, it is often hard to determine by the shape which
   * tip is which, and in case of showers the end might be hard to determine.
   * As a consequence, although the two tips are called "start" and "end", their
   * order is not unerringly; the tip labelled "start" is still deemed to be
   * more likely the beginning of the cluster rather than the end.
   * In the extreme case the "end" should be considered just as an alternative
   * cluster start.
   * 
   * @note Some quantities are expressed in "homogenized" units.
   * A cluster lives in a plane of inhomogeneous coordinates: wire number and
   * tick number. Different ways to make them homogeneous are available.
   * For example, knowing the drift velocity (assuming it constant) and the wire
   * pitch, it is possible to convert both coordinates in a measure of distance.
   * The simpler approach used here is to equalize one wire spacing with one
   * tick. This approach does not reflect the physical dimensions of the cluster
   * in standard metrics (e.g. centimetres), but it is simple, immediate and
   * requires much less restrictive hypotheses (uniformity of spacing in both
   * wire and tick, as opposed to uniformity of drift velocity).
   */
  class Cluster {
      
    public:
      typedef int ID_t; ///< Type of cluster ID
      
      
      typedef enum {
        clStart,       ///< Represents the most likely start of the cluster
        clEnd,         ///< Represents the end, or the alternative start, of the cluster
        NEnds,         ///< End count
        clFirstEnd = 0 ///< Just an alias for loops
      } ClusterEnds_t; ///< Used to decide which end to use
      
      typedef enum {
        cmFit,          ///< Sums from the fitted hit values
        cmADC,          ///< Sums directly from ADC counts
        NChargeModes,   ///< End count
        cmFirstMode = 0 ///< Just an alias for loops
      } ChargeMode_t; ///< Used to decide which style of charge sum to use
      
      
      /// Default constructor: an empty cluster
      Cluster();
      
    private:
      
      unsigned int fNHits; ///< Number of hits in the cluster
      
      //@{
      /// Wire coordinate of the start and end of the cluster (may lie between wires);
      /// index is intended to be of type ClusterEnds_t.
      float fEndWires[NEnds];
      
      /// Tick coordinate of the start and end of the cluster (may be set between ticks);
      /// index is intended to be of type ClusterEnds_t.
      float fEndTicks[NEnds];
      
      /// Charge on the start and end wire of the cluster.
      /// This value can be result of extrapolation or average from a range of hits.
      /// index is intended to be of type ClusterEnds_t.
      float fEndCharges[NEnds];
      
      /// Angle of the start and end of the cluster, defined in [-pi,pi]
      /// and so that tan(angle) = dT/dW (or, more precisely, angle = atan2(dT, dW).
      /// The elements are expressed in homogenized units.
      /// Index is intended to be of type ClusterEnds_t.
      float fAngles[NEnds];
      
      /// Opening angle of the cluster shape at the start and end of the cluster.
      /// The opening is expressed in "homogenized" coordinates.
      /// Index is intended to be of type ClusterEnds_t.
      float fOpeningAngles[NEnds];
      //@}
      
      
      //@{
      /// Sum of the charge of all hits in the cluster.
      /// Index is intended to be of type ChargeMode_t
      float fChargeSum[NChargeModes];
      
      /// Standard deviation of the charge of hits.
      /// Index is intended to be of type ChargeMode_t
      float fChargeStdDev[NChargeModes];
      
      ///< Average of the charge of all hits in the cluster (fChargeSum/NHits()).
      /// Index is intended to be of type ChargeMode_t
      float fChargeAverage[NChargeModes];
      //@}
      
      /// Number of wires covered by the cluster, divided by the number of hits
      /// in the cluster.
      float fNWiresOverNHits;
      
      /// A measure of the cluster width, in homogenized units.
      float fWidth;
      
      /// Identifier of this cluster.
      /// It should be unique per event and per algorithm.
      /// An invalid cluster can be defined by having an ID Cluster::InvalidID.
      ID_t fID;
      
      
      geo::View_t fView; ///< View for this cluster
      
      geo::PlaneID fPlaneID; ///< Location of the start of the cluster

#ifndef __GCCXML__
      
    public:
      
      /// Value for an invalid cluster ID
      static constexpr ID_t InvalidID = -1;
      
      
      /**
       * @brief Constructor: assigns all the fields
       * @param start_wire wire coordinate of the start of the cluster
       * @param start_tick tick coordinate of the start of the cluster
       * @param start_charge charge on the start wire
       * @param start_angle angle of the start of the cluster, in [-pi,pi]
       * @param start_opening opening angle at the start of the cluster
       * @param end_wire wire coordinate of the end of the cluster
       * @param end_tick tick coordinate of the end of the cluster
       * @param end_charge charge on the end wire
       * @param end_angle angle of the end of the cluster, in [-pi,pi]
       * @param end_opening opening angle at the end of the cluster
       * @param integral total charge from fitted shape of hits
       * @param integral_stddev standard deviation of hit charge from fitted shape
       * @param summedADC total charge from signal ADC of hits
       * @param summedADC_stddev standard deviation of signal ADC of hits
       * @param n_hits number of hits in the cluster
       * @param wires_over_hits wires covered by cluster, divided by number of hits
       * @param width a measure of the cluster width
       * @param ID cluster ID
       * @param view view for this cluster
       * @param plane location of the start of the cluster
       *
       * Coordinates are in homogenized units.
       * 
       * See the documentation of the relative data members for more details on
       * the definition and constraints of the various constructor arguments.
       */
      Cluster(
        float start_wire,
        float start_tick,
        float start_charge,
        float start_angle,
        float start_opening,
        float end_wire,
        float end_tick,
        float end_charge,
        float end_angle,
        float end_opening,
        float integral,
        float integral_stddev,
        float summedADC,
        float summedADC_stddev,
        unsigned int n_hits,
        float wires_over_hits,
        float width,
        ID_t ID,
        geo::View_t view,
        geo::PlaneID const& plane
        );
      
      
      /// @{
      /// @name Accessors
      
      /// Number of hits in the cluster
      unsigned int NHits() const { return fNHits; }
      
      /** **********************************************************************
       * @brief Returns the wire coordinate of the start of the cluster
       * @return wire coordinate of the start of the cluster (may lie between wires)
       * @see EndWire(), WireCoord(), StartTick()
       *
       * The wire coordinate is in wire units (the homogenized coordinate),
       * but can have a fractional part describing the relative distance from
       * the previous wire.
       */
      float StartWire() const { return fEndWires[clStart]; }
      
      /**
       * @brief Returns the tick coordinate of the start of the cluster
       * @return tick coordinate of the start of the cluster (may br fractional)
       * @see EndTick(), TickCoord(), StartWire()
       *
       * The tick coordinate is in tick units (the homogenized coordinate),
       * but can have a fractional part describing the relative time from
       * the previous tick.
       */
      float StartTick() const { return fEndTicks[clStart]; }
      
      
      /** **********************************************************************
       * @brief Returns the wire coordinate of the end of the cluster
       * @return wire coordinate of the end of the cluster (may lie between wires)
       * @see StartWire(), WireCoord(), EndTick()
       *
       * The "end" of the cluster is, in the more ambiguous cluster shapes,
       * defined as an alternative cluster start.
       * The wire coordinate is in wire units (the homogenized coordinate),
       * but can have a fractional part describing the relative distance from
       * the previous wire.
       */
      float EndWire() const { return fEndWires[clEnd]; }
      
      /**
       * @brief Returns the tick coordinate of the end of the cluster
       * @return tick coordinate of the end of the cluster (may be fractional)
       * @see StartTick(), TickCoord(), EndWire()
       *
       * The "end" of the cluster is, in the more ambiguous cluster shapes,
       * defined as an alternative cluster start.
       * The tick coordinate is in tick units (the homogenized coordinate),
       * but can have a fractional part describing the relative time from
       * the previous tick.
       */
      float EndTick() const { return fEndTicks[clEnd]; }
      
      
      //@{
      /** **********************************************************************
       * @brief Returns the wire coordinate of one of the end sides of the cluster
       * @param side clStart for start, clEnd for end of the cluster
       * @return wire coordinate of the requested end of the cluster (may lie between wires)
       * @see StartWire(), EndWire(), TickCoord()
       *
       * The wire coordinate is in wire units (the homogenized coordinate),
       * but can have a fractional part describing the relative distance from
       * the previous wire.
       * 
       * For algorithms not distinguishing start and end, all the ends can be
       * tested by the loop:
       *     
       *     for (unsigned int side = recob::Cluster::clFirstEnd;
       *       side < recob::Cluster::NEnds; ++side)
       *     {
       *       float wire = cluster.WireCoord(side);
       *       float tick = cluster.TickCoord(side);
       *       // ...
       *     } // for
       *     
       */
      float WireCoord(ClusterEnds_t side) const { return fEndWires[side]; }
      float WireCoord(unsigned int side) const { return fEndWires[side]; }
      //@}
      
      //@{
      /**
       * @brief Returns the tick coordinate of one of the end sides of the cluster
       * @param side clStart for start, clEnd for end of the cluster
       * @return tick coordinate of the requested end of the cluster (may be fractional)
       * @see StartTick(), EndTick(), WireCoord()
       *
       * The tick coordinate is in tick units (the homogenized coordinate),
       * but can have a fractional part describing the relative time from
       * the previous tick.
       * 
       * For algorithms not distinguishing start and end, all the ends can be
       * tested by the loop:
       *     
       *     for (unsigned int side = recob::Cluster::clFirstEnd;
       *       side < recob::Cluster::NEnds; ++side)
       *     {
       *       float wire = cluster.WireCoord(side);
       *       float tick = cluster.TickCoord(side);
       *       // ...
       *     } // for
       *     
       */
      float TickCoord(ClusterEnds_t side) const { return fEndTicks[side]; }
      float TickCoord(unsigned int side) const { return fEndTicks[side]; }
      //@}
      
      
      /** **********************************************************************
       * @brief Returns the charge on the first wire of the cluster
       * @return charge on the first wire in ADC counts, negative if not available
       * @see EndCharge(), EdgeCharge()
       * 
       * The returned value is in unit of ADC count, although it may be
       * fractional.
       * This value can be result of extrapolation or average from a range of hits.
       */
      float StartCharge() const { return fEndCharges[clStart]; }
      
      /**
       * @brief Returns the starting angle of the cluster
       * @return angle in radians
       * @see EndAngle(), Angle()
       * 
       * The angle of the group of hits at the start position of the cluster is
       * returned. This is from homogenized coordinates and in the range
       * @f$ \alpha \in [ -\pi, \pi ]@f$, and so that
       * @f$ \tan(\alpha) = dT/dW @f$ (or, more precisely,
       * `alpha = atan2(dT, dW)`).
       * The angle is pointing toward the inside of the cluster (that is,
       * @f$ dW @f$ is positive going from the first wire on).
       */
      float StartAngle() const { return fAngles[clStart]; }
      
      /**
       * @brief Returns the opening angle at the start of the cluster
       * @return opening angle in radians
       * @see EndOpeningAngle(), OpeningAngle()
       * 
       * The returned value is from homogenized coordinates and in the range
       * @f$[ 0, \pi ]@f$.
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float StartOpeningAngle() const { return fOpeningAngles[clStart]; }
      
      
      /**
       * @brief Returns the charge on the last wire of the cluster
       * @return charge on the last wire in ADC counts, negative if not available
       * @see StartCharge(), EdgeCharge()
       * 
       * The returned value is in unit of ADC count, although it may be
       * fractional.
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float EndCharge() const { return fEndCharges[clEnd]; }
      
      /**
       * @brief Returns the ending angle of the cluster
       * @return angle in radians
       * @see StartAngle(), Angle()
       * 
       * The angle of the group of hits at the end position of the cluster is
       * returned. This is from homogenized coordinates and in the range
       * @f$ \alpha \in [ -\pi, \pi ]@f$, and so that
       * @f$ \tan(\alpha) = dT/dW @f$ (or, more precisely,
       * `alpha = atan2(dT, dW)`).
       * The angle is pointing toward the outside of the cluster (that is,
       * @f$ dW @f$ is positive going toward the last wire).
       */
      float EndAngle() const { return fAngles[clEnd]; }
      
      /**
       * @brief Returns the opening angle at the end of the cluster
       * @return opening angle in radians
       * @see StartOpeningAngle(), OpeningAngle()
       * 
       * The returned value is from homogenized coordinates and in the range
       * @f$[ 0, \pi ]@f$.
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float EndOpeningAngle() const { return fOpeningAngles[clEnd]; }
      
      
      //@{
      /**
       * @brief Returns the charge on the first or last wire of the cluster
       * @param side clStart for start, clEnd for end of the cluster
       * @return charge on the requested wire in ADC counts, negative if not available
       * @see StartCharge(), EndCharge()
       * 
       * The returned value is in unit of ADC count, although it may be
       * fractional.
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float EdgeCharge(ClusterEnds_t side) const { return fEndCharges[side]; }
      float EdgeCharge(unsigned int side) const { return fEndCharges[side]; }
      //@}
      
      //@{
      /**
       * @brief Returns the angle at either end of the cluster
       * @param side clStart for start, clEnd for end of the cluster
       * @return angle in radians
       * @see StartAngle(), EndAngle()
       * 
       * The angle of the group of hits at the specified position of the cluster
       * is returned. This is from homogenized coordinates and in the range
       * @f$ \alpha \in [ -\pi, \pi ]@f$, and so that
       * @f$ \tan(\alpha) = dT/dW @f$ (or, more precisely,
       * `alpha = atan2(dT, dW)`).
       * The angle is pointing so that increasing wire number yields positive
       * @f$ dW @f$.
       */
      float Angle(ClusterEnds_t side) const { return fAngles[side]; }
      float Angle(unsigned int side) const { return fAngles[side]; }
      //@}
      
      //@{
      /**
       * @brief Returns the opening angle at either end of the cluster
       * @return opening angle in radians
       * @see StartOpeningAngle(), EndOpeningAngle()
       * 
       * The returned value is from homogenized coordinates and in the range
       * @f$[ 0, \pi ]@f$.
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float OpeningAngle(ClusterEnds_t side) const
        { return fOpeningAngles[side]; }
      float OpeningAngle(unsigned int side) const
        { return fOpeningAngles[side]; }
      //@}
      
      
      /** **********************************************************************
       * @brief Returns the total charge of the cluster from hit shape
       * @return total charge of the cluster from hit shape, in ADC counts
       * @see IntegralStdDev(), IntegralAverage(), SummedADC(), Charge()
       *
       * The total charge is computed as the sum of the charge of all the hits.
       * The charge of a single hit includes the hit shape (fit) and is obtained
       * by recob::Hit::Integral().
       */
      float Integral() const { return fChargeSum[cmFit]; }
      
      /**
       * @brief Returns the standard deviation of the charge of the cluster hits
       * @return standard deviation of the charge of the cluster hits, in ADC counts
       * @see Integral(), IntegralAverage(), SummedADCstdDev(), ChargeStdDev()
       *
       * The charge of a single hit includes the hit shape (fit) and is obtained
       * by recob::Hit::Integral().
       * It should return 0 if less than two hits are available.
       */
      float IntegralStdDev() const { return fChargeStdDev[cmFit]; }
      
      /**
       * @brief Returns the average charge of the cluster hits
       * @return average of the charge of the cluster hits, in ADC counts
       * @see Integral(), IntegralStdDev(), SummedADCaverage(), ChargeAverage()
       *
       * The charge of a single hit includes the hit shape (fit) and is obtained
       * by recob::Hit::Integral().
       * It should return 0 if no hit is available.
       */
      float IntegralAverage() const { return fChargeAverage[cmFit]; }
      
      
      /** **********************************************************************
       * @brief Returns the total charge of the cluster from signal ADC counts
       * @return total charge of the cluster from signal ADC, in ADC counts
       * @see SummedADCstdDev(), SummedADCaverage(), Integral(), Charge()
       *
       * The total charge is computed as the sum of the charge of all the hits.
       * The charge of a single hit includes the signal ADC counts and is
       * obtained by recob::Hit::SummedADC().
       */
      float SummedADC() const { return fChargeSum[cmADC]; }
      
      /**
       * @brief Returns the standard deviation of the signal ADC counts of the cluster hits
       * @return standard deviation of the signal of the cluster hits, in ADC counts
       * @see SummedADC(), SummedADCaverage(), IntegralStdDev(), ChargeStdDev()
       *
       * The charge of a single hit includes the signal ADC counts and is
       * obtained by recob::Hit::SummedADC().
       * It should return 0 if less than two hits are available.
       */
      float SummedADCstdDev() const { return fChargeStdDev[cmADC]; }
      
      /**
       * @brief Returns the average signal ADC counts of the cluster hits
       * @return average of the signal of the cluster hits, in ADC counts
       * @see SummedADC(), SummedADCstdDev(), IntegralAverage(), ChargeAverage()
       *
       * The charge of a single hit includes the signal ADC counts and is
       * obtained by recob::Hit::SummedADC().
       * It should return 0 if no hit is available.
       */
      float SummedADCaverage() const { return fChargeAverage[cmADC]; }
      
      
      //@{
      /** **********************************************************************
       * @brief Returns the total charge of the cluster
       * @param mode cmFit to use fitted hit shape, cmADC for signal ADCs
       * @return total charge of the cluster, in ADC counts
       * @see ChargeStdDev(), ChargeAverage(), SummedADC(), Integral()
       *
       * The total charge is computed as the sum of the charge of all the hits.
       * The charge of a single hit comes from the fitted hit shape
       * (recob::Hit::Integral()) for cmFit, and signal ADC counts
       * (recob::Hit::SummedADC()) for cmADC.
       */
      float Charge(ChargeMode_t mode) const { return fChargeSum[mode]; }
      float Charge(unsigned int mode) const { return fChargeSum[mode]; }
      //@}
      
      //@{
      /**
       * @brief Returns the standard deviation of charge of the cluster hits
       * @return standard deviation of charge of the cluster hits, in ADC counts
       * @see Charge(), ChargeAverage(), SummedADCstdDev(), IntegralStdDev()
       *
       * The charge of a single hit comes from the fitted hit shape
       * (recob::Hit::Integral()) for cmFit, and signal ADC counts
       * (recob::Hit::SummedADC()) for cmADC.
       * It should return 0 if less than two hits are available.
       */
      float ChargeStdDev(ChargeMode_t mode) const
        { return fChargeStdDev[mode]; }
      float ChargeStdDev(unsigned int mode) const
        { return fChargeStdDev[mode]; }
      //@}
      
      //@{
      /**
       * @brief Returns the average charge of the cluster hits
       * @return average of the charge of the cluster hits, in ADC counts
       * @see Charge(), ChargeStdDev(), SummedADCaverage(), IntegralAverage()
       *
       * The charge of a single hit comes from the fitted hit shape
       * (recob::Hit::Integral()) for cmFit, and signal ADC counts
       * (recob::Hit::SummedADC()) for cmADC.
       * It should return 0 if no hit is available.
       */
      float ChargeAverage(ChargeMode_t mode) const
        { return fChargeAverage[mode]; }
      float ChargeAverage(unsigned int mode) const
        { return fChargeAverage[mode]; }
      //@}
      
      /// Number of wires covered by the cluster, divided by the number of hits
      /// in the cluster.
      float WiresOverHits() const { return fNWiresOverNHits; }
      
      
      /// A measure of the cluster width, in homogenized units.
      float Width() const { return fWidth; }
      
      
      /**
       * @brief Identifier of this cluster
       * @return the identifier of this cluster
       * 
       * The identifier should be unique per event and per algorithm.
       * An invalid cluster can be defined by having an ID Cluster::InvalidID.
       */
      ID_t  ID() const { return fID; }
      
      /// Returns the view for this cluster
      geo::View_t View() const { return fView; }
      
      /// Returns the plane ID this cluster lies on
      geo::PlaneID Plane() const { return fPlaneID; }
      
      /// @}
      
      /// Returns whether geometry plane is valid
      bool hasPlane() const;
      
      
#if 0
      // FIXME DELME
      /// Moves the cluster to the specified plane
      Cluster& MoveToPlane(const geo::PlaneID& new_plane);
      
      /// Makes the plane of this cluster invalid
      Cluster& InvalidatePlane();
      
      /**
       * @brief Merges two clusters
       * @param other another cluster to add to this one
       * @return a new, temporary cluster with hits from this and other
       * @todo needs details to be defined
       * 
       * The two clusters must have the same view and must lay on the same
       * plane. Otherwise, an invalid cluster is returned.
       * If instead only one of the two locations is valid, the resulting
       * cluster inherits it.
       *
       */
      Cluster operator + (Cluster const& other) const;
      
#endif // 0
      
      friend std::ostream& operator << (std::ostream& o, Cluster const& c);
      friend bool          operator <  (Cluster const& a, Cluster const& b);
    
#endif // __GCCXML__
    
  }; // class Cluster
  
} // namespace recob

#ifndef __GCCXML__

inline bool recob::Cluster::hasPlane() const { return fPlaneID.isValid; }

#if 0 // FIXME DELME
inline recob::Cluster&       recob::Cluster::MoveToPlane(const geo::PlaneID& new_plane)
  { fPlaneID = new_plane; return *this; }

inline recob::Cluster&       recob::Cluster::InvalidatePlane()
  { return MoveToPlane(geo::PlaneID()); }
#endif // 0

#endif // __GCCXML__

#endif //CLUSTER_H
