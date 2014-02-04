////////////////////////////////////////////////////////////////////////
// $Id: LArFFT.cxx,v 1.11 2010/07/02 20:33:09 bpage Exp $
//
// \file LArFFT_plugin
//
//  This class simplifies implementation of Fourier transforms. 
//  Because all data inputs and outputs are purely real,  the 
//  transforms implemented in this way get a substantial performance
//  increase ~2x.
//
// \author pagebri3@msu.edu
//
////////////////////////////////////////////////////////////////////////

// Framework includes

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}
#include "Utilities/LArFFT.h"
#include "Utilities/DetectorProperties.h"
#include "messagefacility/MessageLogger/MessageLogger.h" 

//-----------------------------------------------
util::LArFFT::LArFFT(fhicl::ParameterSet const& pset, art::ActivityRegistry& /* reg */) 
  : fSize    (art::ServiceHandle<util::DetectorProperties>()->ReadOutWindowSize())    
  , fOption  (pset.get< std::string >("FFTOption"))
  , fFitBins (pset.get< int         >("FitBins"))
{
  
  int i;
  // the <= is to allow ArgoNeut to go from 2048 to 4096. 
  // This may have to be revisited if another detector comes 
  // in with a timetick number that is equal to a power of two.
  for(i = 1; i <= fSize; i *= 2){ }   
  mf::LogInfo("LArFFt") << "calculated FFT size: " << i << " det time ticks: " << fSize ;
   
  fSize=i;
  fFreqSize = fSize/2+1;  

  // allocate and setup Transform objects  
  fFFT        = new TFFTRealComplex(fSize, false);  
  fInverseFFT = new TFFTComplexReal(fSize, false);  

  int dummy[1] = {0}; 
  // appears to be dummy argument from root page  
  fFFT->Init(fOption.c_str(),-1,dummy);  
  fInverseFFT->Init(fOption.c_str(),1,dummy);  

  fPeakFit = new TF1("fPeakFit","gaus"); //allocate function used for peak fitting  
  fConvHist = new TH1D("fConvHist","Convolution Peak Data",fFitBins,0,fFitBins);  //allocate histogram for peak fitting
  //allocate other data vectors  
  fCompTemp.resize(fFreqSize);  
  fKern.resize(fFreqSize);
}

//------------------------------------------------
util::LArFFT::~LArFFT() 
{
  delete fFFT;
  delete fInverseFFT;
  delete fPeakFit;
}

//-------------------------------------------------
// For the sake of efficiency, as all transforms should
// be of the same size, all functions expect vectors 
// to be of the desired size.
// Note that because the transforms are real to real
// there is a redundancy in the information in the
// complex part in the positive and negative
// components of the FFT, thus the size of the
// frequency vectors are input_fFreqSize 
// --see the FFTW3 or Root docmentation for details


//According to the Fourier transform identity
//f(x-a) = Inverse Transform(exp(-2*Pi*i*a*w)F(w))
//--------------------------------------------------
void util::LArFFT::ShiftData(std::vector<TComplex> & input,	
			     double shift)
{ 
  double factor = -2.0*TMath::Pi()*shift/(double)fSize;  

  for(int i = 0; i < fFreqSize; i++)    
    input[i] *= TComplex::Exp(TComplex(0,factor*(double)i));  

  return;
}

namespace util{
 
  DEFINE_ART_SERVICE(LArFFT)

}
