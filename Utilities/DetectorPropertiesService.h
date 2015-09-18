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

///General LArSoft Utilities
namespace util{
  class DetectorPropertiesService {
    public:
      DetectorPropertiesService(fhicl::ParameterSet const& pset,
				art::ActivityRegistry& reg);
      virtual ~DetectorPropertiesService();

      void   reconfigure(fhicl::ParameterSet const& pset);
      void   preProcessEvent(const art::Event& evt);
      void   postOpenFile(std::string& filename);
      
      const  dataprov::DetectorProperties* getDetectorProperties() { return fProp.get();}
      
    private:

      std::unique_ptr<dataprov::DetectorProperties> fProp;
      fhicl::ParameterSet   fPS;       ///< Original parameter set.
      
      bool  isDetectorPropertiesService(const fhicl::ParameterSet& ps);
      
    }; // class DetectorPropertiesService
} //namespace utils
DECLARE_ART_SERVICE(util::DetectorPropertiesService, LEGACY)
#endif // DETPROPERTIES_SERVICE_H
