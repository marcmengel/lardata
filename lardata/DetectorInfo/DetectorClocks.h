/**
 * \file lardata/DetectorInfo/DetectorClocks.h
 * 
 * \brief pure virtual base interface for detector clocks
 * 
 * \author jpaley@fnal.gov
 * 
 */
#ifndef LARDATA_DETECTORINFO_DETECTORCLOCKS_H
#define LARDATA_DETECTORINFO_DETECTORCLOCKS_H

#include "lardata/DetectorInfo/ElecClock.h"

namespace detinfo{

  enum InheritConfigType_t {
    kG4RefTime=0,
    kTriggerOffsetTPC,
    kFramePeriod,
    kClockSpeedTPC,
    kClockSpeedOptical,
    kClockSpeedTrigger,
    kClockSpeedExternal,
    kDefaultTrigTime,
    kDefaultBeamTime,
    kInheritConfigTypeMax
  };

  /** **************************************************************************
   * @brief Conversion of times between different formats and references.
   * 
   * Valuable and very complete documentation on LArSoft timing is described for
   * MicroBooNE by Herbert Greenlee on a document of October 2017 (DocDB 12290).
   * The content in there includes MicroBooNE-specific and general information,
   * and the former is usually clearly distinguished from the latter.
   * 
   * The following information is more terse and aimed to the documentation of
   * this service provider rather than to the more general topic of timing in
   * LArSoft.
   * 
   * 
   * Time definitions
   * =================
   * 
   * @anchor DetectorClocksTimeDefinitions
   * Many different components contribute to the operation of a LArTPC, and more
   * are needed for the simulation.
   * Some of the relevant events and time points associated with time are:
   * * *hardware trigger*:
   *     @anchor DetectorClocksHardwareTrigger
   *     the instant the trigger happens or is expected; data acquisition reacts
   *     at this time (_multi-event_)
   * * *beam gate opening*:
   *     @anchor DetectorClocksBeamGateOpening
   *     the instant the beam particles are expected to enter the detector
   *     (_multi-event_)
   * * *software trigger*:
   *     @anchor DetectorClocksSoftwareTrigger
   *     additional logic can provide a more detailed estimation of the physics
   *     event time; this usually comes too late for the electronics time to be
   *     defined by it (_multi-event_)
   * * *electronics start time*:
   *     @anchor DetectorClocksElectronicsStartTime
   *     a reference time instant; this is usually matching the 
   *     @ref DetectorClocksTPCelectronicsStartTime "instant the TPC readout starts recording raw digits"
   *     (_multi-event_)
   * * *TPC electronics start time*:
   *     @anchor DetectorClocksTPCelectronicsStartTime
   *     the instant corresponding to the first sample of the raw digits from
   *     TPC; for untampered `raw::RawDigit` objects, this time matches the
   *     general @ref DetectorClocksElectronicsStartTime "electronics start time"
   *     definition, and for the model of data acquisition described here, this
   *     time is by definition function of the
   *     @ref DetectorClocksHardwareTrigger "hardware trigger". Trimmed
   *     `raw::RawDigit` with a start portion clipped away will break the
   *     assumption of electronics time matching the TPC one (which is defined
   *     as seen by `raw::RawDigit`) (_multi-event_)
   * * other electronics start times (*optical detector*, *cosmic ray tagger*,
   *     etc.)
   *     @anchor DetectorClocksOpticalElectronicsStartTime
   *     @anchor DetectorClocksExternalElectronicsStartTime
   *     represent the instant the respective data acquisition subsystems start
   *     digitizing the data from their detectors (_multi-event_)
   * * *event trigger*:
   *     @anchor DetectorClocksExternalEventTrigger
   *     the instant the physics event is detected (in reconstruction jargon
   *     this is a @f$ t_{0} @f$); ideally, for most events involving beam
   *     particles this is the a little after
   *     @ref DetectorClocksBeamGateOpening "beam gate opening", and on top of
   *     the @ref DetectorClocksHardwareTrigger "hardware trigger" (_per event_)
   * * *charge collection*:
   *     @anchor DetectorClocksChargeCollection
   *     the instant the ionisation from a particle reaches the charge readout
   *     (_per particle_)
   * * *photon collection*:
   *     @anchor DetectorClocksPhotonCollection
   *     the instant scintillation photons from a particle reaches the optical
   *     readout (_per particle_)
   * * *physics event time*:
   *     @anchor DetectorClocksPhysicsEvent
   *     the instant the main interaction happens; this is assigned by the event
   *     generator (_per event_)
   * * *Geant4 time start*: 
   *     @anchor DetectorClocksGeant4TimeStart
   *     assigned to each particle by the simulation of propagation
   *     through matter (Geant4); note that this is not necessarily the same as
   *     the @ref DetectorClocksPhysicsEvent "event time". (_per event_)
   * 
   * In parentheses, it is indicated if such instant is defined for each
   * considered particle, or for the complete physics process ("event") or for
   * all physics processes recorded in the same time window ("multi-event").
   * 
   * @anchor DetectorClocksTimeFrameDefinitions
   * We can also define many ways to measure these times with respect to the
   * others. We use the following time scales and frames:
   * * *electronics time*:
   *     @anchor DetectorClocksElectronicsTime
   *     from the
   *     @ref DetectorClocksElectronicsStartTime "electronics start time"; this
   *     is the "glue" time frame connecting most of the others _[&micro;s]_
   * * *TPC time*:
   *     @anchor DetectorClocksTPCelectronicsTime
   *     from the
   *     @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time"
   *     _[&micro;s]_
   * * other electronics times (
   *     @anchor DetectorClocksOpticalElectronicsTime
   *     *optical electronics time*,
   *     @anchor DetectorClocksExternalElectronicsTime
   *     *external electronics time*, etc.):
   *     from the respective start times; note that in LArSoft the definition of
   *     these times is not fully defined (see
   *     @ref DetectorClocksElectronicsTimesNote "below").
   *     _[&micro;s]_
   * * *beam gate time*:
   *     @anchor DetectorClocksBeamGateTime
   *     from the @ref DetectorClocksBeamGateOpening "beam gate opening time"
   *     _[&micro;s]_
   * * *trigger time*:
   *     @anchor DetectorClocksTriggerTime
   *     from the @ref DetectorClocksHardwareTrigger "hardware trigger"
   *     _[&micro;s]_
   * * *event time*:
   *     @anchor DetectorClocksEventTime
   *     from an @ref DetectorClocksPhysicsEvent "event trigger" _[&micro;s]_
   * * *generation time*:
   *     @anchor DetectorClocksGenerationTime
   *     from the time the generators assigns to the
   *     @ref DetectorClocksPhysicsEvent "main physics event"; this is
   *     stored in the trajectory points of `simb::MCParticle` (usually the
   *     generators store a single trajectory point for each particle) _[ns]_
   * * *Geant4 time*:
   *     @anchor DetectorClocksGeant4Time
   *     from the @ref DetectorClocksGeant4TimeStart "Geant4 time start"; this
   *     is stored into each trajectory point of `simb::MCParticle` produced by
   *     `LArG4` module or equivalent _[ns]_
   * * *simulation time*:
   *     @anchor DetectorClocksSimulationTime
   *     the same as @ref DetectorClocksGeant4Time "Geant4 time" (and also
   *     generation time, which in LArSoft happens to match the former) _[ns]_
   * 
   * The list above reports in square brackets the "standard" unit, used for
   * that times in that scale by all LArSoft code unless explicitly specified
   * otherwise.
   * For the times bound to readout, there is also an equivalent tick count time
   * (from the TDC), which is an integral value representing the number of
   * elapsed sampling periods of the detector they refer too (and that period
   * may be different for each piece of hardware). Be sure to read the
   * documentation about the methods you want to use to convert ticks, because
   * their implications and assumptions may be quite subtle.
   * 
   * 
   * Time conversions
   * =================
   * 
   * While this system is carefully designed to give physicists an headache, the
   * `detinfo::DetectorClocks` service provider attempts to tame this complexity
   * by providing methods to convert from one time to the other.
   * 
   * The following table represents the available conversion functions, with the
   * time in the first column as the time to be converted and in the columns the
   * times to convert to:
   * 
   * |                                                     to &rarr; | electronics time      | (_ticks_)            | TPC time ticks     | trigger time              | trigger clock ticks | beam gate time            | Optical clock ticks   | External clock ticks   |
   * | ------------------------------------------------------------: | --------------------- | -------------------- | ------------------ | ------------------------- | ------------------- | ------------------------- | --------------------- | ---------------------- |
   * | (unit)                                                        | &micro;s              |                      |                    | &micro;s                  | `TriggerClock()`    | &micro;s                  |                       |                        |
   * | @ref DetectorClocksHardwareTrigger         "hardware trigger" | `TriggerTime()`       | `TPCClock()`         |                    |                           |                     |                           | `OpticalClock()`      | `ExternalClock()`      |
   * | @ref DetectorClocksBeamGateOpening         "beam gate point"  | `BeamGateTime()`      |                      |                    |                           |                     |                           |                       |                        |
   * | @ref DetectorClocksElectronicsTime         "electronics time" |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   |                       |                      | `TPCTDC2Tick()`    |                           |                     |                           |                       |                        |
   * | @ref DetectorClocksTPCelectronicsTime      "TPC time"         |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   | `TPCTick2Time()`      | `TPCTick2TDC()`      |                    | `TPCTick2TrigTime()`      |                     | `TPCTick2BeamTime()`      |                       |                        |
   * | @ref DetectorClocksTriggerTime             "trigger time"     |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   |                       |                      |                    |                           |                     |                           |                       |                        |
   * | @ref DetectorClocksOpticalElectronicsTime  "Optical"          |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   | `OpticalTick2Time()`  | `OpticalTick2TDC()`  |                    | `OpticalTick2TrigTime()`  |                     | `OpticalTick2BeamTime()`  |                       |                        |
   * | @ref DetectorClocksExternalElectronicsTime "External"         |                       |                      |                    |                           |                     |                           |                       |                        |
   * |                                            &nbsp; (_ticks_)   | `ExternalTick2Time()` | `ExternalTick2TDC()` |                    | `ExternalTick2TrigTime()` |                     | `ExternalTick2BeamTime()` |                       |                        |
   * | @ref DetectorClocksSimulationTime          "simulation time"  | `G4ToElecTime()`      | `TPCG4Time2TDC()`    | `TPCG4Time2Tick()` |                           |                     |                           | `OpticalG4Time2TDC()` | `ExternalG4Time2TDC()` |
   * 
   * Note that the complete definition of optical and external time requires
   * additional information: see the
   * @ref DetectorClocksElectronicsTimesNote "note on electonics time frames"
   * below.
   * 
   * The names are not thoroughly consistent, but they roughly follow the
   * following rules:
   * * the "trigger time" is named as `TrigTime` (but see below)
   * * the "beam time" is named as `BeamTime` (but see below)
   * * the @ref DetectorClocksElectronicsTime "electronics time" is named simply
   *     as `Time`; note that e.g. `TriggerTime()` and `BeamGateTime()` must be
   *     read as `Trigger` and `BeamGate` plus `Time()`, that is they return a
   *     `Time` value (i.e., electronics time) of the
   *     @ref DetectorClocksHardwareTrigger "hardware trigger instant" and
   *     @ref DetectorClocksBeamGateOpening "beam gate opening" instant, in the
   *     electronics time frame
   * * the @ref DetectorClocksElectronicsTime "electronics time ticks" are
   *     labelled `TDC`
   * * the @ref DetectorClocksSimulationTime "simulation time" is labelled `G4`
   *     or `G4Time`
   * * `TPCTick` is the TPC electronics clock ticks", measured from the
   *     @ref DetectorClocksTPCelectronicsTime "TPC electronics start time"
   * * `TPC` is usually used in conjunction with `Tick`, where it means
   *   `TPCTick`; but in `TPCG4Time2TDC()` it is a misnomer (the `TDC` wins in
   *   that the result is in ticks in
   *   @ref DetectorClocksElectronicsTime "electronics time" reference, not in
   *   @ref DetectorClocksTPCelectronicsTime "TPC electronics time")
   * 
   * 
   * @anchor DetectorClocksElectronicsTimesNote
   * Note on electronics time frames
   * --------------------------------
   * 
   * The conversion of some electronic time ticks, especially the
   * @ref DetectorClocksOpticalElectronicsTime "optical" and
   * @ref DetectorClocksExternalElectronicsTime "external" ones, assumes that
   * the tick values to be converted are relative to an arbitrary reference,
   * meaning that the absolute (or, e.g., electronics) time for tick `0` is not
   * centrally defined. Instead, it is expected that additional information is
   * provided to locate when this tick `0` actually happens. This is usually
   * done via another pair of arguments, the frame number and the sample number
   * within that frame. In the end, this just moves the questions to when frame
   * `0` sample `0` happens: as those are input arguments, that absolute
   * reference is left to the caller to define; but usually it is referring to
   * the @ref DetectorClocksElectronicsStartTime "electronics start time".
   * 
   * 
   * @anchor DetectorClocksIntroClocks
   * Clocks
   * -------
   * 
   * A clock object (`detinfo::ElecClock`) contains settings for a specific
   * hardware clock. `DetectorClocks` provides four clock objects:
   * 
   * clock name        | purpose                               | default time
   * ----------------- | ------------------------------------- | ---------------
   * `TPCClock()`      | TPC readout and "general" electronics | `TriggerTime()`
   * `OpticalClock()`  | optical detector electronics          | `TriggerTime()`
   * `TriggerClock()`  | hardware trigger electronics          | `TriggerTime()`
   * `ExternalClock()` | _not specified_                       | `TriggerTime()`
   * 
   * A clock object _does not change the time frame_: in the conversions between
   * times and ticks, and the other way around, it always assumes that at tick 0
   * (and frame 0, sample 0, if needed), the time is also 0 (see the
   * @ref DetectorClocksElectronicsTimesNote "note on electronics time frames"
   * above). Therefore, once again, **a clock object does not help in converting
   * between different time scales**, and the output times are in the same time
   * frame as the input.
   * 
   * The "default time" is the time the clock is set when returned by the
   * methods with no argument (e.g. `TriggerClock()`).
   * 
   * All times (including frame lengths) are measured in microseconds, and all
   * the frequencies are measured in megahertz.
   * 
   * `TPCClock()` deals with TPC readout, and the "electronics time" clock is
   * defined to have the same parameters as the TPC readout one.
   * 
   * @note The `TriggerClock()` clock has little in common with `TriggerTime()`:
   *       the former is an electronics setting for a piece of hardware
   *       generating the trigger, while the latter is the instant the
   *       @ref DetectorClocksHardwareTrigger "hardware trigger" actually
   *       happened.
   * 
   * 
   * Note on configuration (and for implementers)
   * =============================================
   * 
   * `detinfo::DetectorClocks` is an abstract interface enclosing the set of
   * supported queries. A concrete implementation of this interface needs to be
   * provided in order to use this facility. LArSoft provides
   * `detinfo::DetectorClocksStandard` as a simple, reasonable implementation.
   * Initialization and configuration are designed by each implementation.
   * 
   * 
   * @todo Add a method to convert electronics time tick into electronics time.
   * @todo Add a method returning the @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time" in @ref DetectorClocksElectronicsTime "electronics time" (that is `TriggerTime() + TriggerOffsetTPC()`).
   * 
   */
  class DetectorClocks {

