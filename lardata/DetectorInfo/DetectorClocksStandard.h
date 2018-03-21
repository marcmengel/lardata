////////////////////////////////////////////////////////////////////////
//
// DetectorClocks.h
//
//     This class provides electronics various electronics clocks. Currently supports
//     three types of clocks: TPC, Optical, and Trigger in order to support the
//     MicroBooNE experiment.
//  
//     Formally known as TimeService.
//
////////////////////////////////////////////////////////////////////////

#ifndef DETINFO_DETCLOCKSSTD_H
#define DETINFO_DETCLOCKSSTD_H

#include "fhiclcpp/ParameterSet.h"

#include "lardata/DetectorInfo/DetectorClocks.h"

namespace detinfo{

  /**
   * @brief Implementation of `detinfo::DetectorClocks` interface with fixed
   *        settings from configuration.
   * 
   * In this implementation, all quantities are defined at configuration time
   * and are unchanged for the duration of the entire job, with the exception
   * of the trigger time (see below).
   * 
   * Configuration parameters
   * =========================
   * 
   * All configuration parameters are mandatory, unless explicitly stated.
   * 
   * * *G4RefTime* (_nanoseconds_):
   *     @ref DetectorClocksSimulationTime "simulation (Geant4) start time" in
   *     @ref DetectorClocksElectronicsTime "electronics time scale", i.e. when
   *     time `0.0` of simulation happens in the electronics time scale
   * * *TriggerOffsetTPC*: time elapsed between the
   *     @ref DetectorClocksTPCelectronicsStartTime "start of the TPC readout clock"
   *     and the @ref DetectorClocksHardwareTrigger "hardware trigger"; it can
   *     be expressed in one of two ways:
   *       * negative number [&micro;s]: the offset of the start of the TPC
   *         readout clock start respect to the trigger time (where negative
   *         means that the clock starts _before_ the trigger arrives)
   *       * positive number [ticks]: the number of TPC readout clock tick at
   *         which the trigger arrives; despite this being a tick number, it can
   *         be fractional for added precision
   *     
   *     For example, `TriggerOffsetTPC` of `-1600.0` means that the TDC clock
   *     starts 1.6 milliseconds before the hardware trigger. `TriggerOffsetTPC`
   *     of `3200.0` means that the trigger arrives at the exact start of tick
   *     3200 of the TPC readout. In this example, if the sampling frequency of
   *     that readout is 2 MHz, these two settings are equivalent.
   * * *FramePeriod* (_microseconds_): duration of an electronics clock frame;
   *     @ref DetectorClocksIntroClocks "all clocks" share the same frame period
   * * *ClockSpeedTPC* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "TPC electronics clock"
   * * *ClockSpeedOptical* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "optical electronics clock"
   * * *ClockSpeedTrigger* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "trigger electronics clock"
   * * *ClockSpeedExternal* (_megahertz_): frequency of the
   *     @ref DetectorClocksIntroClocks "external electronics clock"
   * * *DefaultTrigTime* (_microseconds_): the default
   *     @ref DetectorClocksHardwareTrigger "hardware trigger time", measured in
   *     the @ref DetectorClocksElectronicsTime "electronics time frame"
   * * *DefaultBeamTime* (_microseconds_): the default
   *     @ref DetectorClocksBeamGateOpening "beam gate opening time", measured
   *     in the @ref DetectorClocksElectronicsTime "electronics time frame"
   * * *TrigModuleName* (_string_): input tag for the trigger data product
   *     (see "Trigger time" section below)
   * * *InheritClockConfig* (_boolean_): whether to inherit the configuration
   *     from previous jobs (see "Consistency check" below)
   * 
   * 
   * Consistency check
   * ------------------
   * 
   * The consistency check feature verifies that the current configuration of
   * `detinfo::DetectorClocksStandard` is compatible with the one from previous
   * jobs. It is expected that when this feature is enabled
   * (`InheritClockConfig` is configured to be `true`), either the configuration
   * is overridden to follow the previous ones, or an exception is thrown in
   * case of inconsistency.
   * 
   * The service provider (`detinfo::DetectorClocksStandard`) does not provide
   * a facility to inherit configuration from a previous job, but it stores the
   * expectation whether this should happen (`InheritClockConfig()`).
   * The service provider manager is in charge of implementing this feature
   * (see `detinfo::DetectorClocksStandardService` for the manager in the _art_
   * environment).
   * 
   * 
   * Timing specifics
   * =================
   * 
   * For the general timing requirements, see the documentation of
   * `detinfo::DetectorClocks`.
   * 
   * 
   * Trigger time
   * -------------
   * 
   * In this implementation, the trigger time is always defined to match the
   * beam gate time, and both are at 0.0 &micro;s. Despite the fact that two
   * values are required in the configuration for default trigger and beam time,
   * these values are not propagated to the current trigger and bean gate
   * opening times (unless [issue #19446](https://cdcvs.fnal.gov/redmine/issues/19446) is solved).
   * 
   * The manager of this provider is expected to set those values by calling
   * `detinfo::DetectorClocksStandard::SetTriggerTime()`. See
   * `detinfo::DetectorClocksStandardService` for the manager in the _art_
   * environment.
   * 
   * 
   * @bug `ExternalClock()` clock is never initialized!
   * 
   */
  class DetectorClocksStandard : public DetectorClocks {

