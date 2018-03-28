////////////////////////////////////////////////////////////////////////
// DetectorClocksServiceStandard.h
//
// Service interface for Detector Clock functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef DETECTORCLOCKSSERVICESTANDARD_H
#define DETECTORCLOCKSSERVICESTANDARD_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Event.h"

#include "lardata/DetectorInfo/DetectorClocksStandard.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"

///General LArSoft Utilities
namespace detinfo{
  class DetectorClocksServiceStandard : public DetectorClocksService {
  public:
    DetectorClocksServiceStandard(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
    
    virtual void   reconfigure(fhicl::ParameterSet const& pset) override;
    void   preBeginRun(const art::Run& run);
    void   preProcessEvent(const art::Event& evt);
    void   postOpenFile(const std::string& filename);
    
    virtual const provider_type* provider() const override { return fClocks.get();}
    
  private:
    
    std::unique_ptr<detinfo::DetectorClocksStandard> fClocks;
    
  };
} //namespace detinfo
DECLARE_ART_SERVICE_INTERFACE_IMPL(detinfo::DetectorClocksServiceStandard, detinfo::DetectorClocksService, LEGACY)
#endif // DETECTORCLOCKSSERVICESTANDARD_H