  public:
    DetectorClocks(const DetectorClocks &) = delete;
    DetectorClocks(DetectorClocks &&) = delete;
    DetectorClocks& operator = (const DetectorClocks &) = delete;
    DetectorClocks& operator = (DetectorClocks &&) = delete;
    virtual ~DetectorClocks() = default;
    
    // --- BEGIN Configuration information -------------------------------------
    /// @{
    /// @name Configuration information
    
    /// Returns a list of provider configuration names.
    /// @see ConfigValues()
    virtual std::vector<std::string> ConfigNames() const = 0;
    
    /// Returns a list of provider configuration values
    /// (same order as `ConfigNames()`).
    /// @see ConfigNames()
    virtual std::vector<double> ConfigValues() const = 0;

    /// @}
    // --- END Configuration information ---------------------------------------
    
    
    
    // --- BEGIN Time points and intervals -------------------------------------
    /// @{
    /// @name Time points and intervals
    
    /**
     * @brief Time offset from @ref DetectorClocksHardwareTrigger "hardware trigger" to @ref DetectorClocksTPCelectronicsStartTime "TPC electronics start time".
     * @return time from hardware trigger to TPC electronics [&micro;s]
     * 
     * This is the
     * @ref DetectorClocksTPCelectronicsStartTime "time the TPC readout starts",
     * with respect to the
     * @ref DetectorClocksHardwareTrigger "harware trigger time". This is also
     * the time where the TPC clock starts, again respect to the trigger time.
     * 
     * When negative, it means the TDC electronics clock started before the
     * trigger. And it'd better be negative... or your readout missed the
     * trigger!
     */
    virtual double TriggerOffsetTPC() const = 0;
    
