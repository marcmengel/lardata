////////////////////////////////////////////////////////////////////////
//
// TimeService.h

////////////////////////////////////////////////////////////////////////
#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"

#include "SimpleTimeService.h"

namespace util{

  class TimeService : public SimpleTimeService {

  public:
    TimeService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);

    ~TimeService(){};

  public:
    void   reconfigure(fhicl::ParameterSet const& pset);
    
  private:
    
    }; // class TimeService

} //namespace utils

DECLARE_ART_SERVICE(util::TimeService, LEGACY)

#endif 
