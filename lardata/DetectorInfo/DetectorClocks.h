////////////////////////////////////////////////////////////////////////
// \file LArProperties.h
//
// \brief pure virtual base interface for detector clocks
//
// \author jpaley@fnal.gov
// 
////////////////////////////////////////////////////////////////////////
#ifndef DETINFO_IDETCLOCKS_H
#define DETINFO_IDETCLOCKS_H

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
   * Many different components contribute to the operation of a LArTPC, and more
   * are needed for the simulation. We can consider:
   * 
   * * *trigger time*: the instant the physics event is detected (in
   *     reconstruction jargon this is a @f$ t_{0} @f$)
   * * *TPC electronics time*: the instant the ionisation from a particle
   *      reaches the charge readout
   * * *optical electronics time*: the instant the scintillation from a particle
   *      reaches the optical readout
   * * *beam gate time*: the instant the particle beams enters the detector;
   *      this can be taken of the trigger time for the beam particles
   * * *external time*: another time frame from hardware other than TPC and
   *      optical readout
   * 
   * In addition, other times are involved in the simulation:
   * * *generation time* assigned by the event generator (e.g. GENIE); this is
   *     stored in the trajectory points of `simb::MCParticle` (usually the
   *     generators store a single trajectory point for each particle)
   * * *Geant4 time* assigned by the simulation of propagation of particle
   *     through matter (Geant4), and stored into each trajectory point of
   *     `simb::MCParticle` produced by `LArG4` module or equivalent;
   *     _Note that this is not necessarily the same as the generator one, even
   *     for the particles present in both collections._
   * 
   * All these times are measured in time units (most often in microseconds or
   * nanoseconds). For the times bound to readout, there is also an equivalent
   * tick count time (the TDC), which is an integral value representing the
   * number of elapsed sampling periods of the detector they refer too (and that
   * period is not the same for charge and optical readout).
   * 
   * Finally, it is important to keep in mind that all times are measured
   * relative to a starting time, and that starting time is different for the
   * different categories.
   * 
   * While this is specifically designed with the aim of giving physicists
   * headache, the `detinfo::DetectorClocks` service provider attempts to tame
   * this complexity by providing methods to convert from one time to the other.
   * 
   * The following table represents the available conversion functions, with the
   * time in the first column as the starting time and in the columns the times
   * to convert to:
   * 
   *              to: | Trigger                   | Beam gate                 | Geant4  | TPC                | TPC (ticks) | Optical | Optical (ticks) 
   * ---------------: | ------------------------- | ------------------------- | ------- | ------------------ | ----------- | ------- | --------------- 
   * (unit)           | &micro;s                  |                           |    ns   | &micro;s           |             |         |                 
   * Trigger          |                           |                           |         | `TriggerTime()`    |             |         |                 
   * Beam gate        |                           |                           |         | `BeamGateTime()`   |             |         |                 
   * Geant4           |                           |                           |         | `G4ToElecTime()`   |             |         |                 
   * TPC              |                           |                           |         |                    |             |         |                 
   * TPC (ticks)      | `TPCTick2TrigTime()`      | `TPCTick2BeamTime()`      |         |                    |             |         |                 
   * Optical          |                           |                           |         |                    |             |         |                 
   * Optical (ticks)  | `OpticalTick2TrigTime()`  | `OpticalTick2BeamTime()`  |         |                    |             |         |
   * External         |                           |                           |         |                    |             |         |                 
   * External (ticks) | `ExternalTick2TrigTime()` | `ExternalTick2BeamTime()` |         |                    |             |         |                 
   * 
   * 
   * Note that the complete definition of optical and external time requires
   * additional information to identify the absolute reference time their
   * ticks are measured respect to (namely sample and frame numbers).
   * 


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
    // Getters of TPC ElecClock
    //
    /// Borrow a const TPC clock with time set to Trigger time [us]
    virtual const ::detinfo::ElecClock& TPCClock() const = 0;

    /// Create a TPC clock for a given time [us] from clock counting start
    virtual ::detinfo::ElecClock TPCClock(double time) const = 0; 

    /// Create a TPC clock for a given sample/frame number in TPC clock frequency
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
      

   * 
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
    
    /// Given Geant4 time [ns], returns relative time [us] w.r.t. electronics time T0 
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


#endif 
