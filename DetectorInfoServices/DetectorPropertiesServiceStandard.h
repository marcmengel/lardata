////////////////////////////////////////////////////////////////////////
// DetectorPropertiesService.h
//
// Service interface for DetectorProperties functions
//
//  jpaley@fnal.gov
//
////////////////////////////////////////////////////////////////////////
#ifndef DETPROPERTIES_SERVICE_H
#define DETPROPERTIES_SERVICE_H

#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Principal/Run.h"
#include "DataProviders/DetectorPropertiesStandard.h"
#include "Utilities/DetectorPropertiesService.h"

///General LArSoft Utilities
namespace util{
  class DetectorPropertiesServiceStandard : public DetectorPropertiesService {

    public:
      DetectorPropertiesServiceStandard(fhicl::ParameterSet const& pset,
				art::ActivityRegistry& reg);

      virtual void   reconfigure(fhicl::ParameterSet const& pset) override;
      void   preProcessEvent(const art::Event& evt);
      void   postOpenFile(const std::string& filename);
      
      virtual const provider_type* provider() const override { return fProp.get();}
      
    private:

      std::unique_ptr<dataprov::DetectorPropertiesStandard> fProp;
      fhicl::ParameterSet   fPS;       ///< Original parameter set.
      
      bool  isDetectorPropertiesService(const fhicl::ParameterSet& ps);
      
    }; // class DetectorPropertiesService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE_IMPL(util::DetectorPropertiesServiceStandard, util::DetectorPropertiesService, LEGACY)
#endif // DETPROPERTIES_SERVICE_H
