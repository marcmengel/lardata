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
#include "DataProviders/DetectorProperties.h"
#include "Utilities/IDetectorPropertiesService.h"

///General LArSoft Utilities
namespace util{
  class DetectorPropertiesService : public IDetectorPropertiesService {

    public:
      DetectorPropertiesService(fhicl::ParameterSet const& pset,
				art::ActivityRegistry& reg);

      virtual void   reconfigure(fhicl::ParameterSet const& pset) override;
      void   preProcessEvent(const art::Event& evt);
      void   postOpenFile(const std::string& filename);
      
      virtual const provider_type* provider() const override { return fProp.get();}
      
    private:

      std::unique_ptr<dataprov::DetectorProperties> fProp;
      fhicl::ParameterSet   fPS;       ///< Original parameter set.
      
      bool  isDetectorPropertiesService(const fhicl::ParameterSet& ps);
      
    }; // class DetectorPropertiesService
} //namespace utils
DECLARE_ART_SERVICE_INTERFACE_IMPL(util::DetectorPropertiesService, util::IDetectorPropertiesService, LEGACY)
#endif // DETPROPERTIES_SERVICE_H
