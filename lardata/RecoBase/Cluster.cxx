/** ****************************************************************************
 * @file   Cluster.cxx
 * @brief  Declaration of cluster object.
 * @author mitchell.soderberg@yale.edu
 * @see    Cluster.h
 * 
 * Changes:
 * 20141212 Gianluca Petrillo (petrillo@fnal.gov)
 *   data architecture revision changes
 * 
 * ****************************************************************************/


// class header
#include "lardata/RecoBase/Cluster.h"

// C/C++ standard libraries
#include <iomanip> // std::setw(), ...
#include <cmath> // std::sqrt
#include <algorithm> // std::min(), std::max()


namespace {
  template <typename T>
  inline T sqr(T v) { return v*v; }
} // local namespace


namespace recob{
  
  const Cluster::SentryArgument_t Cluster::Sentry;
  
  
  //----------------------------------------------------------------------
  Cluster::Cluster()
    : fNHits(0)
    , fEndWires{ 0., 0. }
    , fSigmaEndWires{ 0., 0. }
    , fEndTicks{ 0., 0. }
    , fSigmaEndTicks{ 0., 0. }
    , fEndCharges{ 0., 0. }
    , fAngles{ 0., 0. }
    , fOpeningAngles{ 0., 0. }
    , fChargeSum{ 0., 0. }
    , fChargeStdDev{ 0., 0. }
    , fChargeAverage{ 0., 0. }
    , fMultipleHitDensity(0.)
    , fWidth(0.)
    , fID(InvalidID)
    , fView(geo::kUnknown)
    , fPlaneID()
  {} // Cluster::Cluster()
  
  
  //----------------------------------------------------------------------
  Cluster::Cluster(
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
    SentryArgument_t /* sentry = Sentry */
    )
    : fNHits(n_hits)
    , fEndWires{ start_wire, end_wire }
    , fSigmaEndWires{ sigma_start_wire, sigma_end_wire }
    , fEndTicks{ start_tick, end_tick }
    , fSigmaEndTicks{ sigma_start_tick, sigma_end_tick }
    , fEndCharges{ start_charge, end_charge }
    , fAngles{ start_angle, end_angle }
    , fOpeningAngles{ start_opening, end_opening }
    , fChargeSum{ integral, summedADC }
    , fChargeStdDev{ integral_stddev, summedADC_stddev }
    , fChargeAverage{}
    , fMultipleHitDensity(multiple_hit_density)
    , fWidth(width)
    , fID(ID)
    , fView(view)
    , fPlaneID(plane)
  {
    
    for (unsigned int mode = cmFirstMode; mode < NChargeModes; ++mode)
      fChargeAverage[mode] = (fNHits > 0)? fChargeSum[mode] / fNHits: 0.;
    
  } // Cluster::Cluster(float...)
  
  
#if 0
  // FIXME DELME
  //----------------------------------------------------------------------
  //  Addition operator.
  //
  // The two clusters must have the same view and must lay on the same plane.
  // If one of the clusters has an invalid plane, the sum will inherit the
  // other's plane. If both are invalid, sum will also have an invalid plane.
  //
  Cluster Cluster::operator + (Cluster const& other) const {
    
    // throw exception if the clusters are not from the same view
    if (other.View() != View()) return {};
    
    if (other.hasPlane() && hasPlane() && (other.Plane() != Plane())) return {};
    
    const unsigned int n_hits = NHits() + other.NHits();
    double charge_stddev[2];
    for (unsigned int mode = cmFirstMode; mode < NChargeModes; ++mode) {
      
      // this assumes that the definition of the std dev is unbiased...
      const double this_variance = sqr(ChargeStdDev(mode)) * (NHits()-1.)/NHits();
      const double other_variance
        = sqr(other.ChargeStdDev(mode)) * (other.NHits()-1.) / other.NHits();
      
      const double e2 = (
        (sqr(NHits()) + sqr(other.NHits())) / sqr(n_hits)
          * sqr(ChargeAverage(mode) - other.ChargeAverage(mode))
        + NHits() * this_variance
        + other.NHits() * other_variance
        ) / (n_hits - 1.); // unbiased
      
      charge_stddev[mode] = std::sqrt(e2);
    } // for charge mode
    
    return Cluster (
      std::min(StartWire(), other.StartWire()), // start_wire
      std::min(StartTick(), other.StartTick()), // start_tick
      0.,                                       // start_charge
      0.,                                       // start_angle
      0.,                                       // start_opening
      std::max(EndWire(), other.EndWire()),     // end_wire
      std::max(EndTick(), other.EndTick()),     // end_tick
      0.,                                       // end_charge
      0.,                                       // end_angle
      0.,                                       // end_opening
      Integral() + other.Integral(),            // integral
      charge_stddev[cmFit],                     // integral_stddev
      SummedADC() + other.SummedADC(),          // summedADC
      charge_stddev[cmADC],                     // summedADC_stddev
      n_hits,                                   // n_hits
      0.,                                       // multiple_hit_density
      0.,                                       // width
      InvalidID,                                // ID
      View(),                                   // view
      hasPlane()? Plane(): other.Plane()        // plane
      );
    
  } // Cluster::operator+ ()
  
#endif // 0
  
  
  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& o, Cluster const& c) {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o << "Cluster ID "     << std::setw(5)  << std::right << c.ID()
      << " : Cryo = "      << std::setw(3)  << std::right << c.Plane().Cryostat
      << " TPC = "         << std::setw(3)  << std::right << c.Plane().TPC
      << " Plane = "       << std::setw(3)  << std::right << c.Plane().Plane
      << " View = "        << std::setw(3)  << std::right << c.View() 
      << " StartWire = "   << std::setw(7)  << std::right << c.StartWire()
      << " EndWire = "     << std::setw(7)  << std::right << c.EndWire()
      << " StartTime = "   << std::setw(9)  << std::right << c.StartTick()
      << " EndTime = "     << std::setw(9)  << std::right << c.EndTick()
      << " N hits =      " << std::setw(5)  << std::right << c.NHits()
      << " Width =       " << std::setw(5)  << std::right << c.Width()
      << " Charge(fit) = " << std::setw(10) << std::right << c.Integral()
      << " Charge(ADC) = " << std::setw(10) << std::right << c.SummedADC()
      ;
    return o;
  } // operator<< (ostream, Cluster)
  
  
  //----------------------------------------------------------------------
  // < operator.
  //
  bool operator < (Cluster const& a, Cluster const& b) {
    
    if (a.hasPlane() && b.hasPlane() && a.Plane() != b.Plane())
      return a.Plane() < b.Plane();
    if (a.View() != b.View())
      return a.View() < b.View();
    if (a.ID() != b. ID())
      return a.ID() < b.ID();
    if (a.StartTick() != b.StartTick())
      return a.StartTick() < b.StartTick();
    if (a.EndTick() != b.EndTick())
      return a.EndTick() < b.EndTick();
    
    return false; // they are equal enough
  } // operator < (Cluster, Cluster)
  
}// namespace