    /// @ref DetectorClocksHardwareTrigger "Harware trigger time" (in @ref DetectorClocksElectronicsTime "electronics time frame") [&micro;s].
    virtual double TriggerTime() const = 0;
    
    /// @ref DetectorClocksBeamGateOpening "Beam gate opening time" (in @ref DetectorClocksElectronicsTime "electronics time frame") [&micro;s].
    virtual double BeamGateTime() const = 0;
    
    
    
    /// @}
    // --- END Time points and intervals ---------------------------------------
    
    
    
    // --- BEGIN Time conversions ----------------------------------------------
    /// @{
    /// @name Time conversions
    
    /// Given a @ref DetectorClocksSimulationTime "simulation time" [ns], converts it into @ref DetectorClocksElectronicsTime "electronics time" [&micro;s].
    virtual double G4ToElecTime(double g4_time) const = 0;
    
    //
    // Getters for time [us] w.r.t. trigger given information from waveform
    //

    /// Converts a @ref DetectorClocksTPCelectronicsTime "TPC time" (in ticks) into a @ref DetectorClocksTriggerTime "trigger time" [&micro;s].
    virtual double TPCTick2TrigTime(double tick) const = 0;
    /// Converts a @ref DetectorClocksTPCelectronicsTime "TPC time" (in ticks) into a @ref DetectorClocksBeamGateTime "beam gate time" [&micro;s].
    virtual double TPCTick2BeamTime(double tick) const = 0;
    
    
    /// @}
    // --- END Time conversions ------------------------------------------------
    
    
    
