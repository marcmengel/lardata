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

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "lardataalg/DetectorInfo/DetectorProperties.h"

namespace detinfo {
  class DetectorPropertiesService {
  public:
    using provider_type = detinfo::DetectorProperties;

    virtual ~DetectorPropertiesService() = default;
    virtual provider_type const* provider() const = 0;
  };
} //namespace detinfo

DECLARE_ART_SERVICE_INTERFACE(detinfo::DetectorPropertiesService, SHARED)

#endif // DETECTORPROPERTIESSERVICE_H
