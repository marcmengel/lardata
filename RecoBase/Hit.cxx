////////////////////////////////////////////////////////////////////////
// \version $Id: Hit.cxx,v 1.7 2010/02/15 20:32:46 brebel Exp $
//
// \brief Definition of Hit reconstruction object
//
// \author mitchell.soderberg@yale.edu
////////////////////////////////////////////////////////////////////////

// Hit header
#include "RecoBase/Hit.h"

// C/C++ standard library
#include <iomanip>
#include <ostream>


namespace recob {

  //----------------------------------------------------------------------
  Hit::Hit()
    : fHitSignal() // empty
    , fChannel(raw::InvalidChannelID)
    , fStartTick(0)
    , fEndTick(0)
    , fPeakTime(0.)
    , fSigmaPeakTime(-1.)
    , fRMS(0.)
    , fPeakAmplitude(0.)
    , fSigmaPeakAmplitude(-1.)
    , fSummedADC(0.)
    , fIntegral(0.)
    , fSigmaIntegral(-1.)
    , fMultiplicity(0)
    , fLocalIndex(-1)
    , fGoodnessOfFit(0.)
    , fNDF(-1)
    , fView(geo::kUnknown)
    , fSignalType(geo::kMysteryType)
    , fWireID() // uinvalid
    {}
  
  //----------------------------------------------------------------------
  Hit::Hit(
    raw::ChannelID_t        channel,
    raw::TDCtick_t          start_tick,
    raw::TDCtick_t          end_tick,
    float                   peak_time,
    float                   sigma_peak_time,
    float                   rms,
    float                   peak_amplitude,
    float                   sigma_peak_amplitude,
    float                   summedADC,
    float                   hit_integral,
    float                   hit_sigma_integral,
    short int               multiplicity,
    short int               local_index,
    float                   goodness_of_fit,
    int                     dof,
    geo::View_t             view,
    geo::SigType_t          signal_type,
    geo::WireID             wireID,
    std::vector<float> const& signal
    )
    : fHitSignal(signal)
    , fChannel(channel)
    , fStartTick(start_tick)
    , fEndTick(end_tick)
    , fPeakTime(peak_time)
    , fSigmaPeakTime(sigma_peak_time)
    , fRMS(rms)
    , fPeakAmplitude(peak_amplitude)
    , fSigmaPeakAmplitude(sigma_peak_amplitude)
    , fSummedADC(summedADC)
    , fIntegral(hit_integral)
    , fSigmaIntegral(hit_sigma_integral)
    , fMultiplicity(multiplicity)
    , fLocalIndex(local_index)
    , fGoodnessOfFit(goodness_of_fit)
    , fNDF(dof)
    , fView(view)
    , fSignalType(signal_type)
    , fWireID(wireID)
  {}
  
  //----------------------------------------------------------------------
  Hit::Hit(
    raw::ChannelID_t        channel,
    raw::TDCtick_t          start_tick,
    raw::TDCtick_t          end_tick,
    float                   peak_time,
    float                   sigma_peak_time,
    float                   rms,
    float                   peak_amplitude,
    float                   sigma_peak_amplitude,
    float                   summedADC,
    float                   hit_integral,
    float                   hit_sigma_integral,
    short int               multiplicity,
    short int               local_index,
    float                   goodness_of_fit,
    int                     dof,
    geo::View_t             view,
    geo::SigType_t          signal_type,
    geo::WireID             wireID,
    std::vector<float>&&    signal
    )
    : fHitSignal(signal)
    , fChannel(channel)
    , fStartTick(start_tick)
    , fEndTick(end_tick)
    , fPeakTime(peak_time)
    , fSigmaPeakTime(sigma_peak_time)
    , fRMS(rms)
    , fPeakAmplitude(peak_amplitude)
    , fSigmaPeakAmplitude(sigma_peak_amplitude)
    , fSummedADC(summedADC)
    , fIntegral(hit_integral)
    , fSigmaIntegral(hit_sigma_integral)
    , fMultiplicity(multiplicity)
    , fLocalIndex(local_index)
    , fGoodnessOfFit(goodness_of_fit)
    , fNDF(dof)
    , fView(view)
    , fSignalType(signal_type)
    , fWireID(wireID)
  {}
  
  
  //----------------------------------------------------------------------
  // ostream operator.
  //
  std::ostream& operator<< (std::ostream& o, Hit const& hit) {
    o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
    o <<   " Channel "          << std::setw(5) << std::right << hit.Channel()
      <<   " View = "           << std::setw(3) << std::right << hit.View()
      <<   " Wire = "           << std::setw(3) << std::right << hit.WireID()
      << "\n\tStartTick = "     << std::setw(7) << std::right << hit.StartTick()
        << "\tEndTick = "       << std::setw(7) << std::right << hit.EndTick()
        << "\tPeakTime = "      << std::setw(7) << std::right << hit.PeakTime()
        << " +/- "              << std::setw(7) << std::right << hit.SigmaPeakTime()
      << "\n\tIntegral = "      << std::setw(7) << std::right << hit.Integral()
        << " +/- "              << std::setw(7) << std::right << hit.SigmaIntegral()
        << "\tADCsum = "        << std::setw(7) << std::right << hit.SummedADC()
        << "\tMultiplicity = "  << std::setw(5) << std::right << hit.LocalIndex() << " of " << hit.Multiplicity()
        << "\tGoodnessOfFit = " << std::setw(7) << std::right << hit.GoodnessOfFit()
        << " DoF = "           << std::setw(7) << std::right << hit.DegreesOfFreedom()
      << std::endl;
    return o;
  } // operator<< (std::ostream, Hit)
  
  
  //----------------------------------------------------------------------
  // < operator.
  //
  bool operator < (const Hit & a, const Hit & b)
  {
    if (a.Channel() != b.Channel())
      return a.Channel() < b.Channel();
    if (a.View() != b.View())
      return a.View() < b.View();
    if (a.StartTick() != b.StartTick())
      return a.StartTick() < b.StartTick();

    return false; //They are equal
  } // operator< (Hit, Hit)
  
  
  //----------------------------------------------------------------------
} // namespace recob
