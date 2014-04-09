////////////////////////////////////////////////////////////////////////
// $Id: RawDigit.cxx,v 1.13 2010/03/26 19:36:42 brebel Exp $
//
// RawDigit class
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

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
  void RawDigit::SetPedestal(double ped)
  {

    fPedestal = ped;
    fSigma = 1.;

  }
}
////////////////////////////////////////////////////////////////////////

