////////////////////////////////////////////////////////////////////////
// LArProperties_service.h
//
// Service interface for Utility LAr functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef LARPROPERTIES_SERVICE_H
#define LARPROPERTIES_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "DataProviders/LArProperties.h"

///General LArSoft Utilities
namespace util{
  class LArPropertiesService {
    public:
      LArPropertiesService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      virtual ~LArPropertiesService();

      void   reconfigure(fhicl::ParameterSet const& pset);
      void   preBeginRun(const art::Run& run);

      const  LArProperties* getLArProperties() { return fProp.get();}

    private:

      std::unique_ptr<dataprov::LArProperties> fProp;

    }; // class LArPropertiesService
} //namespace utils
DECLARE_ART_SERVICE(util::LArPropertiesService, LEGACY)
#endif // LARPROPERTIES_SERVICE_H
