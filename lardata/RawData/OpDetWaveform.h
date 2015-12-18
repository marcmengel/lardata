/**
 * raw/OpDetWaveform.h
 *
 * Raw signals from the photon detectors.
 * Waveform (adcs in time bins), a channel number, and a time stamp.
 *
 */


#ifndef OpDetWaveform_h
#define OpDetWaveform_h

#include <vector>
#include <functional> // so we can redefine less<> below
#include <limits>
#include <iosfwd>


namespace raw {

    // Define the types used
    typedef short              ADC_Count_t;
    typedef unsigned int       Channel_t;
    typedef double             TimeStamp_t;  ///< us since 1970, based on TimeService

    class OpDetWaveform  : public std::vector< ADC_Count_t >
    {
      private:
        Channel_t   fChannel;
        TimeStamp_t fTimeStamp;


      public:
        // Simple constructors/destructors. 
        // Just in case the user forgets to supply the default channel, use
        // a garbage value to indicate that there's a problem.
        // To save on memory reallocations, offer an option to specify the
        // the initial memory allocation of the channel vector.
        OpDetWaveform( TimeStamp_t time = std::numeric_limits<TimeStamp_t>::max(),
                       Channel_t   chan = std::numeric_limits<Channel_t>::max(),
                       size_type   len  = 0 )
            : fChannel(chan)
            , fTimeStamp(time)
        {
            this->reserve(len);
        };

        
        OpDetWaveform( TimeStamp_t time,
                       Channel_t   chan,
                       std::vector< uint16_t > rhs )
            : fChannel(chan)
            , fTimeStamp(time)
        {
            this->reserve(rhs.size());
            for (unsigned int i =0; i < rhs.size(); i++)
                this->push_back(rhs[i]);
        };

        
        ~OpDetWaveform() {};

        // Functions included for backwards compatability with previous data types
        std::vector<ADC_Count_t>& Waveform()         { return *this;  }

#ifndef __GCCXML__

        static_assert(sizeof(unsigned long long)==8,"unsigned long long is not 8 bytes");
        
        // The sections bracketed with GCCXML tests handle a problem ART
        // with generating its data dictionaries.

        Channel_t   ChannelNumber() const            { return fChannel; }
        TimeStamp_t TimeStamp() const                { return fTimeStamp; }
        void        SetChannelNumber(Channel_t chan) { fChannel = chan; }
        void        SetTimeStamp(TimeStamp_t time)   { fTimeStamp = time; }
#endif
    
    };
}



#ifndef __GCCXML__

namespace raw {
  bool operator<( const OpDetWaveform& lhs, const OpDetWaveform& rhs )
  {
      // Sort by channel, then time
      if ( lhs.ChannelNumber()  < rhs.ChannelNumber() ) return true;
      if ( rhs.ChannelNumber()  > rhs.ChannelNumber() ) return false;

      return ( lhs.TimeStamp() < rhs.TimeStamp() );
  }
} // namespace raw


// For no extra charge, include how to sort ChannelData*, just in
// case we want (for example) a std::set<ChannelData*>.
namespace std {
  template <> 
  class less<raw::OpDetWaveform*>
  {
  public:
    bool operator()( const raw::OpDetWaveform* lhs, const raw::OpDetWaveform* rhs )
    {
      return (*lhs) < (*rhs);
    }
  };
}
#endif


#endif
