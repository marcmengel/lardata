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
#ifndef DATAPROV_DETECTORPROPERTIESSTD_H
#define DATAPROV_DETECTORPROPERTIESSTD_H

#include "fhiclcpp/ParameterSet.h"
#include "DataProviders/LArProperties.h"
#include "DataProviders/DetectorClocks.h"
#include "DataProviders/DetectorProperties.h"

///General LArSoft Utilities
namespace dataprov{
  
  class DetectorPropertiesStandard : public DetectorProperties {
    public:

      DetectorPropertiesStandard();
      DetectorPropertiesStandard(fhicl::ParameterSet const& pset, 
			 const geo::GeometryCore* geo,
			 const dataprov::LArProperties* lp,
			 const dataprov::DetectorClocks* c);
      DetectorPropertiesStandard(DetectorPropertiesStandard const&) = delete;
      virtual ~DetectorPropertiesStandard();
      
      void Configure(fhicl::ParameterSet const& p);
      bool Update(uint64_t ts);
      bool UpdateClocks(const dataprov::DetectorClocks* clks);

      void SetGeometry(const geo::GeometryCore* g) { fGeo = g; }
      void SetLArProperties(const dataprov::LArProperties* lp) { fLP = lp; }
      void SetDetectorClocks(const dataprov::DetectorClocks* clks) { fClocks = clks; }

      void SetNumberTimeSamples(unsigned int nsamp) { fNumberTimeSamples=nsamp;}
      // Accessors.

      virtual double Efield(unsigned int planegap=0) const override; ///< kV/cm
      void SetEfield(std::vector<double> e) { fEfield = e;}

      virtual double DriftVelocity(double efield=0., double temperature=0.) const override;  ///< cm/us
      
      /// dQ/dX in electrons/cm, returns dE/dX in MeV/cm.
      virtual double BirksCorrection(double dQdX) const override;
      virtual double ModBoxCorrection(double dQdX) const override;

      virtual double ElectronLifetime() const override { return fElectronlifetime; }   //< microseconds
      
      virtual double       SamplingRate()      const override { return fTPCClock.TickPeriod() * 1.e3; }
      virtual double       ElectronsToADC()    const override { return fElectronsToADC; }
      virtual unsigned int NumberTimeSamples() const override { return fNumberTimeSamples; }
      virtual unsigned int ReadOutWindowSize() const override { return fReadOutWindowSize; }
      virtual int          TriggerOffset()     const override;
      virtual double       TimeOffsetU()       const override{ return fTimeOffsetU; };
      virtual double       TimeOffsetV()       const override { return fTimeOffsetV; };
      virtual double       TimeOffsetZ()       const override{ return fTimeOffsetZ; };

      virtual double       ConvertXToTicks(double X, int p, int t, int c) const override;
      virtual double       ConvertXToTicks(double X, geo::PlaneID const& planeid) const override
        { return ConvertXToTicks(X, planeid.Plane, planeid.TPC, planeid.Cryostat); }
      virtual double       ConvertTicksToX(double ticks, int p, int t, int c) const override;
      virtual double       ConvertTicksToX(double ticks, geo::PlaneID const& planeid) const override
        { return ConvertTicksToX(ticks, planeid.Plane, planeid.TPC, planeid.Cryostat); }

      virtual double       GetXTicksOffset(int p, int t, int c) const override;
      virtual double       GetXTicksOffset(geo::PlaneID const& planeid) const override
        { return GetXTicksOffset(planeid.Plane, planeid.TPC, planeid.Cryostat); }
      virtual double       GetXTicksCoefficient(int t, int c) const override;
      virtual double       GetXTicksCoefficient(geo::TPCID const& tpcid) const override
        { return GetXTicksCoefficient(tpcid.TPC, tpcid.Cryostat); }
      virtual double       GetXTicksCoefficient() const override;

      // The following methods convert between TDC counts (SimChannel time) and
      // ticks (RawDigit/Wire time).
      virtual double       ConvertTDCToTicks(double tdc) const override;
      virtual double       ConvertTicksToTDC(double ticks) const override;

      virtual bool         InheritNumberTimeSamples() const override { return fInheritNumberTimeSamples; }
      
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
    }; // class DetectorPropertiesStandard
} //namespace dataprov

#endif // DATAPROV_DETECTOR_PROPERTIES_H
