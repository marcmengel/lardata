/// \file    raw.h
/// \brief   Collect all the RawData header files together
/// \author  brebel@fnal.gov
/// \modified by jti3@fnal.gov
/// \version $Id: raw.h,v 2.0 2013/01/16  jti3 Exp $
#ifndef RAWDATA_RAW_H
#define RAWDATA_RAW_H

#include <vector>

#include "SimpleTypesAndConstants/RawTypes.h"
#include "RawData/RawDigit.h"

namespace raw{

  void Uncompress(const std::vector<short>& adc, 
		  std::vector<short>      &uncompressed, 
		  raw::Compress_t          compress);
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		int                &nearestneighbor);
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold, 
		int &nearestneighbor);
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress);
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold);
  void CompressHuffman(std::vector<short> &adc);
  void UncompressHuffman(const std::vector<short>& adc, 
			 std::vector<short>      &uncompressed);
  void ZeroSuppression(std::vector<short> &adc, 
		       unsigned int       &zerothreshold, 
		       int                &nearestneighbor);
  void ZeroSuppression(std::vector<short> &adc, 
		       unsigned int       &zerothreshold);
  void ZeroUnsuppression(const std::vector<short>& adc, 
			 std::vector<short>      &uncompressed);
}

#endif
