////////////////////////////////////////////////////////////////////////
// IDetectorPropertiesService.h
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
#include "DataProviders/ILArProperties.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class ILArPropertiesService {

    public:
    typedef dataprov::ILArProperties provider_type;

    public:
      virtual ~ILArPropertiesService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::ILArProperties* provider() const = 0;
            
    }; // class ILArPropertiesService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::ILArPropertiesService, LEGACY)
#endif // ILARPROPERTIES_SERVICE_H
