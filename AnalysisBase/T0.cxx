////////////////////////////////////////////////////////////////////////
// \version $Id: 
//
// \brief Definition of T0 analysis object
//
// \author k.warburton@sheffield.ac.uk
////////////////////////////////////////////////////////////////////////

#include "AnalysisBase/T0.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace anab{

  //----------------------------------------------------------------------
  T0::T0() 
    : fTime(0)
    , fTriggerType(0)
    , fTriggerBits(0)
  {
  }

  //----------------------------------------------------------------------
  T0::T0(double Time,
	 unsigned int TriggerType,
	 unsigned int TriggerBits)
    : fTime(Time)
    , fTriggerType(TriggerType)
    , fTriggerBits(TriggerBits)
  {

  }

  //----------------------------------------------------------------------
  // ostream operator.  
  //
  std::ostream& operator<< (std::ostream & o, T0 const& a)
  {
    o << "T0 with Time: "    << a.fTime
      << "\n from Trigger type: "   << a.fTriggerType
      << "\n with bits: "           << a.fTriggerBits
      <<std::endl;
    

    return o;
  }
  
}
