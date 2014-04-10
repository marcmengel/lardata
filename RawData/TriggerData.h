// TriggerData/TriggerData.h
#ifndef TRIGGER_H
#define TRIGGER_H

// C++ includes
#include <vector>
#include <limits>
#include <stdexcept>
#include <iostream>
namespace raw {

  class Trigger {

  public:

    /// Default ctor
    Trigger();

  private:

    unsigned int fTriggerNumber;       ///< Trigger counter
    double       fTriggerTime;         ///< Trigger time w.r.t. electronics clock T0
    double       fBeamGateTime;        ///< BeamGate time w.r.t. electronics clock T0
    double       fReadOutStartTPC;     ///< TPC readout start time w.r.t. electronics clock T0
    double       fReadOutStartOptical; ///< Optical readout start time w.r.t. electronics clock T0
    unsigned int fTriggerBits;         ///< Trigger bits ... dedicated bit-by-bit function available

#ifndef __GCCXML__
  public:

    /// Alternative constructor    
    Trigger(unsigned int counter,
	    double       trigger_time,
	    double       beamgate_time,
	    double       tpc_readout_start,
	    double       opt_readout_start,
	    uint32_t     bits);
    
    /// Trigger number
    unsigned int TriggerNumber()          const;
    /// Trigger time w.r.t. electronics clock T0 in ns
    double       TriggerTime  ()          const;
    /// BeamGate time w.r.t. electronics clock T0 in ns
    double       BeamGateTime ()          const;
    /// Beginning of TPC readout start time w.r.t. electronics clock T0 in ns
    double       ReadOutStartTPC     ()   const;
    /// Beginning of Optical readout start time w.r.t. electronics clock T0 in ns
    double       ReadOutStartOptical ()   const;
    /// Trigger Bits
    unsigned int TriggerBits  ()          const;
    /// Accessor to specific bit
    bool         Triggered(const unsigned char bit) const;
#endif
  };
}

#ifndef __GCCXML__

raw::Trigger::Trigger()
{
  fTriggerNumber       = std::numeric_limits<unsigned int>::max();
  
  fTriggerTime         = std::numeric_limits<double>::max();
  fBeamGateTime        = std::numeric_limits<double>::max();
  
  fReadOutStartOptical = std::numeric_limits<double>::max();
  fReadOutStartTPC     = std::numeric_limits<double>::max();
  
  fTriggerBits         = 0x0;
}

raw::Trigger::Trigger(unsigned int counter,
		      double       trigger_time,
		      double       beamgate_time,
		      double       tpc_readout_start,
		      double       opt_readout_start,
		      uint32_t     bits)
  : fTriggerNumber       ( counter           ),
    fTriggerTime         ( trigger_time      ),
    fBeamGateTime        ( beamgate_time     ),
    fReadOutStartTPC     ( tpc_readout_start ),
    fReadOutStartOptical ( opt_readout_start ),
    fTriggerBits         ( bits              )
{}

unsigned int raw::Trigger::TriggerNumber()          const { return fTriggerNumber;       }
double       raw::Trigger::TriggerTime  ()          const { return fTriggerTime;         }
double       raw::Trigger::BeamGateTime ()          const { return fBeamGateTime;        }
double       raw::Trigger::ReadOutStartTPC     ()   const { return fReadOutStartTPC;     }
double       raw::Trigger::ReadOutStartOptical ()   const { return fReadOutStartOptical; }
unsigned int raw::Trigger::TriggerBits  ()          const { return fTriggerBits;         }

#endif

#endif 