  public:
    DetectorClocksStandard();
    DetectorClocksStandard(fhicl::ParameterSet const& pset);
    DetectorClocksStandard(DetectorClocksStandard const&) = delete;
    virtual ~DetectorClocksStandard() {};
    
    bool Configure(fhicl::ParameterSet const& pset);
    bool Update(uint64_t ts=0);
    
    /**
     * @see `detinfo::DetectorClocks::TriggerOffsetTPC()`
     * 
     * This offset is set via configuration parameter `TriggerOffsetTPC`.
     */
    virtual double TriggerOffsetTPC() const
    { 
      if (fTriggerOffsetTPC<0)
	return fTriggerOffsetTPC; 
      else
	return -fTriggerOffsetTPC/fTPCClock.Frequency(); //convert ticks to us
    }
    
    void debugReport() const;
    
    /**
     * @brief Returns the input tag of the trigger data product.
     * @return the input tag of the trigger data product (as string)
     * 
     * The trigger module name is set directly in the configuration as
     * `TrigModuleName`.
     */
    std::string TrigModuleName() const { return fTrigModuleName; }
    
    /// Sets trigger and beam gate time from default configuration values.
    void SetDefaultTriggerTime();

    /// Given Geant4 time [ns], returns relative time [us] w.r.t. electronics time T0 
    virtual double G4ToElecTime(double g4_time) const {return g4_time * 1.e-3 - fG4RefTime; }
    
    /// Trigger electronics clock time in [us]
    virtual double TriggerTime() const { return fTriggerTime; }
    
    /// Beam gate electronics clock time in [us]
    virtual double BeamGateTime() const { return fBeamGateTime; }

    virtual std::vector<std::string> ConfigNames() const { return fConfigName; }
    virtual std::vector<double> ConfigValues() const { return fConfigValue; }

    void SetConfigValue(size_t i, double val) { fConfigValue[i] = val; }

    /**
     * @brief Setter for trigger times.
     * @param trig_time @ref DetectorClocksHardwareTrigger "hardware trigger time" in @ref DetectorClocksElectronicsTime "electronics time scale"
     * @param beam_time @ref DetectorClocksBeamGateOpening "beam gate opening time" in @ref DetectorClocksElectronicsTime "electronics time scale"
     * 
     * The @ref DetectorClocksHardwareTrigger "hardware trigger" and
     * @ref DetectorClocksBeamGateOpening "beam gate opening" times are set, and
     * the electronic clocks are updated to store the new trigger time.
     */
    virtual void SetTriggerTime(double trig_time, double beam_time)
    { 
      fTriggerTime  = trig_time;
      fBeamGateTime = beam_time;
      fTPCClock.SetTime(trig_time);
      fOpticalClock.SetTime(trig_time);
      fTriggerClock.SetTime(trig_time);
    }

    //
    // Getters of TPC ElecClock
    //
    /// Borrow a const TPC clock with time set to Trigger time [us]
    virtual const ::detinfo::ElecClock& TPCClock() const
    { return fTPCClock; }

    /// Create a TPC clock for a given time [us] from clock counting start
    virtual ::detinfo::ElecClock TPCClock(double time) const 
	{ return ::detinfo::ElecClock(time,fTPCClock.FramePeriod(),fTPCClock.Frequency());}

