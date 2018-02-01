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
   * Many different components contribute to the operation of a LArTPC, and more
   * are needed for the simulation.
   * Some of the relevant events and time points associated with time are:
   * * *hardware trigger*: the instant the trigger happens or is expected;
   *     data acquisition reacts at this time (_multi-event_)
   * * *beam gate opening*: the instant the beam particles are expected to enter
   *     the detector (_multi-event_)
   * * *software trigger*: additional logic can provide a more detailed
   *     estimation of the physics event time; this usually comes too late for
   *     the electronics time to be defined by it (_multi-event_)
   * * *electronics start time*: a reference time instant; this is usually
   *     matching the instant the TPC readout starts recording raw digits
   *     (_multi-event_)
   * * *TPC electronics start time*: the instant corresponding to the first
   *     sample of the raw digits from TPC; for untampered `raw::RawDigit`
   *     objects, this time matches the general "electronics start time"
   *     definition, and for the model of data acquisition described here,
   *     this time is by definition function of the hardware trigger. Trimmed
   *     `raw::RawDigit` with a start portion clipped away will break the
   *     assumption of electronics time matching the TPC one (which is defined
   *     as seen by `raw::RawDigit`) (_multi-event_)
   * * other electronics start times (*optical detector*, *cosmic ray tagger*,
   *     etc.) represent the instant the respective data acquisition subsystems
   *     start digitizing the data from their detectors (_multi-event_)
   * * *event trigger*: the instant the physics event is detected (in
   *     reconstruction jargon this is a @f$ t_{0} @f$); ideally, for most
   *     events involving beam particles this is the a little after beam gate
   *     time, and on top of the hardware trigger time (_per event_)
   * * *charge collection*: the instant the ionisation from a particle reaches
   *     the charge readout (_per particle_)
   * * *optical collection*: the instant the scintillation from a particle
   *      reaches the optical readout (_per particle_)
   * * *physics event time*: the instant the main interaction happens; this is
   *      assigned by the event generator (_per event_)
   * * *Geant4 time*: assigned to each particle by the simulation of propagation
   *     through matter (Geant4);
   *     _note that this is not necessarily the same as the event time_. (_per
   *     event_)
   * 
   * In parentheses, it is indicated if such instant is defined for each
   * considered particle, or for the complete physics process ("event") or for
   * all physics processes recorded in the same time window ("multi-event").
   * 
   * We can also define many ways to measure these times with respect to the
   * others. We use the following time scales:
   * * *electronics time*: from the _electronics start time_; this is the "glue"
   *     time frame connecting most of the others _[&micro;s]_
   * * *TPC time*: from the _TPC electronics start time_ _[&micro;s]_
   * * *beam gate time*: from the _beam gate opening time_ _[&micro;s]_
   * * *trigger time*: from the _hardware trigger_ _[&micro;s]_
   * * *event time*: from an _event trigger_ _[&micro;s]_
   * * *generation time*: from the _physics event time_; this is
   *     stored in the trajectory points of `simb::MCParticle` (usually the
   *     generators store a single trajectory point for each particle) _[ns]_
   * * *Geant4 time*: from the _Geant4 time_; this is stored into each
   *     trajectory point of `simb::MCParticle` produced by `LArG4` module or
   *     equivalent _[ns]_
   * * *simulation time*: the same as _Geant4 time_ (and also generation
   *     time, which in LArSoft happens to match the former) _[ns]_
   * 
   * The list above reports in square brackets the "standard" unit, used for
   * that times in that scale by all LArSoft code unless explicitly specified
   * otherwise.
   * For the times bound to readout, there is also an equivalent tick count time
   * (from the TDC), which is an integral value representing the number of
   * elapsed sampling periods of the detector they refer too (and that period
   * may be different for each piece of hardware).
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
   * time in the first column as the starting time and in the columns the times
   * to convert to:
   * 
   * |        to &rarr; | electronics time      | (_ticks_)            | TPC time ticks     | trigger time              | trigger clock ticks | beam gate time            | Optical clock ticks   | External clock ticks   |
   * | ---------------: | --------------------- | -------------------- | ------------------ | ------------------------- | ------------------- | ------------------------- | --------------------- | ---------------------- |
   * | (unit)           | &micro;s              |                      |                    | &micro;s                  | `TriggerClock()`    | &micro;s                  |                       |                        |
   * | hardware trigger | `TriggerTime()`       | `TPCClock()`         |                    |                           |                     |                           | `OpticalClock()`      | `ExternalClock()`      |
   * | beam gate point  | `BeamGateTime()`      |                      |                    |                           |                     |                           |                       |                        |
   * | electronics time |                       |                      |                    |                           |                     |                           |                       |                        |
   * | &nbsp; (_ticks_) |                       |                      | `TPCTDC2Tick()`    |                           |                     |                           |                       |                        |
   * | TPC time         |                       |                      |                    |                           |                     |                           |                       |                        |
   * | &nbsp; (_ticks_) | `TPCTick2Time()`      | `TPCTick2TDC()`      |                    | `TPCTick2TrigTime()`      |                     | `TPCTick2BeamTime()`      |                       |                        |
   * | trigger time     |                       |                      |                    |                           |                     |                           |                       |                        |
   * | &nbsp; (_ticks_) |                       |                      |                    |                           |                     |                           |                       |                        |
   * | Optical          |                       |                      |                    |                           |                     |                           |                       |                        |
   * | &nbsp; (_ticks_) | `OpticalTick2Time()`  | `OpticalTick2TDC()`  |                    | `OpticalTick2TrigTime()`  |                     | `OpticalTick2BeamTime()`  |                       |                        |
   * | External         |                       |                      |                    |                           |                     |                           |                       |                        |
   * | &nbsp; (_ticks_) | `ExternalTick2Time()` | `ExternalTick2TDC()` |                    | `ExternalTick2TrigTime()` |                     | `ExternalTick2BeamTime()` |                       |                        |
   * | simulation time  | `G4ToElecTime()`      | `TPCG4Time2TDC()`    | `TPCG4Time2Tick()` |                           |                     |                           | `OpticalG4Time2TDC()` | `ExternalG4Time2TDC()` |
   * 
   * Note that the complete definition of optical and external time requires
   * additional information to identify the absolute reference time their
   * ticks are measured from, which is in fact the electronics time reference.
   * Also, the tick conversion includes also a reference frame and sample (both
   * are passed to the conversion methods as additional arguments).
   * 
   * The names are not thoroughly consistent, but they roughly follow the
   * following rules:
   * * the "trigger time" is named as `TrigTime` (but see below)
   * * the "beam time" is named as `BeamTime` (but see below)
   * * the "electronics time" is named simply as `Time`; note that e.g.
   *     `TriggerTime()` and `BeamGateTime()` must be read as `Trigger` and
   *     `BeamGate` plus `Time()`, that is they return a `Time` value (i.e.,
   *     electronics time) of the hardware trigger instant and beam gate opening
   *     instant, in electronics time frame
   * * the "electronics time" ticks are labelled `TDC`
   * * the "simulation time" is labelled `G4` or `G4Time`
   * * `TPCTick` is the TPC electronics clock ticks, measured from the TPC
   *   electronics start time
   * * `TPC` is usually used in conjunction with `Tick`, where it means
   *   `TPCTick`; but in `TPCG4Time2TDC()` it is a misnomer (the `TDC` wins in
   *   that the result is in ticks in electronics time reference, not in TPC
   *   electronics time)
   * 
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
   * A clock object does not change the time frame: in the conversions between
   * times and ticks, and the other way around, it always assumes that at tick 0
   * (and frame 0, sample 0, if needed), the time is also 0.
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
   *       generating the trigger, while the latter is the instant the hardware
   *       trigger actually happened.
   * 
   * @todo Add a method to convert electronics time tick into electronics time.
   * 
   */
  class DetectorClocks {

  public:
    DetectorClocks(const DetectorClocks &) = delete;
    DetectorClocks(DetectorClocks &&) = delete;
    DetectorClocks& operator = (const DetectorClocks &) = delete;
    DetectorClocks& operator = (DetectorClocks &&) = delete;
    virtual ~DetectorClocks() = default;
    
    /**
     * @brief Time offset from trigger to TPC readout start [&micro;s].
     * 
     * This is the time the TPC readout starts, with respect to the trigger
     * time. This is also the time where the TPC clock starts, again respect to
     * the trigger time.
     * 
     * When negative, it means the TDC clock started before the trigger.
     * And it'd better be negative... or your readout missed the trigger!
     */
    virtual double TriggerOffsetTPC() const = 0;
    
    /// Given Geant4 time [ns], converts it into the electronics time [&micro;s].
    virtual double G4ToElecTime(double g4_time) const = 0;
    
    /// Trigger electronics clock time in [us].
    virtual double TriggerTime() const = 0;
    
    /// Beam gate electronics clock time in [us].
    virtual double BeamGateTime() const = 0;

    virtual std::vector<std::string> ConfigNames() const = 0;
    virtual std::vector<double> ConfigValues() const = 0;

    //
    // Getters of TPC ElecClock
    //
    
    /**
     * @brief Lends a constant TPC clock with time set to _trigger time_
     *        [&micro;s].
     * @return a constant reference to the TPC clock class
     * 
     * This clock has the same timing settings as the TPC channel readout
     * electronics (which ultimately fills `raw::RawDigit` objects).
     * The clock is not bound to any reference time: whatever the reference time
     * of the `time` argument is, that is also the reference of the clock.
     * Thus clock will return tick #0 (and time 0.0) for that reference time,
     * which is not so surprising since that reference time is measured to be
     * 0.0 by definition.
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * 
     * The current time of this object can't be changed, and it is set to the
     * trigger time. As a consequence, when the current time of the clock is
     * used, the reference is the same as the one of the trigger time
     * (`TriggerTime()`).
     */
    virtual const ::detinfo::ElecClock& TPCClock() const = 0;

    /**
     * @brief Returns a TPC clock with time set to the specified time
     *        [&micro;s].
     * @param time the current time of the new clock, in microseconds
     * @return a clock class with TPC readout electronics timing settings
     * 
     * This clock has the same timing settings as the TPC channel readout
     * electronics (which ultimately fills `raw::RawDigit` objects).
     * The clock is not bound to any reference time: whatever the reference time
     * of the `time` argument is, that is also the reference of the clock.
     * Thus clock will return tick #0 (and time 0.0) for that reference time,
     * which is not so surprising since that reference time is measured to be
     * 0.0 by definition.
     * 
     * The clock can be used to convert into time the number of ticks or a
     * sample in a frame, and vice versa.
     * The current time of this object is set to the specified time, and it can
     * can be set to any value afterwards.
     */
    virtual ::detinfo::ElecClock TPCClock(double time) const = 0; 

    /**
     * @brief Returns a TPC clock with time set to the specified time sample.
     * @param sample number of sample of the clock time in microseconds
     * @param frame number of the frame the selected sample belongs to
     * @return a clock class with TPC readout electronics settings
     * 
     * This clock represents the TPC channel readout (which ultimately fills
     * `raw::RawDigit` objects).
     * It can be used to convert into time the number of ticks or a sample in a
     * frame, and vice versa.
     * The current time of this object is set to the specified time, and it can
     * can be set to any value afterwards.
     * 
     * @todo Document the meaning of tick 0
     */
    virtual detinfo::ElecClock TPCClock(unsigned int sample,unsigned int frame) const = 0;
    
    //
    // Getters of Optical ElecClock
    //
    /// Borrow a const Optical clock with time set to Trigger time [us]
    virtual const detinfo::ElecClock& OpticalClock() const = 0;

    /// Create a Optical clock for a given time [us] from clock counting start
    virtual detinfo::ElecClock OpticalClock(double time) const = 0;

    /// Create a Optical clock for a given sample/frame number in Optical clock frequency
    virtual detinfo::ElecClock OpticalClock(unsigned int sample, unsigned int frame) const = 0;
      
    //
    // Getters of Trigger ElecClock
    //
    /// Borrow a const Trigger clock with time set to Trigger time [us]
    virtual const detinfo::ElecClock& TriggerClock() const = 0;

    /// Create a Trigger clock for a given time [us] from clock counting start    
    virtual detinfo::ElecClock TriggerClock(double time) const = 0;

    /// Create a Trigger clock for a given sample/frame number in Trigger clock frequency
    virtual detinfo::ElecClock TriggerClock(unsigned int sample, unsigned int frame) const = 0;

    //
    // Getters of External ElecClock
    //
    /// Borrow a const Trigger clock with time set to External Time [us]
    virtual const detinfo::ElecClock& ExternalClock() const = 0;

    /// Create a External clock for a given time [us] from clock counting start
    virtual detinfo::ElecClock ExternalClock(double time) const = 0;
    
    /// Create a External clock for a given sample/frame number in External clock frequency
    virtual detinfo::ElecClock ExternalClock(unsigned int sample, unsigned int frame) const = 0;
        
    //
    // Getters for time [us] w.r.t. trigger given information from waveform
    //

    /// Given TPC time-tick (waveform index), returns time [us] w.r.t. trigger time stamp
    virtual double TPCTick2TrigTime(double tick) const = 0;
    /// Given TPC time-tick (waveform index), returns time [us] w.r.t. beam gate time
    virtual double TPCTick2BeamTime(double tick) const = 0;
   
    /// Given Optical time-tick (waveform index), sample and frame number, returns time [us] w.r.t. trigger time stamp
    virtual double OpticalTick2TrigTime(double tick, size_t sample, size_t frame) const = 0;

    /// Given Optical time-tick (waveform index), sample and frame number, returns time [us] w.r.t. beam gate time stamp
    virtual double OpticalTick2BeamTime(double tick, size_t sample, size_t frame) const = 0;

    /// Given External time-tick (waveform index), sample and frame number, returns time [us] w.r.t. trigger time stamp
    virtual double ExternalTick2TrigTime(double tick, size_t sample, size_t frame) const = 0;

    /// Given External time-tick (waveform index), sample and frame number, returns time [us] w.r.t. beam gate time stamp
    virtual double ExternalTick2BeamTime(double tick, size_t sample, size_t frame) const = 0;
    
    //
    // Getters for time [tdc] (electronics clock counting ... in double precision) 
    //

    /// Given TPC time-tick (waveform index), returns electronics clock count [tdc]
    virtual double TPCTick2TDC(double tick) const = 0;
    
    /// Given G4 time [ns], returns corresponding TPC electronics clock count [tdc]
    virtual double TPCG4Time2TDC(double g4time) const = 0;

    /// Given Optical time-tick (waveform index), sample and frame number, returns time electronics clock count [tdc]
    virtual double OpticalTick2TDC(double tick, size_t sample, size_t frame) const = 0;

    /// Given G4 time [ns], returns corresponding Optical electronics clock count [tdc]  
    virtual double OpticalG4Time2TDC(double g4time) const = 0;

    /// Given External time-tick (waveform index), sample and frame number, returns time electronics clock count [tdc]
    virtual double ExternalTick2TDC(double tick, size_t sample, size_t frame) const = 0;
      
    /// Given G4 time [ns], returns corresponding External electronics clock count [tdc]
    virtual double ExternalG4Time2TDC(double g4time) const = 0;

    //
    // Getters for time [us] (electronics clock counting ... in double precision)
    //
    /// Given TPC time-tick (waveform index), returns electronics clock [us]
    virtual double TPCTick2Time(double tick) const = 0;
    
    /// Given Optical time-tick (waveform index), sample and frame number, returns electronics clock [us]
    virtual double OpticalTick2Time(double tick, size_t sample, size_t frame) const = 0;

    /// Given External time-tick (waveform index), sample and frame number, returns electronics clock [us]
    virtual double ExternalTick2Time(double tick, size_t sample, size_t frame) const = 0;

    //
    // Getters for time [ticks] (waveform index number)
    //

    /// Given electronics clock count [tdc] returns TPC time-tick
    virtual double TPCTDC2Tick(double tdc) const = 0;

    /// Given G4 time returns electronics clock count [tdc]
    virtual double TPCG4Time2Tick(double g4time) const = 0;
    
    protected:
      DetectorClocks() = default;

  }; // class DetectorClocks

} //namespace detinfo


#endif // LARDATA_DETECTORINFO_DETECTORCLOCKS_H
