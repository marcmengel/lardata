////////////////////////////////////////////////////////////////////////
// \file DetectorProperties.h
//
// \brief service to contain information about detector electronics, etc
//
// \author brebel@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef UTIL_DETECTORPROPERTIES_H
#define UTIL_DETECTORPROPERTIES_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"


///General LArSoft Utilities
namespace util{
    class DetectorProperties {
    public:

      DetectorProperties(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~DetectorProperties();
      
      void reconfigure(fhicl::ParameterSet const& p);

      // Accessors.

      const double       SamplingRate()      const { return fSamplingRate;   }
      const double       ElectronsToADC()    const { return fElectronsToADC; }
      const unsigned int NumberTimeSamples() const { return fNumberTimeSamples; }
      const unsigned int ReadOutWindowSize() const { return fReadOutWindowSize; }
      const int  	 TriggerOffset()     const    ;
      const double       TimeOffsetU()       const { return fTimeOffsetU; };
      const double       TimeOffsetV()       const { return fTimeOffsetV; };
      const double       TimeOffsetZ()       const { return fTimeOffsetZ; };

      double             ConvertXToTicks(double X,     int p, int t, int c) ;
      double             ConvertTicksToX(double ticks, int p, int t, int c) ;

      double             GetXTicksOffset(int p, int t, int c) ;
      double             GetXTicksCoefficient() ;      

      // The following methods convert between TDC counts (SimChannel time) and
      // ticks (RawDigit/Wire time).
      double             ConvertTDCToTicks(double tdc) const {
	return fNumberTimeSamples == fReadOutWindowSize ? tdc : tdc - fReadOutWindowSize;}
      double             ConvertTicksToTDC(double ticks) const {
	return fNumberTimeSamples == fReadOutWindowSize ? ticks : ticks + fReadOutWindowSize;}


    private:

      // Callbacks.

      void         postOpenFile(std::string const& filename);
      void 	   preBeginRun(art::Run const& run);

      void 	   checkDBstatus() 	const;
      void         CalculateXTicksParams();

      static bool  isDetectorProperties(const fhicl::ParameterSet& ps);

      double       fSamplingRate;      ///< in ns
      int    	   fTriggerOffset;     ///< in # of clock ticks					       	 
      double 	   fElectronsToADC;    ///< conversion factor for # of ionization electrons to 1 ADC count
      unsigned int fNumberTimeSamples; ///< number of clock ticks per event
      unsigned int fReadOutWindowSize; ///< number of clock ticks per readout window
      double       fTimeOffsetU;       ///< time offsets to convert spacepoint
      double       fTimeOffsetV;       ///< coordinates to hit times on each
      double       fTimeOffsetZ;       ///< view
            
      bool         fInheritTriggerOffset;     ///< Flag saying whether to inherit TriggerOffset
      bool         fInheritNumberTimeSamples; ///< Flag saying whether to inherit NumberTimeSamples

      double       fXTicksCoefficient; ///< Parameters for x<-->ticks
      bool         fXTicksParamsLoaded;///<  calculations
      std::vector<std::vector<std::vector<double> > > fXTicksOffsets;

      fhicl::ParameterSet   fPS;       ///< Original parameter set.

      bool	   fAlreadyReadFromDB; ///< tests whether the values have alread been picked up from the Database
    }; // class DetectorProperties
} //namespace utils
DECLARE_ART_SERVICE(util::DetectorProperties, LEGACY)
#endif // UTIL_DETECTOR_PROPERTIES_H
