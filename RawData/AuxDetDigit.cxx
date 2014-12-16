////////////////////////////////////////////////////////////////////////
// $Id: AuxDetDigit.cxx,v 1.13 2010/03/26 19:36:42 brebel Exp $
//
// AuxDetDigit class
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#include "RawData/AuxDetDigit.h"

#include "cetlib/exception.h"

namespace raw{

  //----------------------------------------------------------------------
  AuxDetDigit::AuxDetDigit()  
    : fADC(0)
    , fChannel(0) 
    , fAuxDetName("UnknownAuxDet")
  {

  }

  //----------------------------------------------------------------------
  AuxDetDigit::AuxDetDigit(unsigned short     channel,
			   std::vector<short> adclist, 
			   std::string        name) 
    : fADC(adclist) 
    , fChannel(channel) 
    , fAuxDetName(name)
  { 

  }

  //--------------------------------------------------
  short AuxDetDigit::ADC(size_t i) const
  {
    if(i >= fADC.size())
      throw cet::exception("AuxDetDigit") << "illegal index requested for ADC vector: "
					  << i << "\n";

    return fADC[i];
  }

}
////////////////////////////////////////////////////////////////////////

