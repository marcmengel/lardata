/// OpticalRawDigit.h
/// Kazuhiro Terao <kazuhiro@nevis.columbia.edu>
/// Alex Himmel <ahimmel@phy.duke.edu>
///
/// Equivalent of raw::RawDigit (waveform for TPC) for Optical system
/// except the code is a complete copy from FIFOChannel written by William Seligman.
/// William Seligman <seligman@nevis.columbia.edu>
///
/// Modified to be less uboone-specific
///

#ifndef optdata_OpticalRawDigit_h
#define optdata_OpticalRawDigit_h

// LArSoft includes
#include "OpticalDetectorData/ChannelData.h"

// C++ includes
#include <vector>
#include <functional> // so we can redefine less<> below
#include <limits>

namespace optdata {

  class OpticalRawDigit : public ChannelData
  {
  public:

    // Simple constructor/destructor.
    OpticalRawDigit ( TimeSlice_t time = 0,
		      Channel_t channel = std::numeric_limits<Channel_t>::max(),
		      size_type len = 0 ) 
      : ChannelData(channel,len)
      , fm_timeSlice(time)
    {};

    virtual ~OpticalRawDigit() {};

    // For compatibility with algorithms which assume there are frame numbers
    virtual Frame_t Frame() const { return 0; } 

#ifndef __GCCXML__

    // A time slice associated with the first bin in the channel
    // data. For example, the first bin of the ADC channel may refer
    // to clock value 8595824 (in some arbitrary units).
    TimeSlice_t TimeSlice() const { return fm_timeSlice; }
    void SetTimeSlice( TimeSlice_t t ) { fm_timeSlice = t; }
#endif
      

  private:
      TimeSlice_t fm_timeSlice;       // The time of the first slice in the channel data

  };

#ifndef __GCCXML__
  // In case we want to sort a collection of OpticalRawDigits (e.g.,
  // std::set<OpticalRawDigit>), here's the definition of the less-than
  // operator.
  bool operator<( const OpticalRawDigit& lhs, const OpticalRawDigit& rhs )
  {
    // Sort by channel, frame number, and time associated with the first bin.
    if ( lhs.ChannelNumber()   < rhs.ChannelNumber()  &&
	 lhs.TimeSlice()       < rhs.TimeSlice() )
      return true;
    return false;
  }
#endif

} // namespace optdata

#ifndef __GCCXML__
// For no extra charge, include how to sort OpticalRawDigit*, just in
// case we want (for example) a std::set<OpticalRawDigit*>.
namespace std {
  template <> 
  class less<optdata::OpticalRawDigit*>
  {
  public:
    bool operator()( const optdata::OpticalRawDigit* lhs, const optdata::OpticalRawDigit* rhs )
    {
      return (*lhs) < (*rhs);
    }
  };
} // std
#endif

#endif // optdata_OpticalRawDigit_h
