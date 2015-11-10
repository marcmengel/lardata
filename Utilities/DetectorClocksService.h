////////////////////////////////////////////////////////////////////////
// DetectorClocksService.h
//
// Pure virtual service interface for DetectorClocks functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef IDETCLOCKS_SERVICE_H
#define IDETCLOCKS_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "DataProviders/DetectorClocks.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class DetectorClocksService {

    public:
    typedef dataprov::DetectorClocks provider_type;

    public:
      virtual ~DetectorClocksService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::DetectorClocks* provider() const = 0;
            
    }; // class DetectorClocksService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::DetectorClocksService, LEGACY)
#endif // IDETCLOCKS_SERVICE_H