    /// Create a TPC clock for a given sample/frame number in TPC clock frequency
    detinfo::ElecClock TPCClock(unsigned int sample,
		       unsigned int frame) const
      { detinfo::ElecClock clock = TPCClock(); clock.SetTime(sample,frame); return clock; }
    
    //
    // Getters of Optical ElecClock
    //
    /// Borrow a const Optical clock with time set to Trigger time [us]
    virtual const detinfo::ElecClock& OpticalClock() const
    { return fOpticalClock; }

    /// Create a Optical clock for a given time [us] from clock counting start
    virtual detinfo::ElecClock OpticalClock(double time) const 
      { return detinfo::ElecClock(time,fOpticalClock.FramePeriod(),fOpticalClock.Frequency());}

    /// Create a Optical clock for a given sample/frame number in Optical clock frequency
    virtual detinfo::ElecClock OpticalClock(unsigned int sample,
			   unsigned int frame) const
    { detinfo::ElecClock clock = OpticalClock(); clock.SetTime(sample,frame); return clock; }

    //
    // Getters of Trigger ElecClock
    //
    /// Borrow a const Trigger clock with time set to Trigger time [us]
    virtual const detinfo::ElecClock& TriggerClock() const
    { return fTriggerClock; }

    /// Create a Trigger clock for a given time [us] from clock counting start    
    virtual detinfo::ElecClock TriggerClock(double time) const 
    { return detinfo::ElecClock(time,fTriggerClock.FramePeriod(),fTriggerClock.Frequency());}

    /// Create a Trigger clock for a given sample/frame number in Trigger clock frequency
    virtual detinfo::ElecClock TriggerClock(unsigned int sample,
			   unsigned int frame) const
    { detinfo::ElecClock clock = TriggerClock(); clock.SetTime(sample,frame); return clock; }

    //
    // Getters of External ElecClock
    //
    /// Borrow a const Trigger clock with time set to External Time [us]
    virtual const detinfo::ElecClock& ExternalClock() const
    { return fExternalClock; }

    /// Create a External clock for a given time [us] from clock counting start
    /// @bug The frequency is taken from `TriggerClock()`, not `ExternalClock()`
    virtual detinfo::ElecClock ExternalClock(double time) const
      { return detinfo::ElecClock(time,fExternalClock.FramePeriod(),fTriggerClock.Frequency());}
    
    /// Create a External clock for a given sample/frame number in External clock frequency
    virtual detinfo::ElecClock ExternalClock(unsigned int sample,
				  unsigned int frame) const
      { detinfo::ElecClock clock = ExternalClock(); clock.SetTime(sample,frame); return clock; }
    
    //
    // Getters for time [us] w.r.t. trigger given information from waveform
    //

    /// Given TPC time-tick (waveform index), returns time [us] w.r.t. trigger time stamp
    virtual double TPCTick2TrigTime(double tick) const
    { return fTPCClock.TickPeriod() * tick + TriggerOffsetTPC(); }
    /// Given TPC time-tick (waveform index), returns time [us] w.r.t. beam gate time
    /// @todo Use `TPCTick2TrigTime()` in the implementation
    virtual double TPCTick2BeamTime(double tick) const
    { return fTPCClock.TickPeriod() * tick + TriggerOffsetTPC() + TriggerTime() - BeamGateTime(); }
    /// Given Optical time-tick (waveform index), sample and frame number, returns time [us] w.r.t. trigger time stamp
    virtual double OpticalTick2TrigTime(double tick, size_t sample, size_t frame) const
    { return fOpticalClock.TickPeriod() * tick + fOpticalClock.Time(sample,frame) - TriggerTime();  }
    /// Given Optical time-tick (waveform index), sample and frame number, returns time [us] w.r.t. beam gate time stamp
    virtual double OpticalTick2BeamTime(double tick, size_t sample, size_t frame) const
    { return fOpticalClock.TickPeriod() * tick + fOpticalClock.Time(sample,frame) - BeamGateTime(); }
    /// Given External time-tick (waveform index), sample and frame number, returns time [us] w.r.t. trigger time stamp
    virtual double ExternalTick2TrigTime(double tick, size_t sample, size_t frame) const
    { return fExternalClock.TickPeriod() * tick + fExternalClock.Time(sample,frame) - TriggerTime(); }
    /// Given External time-tick (waveform index), sample and frame number, returns time [us] w.r.t. beam gate time stamp
    virtual double ExternalTick2BeamTime(double tick, size_t sample, size_t frame) const
    { return fExternalClock.TickPeriod() * tick + fExternalClock.Time(sample,frame) - BeamGateTime(); }
    
