////////////////////////////////////////////////////////////////////////
// \file DetectorPropertiesServiceArgoNeuT.h
//
// \brief service to contain information about detector electronics, etc
//
// \author brebel@fnal.gov
//
// From the original DetectorProperties.h ; this one preserves the dependency on
// DatabaseUtil service and the ability to read information from a database
// with direct DB connection.
// For new experiments, an indirect connection should be used instead.
//
// PLEASE DO NOT take this as a model to develop a service:
// this is just a backward-compatible hack.
//
////////////////////////////////////////////////////////////////////////
#ifndef UTIL_DETECTORPROPERTIESSERVICEARGONEUT_H
#define UTIL_DETECTORPROPERTIESSERVICEARGONEUT_H

#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/Utilities/LArPropertiesServiceArgoNeuT.h"
#include "lardataalg/DetectorInfo/DetectorProperties.h"
#include "lardataalg/DetectorInfo/ElecClock.h"

#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

///General LArSoft Utilities
namespace util {
  class DetectorPropertiesServiceArgoNeuT
    : public detinfo::DetectorProperties // implements provider interface
    ,
      public detinfo::DetectorPropertiesService // implements service interface
  {
  public:
    DetectorPropertiesServiceArgoNeuT(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    //------------------------------------------------------------------------
    //--- art service interface

    /// type of service provider, that is this very same
    using provider_type = DetectorPropertiesServiceArgoNeuT;

    void reconfigure(fhicl::ParameterSet const& pset);

    /// Returns our service provider, that is this very same class
    virtual const detinfo::DetectorProperties*
    provider() const override
    {
      return this;
    }

    //------------------------------------------------------------------------
    //--- service provider interface
    virtual double Efield(unsigned int planegap = 0) const override ///< kV/cm
    {
      return fLP->Efield(planegap);
    }

    virtual double
    DriftVelocity(double efield = 0., double temperature = 0.) const override
    {
      return fLP->DriftVelocity(efield, temperature);
    }

    /// dQ/dX in electrons/cm, returns dE/dX in MeV/cm.
    virtual double
    BirksCorrection(double dQdX) const override
    {
      return fLP->BirksCorrection(dQdX);
    }

    virtual double
    ModBoxCorrection(double dQdX) const override
    {
      return fLP->ModBoxCorrection(dQdX);
    }

    virtual double
    ElectronLifetime() const override
    {
      return fLP->ElectronLifetime();
    }

    virtual double
    Temperature() const override
    {
      return fLP->Temperature();
    }

    virtual double
    Density(double temperature) const override
    {
      return fLP->Density(temperature);
    }
    virtual double
    Density() const override
    {
      return Density(Temperature());
    }

    virtual double
    Eloss(double mom, double mass, double tcut) const override
    {
      return fLP->Eloss(mom, mass, tcut);
    }

    virtual double
    ElossVar(double mom, double mass) const override
    {
      return fLP->ElossVar(mom, mass);
    }

    virtual double
    SamplingRate() const override
    {
      return fTPCClock.TickPeriod() * 1.e3;
    }
    virtual double
    ElectronsToADC() const override
    {
      return fElectronsToADC;
    }
    virtual unsigned int
    NumberTimeSamples() const override
    {
      return fNumberTimeSamples;
    }
    virtual unsigned int
    ReadOutWindowSize() const override
    {
      return fReadOutWindowSize;
    }
    virtual int TriggerOffset() const override;
    virtual double
    TimeOffsetU() const override
    {
      return fTimeOffsetU;
    }
    virtual double
    TimeOffsetV() const override
    {
      return fTimeOffsetV;
    }
    virtual double
    TimeOffsetZ() const override
    {
      return fTimeOffsetZ;
    }
    virtual double
    TimeOffsetY() const override
    {
      return 0;
    }

    virtual double ConvertXToTicks(double X, int p, int t, int c) const override;
    virtual double
    ConvertXToTicks(double X, geo::PlaneID const& planeid) const override
    {
      return ConvertXToTicks(X, planeid.Plane, planeid.TPC, planeid.Cryostat);
    }
    virtual double ConvertTicksToX(double ticks, int p, int t, int c) const override;
    virtual double
    ConvertTicksToX(double ticks, geo::PlaneID const& planeid) const override
    {
      return ConvertTicksToX(ticks, planeid.Plane, planeid.TPC, planeid.Cryostat);
    }

    virtual double GetXTicksOffset(int p, int t, int c) const override;
    virtual double
    GetXTicksOffset(geo::PlaneID const& planeid) const override
    {
      return GetXTicksOffset(planeid.Plane, planeid.TPC, planeid.Cryostat);
    }
    virtual double GetXTicksCoefficient(int t, int c) const override;
    virtual double
    GetXTicksCoefficient(geo::TPCID const& tpcid) const override
    {
      return GetXTicksCoefficient(tpcid.TPC, tpcid.Cryostat);
    }
    virtual double GetXTicksCoefficient() const override;

    // The following methods convert between TDC counts (SimChannel time) and
    // ticks (RawDigit/Wire time).
    virtual double ConvertTDCToTicks(double tdc) const override;
    virtual double ConvertTicksToTDC(double ticks) const override;

    virtual bool
    SimpleBoundary() const override
    {
      return fSimpleBoundary;
    }

    //------------------------------------------------------------------------

    // Accessors.

  private:
    // Callbacks.
    void checkDBstatus() const;

    void preProcessEvent(const art::Event& evt, art::ScheduleContext);

    void postOpenFile(std::string const& filename);

    void CalculateXTicksParams() const;

    static bool isDetectorPropertiesServiceArgoNeuT(const fhicl::ParameterSet& ps);

    //  double       fSamplingRate;      ///< in ns
    double fElectronsToADC; ///< conversion factor for # of ionization electrons to 1 ADC count
    unsigned int fNumberTimeSamples; ///< number of clock ticks per event
    unsigned int fReadOutWindowSize; ///< number of clock ticks per readout window
    double fTimeOffsetU;             ///< time offsets to convert spacepoint
    double fTimeOffsetV;             ///< coordinates to hit times on each
    double fTimeOffsetZ;             ///< view

    bool fInheritNumberTimeSamples;   ///< Flag saying whether to inherit NumberTimeSamples
    mutable bool fXTicksParamsLoaded; ///<  calculations

    mutable double fXTicksCoefficient; ///< Parameters for x<-->ticks

    mutable std::vector<std::vector<std::vector<double>>> fXTicksOffsets;
    mutable std::vector<std::vector<double>> fDriftDirection;

    fhicl::ParameterSet fPS; ///< Original parameter set.

    bool
      fAlreadyReadFromDB; ///< tests whether the values have alread been picked up from the Database

    detinfo::ElecClock fTPCClock; ///< TPC electronics clock

    /// Pointer to the specific LArPropertiesServiceArgoNeuT service (provider)
    util::LArPropertiesServiceArgoNeuT const* fLP;

    bool fSimpleBoundary;

  }; // class DetectorPropertiesServiceArgoNeuT
} //namespace util
DECLARE_ART_SERVICE_INTERFACE_IMPL(util::DetectorPropertiesServiceArgoNeuT,
                                   detinfo::DetectorPropertiesService,
                                   LEGACY)
#endif // UTIL_DETECTORPROPERTIESSERVICEARGONEUT_H