    // --- BEGIN Electronics clocks --------------------------------------------
    /// @{
    /**
     * @name Electronics clocks
     *
     * The clocks are also the main route to discover the sampling frequency
     * and the period of the electronics. For example:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * double const opticalTickDuration = OpticalClock().TickPeriod();
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * will retrieve the "official" optical detector clock period.
     * 
     * All times from the clocks (including frame lengths) are measured in
     * microseconds, and all the frequencies are measured in megahertz.
     * 
     * Clock objects (`detinfo::ElecClock`) are not bound to any reference time.
     * When specifying a time in one of their methods, whatever the reference of
     * that time is, that is also the reference of the values returned by those
     * methods.
     * The same is true for the time set into the clock at construction: when
     * using one of the methods where _no time_ is required, the time set by
     * `ElecClock::SetTime()` or at construction is used, and whatever the
     * reference of that time is, that is also the reference of the values
     * returned by those methods.
     * 
     * Some overview information on clocks is given in the
     * @ref DetectorClocksIntroClocks "Clocks" section of this documentation.
     */
    
    //
    // Getters of TPC ElecClock
    //
    /**
     * @brief Lends a constant TPC clock with time set to @ref DetectorClocksHardwareTrigger "trigger time".
     * @return a constant reference to the TPC clock object
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the TPC channel readout
     * electronics (which ultimately fills `raw::RawDigit` objects).
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * 
     * The current time of this object can't be changed, and it is set to the
     * @ref DetectorClocksHardwareTrigger "trigger time", in the same time scale
     * as `TriggerTime()`, that is
     * @ref DetectorClocksElectronicsTime "electronics time".
     */
    virtual const ::detinfo::ElecClock& TPCClock() const = 0;
    
    /**
     * @brief Returns a TPC clock with time set to the specified time.
     * @param time the current time of the new clock [&micro;s]
     * @return a clock class with TPC readout electronics timing settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the TPC channel readout
     * electronics (which ultimately fills `raw::RawDigit` objects).
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * The current time of this object is set to the specified `time`, and it
     * can be set to any value afterwards.
     */
    virtual ::detinfo::ElecClock TPCClock(double time) const = 0; 
    