    //
    // Getters for time [tdc] (electronics clock counting ... in double precision) 
    //

    /// Given TPC time-tick (waveform index), returns electronics clock count [tdc]
    virtual double TPCTick2TDC(double tick) const
    { return ( (TriggerTime() + TriggerOffsetTPC()) / fTPCClock.TickPeriod() + tick ); }
    /// Given G4 time [ns], returns corresponding TPC electronics clock count [tdc]
    virtual double TPCG4Time2TDC(double g4time) const
    { return G4ToElecTime(g4time) / fTPCClock.TickPeriod(); }
    /// Given Optical time-tick (waveform index), sample and frame number, returns time electronics clock count [tdc]
    virtual double OpticalTick2TDC(double tick, size_t sample, size_t frame) const
    { return fOpticalClock.Ticks(sample,frame) + tick; }
    /// Given G4 time [ns], returns corresponding Optical electronics clock count [tdc]  
    virtual double OpticalG4Time2TDC(double g4time) const
    { return G4ToElecTime(g4time) / fOpticalClock.TickPeriod(); }
    /// Given External time-tick (waveform index), sample and frame number, returns time electronics clock count [tdc]
    virtual double ExternalTick2TDC(double tick, size_t sample, size_t frame) const
    { return fExternalClock.Ticks(sample,frame) + tick; }
    /// Given G4 time [ns], returns corresponding External electronics clock count [tdc]
    virtual double ExternalG4Time2TDC(double g4time) const
    { return G4ToElecTime(g4time) / fExternalClock.TickPeriod(); }

    //
    // Getters for time [us] (electronics clock counting ... in double precision)
    //
    /// Given TPC time-tick (waveform index), returns electronics clock [us]
    virtual double TPCTick2Time(double tick) const
    { return TriggerTime() + TriggerOffsetTPC() + tick * fTPCClock.TickPeriod(); }
    /// Given Optical time-tick (waveform index), sample and frame number, returns electronics clock [us]
    virtual double OpticalTick2Time(double tick, size_t sample, size_t frame) const
    { return fOpticalClock.Time(sample,frame) + tick * fOpticalClock.TickPeriod(); }
    /// Given External time-tick (waveform index), sample and frame number, returns electronics clock [us]
    virtual double ExternalTick2Time(double tick, size_t sample, size_t frame) const
    { return fExternalClock.Time(sample,frame) + tick * fExternalClock.TickPeriod(); }

    //
    // Getters for time [ticks] (waveform index number)
    //

    /// Given electronics clock count [tdc] returns TPC time-tick
    virtual double TPCTDC2Tick(double tdc) const
    { return ( tdc - (TriggerTime() + TriggerOffsetTPC()) / fTPCClock.TickPeriod() ); }
    /// Given G4 time returns electronics clock count [tdc]
    virtual double TPCG4Time2Tick(double g4time) const
    { return (G4ToElecTime(g4time) - (TriggerTime() + TriggerOffsetTPC())) / fTPCClock.TickPeriod(); }

    bool InheritClockConfig() { return fInheritClockConfig; }
    
    /// Internal function to apply loaded parameters to member attributes
    void ApplyParams();

    /// Internal function used to search for the right configuration set in the data file
    bool IsRightConfig(const fhicl::ParameterSet& ps) const;
    
  protected:

    std::vector<std::string> fConfigName;

    std::vector<double>      fConfigValue;

    bool fInheritClockConfig;

    std::string fTrigModuleName;

        /// Electronics clock counting start time in G4 time frame [us]
    double fG4RefTime;

    /// Frame period
    double fFramePeriod;

    /// TPC clock
    ::detinfo::ElecClock fTPCClock;

    /// Optical clock
    ::detinfo::ElecClock fOpticalClock;

    /// Trigger clock
    ::detinfo::ElecClock fTriggerClock;

    /// External clock
    ::detinfo::ElecClock fExternalClock;

    /// Time offset from trigger to TPC readout start
    double fTriggerOffsetTPC;

    /// Trigger time in [us]
    double fTriggerTime;

    /// BeamGate time in [us]
    double fBeamGateTime;

  }; // class DetectorClocksStandard

} //namespace detinfo


#endif 
