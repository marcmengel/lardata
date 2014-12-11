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

  //--------------------------------------------------
  short RawDigit::ADC(int i) const
  {
    unsigned int j = i;
    if(i < 0 || (j>fADC.size()) )
      throw cet::exception("RawDigit") << "asked for illegal ADC index: " << i << "\n";

    return fADC[j];
  }


  //----------------------------------------------------------------------
  void RawDigit::SetPedestal(float ped, float sigma /* = 1. */ )
  {

    fPedestal = ped;
    fSigma = sigma;

  } // RawDigit::SetPedestal()


} // namespace raw
////////////////////////////////////////////////////////////////////////

