////////////////////////////////////////////////////////////////////////
// DetectorClocksService.h
//
// Service interface for Detector Clock functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef DETCLOCKS_SERVICE_H
#define DETCLOCKS_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"

#include "DataProviders/DetectorClocks.h"

///General LArSoft Utilities
namespace util{
  class DetectorClocksService {
  public:
    DetectorClocksService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    virtual ~DetectorClocksService();
    
    virtual void   reconfigure(fhicl::ParameterSet const& pset);
    virtual void   preBeginRun(const art::Run& run);
    virtual void   preProcessEvent(const art::Event& evt);
    virtual void   postOpenFileconst(std::string& filename);
    
    const  dataprov::DetectorClocks* getDetectorClocks() { return fClocks.get();}
    
  private:
    
    std::unique_ptr<dataprov::DetectorClocks> fClocks;
    
  };
} //namespace utils
DECLARE_ART_SERVICE(util::DetectorClocksService, LEGACY)
#endif // LARPROPERTIES_SERVICE_H
