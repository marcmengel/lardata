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

  void Uncompress(const std::vector<short>& adc, 
                  std::vector<short>      &uncompressed, 
                  raw::Compress_t          compress);

  void Uncompress(const std::vector<short>& adc, 
                  std::vector<short>      &uncompressed, 
		  int       &pedestal,
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
	        int       &pedestal,
                int &nearestneighbor,
		bool fADCStickyCodeFeature=false);

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
		int       &pedestal,
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
		       int       &pedestal,
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
		       int       &pedestal,
                       int                &nearestneighbor,
		       bool fADCStickyCodeFeature=false);


  void ZeroUnsuppression(const std::vector<short>& adc, 
                         std::vector<short>      &uncompressed);

  void ZeroUnsuppression(const std::vector<short>& adc, 
                         std::vector<short>      &uncompressed,
			 int       &pedestal);

  const unsigned int onemask = 0x003f; // Unsigned int ending in 111111 used to select 6 LSBs with bitwise AND

  int ADCStickyCodeCheck(const short &adc_current_value, // Function to check if ADC value may be ADC sticky code in DUNE35t data
			 const int   &pedestal,
			 bool fADCStickyCodeFeature);

} // namespace raw

#endif // RAWDATA_RAW_H
