////////////////////////////////////////////////////////////////////////
// $Id: AuxDetDigit.h,v 1.15 2010/03/26 20:06:04 brebel Exp $
//
// Definition of basic digits for auxiliary detectors
//
// brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#ifndef RAWDATA_AUXDETDIGIT_H
#define RAWDATA_AUXDETDIGIT_H

#include <vector>
#include <iosfwd>

#include "SimpleTypesAndConstants/RawTypes.h"

///Raw data description
namespace raw {
  
  class AuxDetDigit {

  public:
    AuxDetDigit(); // Default constructor
    

  private:

    std::vector<short> fADC;        ///< vector of adc counts
    unsigned short     fChannel;    ///< channel in the readout
    raw::AuxDetType_t  fAuxDetType; ///< type of detector 
    
#ifndef __GCCXML__
  public:
    
    AuxDetDigit(unsigned short channel,
		std::vector<short> adclist,
		raw::AuxDetType_t type);
    
    
    // Get Methods
    size_t            NADC()        const;
    short             ADC(size_t i) const;
    unsigned short    Channel()     const;
    raw::AuxDetType_t Type()        const;

#endif
  };
}

#ifndef __GCCXML__

inline size_t            raw::AuxDetDigit::NADC()    const { return fADC.size(); }
inline unsigned short    raw::AuxDetDigit::Channel() const { return fChannel;    }
inline raw::AuxDetType_t raw::AuxDetDigit::Type()    const { return fAuxDetType; }

#endif

#endif // RAWDATA_AUXDETDIGIT_H

////////////////////////////////////////////////////////////////////////
