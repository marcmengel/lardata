/** ****************************************************************************
 * @file RawDigit.cxx
 * @brief Definition of basic raw digits
 * @author brebel@fnal.gov
 * @see  RawDigit.h raw.h
 * 
 * Compression/uncompression utilities are declared in lardata/RawData/raw.h .
 * 
 * ****************************************************************************/

#include "RawData/RawDigit.h"

#include "cetlib/exception.h"

namespace raw{

  //----------------------------------------------------------------------
  void RawDigit::SetPedestal(float ped, float sigma /* = 1. */ )
  {

    fPedestal = ped;
    fSigma = sigma;

  } // RawDigit::SetPedestal()


} // namespace raw
////////////////////////////////////////////////////////////////////////

