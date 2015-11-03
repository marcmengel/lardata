////////////////////////////////////////////////////////////////////////
// IDetectorPropertiesService.h
//
// Pure virtual service interface for DetectorProperties functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef IDETPROPERTIES_SERVICE_H
#define IDETPROPERTIES_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "DataProviders/IDetectorProperties.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class IDetectorPropertiesService {

    public:
    typedef dataprov::IDetectorProperties provider_type;

    public:
      virtual ~IDetectorPropertiesService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::IDetectorProperties* provider() const = 0;
            
    }; // class IDetectorPropertiesService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::IDetectorPropertiesService, LEGACY)
#endif // IDETPROPERTIES_SERVICE_H
