////////////////////////////////////////////////////////////////////////
// DetectorPropertiesService.h
//
// Pure virtual service interface for DetectorProperties functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef DETECTORPROPERTIESSERVICE_H
#define DETECTORPROPERTIESSERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "lardata/DetectorInfo/DetectorProperties.h"
#include "larcore/CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace detinfo{
  class DetectorPropertiesService {

    public:
    typedef detinfo::DetectorProperties provider_type;

    public:
      virtual ~DetectorPropertiesService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  detinfo::DetectorProperties* provider() const = 0;
            
    }; // class DetectorPropertiesService
} //namespace detinfo
DECLARE_ART_SERVICE_INTERFACE(detinfo::DetectorPropertiesService, LEGACY)
#endif // DETECTORPROPERTIESSERVICE_H
