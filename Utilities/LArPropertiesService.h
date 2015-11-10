////////////////////////////////////////////////////////////////////////
// DetectorPropertiesService.h
//
// Pure virtual service interface for DetectorProperties functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef ILARPROPERTIES_SERVICE_H
#define ILARPROPERTIES_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "DataProviders/LArProperties.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class LArPropertiesService {

    public:
    typedef dataprov::LArProperties provider_type;

    public:
      virtual ~LArPropertiesService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::LArProperties* provider() const = 0;
            
    }; // class LArPropertiesService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::LArPropertiesService, LEGACY)
#endif // ILARPROPERTIES_SERVICE_H
