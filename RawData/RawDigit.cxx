////////////////////////////////////////////////////////////////////////
// $Id: RawDigit.cxx,v 1.13 2010/03/26 19:36:42 brebel Exp $
//
// RawDigit class
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#include "RawData/RawDigit.h"
#include <string>
#include <iostream>
#include <cassert>

#include "cetlib/exception.h"

namespace raw{

  //----------------------------------------------------------------------
  RawDigit::RawDigit()  
    : fADC(0)
    , fChannel(0) 
    , fSamples(0) 
    , fPedestal(0.) 
    , fSigma(0.)
    , fCompression(raw::kNone)
  {

  }

  //----------------------------------------------------------------------
  RawDigit::RawDigit(uint32_t           channel,
		     unsigned short     samples,
		     std::vector<short> adclist, 
		     raw::Compress_t    compression) 
    : fADC(adclist) 
    , fChannel(channel) 
    , fSamples(samples)
    , fPedestal(0.) 
    , fSigma(0.)
    , fCompression(compression)
  { 

  }

  //----------------------------------------------------------------------
  RawDigit::RawDigit(uint32_t           channel,
		     std::vector<short> adclist,
		     raw::Compress_t    compression)
    : fADC(adclist)
    , fChannel(channel)
    , fSamples(0)
    , fPedestal(0.) 
    , fSigma(0.)
    , fCompression(compression)
  {

  }

  //--------------------------------------------------
  short RawDigit::ADC(int i) const
  {
    unsigned int j = i;
    if(i < 0 || (j>fADC.size()) )
      throw cet::exception("RawDigit") << "asked for illegal ADC index: " << i;

    return fADC[j];
  }


  //----------------------------------------------------------------------
  void RawDigit::SetPedestal(double ped)
  {

    fPedestal = ped;
    fSigma = 1.;

  }
}
////////////////////////////////////////////////////////////////////////

