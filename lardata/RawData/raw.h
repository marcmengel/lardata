/// \file    raw.h
/// \brief   Collect all the RawData header files together
/// \author  brebel@fnal.gov
/// \modified by jti3@fnal.gov
/// \version $Id: raw.h,v 2.0 2013/01/16  jti3 Exp $

#ifndef RAWDATA_RAW_H
#define RAWDATA_RAW_H

#include <vector>
#include <boost/circular_buffer.hpp>
#include "larcore/SimpleTypesAndConstants/RawTypes.h"

namespace raw{

  /**
   * @brief Uncompresses a raw data buffer
   * @param adc compressed buffer
   * @param uncompressed buffer to be filled with uncompressed data
   * @param compress type of compression in the adc buffer
   *
   * This function dispatches the uncompression to the correct uncompress
   * function according to compression type in compress.
   *
   * The uncompressed buffer *must* be already allocated with enough space
   * to store the full inflated adc data. Uncompressing raw::RawDigit can
   * be done as follows:
   *     
   *     std::vector<ADC_t> uncompressed(digit.Samples(), 0);
   *     raw::Uncompress(digit.ADC(), uncompressed, digit.ADC());
   *     
   *
   */ 
  void Uncompress(const std::vector<short>& adc, 
                  std::vector<short>      &uncompressed, 
                  raw::Compress_t          compress);

  void Uncompress(const std::vector<short>& adc, 
                  std::vector<short>      &uncompressed, 
		  int       pedestal,
                  raw::Compress_t          compress);

  void Compress(std::vector<short> &adc, 
                raw::Compress_t     compress, 
                int                &nearestneighbor);
  void Compress(std::vector<short> &adc, 
                raw::Compress_t     compress, 
                unsigned int       &zerothreshold, 
                int &nearestneighbor);
  void Compress(std::vector<short> &adc, 
                raw::Compress_t     compress, 
                unsigned int       &zerothreshold, 
	        int       pedestal,
                int &nearestneighbor,
		bool fADCStickyCodeFeature=false);

  /**
   * @brief Compresses a raw data buffer
   * @param adc buffer with uncompressed data
   * @param compress type of compression to be applied
   *
   * This function dispatches the compression to the function appropriate
   * for the specified compression type.
   * The resulting compressed data replaces the input buffer content, which is lost.
   * Compression is expected to reduce the size of the data, so that there is
   * in principle no need for reallocation of the input buffer, adc, to store
   * the result.
   */ 
  void Compress(std::vector<short> &adc, 
                raw::Compress_t     compress);
  void Compress(std::vector<short> &adc, 
                raw::Compress_t     compress, 
                unsigned int       &zerothreshold);

  void Compress(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		std::vector<short> &adc, 
                raw::Compress_t     compress, 
                unsigned int       &zerothreshold, 
                int &nearestneighbor);

  void Compress(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		std::vector<short> &adc, 
                raw::Compress_t     compress, 
                unsigned int       &zerothreshold,
		int       pedestal,
                int &nearestneighbor,
		bool fADCStickyCodeFeature=false);

  void CompressHuffman(std::vector<short> &adc);

  void UncompressHuffman(const std::vector<short>& adc, 
                         std::vector<short>      &uncompressed);

  void ZeroSuppression(std::vector<short> &adc, 
                       unsigned int       &zerothreshold, 
                       int                &nearestneighbor);

  void ZeroSuppression(std::vector<short> &adc, 
                       unsigned int       &zerothreshold,
		       int       pedestal,
                       int                &nearestneighbor,
		       bool fADCStickyCodeFeature=false);


  void ZeroSuppression(std::vector<short> &adc, 
                       unsigned int       &zerothreshold);

  void ZeroSuppression(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		       std::vector<short> &adc,
                       unsigned int       &zerothreshold, 
                       int                &nearestneighbor);

  void ZeroSuppression(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		       std::vector<short> &adc,
                       unsigned int       &zerothreshold, 
		       int       pedestal,
                       int                &nearestneighbor,
		       bool fADCStickyCodeFeature=false);


  void ZeroUnsuppression(const std::vector<short>& adc, 
                         std::vector<short>      &uncompressed);

  void ZeroUnsuppression(const std::vector<short>& adc, 
                         std::vector<short>      &uncompressed,
			 int       pedestal);

  const unsigned int onemask = 0x003f; // Unsigned int ending in 111111 used to select 6 LSBs with bitwise AND

  int ADCStickyCodeCheck(const short adc_current_value, // Function to check if ADC value may be ADC sticky code in DUNE35t data
			 const int   pedestal,
			 bool fADCStickyCodeFeature);

} // namespace raw

#endif // RAWDATA_RAW_H
