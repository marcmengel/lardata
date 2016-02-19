/** ****************************************************************************
 * @file RawDigit.cxx
 * @brief Definition of basic raw digits
 * @author brebel@fnal.gov
 * @see  RawDigit.h raw.h
 * 
 * Compression/uncompression utilities are declared in lardata/RawData/raw.h .
 * 
 * ****************************************************************************/

#include "lardata/RawData/RawDigit.h"

// C/C++ standard libraries
#include <utility> // std::move()


namespace raw{
  
  //----------------------------------------------------------------------
  RawDigit::RawDigit()
    : fADC(0)
    , fChannel(InvalidChannelID) 
    , fSamples(0) 
    , fPedestal(0.) 
    , fSigma(0.)
    , fCompression(kNone)
  //  , fFlags(DefaultFlags)
  {}
  
  
  //----------------------------------------------------------------------
  RawDigit::RawDigit(
    ChannelID_t                  channel,
    unsigned short               samples,
    RawDigit::ADCvector_t const& adclist,
    Compress_t                   compression /* = kNone */ /*,
    const Flags_t&               flags / * = DefaultFlags * / */
  )
    : fADC(adclist)
    , fChannel(channel)
    , fSamples(samples)
    , fPedestal(0.)
    , fSigma(0.)
    , fCompression(compression)
  //  , fFlags(flags)
  {}
  
  
  //----------------------------------------------------------------------
  RawDigit::RawDigit(
    ChannelID_t             channel,
    unsigned short          samples,
    RawDigit::ADCvector_t&& adclist,
    Compress_t              compression /* = kNone */ /*,
    const Flags_t&          flags / * = DefaultFlags * / */
  )
    : fADC(std::move(adclist))
    , fChannel(channel)
    , fSamples(samples)
    , fPedestal(0.)
    , fSigma(0.)
    , fCompression(compression)
  //  , fFlags(flags)
  {}
  
  
  //----------------------------------------------------------------------
  void RawDigit::SetPedestal(float ped, float sigma /* = 1. */ )
  {
    
    fPedestal = ped;
    fSigma = sigma;
    
  } // RawDigit::SetPedestal()
  
  
} // namespace raw
////////////////////////////////////////////////////////////////////////