    /**
     * @brief Returns a TPC clock with time set to the specified time sample.
     * @param sample number of sample of the clock time
     * @param frame number of the frame the selected sample belongs to
     * @return a clock class with TPC readout electronics settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock represents the TPC channel readout (which ultimately fills
     * `raw::RawDigit` objects).
     * It can be used to convert into time the number of ticks or a sample in a
     * frame, and vice versa.
     * The current time of this object is set to the time specified as frame and
     * sample number, and it can be set to any value afterwards.
     * 
     */
    virtual detinfo::ElecClock TPCClock(unsigned int sample,unsigned int frame) const = 0;
    
    
    //
    // Getters of Optical ElecClock
    //
    /**
     * @brief Lends a constant optical clock with time set to @ref DetectorClocksHardwareTrigger "trigger time".
     * @return a constant reference to the optical clock class
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the optical readout
     * electronics.
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * 
     * The current time of this object can't be changed, and it is set to the
     * @ref DetectorClocksHardwareTrigger "trigger time", in the same time scale
     * as `TriggerTime()`, that is
     * @ref DetectorClocksElectronicsTime "electronics time".
     */
    virtual const detinfo::ElecClock& OpticalClock() const = 0;
    
    /**
     * @brief Returns a optical clock with time set to the specified time.
     * @param time the current time of the new clock [&micro;s]
     * @return a clock class with optical readout electronics timing settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the optical readout
     * electronics.
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * The current time of this object is set to the specified `time`, and it
     * can be set to any value afterwards.
     */
    virtual detinfo::ElecClock OpticalClock(double time) const = 0;
    
    /**
     * @brief Returns a optical clock with time set to the specified time
     *        sample.
     * @param sample number of sample of the clock time
     * @param frame number of the frame the selected sample belongs to
     * @return a clock class with optical readout electronics settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock represents the optical readout.
     * It can be used to convert into time the number of ticks or a sample in a
     * frame, and vice versa.
     * The current time of this object is set to the time specified as frame and
     * sample number, and it can be set to any value afterwards.
     */
    virtual detinfo::ElecClock OpticalClock(unsigned int sample, unsigned int frame) const = 0;
    
    
    //
    // Getters of Trigger ElecClock
    //
    /**
     * @brief Lends a constant trigger clock with time set to @ref DetectorClocksHardwareTrigger "trigger time".
     * @return a constant reference to the trigger clock class
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the trigger electronics.
     * Note that, despite the name, the clock tick `0` is not the trigger time
     * itself, but the electronics start time (trigger time is measured in that
     * frame: see `TriggerTime()`).
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * 
     * The current time of this object can't be changed, and it is set to the
     * @ref DetectorClocksHardwareTrigger "trigger time", in the same time scale
     * as `TriggerTime()`, that is
     * @ref DetectorClocksElectronicsTime "electronics time".
     */
    virtual const detinfo::ElecClock& TriggerClock() const = 0;
    
    /**
     * @brief Returns a trigger clock with time set to the specified time.
     * @param time the current time of the new clock [&micro;s]
     * @return a clock class with trigger electronics timing settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the trigger electronics.
     * Note that, despite the name, the clock tick `0` is not the trigger time
     * itself, but the electronics start time (trigger time is measured in that
     * frame: see `TriggerTime()`).
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * The current time of this object is set to the specified `time`, and it
     * can be set to any value afterwards.
     */
    virtual detinfo::ElecClock TriggerClock(double time) const = 0;
    
    /**
     * @brief Returns a trigger clock with time set to the specified time
     *        sample.
     * @param sample number of sample of the clock time
     * @param frame number of the frame the selected sample belongs to
     * @return a clock class with trigger electronics settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock represents the trigger readout.
     * It can be used to convert into time the number of ticks or a sample in a
     * frame, and vice versa.
     * Note that, despite the name, the clock tick `0` is not the trigger time
     * itself, but the electronics start time (trigger time is measured in that
     * frame: see `TriggerTime()`).
     * 
     * The current time of this object is set to the time specified as frame and
     * sample number, and it can be set to any value afterwards.
     */
    virtual detinfo::ElecClock TriggerClock(unsigned int sample, unsigned int frame) const = 0;
    
    
    //
    // Getters of External ElecClock
    //
    /**
     * @brief Lends a constant external clock with time set to @ref DetectorClocksHardwareTrigger "trigger time".
     * @return a constant reference to the external clock class
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the external electronics.
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * 
     * The current time of this object can't be changed, and it is set to the
     * @ref DetectorClocksHardwareTrigger "trigger time", in the same time scale
     * as `TriggerTime()`, that is
     * @ref DetectorClocksElectronicsTime "electronics time".
     */
    virtual const detinfo::ElecClock& ExternalClock() const = 0;

    /**
     * @brief Returns a external clock with time set to the specified time.
     * @param time the current time of the new clock [&micro;s]
     * @return a clock class with external electronics timing settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock has the same timing settings as the external electronics.
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * The current time of this object is set to the specified `time`, and it
     * can be set to any value afterwards.
     */
    virtual detinfo::ElecClock ExternalClock(double time) const = 0;
    
