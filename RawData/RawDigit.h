////////////////////////////////////////////////////////////////////////
// $Id: RawDigit.h,v 1.15 2010/03/26 20:06:04 brebel Exp $
//
// Definition of basic raw digits
//
// brebel@fnal.gov
//
// -modified RawDigit class slightly to be compatible with binary output of DAQ software,
//  and to include #samples/channel explicity, instead of via sizeof() methods. 
//  Mitch Soderberg  2/19/2008
//
////////////////////////////////////////////////////////////////////////

#ifndef RAWDATA_RAWDIGIT_H
#define RAWDATA_RAWDIGIT_H

#include <vector>
#include <iosfwd>
#include <stdint.h>

#include "SimpleTypesAndConstants/RawTypes.h"

///Raw data description
namespace raw {
  
  class RawDigit {

  public:
    RawDigit(); // Default constructor
    
    std::vector<short> fADC;

  private:

    uint32_t        fChannel;     ///< channel in the readout
    unsigned short  fSamples;     ///< number of ticks of the clock
    
    double          fPedestal;    ///< pedestal for this channel
    double          fSigma;       ///< sigma of the pedestal counts for this channel

    raw::Compress_t fCompression; ///< compression scheme used for the ADC vector
    
#ifndef __GCCXML__
  public:
    
    RawDigit(uint32_t           channel, 
	     unsigned short     samples,
	     std::vector<short> adclist,
	     raw::Compress_t    compression=raw::kNone);
    RawDigit(uint32_t           channel,
	     std::vector<short> adclist,
	     raw::Compress_t    compression=raw::kNone);
        
    // Set Methods
    void             SetPedestal(double ped);
    
    // Get Methods
    unsigned int    NADC()        const;
    short           ADC(int i)    const;
    uint32_t        Channel()     const;
    unsigned short  Samples()     const;
    double          GetPedestal() const; 
    double          GetSigma()    const; 
    raw::Compress_t Compression() const;

#endif
  };
}

#ifndef __GCCXML__

inline unsigned int    raw::RawDigit::NADC()        const { return fADC.size();  }
inline uint32_t        raw::RawDigit::Channel()     const { return fChannel;     }
inline unsigned short  raw::RawDigit::Samples()     const { return fSamples;     }
inline double          raw::RawDigit::GetPedestal() const { return fPedestal;    } 
inline double          raw::RawDigit::GetSigma()    const { return fSigma;       } 
inline raw::Compress_t raw::RawDigit::Compression() const { return fCompression; }

#endif

#endif // RAWDATA_RAWDIGIT_H

////////////////////////////////////////////////////////////////////////
