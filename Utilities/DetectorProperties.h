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
#include "Utilities/TimeService.h"

///General LArSoft Utilities
namespace util{
    class DetectorProperties {
    public:

      DetectorProperties(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~DetectorProperties();
      
      void reconfigure(fhicl::ParameterSet const& p);

      // Accessors.

      double       SamplingRate()      const { return fTPCClock.TickPeriod(); }
      double       ElectronsToADC()    const { return fElectronsToADC; }
      unsigned int NumberTimeSamples() const { return fNumberTimeSamples; }
      unsigned int ReadOutWindowSize() const { return fReadOutWindowSize; }
      int          TriggerOffset()     const;
      double       TimeOffsetU()       const { return fTimeOffsetU; };
      double       TimeOffsetV()       const { return fTimeOffsetV; };
      double       TimeOffsetZ()       const { return fTimeOffsetZ; };

      double             ConvertXToTicks(double X,     int p, int t, int c) ;
      double             ConvertTicksToX(double ticks, int p, int t, int c) ;

      double             GetXTicksOffset(int p, int t, int c) ;
      double             GetXTicksCoefficient() ;      

      // The following methods convert between TDC counts (SimChannel time) and
      // ticks (RawDigit/Wire time).
      //double             ConvertTDCToTicks(double tdc) const {
      //return fNumberTimeSamples == fReadOutWindowSize ? tdc : tdc - fReadOutWindowSize;}
      //double             ConvertTicksToTDC(double ticks) const {
      //return fNumberTimeSamples == fReadOutWindowSize ? ticks : ticks + fReadOutWindowSize;}


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

      ::util::ElecClock fTPCClock;     ///< TPC electronics clock
    }; // class DetectorProperties
} //namespace utils
DECLARE_ART_SERVICE(util::DetectorProperties, LEGACY)
#endif // UTIL_DETECTOR_PROPERTIES_H