    /**
     * @brief Returns a external clock with time set to the specified time
     *        sample.
     * @param sample number of sample of the clock time
     * @param frame number of the frame the selected sample belongs to
     * @return a clock class with external electronics settings
     * @see @ref DetectorClocksIntroClocks "clock notes"
     * 
     * This clock represents the "external" readout.
     * It can be used to convert into time the number of ticks or a sample in a
     * frame, and vice versa.
     * 
     * The current time of this object is set to the time specified as frame and
     * sample number, and it can be set to any value afterwards.
     */
    virtual detinfo::ElecClock ExternalClock(unsigned int sample, unsigned int frame) const = 0;
    
    /// @}
    // --- END Electronics clocks ----------------------------------------------
    
    
    
    // --- BEGIN Conversions from electronics waveform ticks -------------------
    /// @{
    /**
     * @name Conversions from electronics waveform ticks
     * 
     * This section includes also conversions from electronics waveform ticks
     * _to_ other waveform ticks.
     */
    
    /// Given electronics clock count [tdc] returns TPC time-tick
    /**
     * @brief Converts an @ref DetectorClocksTPCelectronicsTime "electronics time tick" into a @ref DetectorClocksTPCelectronicsTime "TPC time tick".
     * @param tdc an @ref DetectorClocksTPCelectronicsTime "electronics time tick"
     * @return the @ref DetectorClocksTPCelectronicsTime "TPC time tick" corresponding to `tdc`
     * @see `TPCClock()`, `TPCTime()`
     */
    virtual double TPCTDC2Tick(double tdc) const = 0;

    /**
     * @brief Converts a @ref DetectorClocksTPCelectronicsTime "TPC time tick" into a @ref DetectorClocksElectronicsTime "electronics time tick".
     * @param tick the tick number within the TPC waveform
     * @return the specified tick in @ref DetectorClocksElectronicsTime "electronics time ticks"
     * @see TPCTick2Time(), TPCClock()
     * 
     * The idea is that we have a TPC waveform. Given a tick number within that
     * waveform, this method returns the
     * @ref DetectorClocksElectronicsTime "electronics time" corresponding to
     * the start of that tick, but in TPC clock ticks (`TPCClock()`).
     * 
     * More explicitly: `tick` is the number of tick in the waveform, measured
     * from the start of the waveform, which is `tick = 0`; the start point of
     * this waveform is the @ref DetectorClocksTPCelectronicsStartTime
     * "TPC electronics start time".
     */
    virtual double TPCTick2TDC(double tick) const = 0;
    
    /**
     * @brief Converts a @ref DetectorClocksTPCelectronicsTime "TPC electronics tick" into @ref DetectorClocks "electronics time" [&micro;s]
     * @param tick tick from the @ref DetectorClocksTPCelectronicsStartTime "beginning of the TPC waveform"
     * @return the @ref DetectorClocks "electronics time" for that tick [&micro;s]
     * @see `TPCTime()`, `TPCClock()`
     * 
     * Note that the start of the TPC waveform is defined by `TPCTime()`.
     */
    virtual double TPCTick2Time(double tick) const = 0;
    
    
    /**
     * @brief Converts an @ref DetectorClocksOpticalElectronicsTime "optical time tick" into a @ref DetectorClocksTriggerTime "trigger time" [&micro;s]
     * @param tick the tick number within the optical waveform
     * @param sample start sample of the optical waveform
     * @param frame start frame of the optical waveform
     * @return @ref DetectorClocksTriggerTime "trigger time" of the specified tick [&micro;s]
     * @see OpticalTick2BeamTime(), OpticalTick2TDC(), ExternalTick2TrigTime(),
     *      OpticalClock()
     * 
     * The idea is that we have a optical waveform which starts at a given frame
     * number, and at sample number within that frame. Given a tick number
     * within that waveform, this method returns the
     * @ref DetectorClocksTriggerTime "trigger time" corresponding at the start
     * of that tick.
     * 
     * More explicitly: `tick` is the number of tick in the waveform, measured
     * from the start of the waveform, which is `tick = 0`; this start point of
     * the waveform is specified by a `frame` and a `sample`, where the sample
     * `0` of frame `0` is the
     * @ref DetectorClocksElectronicsStartTime "electronics start time".
     * Both `sample` and `ticks` are defined using the period of the optical
     * clock (`OpticalClock()`), and the frame is defined from the same clock as
     * well.
     */
    virtual double OpticalTick2TrigTime(double tick, size_t sample, size_t frame) const = 0;

    /**
     * @brief Converts an @ref DetectorClocksOpticalElectronicsTime "optical time tick" into a @ref DetectorClocksBeamGateTime "beam gate time" [&micro;s]
     * @param tick the tick number within the optical waveform
     * @param sample start sample of the optical waveform
     * @param frame start frame of the optical waveform
     * @return @ref DetectorClocksBeamGateTime "beam gate time" of the specified tick [&micro;s]
     * @see OpticalTick2TrigTime(), OpticalTick2TDC(), ExternalTick2BeamTime(),
     *      OpticalClock()
     * 
     * This method works like `OpticalTick2TrigTime()`, but it returns the
     * result as @ref DetectorClocksBeamGateTime "beam gate time" (instead of
     * @ref DetectorClocksTriggerTime "trigger time").
     */
    virtual double OpticalTick2BeamTime(double tick, size_t sample, size_t frame) const = 0;
    
