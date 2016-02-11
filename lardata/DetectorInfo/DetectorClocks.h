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

#include "lardata/Utilities/ElecClock.h"

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

  class DetectorClocks {

  public:
    DetectorClocks(const DetectorClocks &) = delete;
    DetectorClocks(DetectorClocks &&) = delete;
    DetectorClocks& operator = (const DetectorClocks &) = delete;
    DetectorClocks& operator = (DetectorClocks &&) = delete;
    virtual ~DetectorClocks() = default;
    
    virtual double TriggerOffsetTPC() const = 0;
    
    /// Given Geant4 time [ns], returns relative time [us] w.r.t. electronics time T0 
    virtual double G4ToElecTime(double g4_time) const = 0;
    
    /// Trigger electronics clock time in [us]
    virtual double TriggerTime() const = 0;
    
    /// Beam gate electronics clock time in [us]
    virtual double BeamGateTime() const = 0;

    virtual std::vector<std::string> ConfigNames() const = 0;
    virtual std::vector<double> ConfigValues() const = 0;

    //
    // Getters of TPC ElecClock
    //
    /// Borrow a const TPC clock with time set to Trigger time [us]
    virtual const ::util::ElecClock& TPCClock() const = 0;

    /// Create a TPC clock for a given time [us] from clock counting start
    virtual ::util::ElecClock TPCClock(double time) const = 0; 

    /// Create a TPC clock for a given sample/frame number in TPC clock frequency
    virtual util::ElecClock TPCClock(unsigned int sample,unsigned int frame) const = 0;
    //
    // Getters of Optical ElecClock
    //
    /// Borrow a const Optical clock with time set to Trigger time [us]
    virtual const util::ElecClock& OpticalClock() const = 0;

    /// Create a Optical clock for a given time [us] from clock counting start
    virtual util::ElecClock OpticalClock(double time) const = 0;

    /// Create a Optical clock for a given sample/frame number in Optical clock frequency
    virtual util::ElecClock OpticalClock(unsigned int sample, unsigned int frame) const = 0;
      
    //
    // Getters of Trigger ElecClock
    //
    /// Borrow a const Trigger clock with time set to Trigger time [us]
    virtual const util::ElecClock& TriggerClock() const = 0;

    /// Create a Trigger clock for a given time [us] from clock counting start    
    virtual util::ElecClock TriggerClock(double time) const = 0;

    /// Create a Trigger clock for a given sample/frame number in Trigger clock frequency
    virtual util::ElecClock TriggerClock(unsigned int sample, unsigned int frame) const = 0;

    //
    // Getters of External ElecClock
    //
    /// Borrow a const Trigger clock with time set to External Time [us]
    virtual const util::ElecClock& ExternalClock() const = 0;

    /// Create a External clock for a given time [us] from clock counting start
    virtual util::ElecClock ExternalClock(double time) const = 0;
    
    /// Create a External clock for a given sample/frame number in External clock frequency
    virtual util::ElecClock ExternalClock(unsigned int sample, unsigned int frame) const = 0;
        
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
