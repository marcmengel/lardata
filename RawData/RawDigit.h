/** ****************************************************************************
 * @file RawDigit.h
 * @brief Definition of basic raw digits
 * @author brebel@fnal.gov
 * @see  RawDigit.cxx raw.h
 * 
 * Compression/uncompression utilities are declared in lardata/RawData/raw.h .
 * 
 * Changes:
 * 20141210 Gianluca Petrillo (petrillo@fnal.gov)
 *   data architecture revision changes:
 *   - fADC made private
 *   - removed constructor not assigning sample number
 *   - added flags interface
 * 
 * 2/19/2008 Mitch Soderberg
 * -modified RawDigit class slightly to be compatible with binary output of DAQ software,
 *  and to include #samples/channel explicity, instead of via sizeof() methods. 
 * 
 * ****************************************************************************/

#ifndef RAWDATA_RAWDIGIT_H
#define RAWDATA_RAWDIGIT_H

// C/C++ standard libraries
#include <stdint.h> // uint32_t
#include <cstdlib> // size_t
#include <vector>
// #include <bitset>

// LArSoft libraries
#include "SimpleTypesAndConstants/RawTypes.h" // raw::Compress_t, raw::Channel_t


/// Raw data description and utilities
namespace raw {
  
  /**
   * @brief Collection of charge vs time digitized from a single readout channel
   *
   * This class hosts potentially compressed data.
   * It does not provide methods to uncompress it, not the same object can
   * become compressed/uncompressed or change compression type: to use a
   * compressed RawDigit, one has to create a new buffer, fill and use it:
   *     
   *     raw::RawDigit::ADCvector_t ADCs(digits.Samples()); // fix the size!
   *     raw::Uncompress(digits.ADCs(), ADCs, digits.Compression());
   *     
   * (remember that you have to provide raw::Uncompress() with a buffer large
   * enough to contain the uncompressed data).
   * 
   * The class provides some flags, defined in FlagIndices_t.
   * The construction of a RawDigit should be for example in the form:
   *     
   *     raw::RawDigit::ADCvector_t ADCs;
   *     // ... fill the digits etc.
   *     raw::RawDigit saturatedDigit(
   *       channel, ADCs.size(), ADCs, raw::kNone,
   *       DefaultFlags | SaturationBit
   *       );
   *     raw::RawDigit unsaturatedDigit(
   *       channel, ADCs.size(), ADCs, raw::kNone,
   *       DefaultFlags & ~SaturationBit
   *       );
   *     
   * 
   */
  class RawDigit {

  public:
    /// Type representing a (compressed) vector of ADC counts
    typedef std::vector<short> ADCvector_t;
    
/*
// removed waiting for a real use for flags
    /// Type for the digit flags
    typedef std::bitset<16> Flags_t;
*/
    
    /// Default constructor: an empty raw digit
    RawDigit();
    
#ifndef __GCCXML__
  public:
    
  /*
    // removed waiting for a real use for flags
    typedef enum {
      fiSaturation,       ///< saturation flag: set if saturated at any time
      NLArSoftFlags = 8,  ///< LArSoft reserves flags up to here (this excluded)
      NFlagIndices = NLArSoftFlags ///< number of flags
    } FlagIndices_t; ///< type of index of flags
  */
    
    /**
     * @brief Constructor: sets all the fields
     * @param channel ID of the channel the digits were acquired from
     * @param samples number of ADC samples in the uncompressed collection
     * @param adclist list of ADC counts vs. time, compressed
     * @param compression compression algorithm used in adclist
     * 
     * Data from the adclist is copied into the raw digits.
     * Pedestal is set to 0 by default.
     */
    RawDigit(ChannelID_t        channel,
             unsigned short     samples,
             ADCvector_t const& adclist,
             raw::Compress_t    compression = raw::kNone /*,
             const Flags_t&     flags = DefaultFlags */
             );
    
