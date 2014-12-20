////////////////////////////////////////////////////////////////////////
//  \file CalorimetryAlg.cxx
//
//  \brief Functions to calculate dE/dx. Based on code in Calorimetry.cxx
//
// andrzej.szelc@yale.edu
//
////////////////////////////////////////////////////////////////////////



#include "messagefacility/MessageLogger/MessageLogger.h"
// 

#include "TMath.h"

// LArSoft includes
#include "RecoBase/Hit.h"
#include "AnalysisAlg/CalorimetryAlg.h"

namespace calo{

  //--------------------------------------------------------------------
  CalorimetryAlg::CalorimetryAlg(fhicl::ParameterSet const& pset) 
  {
     this->reconfigure( pset);
  }

  //--------------------------------------------------------------------
  CalorimetryAlg::~CalorimetryAlg() 
  {
    
  }
  
  //--------------------------------------------------------------------
  void   CalorimetryAlg::reconfigure(fhicl::ParameterSet const& pset)
  {
    
    fCalAmpConstants 	= pset.get< std::vector<double> >("CalAmpConstants");
    fCalAreaConstants   = pset.get< std::vector<double> >("CalAreaConstants");
    fUseModBox          = pset.get< bool >("CaloUseModBox");

    return;
  }
 
  //------------------------------------------------------------------------------------//
  // Functions to calculate the dEdX based on the AMPLITUDE of the pulse
  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AMP(art::Ptr< recob::Hit >  hit, double pitch) const
  {
    return dEdx_AMP(hit->PeakAmplitude()/pitch, hit->PeakTime(), hit->WireID().Plane);
  }
  
  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AMP(recob::Hit const&  hit, double pitch) const
  {
    return dEdx_AMP(hit.PeakAmplitude()/pitch, hit.PeakTime(), hit.WireID().Plane);
  }

  ///\todo The plane argument should really be for a view instead
  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AMP(double dQ, double time, double pitch, unsigned int plane) const
  {
    double dQdx   = dQ/pitch;           // in ADC/cm
    return dEdx_AMP(dQdx, time, plane);
  }
    
  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AMP(double dQdx,double time, unsigned int plane) const
  {
    double fADCtoEl=1.;
    
    fADCtoEl = fCalAmpConstants[plane];
    
    double dQdx_e = dQdx/fADCtoEl;  // Conversion from ADC/cm to e/cm
    return dEdx_from_dQdx_e(dQdx_e,time);
  }
  
  //------------------------------------------------------------------------------------//
  // Functions to calculate the dEdX based on the AREA of the pulse
  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AREA(art::Ptr< recob::Hit >  hit, double pitch) const
  {
    return dEdx_AREA(hit->Integral()/pitch, hit->PeakTime(), hit->WireID().Plane);
  }

  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AREA(recob::Hit const&  hit, double pitch) const
  {
    return dEdx_AREA(hit.Integral()/pitch, hit.PeakTime(), hit.WireID().Plane);
  }
    
  // ----------------------------------------------------------------------------------//
  double CalorimetryAlg::dEdx_AREA(double dQ,double time, double pitch, unsigned int plane) const
  {
    double dQdx   = dQ/pitch;           // in ADC/cm
    return dEdx_AREA(dQdx, time, plane);
  }
  
  // ----------------------------------------------------------------------------------//  
  double CalorimetryAlg::dEdx_AREA(double dQdx,double time, unsigned int plane) const
  {
    double fADCtoEl=1.;
    
    fADCtoEl = fCalAreaConstants[plane];
    
    double dQdx_e = dQdx/fADCtoEl;  // Conversion from ADC/cm to e/cm
    return dEdx_from_dQdx_e(dQdx_e, time);
  }
    
  // ----------------- apply Lifetime and recombination correction.  -----------------//
  double CalorimetryAlg::dEdx_from_dQdx_e(double dQdx_e, double time) const
  {
    dQdx_e *= LifetimeCorrection(time);   // Lifetime Correction (dQdx_e in e/cm)
    if(fUseModBox) {
      return LArProp->ModBoxCorrection(dQdx_e);
    } else {
      return LArProp->BirksCorrection(dQdx_e);
    }
  }
  
  
  //------------------------------------------------------------------------------------//
  // for the time being copying from Calorimetry.cxx - should be decided where to keep it.
  // ----------------------------------------------------------------------------------//
  double calo::CalorimetryAlg::LifetimeCorrection(double time) const
  {  
    float t = time;

    double timetick = detprop->SamplingRate()*1.e-3;    //time sample in microsec
    double presamplings = detprop->TriggerOffset();
    
    t -= presamplings;
    time = t * timetick;  //  (in microsec)
    
    double tau = LArProp->ElectronLifetime();
    
    double correction = exp(time/tau);
    return correction;
  }

} // namespace
