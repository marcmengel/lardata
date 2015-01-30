/// FIFOChannel.h
/// William Seligman <seligman@nevis.columbia.edu>
///
/// The information associated with a given FEM channel as stored in its FIFO memory.

#ifndef optdata_FIFOChannel_h
#define optdata_FIFOChannel_h

// LArSoft includes
#include "OpticalDetectorData/OpticalTypes.h"
#include "OpticalDetectorData/OpticalRawDigit.h"

// C++ includes
#include <vector>
#include <functional> // so we can redefine less<> below
#include <limits>

namespace optdata {

  class FIFOChannel : public OpticalRawDigit
  {
  public:

    // Simple constructor/destructor.
    FIFOChannel ( Optical_Category_t category = kUndefined, 
		  TimeSlice_t time = 0,
		  Frame_t frame = 0,
		  Channel_t channel = std::numeric_limits<Channel_t>::max(),
		  size_type len = 0 ) 
        : OpticalRawDigit(time, channel, len)
        , fm_category(category)
        , fm_frame(frame)
    {};

    virtual ~FIFOChannel() {};

    // Here we have getters and setters for the time information.

#ifndef __GCCXML__

    Optical_Category_t Category() const { return fm_category; }

    // The frame number associated with the first frame in the channel.
    virtual Frame_t Frame() const { return fm_frame; }
    void SetFrame( Frame_t f ) { fm_frame = f; }

#endif

  private:
    Optical_Category_t fm_category; // A channel category from Types.h
    Frame_t fm_frame;               // The frame number corresponding to the above time
  };

#ifndef __GCCXML__
  // In case we want to sort a collection of FIFOChannels (e.g.,
  // std::set<FIFOChannel>), here's the definition of the less-than
  // operator.
  bool operator<( const FIFOChannel& lhs, const FIFOChannel& rhs )
  {
    // Sort by channel, frame number, and time associated with the first bin.
    if ( lhs.ChannelNumber()   < rhs.ChannelNumber()  &&
	 lhs.Frame()           < rhs.Frame()          &&
	 lhs.TimeSlice()       < rhs.TimeSlice() )
      return true;
    return false;
  }
#endif

} // namespace optdata

#ifndef __GCCXML__
// For no extra charge, include how to sort FIFOChannel*, just in
// case we want (for example) a std::set<FIFOChannel*>.
namespace std {
  template <> 
  class less<optdata::FIFOChannel*>
  {
  public:
    bool operator()( const optdata::FIFOChannel* lhs, const optdata::FIFOChannel* rhs )
    {
      return (*lhs) < (*rhs);
    }
  };
} // std
#endif

#endif // optdata_FIFOChannel_h
