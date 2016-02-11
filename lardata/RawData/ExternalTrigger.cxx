////////////////////////////////////////////////////////////////////////
// $Id: ExternalTrigger.cxx,v 1.7 2010/02/15 19:34:20 brebel Exp $
//
// ExternalTrigger class
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#include "lardata/RawData/ExternalTrigger.h"
#include <string>
#include <iostream>

namespace raw{

  //----------------------------------------------------------------------
  ExternalTrigger::ExternalTrigger() : 
    fTrigID(0),fTrigTime(0)
  {

  }

  //----------------------------------------------------------------------
  ExternalTrigger::ExternalTrigger(unsigned int trigid, TriggerTimeStamp_t trigtime) 
    : fTrigID(trigid)
    , fTrigTime(trigtime)
  {

  }


}
////////////////////////////////////////////////////////////////////////