    /**
     * @brief Converts an @ref DetectorClocksOpticalElectronicsTime "optical time tick" into ticks from the first optical frame.
     * @param tick the tick number within the optical waveform
     * @param sample start sample of the optical waveform
     * @param frame start frame of the optical waveform
     * @return optical clock ticks elapsed from the starting frame
     * @see OpticalTick2TrigTime(), ExternalTick2TDC(), OpticalClock()
     * 
     * The definition of method arguments is the same as in
     * `OpticalTick2TrigTime()`. This method returns the total number of optical
     * clock ticks elapsed since the first tick, marked as sample `0` of frame
     * `0`. Frame and tick periods are defined by the `OpticalClock()`.
     * 
     * Note that while this method does not assume any absolute time point for
     * the frame `0` (see the
     * @ref DetectorClocksElectronicsTimesNote "note on electronics time frames"),
     * that time point should nevertheless be assumed to be the
     * @ref DetectorClocksElectronicsStartTime "electronics start time" for
     * consistency with the arguments from other methods like
     * `OpticalTick2TrigTime()`.
     */
    virtual double OpticalTick2TDC(double tick, size_t sample, size_t frame) const = 0;
    
    /**
     * @brief Converts an @ref DetectorClocksOpticalElectronicsTime "optical time tick" into time from the first optical frame [&micro;s]
     * @param tick the tick number within the optical waveform
     * @param sample start sample of the optical waveform
     * @param frame start frame of the optical waveform
     * @return time elapsed from the starting frame (see below) [&micro;s]
     * @see OpticalTick2TDC(), OpticalTick2TrigTime(), ExternalTick2Time(), OpticalClock()
     * 
     * This method is similar to `OpticalTick2TDC()`, but the result is in an
     * absolute time interval rather than a tick count.
     * 
     * The note in `OpticalTick2TDC()` is also valid: while this method does not
     * assume any absolute time point for the frame `0` (see the
     * @ref DetectorClocksElectronicsTimesNote "note on electronics time frames"),
     * that time point should nevertheless be assumed to be the
     * @ref DetectorClocksElectronicsStartTime "electronics start time" for
     * consistency with the arguments from other methods like
     * `OpticalTick2TrigTime()`. **With that assumption, the return value is
     * in @ref DetectorClocksElectronicsTime "electronic time frame".**
     */
    virtual double OpticalTick2Time(double tick, size_t sample, size_t frame) const = 0;
    
    
    /**
     * @brief Converts an @ref DetectorClocksExternalElectronicsTime "external time tick" into a @ref DetectorClocksTriggerTime "trigger time" [&micro;s]
     * @param tick the tick number within the external waveform
     * @param sample start sample of the external waveform
     * @param frame start frame of the external waveform
     * @return @ref DetectorClocksTriggerTime "trigger time" of the specified tick [&micro;s]
     * @see OpticalTick2TrigTime(), ExternalTick2BeamTime(), ExternalClock()
     * 
     * This method works like `OpticalTick2TrigTime()`, but it pertains a
     * waveform recorded using the external clock (`ExternalClock()`) instead of
     * the optical one (`OpticalClock()`).
     */
    virtual double ExternalTick2TrigTime(double tick, size_t sample, size_t frame) const = 0;

    /**
     * @brief Converts an @ref DetectorClocksExternalElectronicsTime "external time tick" into a @ref DetectorClocksBeamGateTime "beam gate time" [&micro;s]
     * @param tick the tick number within the external waveform
     * @param sample start sample of the external waveform
     * @param frame start frame of the external waveform
     * @return @ref DetectorClocksBeamGateTime "beam gate time" of the specified tick [&micro;s]
     * @see OpticalTick2BeamTime(), ExternalTick2TrigTime(), ExternalClock()
     * 
     * This method works like `ExternalTick2TrigTime()`, but it returns the
     * result as @ref DetectorClocksBeamGateTime "beam gate time" (instead of
     * @ref DetectorClocksTriggerTime "trigger time").
     */
    virtual double ExternalTick2BeamTime(double tick, size_t sample, size_t frame) const = 0;
    
    /// Given External time-tick (waveform index), sample and frame number, returns time electronics clock count [tdc]
    /**
     * @brief Converts an @ref DetectorClocksExternalElectronicsTime "external time tick" into ticks from @ref DetectorClocksElectronicsStartTime "electronics start time"
     * @param tick the tick number within the external waveform
     * @param sample start sample of the external waveform
     * @param frame start frame of the external waveform
     * @return external clock ticks from the @ref DetectorClocksElectronicsStartTime "electronics start time"
     * @see ExternalTick2TrigTime(), OpticalTick2TDC(), ExternalClock()
     * 
     * This method works like `OpticalTick2TDC()`, but it pertains a
     * waveform recorded using the external clock (`ExternalClock()`) instead of
     * the optical one (`OpticalClock()`).
     */
    virtual double ExternalTick2TDC(double tick, size_t sample, size_t frame) const = 0;
    
