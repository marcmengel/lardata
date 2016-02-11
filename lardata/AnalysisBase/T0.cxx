////////////////////////////////////////////////////////////////////////
// \version $Id: 
//
// \brief Definition of T0 analysis object
//
// \author k.warburton@sheffield.ac.uk
////////////////////////////////////////////////////////////////////////

#include "lardata/AnalysisBase/T0.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace anab{

  //----------------------------------------------------------------------
  T0::T0() 
    : fTime(0)
    , fTriggerType(0)
    , fTriggerBits(0)
    , fID(-1)
    , fTriggerConfidence(-1)
  {
  }

  //----------------------------------------------------------------------
  T0::T0(double Time,
	 unsigned int TriggerType,
	 int TriggerBits,
	 int ID,
	 double TriggerConfidence)
    : fTime(Time)
    , fTriggerType(TriggerType)
    , fTriggerBits(TriggerBits)
    , fID(ID)
    , fTriggerConfidence(TriggerConfidence)
  {

  }

  //----------------------------------------------------------------------
  // ostream operator.  
  //
  std::ostream& operator<< (std::ostream & o, T0 const& a)
  {
    o << "T0 with Time: "           << a.fTime              /// Time in ns
      << "\n from Trigger type: "   << a.fTriggerType       /// Type of trigger used. 0 - Muon Counters, 1 - Photon Detectors, 2 - Monte Carlo Truth
      << "\n with bits: "           << a.fTriggerBits       /// An identifier for the Muon track / Flash / MCParticle used in matching.
      << "\n with ID: "             << a.fID                /// Current size of T0 data product.
      << "\n with confidence: "     << a.fTriggerConfidence /// Confidence with which this T0 is known.
      <<std::endl; 

    return o;
  }
  
}
