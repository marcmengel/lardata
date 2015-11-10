////////////////////////////////////////////////////////////////////////
// IRunHistoryService.h
//
// Pure virtual service interface for RunHistory functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef IRUNHISTORY_SERVICE_H
#define IRUNHISTORY_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "DataProviders/IRunHistory.h"
#include "CoreUtils/ServiceUtil.h"

///General LArSoft Utilities
namespace util{
  class IRunHistoryService {

    public:
    typedef dataprov::IRunHistory provider_type;

    public:
      virtual ~IRunHistoryService() = default;
      
      virtual void   reconfigure(fhicl::ParameterSet const& pset) = 0;
      virtual const  dataprov::IRunHistory* provider() const = 0;
            
    }; // class IRunHistoryService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE(util::IRunHistoryService, LEGACY)

// check that the requirements for util::IRunHistoryService are satisfied
template class lar::details::ServiceRequirementsChecker<util::IRunHistoryService>;

#endif // IRUNHISTORY_SERVICE_H