    /**
     * @brief Converts an @ref DetectorClocksExternalElectronicsTime "external time tick" into time from the first external frame [&micro;s]
     * @param tick the tick number within the external waveform
     * @param sample start sample of the external waveform
     * @param frame start frame of the external waveform
     * @return time elapsed from the starting frame (see below) [&micro;s]
     * @see OpticalTick2TDC(), OpticalTick2TrigTime(), ExternalTick2Time(), OpticalClock()
     * 
     * This method is similar to `OpticalTick2Time()`, but it refers to the
     * @ref DetectorClocksExternalElectronicsTime "external electronics time"
     * and uses the external clock (`ExternalClock()`) instead of the optical
     * one (`OpticalClock()`).
     * 
     * As stated more extensively in `OpticalTick2Time()` documentation, **with
     * the assumption that sample `0` of frame `0` happens at
     * @ref DetectorClocksElectronicsStartTime "electronics start time", the
     * return value of this method is in
     * @ref DetectorClocksElectronicsTime "electronic time frame".**
     */
    virtual double ExternalTick2Time(double tick, size_t sample, size_t frame) const = 0;
    
    /// @}
    // --- END Conversions from electronics waveform ticks ---------------------
    
    
    
    // --- BEGIN Conversions to electronics waveform ticks ---------------------
    /// @{
    /**
     * @name Conversions to electronics waveform ticks
     * 
     * The conversion methods to waveform ticks _from_ other electronics
     * waveform ticks are listed elsewhere, together with all the conversions
     * from electronics ticks.
     */
    
    /**
     * @brief Converts @ref DetectorClocksSimulationTime "simulation time" into a @ref DetectorClocksElectronicsTime "electronics time tick".
     * @param g4time simulation time [ns]
     * @return @ref DetectorClocksElectronicsTime "electronics time tick", including a fractional part
     * 
     * The result is a real number including a fractional part describing how
     * far within the tick the specified instant `g4time` is.
     * To effectively have the tick number, truncate the value, e.g.:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * auto tick = static_cast<int>(detClocks->TPCG4Time2TDC(simTime));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    virtual double TPCG4Time2TDC(double g4time) const = 0;
    
    /**
     * @brief Converts @ref DetectorClocksSimulationTime "simulation time" into a @ref DetectorClocksTPCelectronicsTime "TPC electronics time tick".
     * @param g4time simulation time [ns]
     * @return @ref DetectorClocksTPCelectronicsTime "TPC electronics time tick", including a fractional part
     * 
     * The result is a real number including a fractional part describing how
     * far within the tick the specified instant `g4time` is.
     * To effectively have the tick number, truncate the value, e.g.:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * auto tick = static_cast<int>(detClocks->TPCG4Time2Tick(simTime));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    virtual double TPCG4Time2Tick(double g4time) const = 0;
    
    
    /**
     * @brief Converts a @ref DetectorClocksSimulationTime "simulation time" into @ref DetectorClocksOpticalElectronicsTime "optical electronics time ticks".
     * @param g4time @ref DetectorClocksSimulationTime "simulation time" value [ns]
     * @return number of @ref DetectorClocksOpticalElectronicsTime "optical electronics time ticks" from the @ref DetectorClocksElectronicsStartTime "electronics start time" for the specified time
     * @see `G4ToElecTime()`, `ExternalG4Time2TDC()`, `OpticalClock()`
     * 
     * Note that the result is the number of _optical ticks_ (`OpticalClock()`)
     * from the @ref DetectorClocksElectronicsStartTime "electronics start time"
     * (not the optical electronics start time, which is in fact 
     * @ref DetectorClocksElectronicsTimesNote "not absolutely defined").
     * 
     * The result is a real number including a fractional part describing how
     * far within the tick the specified instant `g4time` is.
     * To effectively have the tick number, truncate the value, e.g.:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * auto tick = static_cast<int>(detClocks->OpticalG4Time2TDC(simTime));
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    virtual double OpticalG4Time2TDC(double g4time) const = 0;
    
    /**
     * @brief Converts a @ref DetectorClocksSimulationTime "simulation time" into @ref DetectorClocksExternalElectronicsTime "external electronics time ticks".
     * @param g4time @ref DetectorClocksSimulationTime "simulation time" value [ns]
     * @return number of @ref DetectorClocksExternalElectronicsTime "external electronics time ticks" from the @ref DetectorClocksElectronicsStartTime "electronics start time" for the specified time
     * @see `G4ToElecTime()`, `OpticalG4Time2TDC()`, `ExternalClock()`
     * 
     * This method is similar to `OpticalG4Time2TDC()`, with the difference that
     * @ref DetectorClocksExternalElectronicsTime "external electronics time" is
     * used instead of the
     * @ref DetectorClocksOpticalElectronicsTime "optical one".
     */
    virtual double ExternalG4Time2TDC(double g4time) const = 0;

    /// @}
    // --- END Conversions to electronics waveform ticks ---------------------
    
    
    protected:
      DetectorClocks() = default;

  }; // class DetectorClocks

} //namespace detinfo


#endif // LARDATA_DETECTORINFO_DETECTORCLOCKS_H
