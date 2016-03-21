/// \file    raw.cxx
/// \brief   raw data utilities
/// \author  brebel@fnal.gov
/// \modified by jti3@fnal.gov
/// \version $Id: raw.cxx,v 2.0 2013/01/16  jti3 Exp $

#include "RawData/raw.h"

#include <iostream>
#include <string>
#include <bitset>

#include "cetlib/exception.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

namespace raw {


  //----------------------------------------------------------
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress)
  {
    if(compress == raw::kHuffman) CompressHuffman(adc);
    else if(compress == raw::kZeroHuffman){
      unsigned int zerothreshold = 5;
      ZeroSuppression(adc,zerothreshold);
      CompressHuffman(adc);
    }
    else if(compress == raw::kZeroSuppression){ 
      unsigned int zerothreshold = 5;
      ZeroSuppression(adc,zerothreshold);
    }
    

    return;
  }
  //----------------------------------------------------------
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		int                &nearestneighbor)
  {
    if(compress == raw::kHuffman) CompressHuffman(adc);
    else if(compress == raw::kZeroHuffman){
      unsigned int zerothreshold = 5;
      ZeroSuppression(adc,zerothreshold, nearestneighbor);
      CompressHuffman(adc);
    }
    else if(compress == raw::kZeroSuppression){ 
      unsigned int zerothreshold = 5;
      ZeroSuppression(adc,zerothreshold, nearestneighbor);
    }
    

    return;
  }

  //----------------------------------------------------------
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold)
  {
    if(compress == raw::kHuffman) CompressHuffman(adc);

    else if(compress == raw::kZeroSuppression) ZeroSuppression(adc,zerothreshold);
    else if(compress == raw::kZeroHuffman){
      ZeroSuppression(adc,zerothreshold);
      CompressHuffman(adc);
    }    

    return;
  }
  //----------------------------------------------------------
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold, 
		int                &nearestneighbor)
  { 
    if(compress == raw::kHuffman) 
      CompressHuffman(adc);
    else if(compress == raw::kZeroSuppression) 
      ZeroSuppression(adc,zerothreshold, nearestneighbor);
    else if(compress == raw::kZeroHuffman){
      ZeroSuppression(adc,zerothreshold, nearestneighbor);
      CompressHuffman(adc);
    }    

    return;
  }

  //----------------------------------------------------------
  void Compress(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		std::vector<short> &adc,
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold, 
		int                &nearestneighbor)
  { 
    if(compress == raw::kHuffman) 
      CompressHuffman(adc);
    else if(compress == raw::kZeroSuppression) 
      ZeroSuppression(adcvec_neighbors,adc,zerothreshold, nearestneighbor);
    else if(compress == raw::kZeroHuffman){
      ZeroSuppression(adcvec_neighbors,adc,zerothreshold, nearestneighbor);
      CompressHuffman(adc);
    }    

    return;
  }

  //----------------------------------------------------------
  void Compress(std::vector<short> &adc, 
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold, 
		int               pedestal,
		int                &nearestneighbor,
		bool              fADCStickyCodeFeature)
  { 
    if(compress == raw::kHuffman) 
      CompressHuffman(adc);
    else if(compress == raw::kZeroSuppression) 
      ZeroSuppression(adc,zerothreshold, pedestal, nearestneighbor, fADCStickyCodeFeature);
    else if(compress == raw::kZeroHuffman){
      ZeroSuppression(adc,zerothreshold, pedestal, nearestneighbor, fADCStickyCodeFeature);
      CompressHuffman(adc);
    }    

    return;
  }

  //----------------------------------------------------------
  void Compress(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		std::vector<short> &adc,
		raw::Compress_t     compress, 
		unsigned int       &zerothreshold, 
		int               pedestal,
		int                &nearestneighbor,
		bool              fADCStickyCodeFeature)
  { 
    if(compress == raw::kHuffman) 
      CompressHuffman(adc);
    else if(compress == raw::kZeroSuppression) 
      ZeroSuppression(adcvec_neighbors,adc,zerothreshold, pedestal, nearestneighbor, fADCStickyCodeFeature);
    else if(compress == raw::kZeroHuffman){
      ZeroSuppression(adcvec_neighbors,adc,zerothreshold, pedestal, nearestneighbor, fADCStickyCodeFeature);
      CompressHuffman(adc);
    }    

    return;
  }


  //----------------------------------------------------------
  // Zero suppression function
  void ZeroSuppression(std::vector<short> &adc, 
		       unsigned int       &zerothreshold)
  {
    const int adcsize = adc.size();
    const int zerothresholdsigned = zerothreshold;
    
    std::vector<short> zerosuppressed(adc.size());
    int maxblocks = adcsize/2 + 1;
    std::vector<short> blockbegin(maxblocks);
    std::vector<short> blocksize(maxblocks);

    unsigned int nblocks = 0;
    unsigned int zerosuppressedsize = 0;
 
    int blockcheck = 0;

    for(int i = 0; i < adcsize; ++i){
      int adc_current_value = std::abs(adc[i]);
    
      if(adc_current_value > zerothresholdsigned){

	if(blockcheck == 0){
	  
	  blockbegin[nblocks] = i;
	  blocksize[nblocks] = 0;
	  blockcheck=1;
	}
	
	zerosuppressed[zerosuppressedsize] = adc[i];
	zerosuppressedsize++;
	blocksize[nblocks]++;
		
	if(i == adcsize-1) nblocks++;
      }
      
      if(adc_current_value <= zerothresholdsigned && blockcheck == 1){
	  zerosuppressed[zerosuppressedsize] = adc[i];
	  zerosuppressedsize++;
	  blocksize[nblocks]++;
	  nblocks++;  
	  blockcheck = 0;
      } 
    }



    adc.resize(2+nblocks+nblocks+zerosuppressedsize);

    adc[0] = adcsize; //fill first entry in adc with length of uncompressed vector
    adc[1] = nblocks;


    
    for(unsigned int i = 0; i < nblocks; ++i)
      adc[i+2] = blockbegin[i];

    for(unsigned int i = 0; i < nblocks; ++i)
      adc[i+nblocks+2] = blocksize[i];

    for(unsigned int i = 0; i < zerosuppressedsize; ++i)
      adc[i+nblocks+nblocks+2] = zerosuppressed[i];
 

  }



  //----------------------------------------------------------
  // Zero suppression function which merges blocks if they are 
  // within parameter nearestneighbor of each other
  void ZeroSuppression(std::vector<short> &adc, 
		       unsigned int       &zerothreshold, 
		       int                &nearestneighbor)
  {
   
    const int adcsize = adc.size();
    const int zerothresholdsigned = zerothreshold;
    
    std::vector<short> zerosuppressed(adcsize);
    int maxblocks = adcsize/2 + 1;
    std::vector<short> blockbegin(maxblocks);
    std::vector<short> blocksize(maxblocks);
    
    int nblocks = 0;
    int zerosuppressedsize = 0;
 
    int blockstartcheck = 0;
    int endofblockcheck = 0;

    for(int i = 0; i < adcsize; ++i){
      int adc_current_value = std::abs(adc[i]);

      if(blockstartcheck==0){
	if(adc_current_value>zerothresholdsigned){
	  if(nblocks>0){
	    if((i-nearestneighbor)<=(blockbegin[nblocks-1]+blocksize[nblocks-1]+1)){
	  
	      nblocks--;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	    else{
	      blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	  }	
	  else{
	    blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	    blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	    blockstartcheck = 1;	    
	  }
	}
      }
      else if(blockstartcheck==1){
	if(adc_current_value>zerothresholdsigned){
	  blocksize[nblocks]++;
	  endofblockcheck = 0;
	}
	else{
	  if(endofblockcheck<nearestneighbor){
	    endofblockcheck++;
	    blocksize[nblocks]++;
	  }
	  //block has ended
	  else if(i+2<adcsize){ //check if end of adc vector is near
	    if(std::abs(adc[i+1]) <= zerothresholdsigned && std::abs(adc[i+2]) <= zerothresholdsigned){  
	      endofblockcheck = 0;
	      blockstartcheck = 0;
	      nblocks++;	    
	    }
	  }
	  
	  
	} // end else
      } // end if blockstartcheck == 1
    }// end loop over adc size

    if(blockstartcheck==1){ // we reached the end of the adc vector with the block still going
      ++nblocks;
    }

    for(int i = 0; i < nblocks; ++i)
      zerosuppressedsize += blocksize[i];
    
    
    adc.resize(2+nblocks+nblocks+zerosuppressedsize);
    zerosuppressed.resize(2+nblocks+nblocks+zerosuppressedsize);
    
    
    int zerosuppressedcount = 0;
    for(int i = 0; i < nblocks; ++i){
      //zerosuppressedsize += blocksize[i];
      for(int j = 0; j < blocksize[i]; ++j){
	zerosuppressed[zerosuppressedcount] = adc[blockbegin[i] + j];
	zerosuppressedcount++;
      }
    }
    
    adc[0] = adcsize; //fill first entry in adc with length of uncompressed vector
    adc[1] = nblocks;
    for(int i = 0; i < nblocks; ++i){
      adc[i+2] = blockbegin[i];
      adc[i+nblocks+2] = blocksize[i];
    }
    
    
    
    for(int i = 0; i < zerosuppressedsize; ++i)
      adc[i+nblocks+nblocks+2] = zerosuppressed[i];
    
    
    // for(int i = 0; i < 2 + 2*nblocks + zerosuppressedsize; ++i)
    //   std::cout << adc[i] << std::endl;
    //adc.resize(2+nblocks+nblocks+zerosuppressedsize);
  }

  //----------------------------------------------------------
  // Zero suppression function which merges blocks if they are 
  // within parameter nearestneighbor of each other
  // after subtracting pedestal value
  void ZeroSuppression(std::vector<short> &adc, 
		       unsigned int       &zerothreshold, 
		       int               pedestal,
		       int                &nearestneighbor,
		       bool              fADCStickyCodeFeature)
  {
   
    const int adcsize = adc.size();
    const int zerothresholdsigned = zerothreshold;
    
    std::vector<short> zerosuppressed(adcsize);
    int maxblocks = adcsize/2 + 1;
    std::vector<short> blockbegin(maxblocks);
    std::vector<short> blocksize(maxblocks);
    
    int nblocks = 0;
    int zerosuppressedsize = 0;
 
    int blockstartcheck = 0;
    int endofblockcheck = 0;

    for(int i = 0; i < adcsize; ++i){
      int adc_current_value = ADCStickyCodeCheck(adc[i],pedestal,fADCStickyCodeFeature);

      if(blockstartcheck==0){
	if(adc_current_value>zerothresholdsigned){
	  if(nblocks>0){
	    if(i-nearestneighbor<=blockbegin[nblocks-1]+blocksize[nblocks-1]+1){
	      nblocks--;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	    else{
	      blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	  }	
	  else{
	    blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	    blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	    blockstartcheck = 1;	    
	  }
	}
      }
      else if(blockstartcheck==1){
	if(adc_current_value>zerothresholdsigned){
	  blocksize[nblocks]++;
	  endofblockcheck = 0;
	}
	else{
	  if(endofblockcheck<nearestneighbor){
	    endofblockcheck++;
	    blocksize[nblocks]++;
	  }
	  //block has ended
	  else if(i+2<adcsize){ //check if end of adc vector is near
	    if(ADCStickyCodeCheck(adc[i+1],pedestal,fADCStickyCodeFeature) <= zerothresholdsigned && ADCStickyCodeCheck(adc[i+2],pedestal,fADCStickyCodeFeature) <= zerothresholdsigned){  
	      endofblockcheck = 0;
	      blockstartcheck = 0;
	      nblocks++;	    
	    }
	  }	  
	} // end else
      } // end if blockstartcheck == 1
    }// end loop over adc size
    
    if(blockstartcheck==1){ // we reached the end of the adc vector with the block still going
      ++nblocks;
    }


    for(int i = 0; i < nblocks; ++i)
      zerosuppressedsize += blocksize[i];
    
    
    adc.resize(2+nblocks+nblocks+zerosuppressedsize);
    zerosuppressed.resize(2+nblocks+nblocks+zerosuppressedsize);
    
    
    int zerosuppressedcount = 0;
    for(int i = 0; i < nblocks; ++i){
      //zerosuppressedsize += blocksize[i];
      for(int j = 0; j < blocksize[i]; ++j){
	zerosuppressed[zerosuppressedcount] = adc[blockbegin[i] + j];
	zerosuppressedcount++;
      }
    }
    
    adc[0] = adcsize; //fill first entry in adc with length of uncompressed vector
    adc[1] = nblocks;
    for(int i = 0; i < nblocks; ++i){
      adc[i+2] = blockbegin[i];
      adc[i+nblocks+2] = blocksize[i];
    }
    
    
    
    for(int i = 0; i < zerosuppressedsize; ++i)
      adc[i+nblocks+nblocks+2] = zerosuppressed[i];
    
    
    // for(int i = 0; i < 2 + 2*nblocks + zerosuppressedsize; ++i)
    //   std::cout << adc[i] << std::endl;
    //adc.resize(2+nblocks+nblocks+zerosuppressedsize);
  }

  //----------------------------------------------------------
  // Zero suppression function which merges blocks if they are 
  // within parameter nearest neighbor of each other and makes
  // blocks if neighboring wires have nonzero blocks there
  void ZeroSuppression(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		       std::vector<short> &adc, 
		       unsigned int       &zerothreshold, 
		       int                &nearestneighbor)
  {
   
    const int adcsize = adc.size();
    const int zerothresholdsigned = zerothreshold;
    
    std::vector<short> zerosuppressed(adcsize);
    const int maxblocks = adcsize/2 + 1;
    std::vector<short> blockbegin(maxblocks);
    std::vector<short> blocksize(maxblocks);
    
    int nblocks = 0;
    int zerosuppressedsize = 0;
 
    int blockstartcheck = 0;
    int endofblockcheck = 0;

    for(int i = 0; i < adcsize; ++i){

      //find maximum adc value among all neighboring channels within the nearest neighbor channel distance

      int adc_current_value = 0;
   
      for(boost::circular_buffer<std::vector<short>>::const_iterator adcveciter = adcvec_neighbors.begin(); adcveciter!=adcvec_neighbors.end();++adcveciter){
	const std::vector<short> &adcvec_current = *adcveciter;
	const int adcvec_current_single = std::abs(adcvec_current[i]);
      
	if(adc_current_value < adcvec_current_single){
	  adc_current_value = adcvec_current_single;
	}
      
      }
      if(blockstartcheck==0){
	if(adc_current_value>zerothresholdsigned){
	  if(nblocks>0){
	    if(i-nearestneighbor<=blockbegin[nblocks-1]+blocksize[nblocks-1]+1){
	      nblocks--;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	    else{
	      blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	  }	
	  else{
	    blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	    blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	    blockstartcheck = 1;	    
	  }
	}
      }
      else if(blockstartcheck==1){
	if(adc_current_value>zerothresholdsigned){
	  blocksize[nblocks]++;
	  endofblockcheck = 0;
	}
	else{
	  if(endofblockcheck<nearestneighbor){
	    endofblockcheck++;
	    blocksize[nblocks]++;
	  }
	  //block has ended
	  else  if(i+2<adcsize){ //check if end of adc vector is near
	    if(std::abs(adc[i+1]) <= zerothresholdsigned && std::abs(adc[i+2]) <= zerothresholdsigned){  
	      endofblockcheck = 0;
	      blockstartcheck = 0;
	      nblocks++;	    
	    }
	  }
	  
	} // end else
      } // end if blockstartcheck == 1
    }// end loop over adc size
    
    if(blockstartcheck==1){ // we reached the end of the adc vector with the block still going
      ++nblocks;
    }



    for(int i = 0; i < nblocks; ++i)
      zerosuppressedsize += blocksize[i];
    
    
    adc.resize(2+nblocks+nblocks+zerosuppressedsize);
    zerosuppressed.resize(2+nblocks+nblocks+zerosuppressedsize);
    
    
    int zerosuppressedcount = 0;
    for(int i = 0; i < nblocks; ++i){
      //zerosuppressedsize += blocksize[i];
      for(int j = 0; j < blocksize[i]; ++j){
	zerosuppressed[zerosuppressedcount] = adc[blockbegin[i] + j];
	zerosuppressedcount++;
      }
    }
    
    adc[0] = adcsize; //fill first entry in adc with length of uncompressed vector
    adc[1] = nblocks;
    for(int i = 0; i < nblocks; ++i){
      adc[i+2] = blockbegin[i];
      adc[i+nblocks+2] = blocksize[i];
    }
    
    
    
    for(int i = 0; i < zerosuppressedsize; ++i)
      adc[i+nblocks+nblocks+2] = zerosuppressed[i];
    
    
    // for(int i = 0; i < 2 + 2*nblocks + zerosuppressedsize; ++i)
    //   std::cout << adc[i] << std::endl;
    //adc.resize(2+nblocks+nblocks+zerosuppressedsize);
  }

  //----------------------------------------------------------
  // Zero suppression function which merges blocks if they are 
  // within parameter nearest neighbor of each other and makes
  // blocks if neighboring wires have nonzero blocks there
  // after subtracting pedestal values
  void ZeroSuppression(const boost::circular_buffer<std::vector<short>> &adcvec_neighbors,
		       std::vector<short> &adc, 
		       unsigned int       &zerothreshold, 
		       int               pedestal,
		       int                &nearestneighbor,
		       bool              fADCStickyCodeFeature)
  {
   
    const int adcsize = adc.size();
    const int zerothresholdsigned = zerothreshold;
    
    std::vector<short> zerosuppressed(adcsize);
    const int maxblocks = adcsize/2 + 1;
    std::vector<short> blockbegin(maxblocks);
    std::vector<short> blocksize(maxblocks);
    
    int nblocks = 0;
    int zerosuppressedsize = 0;
 
    int blockstartcheck = 0;
    int endofblockcheck = 0;

    for(int i = 0; i < adcsize; ++i){

      //find maximum adc value among all neighboring channels within the nearest neighbor channel distance

      int adc_current_value = ADCStickyCodeCheck(adc[i],pedestal,fADCStickyCodeFeature);
   
      for(boost::circular_buffer<std::vector<short>>::const_iterator adcveciter = adcvec_neighbors.begin(); adcveciter!=adcvec_neighbors.end();++adcveciter){
	const std::vector<short> &adcvec_current = *adcveciter;
	const int adcvec_current_single = std::abs(adcvec_current[i] - pedestal);
      
	if(adc_current_value < adcvec_current_single){
	  adc_current_value = adcvec_current_single;
	}
      
      }
      if(blockstartcheck==0){
	if(adc_current_value>zerothresholdsigned){
	  if(nblocks>0){
	    if(i-nearestneighbor<=blockbegin[nblocks-1]+blocksize[nblocks-1]+1){
	      nblocks--;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	    else{
	      blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	      blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	      blockstartcheck = 1;
	    }
	  }	
	  else{
	    blockbegin[nblocks] = (i - nearestneighbor > 0) ? i - nearestneighbor : 0;
	    blocksize[nblocks] = i - blockbegin[nblocks] + 1;
	    blockstartcheck = 1;	    
	  }
	}
      }
      else if(blockstartcheck==1){
	if(adc_current_value>zerothresholdsigned){
	  blocksize[nblocks]++;
	  endofblockcheck = 0;
	}
	else{
	  if(endofblockcheck<nearestneighbor){
	    endofblockcheck++;
	    blocksize[nblocks]++;
	  }
	  //block has ended

	  else  if(i+2<adcsize){ //check if end of adc vector is near
	    if(ADCStickyCodeCheck(adc[i+1],pedestal,fADCStickyCodeFeature) <= zerothresholdsigned && ADCStickyCodeCheck(adc[i+2],pedestal,fADCStickyCodeFeature) <= zerothresholdsigned){  
	      endofblockcheck = 0;
	      blockstartcheck = 0;
	      nblocks++;	    
	    }
	  }
	  
	} // end else
      } // end if blockstartcheck == 1
    }// end loop over adc size
    
    if(blockstartcheck==1){ // we reached the end of the adc vector with the block still going
      ++nblocks;
    }



    for(int i = 0; i < nblocks; ++i)
      zerosuppressedsize += blocksize[i];
    
    
    adc.resize(2+nblocks+nblocks+zerosuppressedsize);
    zerosuppressed.resize(2+nblocks+nblocks+zerosuppressedsize);
    
    
    int zerosuppressedcount = 0;
    for(int i = 0; i < nblocks; ++i){
      //zerosuppressedsize += blocksize[i];
      for(int j = 0; j < blocksize[i]; ++j){
	zerosuppressed[zerosuppressedcount] = adc[blockbegin[i] + j];
	zerosuppressedcount++;
      }
    }
    
    adc[0] = adcsize; //fill first entry in adc with length of uncompressed vector
    adc[1] = nblocks;
    for(int i = 0; i < nblocks; ++i){
      adc[i+2] = blockbegin[i];
      adc[i+nblocks+2] = blocksize[i];
    }
    
    
    
    for(int i = 0; i < zerosuppressedsize; ++i)
      adc[i+nblocks+nblocks+2] = zerosuppressed[i];
    
    
    // for(int i = 0; i < 2 + 2*nblocks + zerosuppressedsize; ++i)
    //   std::cout << adc[i] << std::endl;
    //adc.resize(2+nblocks+nblocks+zerosuppressedsize);
  }


  //----------------------------------------------------------
  // Reverse zero suppression function
  void ZeroUnsuppression(const std::vector<short>& adc, 
			 std::vector<short>      &uncompressed)
  {
    const int lengthofadc = adc[0];
    const int nblocks = adc[1];

    uncompressed.resize(lengthofadc);
    for (int i = 0;i < lengthofadc; ++i){
      uncompressed[i] = 0;
    }
    
    int zerosuppressedindex = nblocks*2 + 2;

    for(int i = 0; i < nblocks; ++i){ //loop over each nonzero block of the compressed vector
      
      for(int j = 0; j < adc[2+nblocks+i]; ++j){//loop over each block size

	//set uncompressed value
	uncompressed[adc[2+i]+j] = adc[zerosuppressedindex];
	zerosuppressedindex++;

      }
    }

    return;
  }

  //----------------------------------------------------------
  // Reverse zero suppression function with pedestal re-addition
  void ZeroUnsuppression(const std::vector<short>& adc, 
			 std::vector<short>      &uncompressed,
			 int               pedestal)
  {
    const int lengthofadc = adc[0];
    const int nblocks = adc[1];

    uncompressed.resize(lengthofadc);
    for (int i = 0;i < lengthofadc; ++i){
      uncompressed[i] = pedestal;
    }
    
    int zerosuppressedindex = nblocks*2 + 2;

    for(int i = 0; i < nblocks; ++i){ //loop over each nonzero block of the compressed vector
      
      for(int j = 0; j < adc[2+nblocks+i]; ++j){//loop over each block size

	//set uncompressed value
	uncompressed[adc[2+i]+j] = adc[zerosuppressedindex];
	zerosuppressedindex++;

      }
    }

    return;
  }


  //----------------------------------------------------------
  // if the compression type is kNone, copy the adc vector into the uncompressed vector
  void Uncompress(const std::vector<short>& adc, 
		  std::vector<short>      &uncompressed, 
		  raw::Compress_t          compress)
  {
    if(compress == raw::kHuffman) UncompressHuffman(adc, uncompressed);
    else if(compress == raw::kZeroSuppression){
      ZeroUnsuppression(adc, uncompressed);
    }
    else if(compress == raw::kZeroHuffman){
      std::vector<short> tmp(2*adc[0]);
      UncompressHuffman(adc, tmp);
      ZeroUnsuppression(tmp, uncompressed);
    }
    else if(compress == raw::kNone){
      for(unsigned int i = 0; i < adc.size(); ++i) uncompressed[i] = adc[i];
    }
    else {
      throw cet::exception("raw")
        << "raw::Uncompress() does not support compression #"
        << ((int) compress);
    }
    return;
  }
  
  //----------------------------------------------------------
  // if the compression type is kNone, copy the adc vector into the uncompressed vector
  void Uncompress(const std::vector<short>& adc, 
		  std::vector<short>      &uncompressed, 
		  int               pedestal,
		  raw::Compress_t          compress)
  {
    if(compress == raw::kHuffman) UncompressHuffman(adc, uncompressed);
    else if(compress == raw::kZeroSuppression){
      ZeroUnsuppression(adc, uncompressed, pedestal);
    }
    else if(compress == raw::kZeroHuffman){
      std::vector<short> tmp(2*adc[0]);
      UncompressHuffman(adc, tmp);
      ZeroUnsuppression(tmp, uncompressed, pedestal);
    }
    else if(compress == raw::kNone){
      for(unsigned int i = 0; i < adc.size(); ++i) uncompressed[i] = adc[i];
    }
    else {
      throw cet::exception("raw")
        << "raw::Uncompress() does not support compression #"
        << ((int) compress);
    }
    return;
  }
  

  // the current Huffman Coding scheme used by uBooNE is
  // based on differences between adc values in adjacent time bins
  // the code is 
  // no change for 4 ticks --> 1
  // no change for 1 tick  --> 01
  // +1 change             --> 001
  // -1 change             --> 0001
  // +2 change             --> 00001
  // -2 change             --> 000001
  // +3 change             --> 0000001
  // -3 change             --> 00000001
  // abs(change) > 3       --> write actual value to short
  // use 15th bit to set whether a block is encoded or raw value
  // 1 --> Huffman coded, 0 --> raw
  // pad out the lowest bits in a word with 0's
  void CompressHuffman(std::vector<short> &adc)
  {
    std::vector<short> workvec(adc);
    std::vector<short> diffs(adc.size(), 0);
    diffs[0] = 0;

    // loop over the rawadc and record the differences between 
    // successive entries.  the first entry is always left
    // as the raw adc value, just set the bit 15 to 0
    for(unsigned int i = 1; i < diffs.size(); ++i) diffs[i] = adc[i] - adc[i-1];

    // now loop over the diffs and do the Huffman encoding
    unsigned int cur = 1;
    unsigned int curb = 15;

    std::bitset<16> bset;
    bset.set(15);

    for(unsigned int i = 1; i < diffs.size(); ++i){

      //check to see if the difference is too large that we have to put the entire adc value in
      if(std::abs(diffs[i]) > 3){

	// put the current value into the adc vec unless the current bit is 15, then there 
	// were multiple large difference values in a row
	if(curb != 15){
	  adc[cur] = bset.to_ulong();
	  ++cur;
	}

	bset.reset();
	bset.set(15);
	curb = 15;

	// put the current adcvalue in adc, with its bit 15 set to 0
	if(workvec[i] > 0) adc[cur] = workvec[i];
	else{
	  std::bitset<16> tbit(-workvec[i]);
	  tbit.set(14);
	  adc[cur] = tbit.to_ulong();
	} 
	++cur;

      }
      else{
	// if the difference is 0, check to see what the next 3 differences are
	if(diffs[i] == 0){
	  if(i < diffs.size() - 3){
	    // if next 3 are also 0, set the next bit to be 1
	    if(diffs[i+1] == 0 && diffs[i+2] == 0 && diffs[i+3] == 0){
	      if(curb > 0){
		--curb;
		bset.set(curb);
		i += 3; 
	      }
	      else{	    
		adc[cur] = bset.to_ulong();
		++cur;

		// reset the bitset to be ready for the next word
		bset.reset();
		bset.set(15);
		bset.set(14); // account for the fact that this is a zero diff
		curb = 14;
		i += 3; 
	      } // end if curb is not big enough to put current difference in bset	  
	    } // end if next 3 are also zero
	    else{
	      // 0 diff is encoded as 01, so move the current bit one to the right
	      if(curb > 1){
		curb -= 2;
		bset.set(curb);
	      } // end if the current bit is large enough to set this one
	      else{	    
		adc[cur] = bset.to_ulong();
		++cur;
		// reset the bitset to be ready for the next word
		bset.reset();
		bset.set(15);
		bset.set(13); // account for the fact that this is a zero diff
		curb = 13;
	      } // end if curb is not big enough to put current difference in bset	  	    
	    } // end if next 3 are not also 0
	  }// end if able to check next 3
	  else{
	    // 0 diff is encoded as 01, so move the current bit one to the right
	    if(curb > 1){
	      curb -= 2;
	      bset.set(curb);
	    } // end if the current bit is large enough to set this one
	    else{	    
	      adc[cur] = bset.to_ulong();
	      ++cur;
	      // reset the bitset to be ready for the next word
	      bset.reset();
	      bset.set(15);
	      bset.set(13); // account for the fact that this is a zero diff
	      curb = 13;
	    } // end if curb is not big enough to put current difference in bset	  
	  }// end if not able to check the next 3
	}// end if current difference is zero
	else if( diffs[i] == 1){
	  if(curb > 2){
	    curb -= 3;
	    bset.set(curb);
	  }
	  else{
	    adc[cur] = bset.to_ulong();
	    ++cur;
	    // reset the bitset to be ready for the next word
	    bset.reset();
	    bset.set(15);
	    bset.set(12); // account for the fact that this is a +1 diff
	    curb = 12;
	  } // end if curb is not big enough to put current difference in bset	  
	}// end if difference = 1
	else if( diffs[i] == -1){
	  if(curb > 3){
	    curb -= 4;
	    bset.set(curb);
	  }
	  else{
	    adc[cur] = bset.to_ulong();
	    ++cur;
	    // reset the bitset to be ready for the next word
	    bset.reset();
	    bset.set(15);
	    bset.set(11); // account for the fact that this is a -1 diff
	    curb = 11;
	  } // end if curb is not big enough to put current difference in bset	  
	}// end if difference = -1
	else if( diffs[i] == 2){
	  if(curb > 4){
	    curb -= 5;
	    bset.set(curb);
	  }
	  else{
	    adc[cur] = bset.to_ulong();
	    ++cur;
	    // reset the bitset to be ready for the next word
	    bset.reset();
	    bset.set(15);
	    bset.set(10); // account for the fact that this is a +2 diff
	    curb = 10;
	  } // end if curb is not big enough to put current difference in bset	  
	}// end if difference = 2
	else if( diffs[i] == -2){
	  if(curb > 5){
	    curb -= 6;
	    bset.set(curb);
	  }
	  else{
	    adc[cur] = bset.to_ulong();
	    ++cur;
	    // reset the bitset to be ready for the next word
	    bset.reset();
	    bset.set(15);
	    bset.set(9); // account for the fact that this is a -2 diff
	    curb = 9;
	  } // end if curb is not big enough to put current difference in bset	  
	}// end if difference = -2
	else if( diffs[i] == 3){
	  if(curb > 6){
	    curb -= 7;
	    bset.set(curb);
	  }
	  else{
	    adc[cur] = bset.to_ulong();
	    ++cur;
	    // reset the bitset to be ready for the next word
	    bset.reset();
	    bset.set(15);
	    bset.set(8); // account for the fact that this is a +3 diff
	    curb = 8;
	  } // end if curb is not big enough to put current difference in bset	  
	}// end if difference = 3
	else if( diffs[i] == -3){
	  if(curb > 7){
	    curb -= 8;
	    bset.set(curb);
	  }
	  else{
	    adc[cur] = bset.to_ulong();
	    ++cur;
	    // reset the bitset to be ready for the next word
	    bset.reset();
	    bset.set(15);
	    bset.set(7); // account for the fact that this is a -3 diff
	    curb = 7;
	  } // end if curb is not big enough to put current difference in bset	  
	}// end if difference = -3
      }// end if difference is <= 3
    }// end loop over differences

    //write out the last bitset
    adc[cur] = bset.to_ulong();
    ++cur;

    diffs.clear();
    workvec.clear();
 
    // resize the adc to be the number of entries used
    adc.resize(cur);

    return;
  }
  //--------------------------------------------------------
  // need to decrement the bit you are looking at to determine the deltas as that is how
  // the bits are set
  void UncompressHuffman(const std::vector<short>& adc, 
			 std::vector<short>      &uncompressed)
  {
    
    //the first entry in adc is a data value by construction
    uncompressed[0] = adc[0];

    unsigned int curu = 1;
    short curADC = uncompressed[0];

    // loop over the entries in adc and uncompress them according to the
    // encoding scheme above the CompressHuffman method
    for(unsigned int i = 1; i < adc.size() && curu < uncompressed.size(); ++i){

      std::bitset<16> bset(adc[i]);

      int numu = 0;

      //check the 15 bit to see if this entry is a full data value or not
      if( !bset.test(15) ){
	curADC = adc[i];
	if(bset.test(14)){
	  bset.set(14, false);
	  curADC = -1*bset.to_ulong();
	}
	uncompressed[curu] = curADC;

	++curu;
      }
      else{

	int  b       = 14;
	int  lowestb = 0;

	// ignore any padding with zeros in the lower order bits
	while( !bset.test(lowestb) && lowestb < 15) ++lowestb;

	if(lowestb > 14){
	  mf::LogWarning("raw.cxx") << "encoded entry has no set bits!!! " 
				    << i << " "
				    << bset.to_string< char,std::char_traits<char>,std::allocator<char> >(); 
	  continue;
	}

	while( b >= lowestb){ 

	  // count the zeros between the current bit and the next on bit
	  int zerocnt = 0;
	  while( !bset.test(b-zerocnt) && b-zerocnt > lowestb) ++zerocnt;

	  b -= zerocnt;

	  if(zerocnt == 0){
	    for(int s = 0; s < 4; ++s){
	      uncompressed[curu] = curADC;
	      ++curu;
	      ++numu;
	      if(curu > uncompressed.size()-1) break;
	    }
	    --b;
	  }
	  else if(zerocnt == 1){
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }
	  else if(zerocnt == 2){
	    curADC += 1;
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }
	  else if(zerocnt == 3){
	    curADC -= 1;
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }
	  else if(zerocnt == 4){
	    curADC += 2;
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }
	  else if(zerocnt == 5){
	    curADC -= 2;
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }
	  else if(zerocnt == 6){
	    curADC += 3;
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }
	  else if(zerocnt == 7){
	    curADC -= 3;
	    uncompressed[curu] = curADC;
	    ++curu;
	    ++numu;
	    --b;
	  }

	  if(curu > uncompressed.size() - 1) break;

	}// end loop over bits
 
	if(curu > uncompressed.size() - 1) break;

      }// end if this entry in the vector is encoded

    }// end loop over entries in adc

    return;
  }

  //--------------------------------------------------------
  // need to decrement the bit you are looking at to determine the deltas as that is how
  // the bits are set
  int ADCStickyCodeCheck(const short adc_value,
			 const int pedestal,
			 bool fADCStickyCodeFeature){

    int adc_return_value = std::abs(adc_value - pedestal);

      if(!fADCStickyCodeFeature){
	return adc_return_value;
      }
     // if DUNE 35t ADC sticky code feature is enabled in simulation, skip over ADC codes with LSBs of 0x00 or 0x3f
      unsigned int sixlsbs = adc_value & onemask;
      
      if((sixlsbs==onemask || sixlsbs==0) && std::abs(adc_value - pedestal) < 64){ 
	adc_return_value = 0; //set current adc value to zero if its LSBs are at sticky values and if it is within one MSB cell (64 ADC counts) of the pedestal value
      }
      return adc_return_value;
  }

}
