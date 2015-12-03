////////////////////////////////////////////////////////////////////////
// DetectorPropertiesServiceStandard.h
//
// Service interface for DetectorProperties functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef DETECTORPROPERTIESSERVICESTANDARD_H
#define DETECTORPROPERTIESSERVICESTANDARD_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "DetectorInfo/DetectorPropertiesStandard.h"
#include "DetectorInfoServices/DetectorPropertiesService.h"

///General LArSoft Utilities
namespace detinfo{
  class DetectorPropertiesServiceStandard : public DetectorPropertiesService {

    public:
      
      // this enables art to print the configuration help:
      using Parameters = art::ServiceTable<detinfo::DetectorPropertiesStandard::Configuration_t>;
      
      DetectorPropertiesServiceStandard(fhicl::ParameterSet const& pset,
				art::ActivityRegistry& reg);

      virtual void   reconfigure(fhicl::ParameterSet const& pset) override;
      void   preProcessEvent(const art::Event& evt);
      void   postOpenFile(const std::string& filename);
      
      virtual const provider_type* provider() const override { return fProp.get();}
      
    private:

      std::unique_ptr<detinfo::DetectorPropertiesStandard> fProp;
      fhicl::ParameterSet   fPS;       ///< Original parameter set.
      
      bool  isDetectorPropertiesService(const fhicl::ParameterSet& ps);
      
    }; // class DetectorPropertiesService
} //namespace detinfo
DECLARE_ART_SERVICE_INTERFACE_IMPL(detinfo::DetectorPropertiesServiceStandard, detinfo::DetectorPropertiesService, LEGACY)
#endif // DETECTORPROPERTIESSERVICESTANDARD_H
