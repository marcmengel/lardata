// TriggerData/TriggerData.h
#ifndef TRIGGERDATA_H
#define TRIGGERDATA_H

// C++ includes
#include <vector>
#include <limits>
#include <stdexcept>
#include <iostream>
namespace raw {
  
  class Trigger {

  public:

    /// Default ctor
    Trigger()
    {
      fTriggerNumber       = std::numeric_limits<unsigned int>::max();
      
      fTriggerTime         = std::numeric_limits<double>::max();
      fBeamGateTime        = std::numeric_limits<double>::max();
      
      fReadOutStartOptical = std::numeric_limits<double>::max();
      fReadOutStartTPC     = std::numeric_limits<double>::max();
      
      fTriggerBits         = 0x0;
    }
    
  private:
    
    unsigned int fTriggerNumber;       ///< Trigger counter
    double       fTriggerTime;         ///< Trigger time w.r.t. electronics clock T0
    double       fBeamGateTime;        ///< BeamGate time w.r.t. electronics clock T0
    double       fReadOutStartTPC;     ///< TPC readout start time w.r.t. electronics clock T0
    double       fReadOutStartOptical; ///< Optical readout start time w.r.t. electronics clock T0
    unsigned int fTriggerBits;         ///< Trigger bits ... dedicated bit-by-bit function available

  public:

#ifndef __GCCXML__

    /// Alternative constructor    
    Trigger(unsigned int counter,
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

    /// Trigger number
    unsigned int TriggerNumber()          const { return fTriggerNumber;       }
    /// Trigger time w.r.t. electronics clock T0 in ns
    double       TriggerTime  ()          const { return fTriggerTime;         }
    /// BeamGate time w.r.t. electronics clock T0 in ns
    double       BeamGateTime ()          const { return fBeamGateTime;        }
    /// Beginning of TPC readout start time w.r.t. electronics clock T0 in ns
    double       ReadOutStartTPC     ()   const { return fReadOutStartTPC;     }
    /// Beginning of Optical readout start time w.r.t. electronics clock T0 in ns
    double       ReadOutStartOptical ()   const { return fReadOutStartOptical; }
    /// Trigger Bits
    unsigned int TriggerBits  ()          const { return fTriggerBits;         }
    /// Accessor to specific bit
    bool         Triggered(const unsigned char bit) const;

#endif
  };
}

#endif 
