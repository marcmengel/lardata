////////////////////////////////////////////////////////////////////////
// $Id: OpDetPulse.h,v 1.15 2010/03/26 20:06:04 brebel Exp $
//
// Waveform for one PMT
//
// Ben Jones and Christie Chiu, June 2012
// bjpjones@mit.edu, cschiu@mit.edu
//
////////////////////////////////////////////////////////////////////////

#ifndef RAWDATA_OPDETPULSE_H
#define RAWDATA_OPDETPULSE_H

#include <vector>
#include <iosfwd>

///Raw data description
namespace raw {
  
  class OpDetPulse {

  public:
    OpDetPulse(); // Default constructor

  private:

    unsigned short  fOpChannel;     ///< channel in the readout
    unsigned short  fSamples;          ///< number of ticks of the clock
    unsigned int    fPMTFrame;         ///< frame number where pulse begins
    unsigned int    fFirstSample;      ///< first sample number within pmt frame

    std::vector<short> fWaveform; 
    
#ifndef __GCCXML__

  public:

    OpDetPulse(unsigned short     OpChannel, 
	       std::vector<short> Waveform,
	       unsigned int       PMTFrame,
	       unsigned int       FirstSample);

    OpDetPulse(unsigned short OpChannel);

    int Integral(unsigned short startbin=0, 
		 unsigned short endbin=0);

    std::vector<short>&  Waveform()    ;
    unsigned short       OpChannel()          const;
    unsigned short       Samples()            const;
    unsigned int         PMTFrame()           const;
    unsigned int         FirstSample()        const;
    
    void SetPMTFrame(    unsigned int Frame       );
    void SetFirstSample( unsigned int FirstSample );

 #endif   
  };
}

#ifndef __GCCXML__
inline std::vector<short>&  raw::OpDetPulse::Waveform()               { return fWaveform;        }
inline unsigned short       raw::OpDetPulse::OpChannel() const     { return fOpChannel;    }
inline unsigned short       raw::OpDetPulse::Samples()      const     { return fWaveform.size(); }
inline unsigned int         raw::OpDetPulse::PMTFrame()     const     { return fPMTFrame; }
inline unsigned int         raw::OpDetPulse::FirstSample()  const     { return fFirstSample; }
inline void                 raw::OpDetPulse::SetPMTFrame(unsigned int Frame)   { fPMTFrame=Frame; }
inline void                 raw::OpDetPulse::SetFirstSample(unsigned int FirstSample)   { fFirstSample = FirstSample; }


#endif

#endif // RAWDATA_OPDETPULSE_H

////////////////////////////////////////////////////////////////////////
