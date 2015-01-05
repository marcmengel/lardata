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
#include <string>

///Raw data description
namespace raw {
  
  class AuxDetDigit {

  public:
    AuxDetDigit(); // Default constructor
    
  private:

    std::vector<short> fADC;        ///< vector of adc counts
    unsigned short     fChannel;    ///< channel in the readout
    std::string        fAuxDetName; ///< name of the detector
    
#ifndef __GCCXML__
  public:
    
    AuxDetDigit(unsigned short channel,
		std::vector<short> adclist,
		std::string name="UknownAuxDet");
    
    
    // Get Methods
    size_t             NADC()        const;
    short              ADC(size_t i) const;
    unsigned short     Channel()     const;
    std::string const& AuxDetName()  const;

#endif
  };
}

#ifndef __GCCXML__

inline size_t             raw::AuxDetDigit::NADC()       const { return fADC.size(); }
inline unsigned short     raw::AuxDetDigit::Channel()    const { return fChannel;    }
inline std::string const& raw::AuxDetDigit::AuxDetName() const { return fAuxDetName; }

#endif

#endif // RAWDATA_AUXDETDIGIT_H

////////////////////////////////////////////////////////////////////////
