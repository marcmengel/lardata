////////////////////////////////////////////////////////////////////////
// DetectorClocksService.h
//
// Pure virtual service interface for DetectorClocks functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////

#ifndef DETECTORCLOCKSSERVICE_H
#define DETECTORCLOCKSSERVICE_H

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "lardataalg/DetectorInfo/DetectorClocks.h"

namespace detinfo {
  class DetectorClocksService {
  public:
    using provider_type = detinfo::DetectorClocks;

    virtual ~DetectorClocksService() = default;
    virtual provider_type const* provider() const = 0;
  };
} //namespace detinfo

DECLARE_ART_SERVICE_INTERFACE(detinfo::DetectorClocksService, SHARED)

#endif // DETECTORCLOCKSSERVICE_H