    /**
     * @brief Constructor: sets all the fields
     * @param channel ID of the channel the digits were acquired from
     * @param samples number of ADC samples in the uncompressed collection
     * @param adclist list of ADC counts vs. time, compressed
     * @param compression compression algorithm used in adclist
     * 
     * Data from the adclist is moved into the raw digits.
     * Pedestal is set to 0 by default.
     */
    RawDigit(ChannelID_t        channel,
             unsigned short     samples,
             ADCvector_t&&      adclist,
             raw::Compress_t    compression = raw::kNone /*,
             const Flags_t&     flags = DefaultFlags */
             );
    
    /// Set pedestal and its RMS (the latter is 0 by default)
    void            SetPedestal(float ped, float sigma = 1.);
    
    ///@{
    ///@name Accessors
    
    /// Reference to the compressed ADC count vector
    const ADCvector_t& ADCs()     const;
    
    /// Number of elements in the compressed ADC sample vector
    size_t          NADC()        const;
    
    /// ADC vector element number i; no decompression is applied
    short           ADC(int i)    const;
    
    /// DAQ channel this raw data was read from
    ChannelID_t     Channel()     const;
    
    /// Number of samples in the uncompressed ADC data
    unsigned short  Samples()     const;
    
    /// Pedestal level (ADC counts)
    /// @deprecated Might be removed soon
    float           GetPedestal() const;
    
    /// TODO RMS of the pedestal level?
    float           GetSigma()    const;
    
    /// Compression algorithm used to store the ADC counts
    raw::Compress_t Compression() const;
    ///@}
    
  /*
    // removed waiting for a real use for flags
    ///@{
    ///@name Raw digit flag interface
    /// Flag set
    const Flags_t&  Flags() const;
    
    /// Returns if saturation bit is set
    bool            isSaturated() const;
    
    // the unsigned long long thing is from std::bitset<> constructors
    /// Bit for saturation
    static const unsigned long long SaturationBit = 1ULL << fiSaturation;
    
    /// Flags for a digit constructred by default
    static const unsigned long long DefaultFlags  = 0ULL;
    
    ///@}
  */
    
    
#endif // !__GCCXML__
  private:
    std::vector<short> fADC;      ///< ADC readout per tick, before pedestal subtraction
    
    ChannelID_t     fChannel;     ///< channel number in the readout
    unsigned short  fSamples;     ///< number of ticks of the clock
    
    float           fPedestal;    ///< pedestal for this channel
    float           fSigma;       ///< sigma of the pedestal counts for this channel
    
    Compress_t      fCompression; ///< compression scheme used for the ADC vector
    

    // removed waiting for a real use for flags
    // Flags_t         fFlags;       ///< set of digit flags
    
  }; // class RawDigit
  
  
} // namespace raw


//------------------------------------------------------------------------------
//--- inline implementation
//---
#ifndef __GCCXML__

inline size_t          raw::RawDigit::NADC()        const { return fADC.size();  }
inline short           raw::RawDigit::ADC(int i)    const { return fADC.at(i);   }
inline const raw::RawDigit::ADCvector_t&
                       raw::RawDigit::ADCs()        const { return fADC;         }
inline raw::ChannelID_t
                       raw::RawDigit::Channel()     const { return fChannel;     }
inline unsigned short  raw::RawDigit::Samples()     const { return fSamples;     }
inline float           raw::RawDigit::GetPedestal() const { return fPedestal;    }
inline float           raw::RawDigit::GetSigma()    const { return fSigma;       }
inline raw::Compress_t raw::RawDigit::Compression() const { return fCompression; }
/*
// removed waiting for a real use for flags
inline const raw::RawDigit::Flags_t&
                       raw::RawDigit::Flags()       const { return fFlags;       }
inline bool            raw::RawDigit::isSaturated() const { return fFlags.test(fiSaturation); }
*/

#endif // !__GCCXML__

#endif // RAWDATA_RAWDIGIT_H

////////////////////////////////////////////////////////////////////////
