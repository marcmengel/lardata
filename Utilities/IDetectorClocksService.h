////////////////////////////////////////////////////////////////////////
// IDetectorClocksService.h
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
#include "DataProviders/IDetectorClocks.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class IDetectorClocksService {

    public:
    typedef dataprov::IDetectorClocks provider_type;

    public:
      virtual ~IDetectorClocksService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::IDetectorClocks* provider() const = 0;
            
    }; // class IDetectorClocksService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::IDetectorClocksService, LEGACY)

// check that the requirements for util::IDetectorClocksService are satisfied
template class lar::details::ServiceRequirementsChecker<util::IDetectorClocksService>;

#endif // IDETCLOCKS_SERVICE_H
