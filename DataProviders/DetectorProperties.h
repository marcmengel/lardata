////////////////////////////////////////////////////////////////////////
// \file DetectorProperties.h
//
// \brief service to contain information about detector electronics, etc
//
// \author brebel@fnal.gov
// 
// Separation of service from data provider class:
// jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef DATAPROV_DETECTORPROPERTIES_H
#define DATAPROV_DETECTORPROPERTIES_H

#include "fhiclcpp/ParameterSet.h"
#include "DataProviders/LArProperties.h"
#include "DataProviders/DetectorClocks.h"
#include "Geometry/Geometry.h"

///General LArSoft Utilities
namespace dataprov{
  
    class DetectorProperties {
    public:

      DetectorProperties();
      DetectorProperties(fhicl::ParameterSet const& pset, 
			 const geo::GeometryCore* geo,
			 const dataprov::LArProperties* lp,
			 const dataprov::DetectorClocks* c);
      virtual ~DetectorProperties();
      
      virtual void Configure(fhicl::ParameterSet const& p);
      virtual bool Update(uint64_t ts);
      virtual bool UpdateClocks(const dataprov::DetectorClocks* clks);

      void SetGeometry(const geo::GeometryCore* g) { fGeo = g; }
      void SetLArProperties(const dataprov::LArProperties* lp) { fLP = lp; }
      void SetDetectorClocks(const dataprov::DetectorClocks* clks) { fClocks = clks; }

      void SetNumberTimeSamples(unsigned int nsamp) { fNumberTimeSamples=nsamp;}
      // Accessors.

      virtual double Efield(unsigned int planegap=0) const; ///< kV/cm
      void SetEfield(std::vector<double> e) { fEfield = e;}

      virtual double DriftVelocity(double efield=0., double temperature=0.) const;  ///< cm/us
      
      /// dQ/dX in electrons/cm, returns dE/dX in MeV/cm.
      virtual double BirksCorrection(double dQdX) const;
      virtual double ModBoxCorrection(double dQdX) const;

      virtual double ElectronLifetime() const { return fElectronlifetime; }   //< microseconds
      
      double       SamplingRate()      const { return fTPCClock.TickPeriod() * 1.e3; }
      double       ElectronsToADC()    const { return fElectronsToADC; }
      unsigned int NumberTimeSamples() const { return fNumberTimeSamples; }
      unsigned int ReadOutWindowSize() const { return fReadOutWindowSize; }
      int          TriggerOffset()     const;
      double       TimeOffsetU()       const { return fTimeOffsetU; };
      double       TimeOffsetV()       const { return fTimeOffsetV; };
      double       TimeOffsetZ()       const { return fTimeOffsetZ; };

      double       ConvertXToTicks(double X, int p, int t, int c) const;
      double       ConvertXToTicks(double X, geo::PlaneID const& planeid) const
        { return ConvertXToTicks(X, planeid.Plane, planeid.TPC, planeid.Cryostat); }
      double       ConvertTicksToX(double ticks, int p, int t, int c) const;
      double       ConvertTicksToX(double ticks, geo::PlaneID const& planeid) const
        { return ConvertTicksToX(ticks, planeid.Plane, planeid.TPC, planeid.Cryostat); }

      double       GetXTicksOffset(int p, int t, int c) const ;
      double       GetXTicksOffset(geo::PlaneID const& planeid) const
        { return GetXTicksOffset(planeid.Plane, planeid.TPC, planeid.Cryostat); }
      double       GetXTicksCoefficient(int t, int c) const;
      double       GetXTicksCoefficient(geo::TPCID const& tpcid) const
        { return GetXTicksCoefficient(tpcid.TPC, tpcid.Cryostat); }
      double       GetXTicksCoefficient() const ;

      // The following methods convert between TDC counts (SimChannel time) and
      // ticks (RawDigit/Wire time).
      double       ConvertTDCToTicks(double tdc) const;
      double       ConvertTicksToTDC(double ticks) const;

      bool         InheritNumberTimeSamples() const { return fInheritNumberTimeSamples; }
      
    private:

      void CheckIfConfigured();
      
    protected:
            
      void         CalculateXTicksParams();
      
      const dataprov::LArProperties* fLP;
      const dataprov::DetectorClocks* fClocks;
      const geo::GeometryCore* fGeo;
      
      std::vector< double >          fEfield;           ///< kV/cm
      double                         fElectronlifetime; ///< microseconds
      double       fSamplingRate;      ///< in ns
      double 	   fElectronsToADC;    ///< conversion factor for # of ionization electrons to 1 ADC count
      unsigned int fNumberTimeSamples; ///< number of clock ticks per event
      unsigned int fReadOutWindowSize; ///< number of clock ticks per readout window
      double       fTimeOffsetU;       ///< time offsets to convert spacepoint
      double       fTimeOffsetV;       ///< coordinates to hit times on each
      double       fTimeOffsetZ;       ///< view
            
      bool         fInheritNumberTimeSamples; ///< Flag saying whether to inherit NumberTimeSamples

      double       fXTicksCoefficient; ///< Parameters for x<-->ticks

      std::vector<std::vector<std::vector<double> > > fXTicksOffsets;
      std::vector<std::vector<double> >               fDriftDirection;

      ::util::ElecClock fTPCClock;     ///< TPC electronics clock
    }; // class DetectorProperties
} //namespace dataprov

#endif // DATAPROV_DETECTOR_PROPERTIES_H
