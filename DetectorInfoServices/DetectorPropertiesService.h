////////////////////////////////////////////////////////////////////////
// DetectorPropertiesService.h
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
#include "DataProviders/DetectorProperties.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class DetectorPropertiesService {

    public:
    typedef dataprov::DetectorProperties provider_type;

    public:
      virtual ~DetectorPropertiesService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::DetectorProperties* provider() const = 0;
            
    }; // class DetectorPropertiesService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::DetectorPropertiesService, LEGACY)
#endif // IDETPROPERTIES_SERVICE_H
