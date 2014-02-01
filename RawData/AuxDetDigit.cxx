////////////////////////////////////////////////////////////////////////
// $Id: AuxDetDigit.cxx,v 1.13 2010/03/26 19:36:42 brebel Exp $
//
// AuxDetDigit class
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#include "RawData/AuxDetDigit.h"

#include <string>
#include <iostream>
#include <cassert>

#include "cetlib/exception.h"

namespace raw{

  //----------------------------------------------------------------------
  AuxDetDigit::AuxDetDigit()  
    : fADC(0)
    , fChannel(0) 
    , fAuxDetType(raw::kUnknownAuxDet)
  {

  }

  //----------------------------------------------------------------------
  AuxDetDigit::AuxDetDigit(unsigned short     channel,
			   std::vector<short> adclist, 
			   raw::AuxDetType_t  type) 
    : fADC(adclist) 
    , fChannel(channel) 
    , fAuxDetType(type)
  { 

  }

  //--------------------------------------------------
  short AuxDetDigit::ADC(size_t i) const
  {
    if(i >= fADC.size())
      throw cet::exception("AuxDetDigit") << "illegal index requested for ADC vector: "
					  << i;

    return fADC[i];
  }

}
////////////////////////////////////////////////////////////////////////

