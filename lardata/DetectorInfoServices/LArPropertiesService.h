////////////////////////////////////////////////////////////////////////
// DetectorPropertiesService.h
//
// Pure virtual service interface for DetectorProperties functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef LARPROPERTIESSERVICE_H
#define LARPROPERTIESSERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "lardataalg/DetectorInfo/LArProperties.h"
#include "larcore/CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace detinfo{
  class LArPropertiesService {

    public:
    typedef detinfo::LArProperties provider_type;

    public:
      virtual ~LArPropertiesService() = default;

      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  detinfo::LArProperties* provider() const = 0;

    }; // class LArPropertiesService
} //namespace detinfo
DECLARE_ART_SERVICE_INTERFACE(detinfo::LArPropertiesService, LEGACY)
#endif // LARPROPERTIESSERVICE_H
