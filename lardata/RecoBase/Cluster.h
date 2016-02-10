/** ****************************************************************************
 * @file   Cluster.h
 * @brief  Declaration of cluster object.
 * @author mitchell.soderberg@yale.edu
 * @see    Cluster.cxx
 * 
 * Changes:
 * 20141212 Gianluca Petrillo (petrillo@fnal.gov)
 *   data architecture revision changes (v14)
 * 20150211 Gianluca Petrillo (petrillo@fnal.gov)
 *   moved from MultipleHitWires() to MultipleHitDensity() (v15)
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
   * @note A cluster lives in a plane of inhomogeneous coordinates: wire number
   * and tick number. Different ways to make them homogeneous are available.
   * For example, knowing the drift velocity (assuming it constant) and the wire
   * pitch, it is possible to convert both coordinates in a measure of distance.
   * Where the wire and time coordinates need to be compared, they are
   * converted into physical distances. Wire coordinate includes the wire pitch
   * information and it can be defined as the distance (in centimetres) from the
   * wire #0 (and negative if it lies on the opposite side than wire #0 with
   * respect to wire #0) of the point. The tick coordinate is converted into
   * a distance from the wire plane (in centimetres) by including the drift
   * velocity. The absolute time for which this coordinate is 0 is defined to
   * be as an "absolute trigger time". This is still ambiguous enough, but
   * matter of fact there should be no need of absolute wire or tick coordinates
   * but only of their difference; for example, to define angles, dT/dW (or some
   * similar quantity) is used.
   * More advanced knowledge of the geometry or status of the detector may lead
   * to different wire pitch or drift velocity: in that case, these quantities
   * need to be recomputed, although it is conceivable that the ones with simple
   * constant pitch and drift are often an approximation close enough.
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
      
      /// @name Data referring to start and end of the cluster
      /// @{
      /// Wire coordinate of the start and end of the cluster (may lie between wires);
      /// index is intended to be of type ClusterEnds_t.
      float fEndWires[NEnds];
      
      /// Uncertainty on wire coordinate of the start and end of the cluster;
      /// index is intended to be of type ClusterEnds_t.
      float fSigmaEndWires[NEnds];
      
      /// Tick coordinate of the start and end of the cluster (may be set between ticks);
      /// index is intended to be of type ClusterEnds_t.
      float fEndTicks[NEnds];
      
      /// Uncertainty on tick coordinate of the start and end of the cluster;
      /// index is intended to be of type ClusterEnds_t.
      float fSigmaEndTicks[NEnds];
      
      /// Charge on the start and end wire of the cluster.
      /// This value can be result of extrapolation or average from a range of hits.
      /// index is intended to be of type ClusterEnds_t.
      float fEndCharges[NEnds];
      
      /// Angle of the start and end of the cluster, defined in [-pi,pi]
      /// and so that tan(angle) = dT/dW (or, more precisely, `angle = atan2(dT, dW)`).
      /// The elements are expressed in physical distances and therefore this
      /// represents a physical angle on the plane orthogonal to the wires in
      /// the view and containing the drift direction ("x"); the angle is 0 or
      /// @f$ \pi @f$ when lying on the wire plane, @f$ \pm\pi/2 @f$ when
      /// pointing into/from the wire plane.
      /// Index is intended to be of type ClusterEnds_t.
      float fAngles[NEnds];
      
      /// Opening angle of the cluster shape at the start and end of the cluster.
      /// The coordinates are expressed in physical distances and therefore this
      /// represents a physical opening angle on the plane orthogonal to the
      /// wires in the view and containing the drift direction ("x").
      /// Index is intended to be of type ClusterEnds_t.
      float fOpeningAngles[NEnds];
      /// @}
      
      
      /// @name Data derived from hit charge
      /// @{
      /// Sum of the charge of all hits in the cluster.
      /// Index is intended to be of type ChargeMode_t
      float fChargeSum[NChargeModes];
      
      /// Standard deviation of the charge of hits.
      /// Index is intended to be of type ChargeMode_t
      float fChargeStdDev[NChargeModes];
      
      ///< Average of the charge of all hits in the cluster (fChargeSum/NHits()).
      /// Index is intended to be of type ChargeMode_t
      float fChargeAverage[NChargeModes];
      /// @}
      
      /// Density of wires in the cluster with more than one hit.
      float fMultipleHitDensity;
      
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
      
      /// Type of sentry argument
      typedef struct {} SentryArgument_t;
      
      /// Value for an invalid cluster ID
      static constexpr ID_t InvalidID = -1;
      
      /// An instance of the sentry object
      static const SentryArgument_t Sentry;
      
      
      /**
       * @brief Constructor: assigns all the fields
       * @param start_wire wire coordinate of the start of the cluster
       * @param sigma_start_wire uncertainty on start_wire
       * @param start_tick tick coordinate of the start of the cluster
       * @param sigma_start_tick uncertainty on start_tick
       * @param start_charge charge on the start wire
       * @param start_angle angle of the start of the cluster, in [-pi,pi]
       * @param start_opening opening angle at the start of the cluster
       * @param end_wire wire coordinate of the end of the cluster
       * @param sigma_end_wire uncertainty on end_wire
       * @param end_tick tick coordinate of the end of the cluster
       * @param sigma_end_tick uncertainty on end_tick
       * @param end_charge charge on the end wire
       * @param end_angle angle of the end of the cluster, in [-pi,pi]
       * @param end_opening opening angle at the end of the cluster
       * @param integral total charge from fitted shape of hits
       * @param integral_stddev standard deviation of hit charge from fitted shape
       * @param summedADC total charge from signal ADC of hits
       * @param summedADC_stddev standard deviation of signal ADC of hits
       * @param n_hits number of hits in the cluster
       * @param multiple_hit_density density of wires with more than one hit
       * @param width a measure of the cluster width
       * @param ID cluster ID
       * @param view view for this cluster
       * @param plane location of the start of the cluster
       * @param sentry a sentry instance
       *
       * Coordinates are in homogenized units.
       * 
       * See the documentation of the relative data members for more details on
       * the definition and constraints of the various constructor arguments.
       * 
       * @note The sentry parameter can be optionally specified so that the
       * compiler will realize if the number of parameters in the constructor
       * has varied.
       */
      Cluster(
        float start_wire,
        float sigma_start_wire,
        float start_tick,
        float sigma_start_tick,
        float start_charge,
        float start_angle,
        float start_opening,
        float end_wire,
        float sigma_end_wire,
        float end_tick,
        float sigma_end_tick,
        float end_charge,
        float end_angle,
        float end_opening,
        float integral,
        float integral_stddev,
        float summedADC,
        float summedADC_stddev,
        unsigned int n_hits,
        float multiple_hit_density,
        float width,
        ID_t ID,
        geo::View_t view,
        geo::PlaneID const& plane,
        SentryArgument_t sentry = Sentry
        );
      
      
      /// Copy constructor: automatically generated
      Cluster(Cluster const&) = default;
      
      /// Move constructor: as copy, but source cluster gets an invalid ID
      Cluster(Cluster&& from): Cluster(from) { from.fID = InvalidID; }
      
      /// Copy assignment: automatically generated
      Cluster& operator= (Cluster const&) = default;
      
      /// Move assignment: as copy, but source cluster gets an invalid ID
      Cluster& operator= (Cluster&& from)
        {
          if (&from != this) { operator=(from); from.fID = InvalidID; }
          return *this;
        }
      
      /// Destructor: automatically generated
      ~Cluster() = default;
      
      
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
      
      /**
       * @brief Returns the uncertainty on wire coordinate of the start of the cluster
       * @return uncertainty on wire coordinate of the start of the cluster
       * @see StartWire()
       *
       * The wire uncertainty is in wire units (as for StartWire()).
       */
      float SigmaStartWire() const { return fSigmaEndWires[clStart]; }
      
      /**
       * @brief Returns the uncertainty on tick coordinate of the start of the cluster
       * @return uncertainty on tick coordinate of the start of the cluster
       * @see StartTick()
       *
       * The tick uncertainty is in tick units (as for StartTick()).
       */
      float SigmaStartTick() const { return fSigmaEndTicks[clStart]; }
      
      
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
      
      /**
       * @brief Returns the uncertainty on wire coordinate of the end of the cluster
       * @return uncertainty on wire coordinate of the end of the cluster
       * @see EndWire()
       *
       * The wire uncertainty is in wire units (as for EndWire()).
       */
      float SigmaEndWire() const { return fSigmaEndWires[clEnd]; }
      
      /**
       * @brief Returns the uncertainty on tick coordinate of the end of the cluster
       * @return uncertainty on tick coordinate of the end of the cluster
       * @see EndTick()
       *
       * The tick uncertainty is in tick units (as for EndTick()).
       */
      float SigmaEndTick() const { return fSigmaEndTicks[clEnd]; }
      
      
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
      
      
      //@{
      /**
       * @brief Returns the uncertainty on wire coordinate of one of the end sides of the cluster
       * @param side clStart for start, clEnd for end of the cluster
       * @return uncertainty on wire coordinate of the requested end of the cluster
       * @see SigmaStartWire(), SigmaEndWire(), SigmaTickCoord(), TimeCoord()
       *
       * Usage of this method is similar to WireCoord().
       */
      float SigmaWireCoord(ClusterEnds_t side) const { return fSigmaEndWires[side]; }
      float SigmaWireCoord(unsigned int side) const { return fSigmaEndWires[side]; }
      //@}
      
      //@{
      /**
       * @brief Returns the uncertainty on tick coordinate of one of the end sides of the cluster
       * @param side clStart for start, clEnd for end of the cluster
       * @return uncertainty on tick coordinate of the requested end of the cluster
       * @see SigmaStartTick(), SigmaEndTick(), SigmaWireCoord(), TimeCoord()
       *
       * Usage of this method is similar to TimeCoord().
       */
      float SigmaTickCoord(ClusterEnds_t side) const { return fSigmaEndTicks[side]; }
      float SigmaTickCoord(unsigned int side) const { return fSigmaEndTicks[side]; }
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
       * returned, defined @f$ \alpha \in [ -\pi, \pi ]@f$ and so that
       * @f$ \tan(\alpha) = dT/dW @f$ (or, more precisely,
       * `angle = atan2(dT, dW)`).
       * The elements are expressed in physical distances and therefore this
       * represents a physical angle on the plane orthogonal to the wires in
       * the view and containing the drift direction ("x"); the angle is 0 or
       * @f$ \pi @f$ when lying on the wire plane, @f$ \pm\pi/2 @f$ when
       * pointing into/from the wire plane.
       * The angle is pointing toward the inside of the cluster (that is,
       * @f$ dW @f$ is positive going from the first wire on).
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float StartAngle() const { return fAngles[clStart]; }
      
      /**
       * @brief Returns the opening angle at the start of the cluster
       * @return opening angle in radians
       * @see EndOpeningAngle(), OpeningAngle()
       * 
       * The returned value is from physical coordinates and in the range
       * @f$[ 0, \pi ]@f$.
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
       * returned, defined @f$ \alpha \in [ -\pi, \pi ]@f$ and so that
       * @f$ \tan(\alpha) = dT/dW @f$ (or, more precisely,
       * `angle = atan2(dT, dW)`).
       * The elements are expressed in physical distances and therefore this
       * represents a physical angle on the plane orthogonal to the wires in
       * the view and containing the drift direction ("x"); the angle is 0 or
       * @f$ \pi @f$ when lying on the wire plane, @f$ \pm\pi/2 @f$ when
       * pointing into/from the wire plane.
       * The angle is pointing toward the outside of the cluster (that is,
       * @f$ dW @f$ is positive going toward the last wire).
       * This value can be result of extrapolation or average from a range of
       * hits.
       */
      float EndAngle() const { return fAngles[clEnd]; }
      
      /**
       * @brief Returns the opening angle at the end of the cluster
       * @return opening angle in radians
       * @see StartOpeningAngle(), OpeningAngle()
       * 
       * The returned value is from homogenized coordinates and in the range
       * @f$[ 0, \pi ]@f$.
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
       * is returned, defined @f$ \alpha \in [ -\pi, \pi ]@f$ and so that
       * @f$ \tan(\alpha) = dT/dW @f$ (or, more precisely,
       * `angle = atan2(dT, dW)`).
       * The elements are expressed in physical distances and therefore this
       * represents a physical angle on the plane orthogonal to the wires in
       * the view and containing the drift direction ("x"); the angle is 0 or
       * @f$ \pi @f$ when lying on the wire plane, @f$ \pm\pi/2 @f$ when
       * pointing into/from the wire plane.
       * The angle is pointing so that increasing wire number yields positive
       * @f$ dW @f$.
       * This value can be result of extrapolation or average from a range of
       * hits.
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
       * 
       * @note Cluster class older than version 14 had a Charge() method too;
       * the new one is not strictly equivalent, although in practice replacing
       * the old `Charge()` with `Charge(cmFit)` should do the trick.
       * The recommended update is to use `Integral()` instead.
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
      
      /**
       * @brief Density of wires in the cluster with more than one hit
       * @return density of wires in the cluster with more than one hit
       *
       * Returns a quantity defined as NMultiHitWires / Length,
       * where NMultiHitWires is the number of wires which have more than just
       * one hit amd Length is an estimation of the length of the cluster, in
       * centimetres.
       */
      float MultipleHitDensity() const { return fMultipleHitDensity; }
      
      
      /// A measure of the cluster width, in homogenized units.
      float Width() const { return fWidth; }
      
      
      /**
       * @brief Identifier of this cluster
       * @return the identifier of this cluster
       * @see isValid()
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
      bool hasPlane() const { return fPlaneID.isValid; }
      
      
      /// Returns if the cluster is valid (that is, if its ID is not invalid)
      bool isValid() const { return ID() != InvalidID; }
      
      
      friend std::ostream& operator << (std::ostream& o, Cluster const& c);
      friend bool          operator <  (Cluster const& a, Cluster const& b);
    
#endif // __GCCXML__
    
  }; // class Cluster
  
} // namespace recob


#endif //CLUSTER_H
