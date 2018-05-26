////////////////////////////////////////////////////////////////////////
// LArPropertiesServiceStandard.h
//
// Service interface for Utility LAr functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef LARPROPERTIESSERVICESTANDARD_H
#define LARPROPERTIESSERVICESTANDARD_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "lardataalg/DetectorInfo/LArPropertiesStandard.h"
#include "lardata/DetectorInfoServices/LArPropertiesService.h"

///General LArSoft Utilities
namespace detinfo{
  class LArPropertiesServiceStandard : public LArPropertiesService {
    public:
      
      // this enables art to print the configuration help:
      using Parameters = art::ServiceTable<detinfo::LArPropertiesStandard::ConfigurationParameters_t>;
      
      LArPropertiesServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

      virtual void   reconfigure(fhicl::ParameterSet const& pset) override;
      void   preBeginRun(const art::Run& run);

      virtual const  provider_type* provider() const override { return fProp.get();}

    private:

      std::unique_ptr<detinfo::LArPropertiesStandard> fProp;

    }; // class LArPropertiesServiceStandard
} //namespace detinfo
DECLARE_ART_SERVICE_INTERFACE_IMPL(detinfo::LArPropertiesServiceStandard, detinfo::LArPropertiesService, LEGACY)
#endif // LARPROPERTIESSERVICESTANDARD_H
