////////////////////////////////////////////////////////////////////////
// \file DetectorProperties.h
//
// \brief service to contain information about detector electronics, etc
//
// \author brebel@fnal.gov
// 
// Separation of service from Detector info class:
// jpaley@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef DETINFO_DETECTORPROPERTIESSTD_H
#define DETINFO_DETECTORPROPERTIESSTD_H

#include "fhiclcpp/ParameterSet.h"
#include "Geometry/GeometryCore.h"
#include "DetectorInfo/ProviderPack.h"
#include "DetectorInfo/LArProperties.h"
#include "DetectorInfo/DetectorClocks.h"
#include "DetectorInfo/DetectorProperties.h"

///General LArSoft Utilities
namespace detinfo{
  
  class DetectorPropertiesStandard : public DetectorProperties {
    public:
      /// List of service providers we depend on
      using providers_type = lar::ProviderPack<
        geo::GeometryCore,
        detinfo::LArProperties,
        detinfo::DetectorClocks
        >;

      DetectorPropertiesStandard();
      DetectorPropertiesStandard(fhicl::ParameterSet const& pset, 
			 const geo::GeometryCore* geo,
			 const detinfo::LArProperties* lp,
			 const detinfo::DetectorClocks* c);
      /**
       * @brief Constructs the provider and sets up the dependencies
       * @param pset FHiCL parameter set for provider configuration
       * @param providers pack of providers DetectorPropertiesStandard depends on
       * @see Setup()
       */
      DetectorPropertiesStandard(fhicl::ParameterSet const& pset,
                         providers_type providers);
      DetectorPropertiesStandard(DetectorPropertiesStandard const&) = delete;
      virtual ~DetectorPropertiesStandard() = default;
      
      void Configure(fhicl::ParameterSet const& p);
      bool Update(uint64_t ts);
      bool UpdateClocks(const detinfo::DetectorClocks* clks);
      
      /**
       * @brief Sets all the providers at once
       * @param providers the pack of service providers we depend on
       * 
       * Example:
       *     
       *     lar::DetectorPropertiesStandard::providers_type providers;
       *     providers.set(lar::providerFrom<geo::Geometry>());
       *     providers.set(lar::providerFrom<detinfo::LArPropertiesService>());
       *     providers.set(lar::providerFrom<detinfo::DetectorClocksService>());
       *     detprop->Setup(providers);
       *
       */
      void Setup(providers_type providers);
        
      void SetGeometry(const geo::GeometryCore* g) { fGeo = g; }
      void SetLArProperties(const detinfo::LArProperties* lp) { fLP = lp; }
      void SetDetectorClocks(const detinfo::DetectorClocks* clks) { fClocks = clks; }

      void SetNumberTimeSamples(unsigned int nsamp) { fNumberTimeSamples=nsamp;}
      // Accessors.

      virtual double Efield(unsigned int planegap=0) const override; ///< kV/cm

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
      
      
      /// Verifies that the provider is in a fully configured status
      /// @throw cet::exception (category DetectorPropertiesStandard) if not ok
      void CheckIfConfigured() const;
      
    protected:
            
      void         CalculateXTicksParams();
      
      // service providers we depend on;
      // in principle could be replaced by a single providerpacl_type.
      const detinfo::LArProperties* fLP;
      const detinfo::DetectorClocks* fClocks;
      const geo::GeometryCore* fGeo;
      
      std::vector< double >          fEfield;           ///< kV/cm (per plane)
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

      ::detinfo::ElecClock fTPCClock;     ///< TPC electronics clock
    }; // class DetectorPropertiesStandard
} //namespace detinfo

#endif // DETINFO_DETECTOR_PROPERTIES_H
