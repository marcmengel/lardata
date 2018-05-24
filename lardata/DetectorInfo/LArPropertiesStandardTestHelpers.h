/**
 * @file   LArPropertiesStandardTestHelpers.h
 * @brief  Helper functions for support of LArPropertiesService in LArSoft tests
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 5th, 2016
 * 
 * This library is a pure header.
 * It requires linkage with:
 * 
 * * `lardata_DetectorInfo`
 * * `mf_MessageLogger`
 * * `mf_Utilities`
 * * `fhiclcpp`
 * 
 */

#ifndef LARDATA_DETECTORINFO_LARPROPERTIESSTANDARDTESTHELPERS_H
#define LARDATA_DETECTORINFO_LARPROPERTIESSTANDARDTESTHELPERS_H 1

// LArSoft libraries
#include "lardataalg/DetectorInfo/LArPropertiesStandard.h"
#include "larcorealg/TestUtils/ProviderTestHelpers.h"

// framework and utility libraries
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>
#include <memory> // std::unique_ptr<>


namespace testing {
  
  /**
   * @brief Set up a detinfo::LArPropertiesStandard from a parameter set
   * @return pointer to a newly created and set up LArPropertiesService
   * 
   * This function specialization enables the support of `SetupProvider()`
   * methods of `testing::TesterEnvironment`.
   */
  template <>
  struct ProviderSetupClass<detinfo::LArPropertiesStandard> {
    
    static std::unique_ptr<detinfo::LArPropertiesStandard> setup
      (fhicl::ParameterSet const& pset)
      {
        // some feedback about whether we are using the right configuration
        std::string ServiceProviderPath;
        if (pset.get_if_present("service_provider", ServiceProviderPath)) {
          std::string ServiceProviderName = ServiceProviderPath;
          size_t iSlash = ServiceProviderPath.rfind('/');
          if (iSlash != std::string::npos)
            ServiceProviderName.erase(0, iSlash + 1);
          
          if (ServiceProviderName == "LArPropertiesServiceStandard") {
            LOG_TRACE("setupProvider")
              << "Verified service implementation for LArPropertiesService: '"
              << ServiceProviderPath << "'";
          }
          else {
            mf::LogWarning("setupProvider")
              << "This set up is for a LArPropertiesStandard provider.\n"
              "Your configuration specifies a '" << ServiceProviderPath
              << "' service implementation that is not known to use that provider.";
          }
        }
        
        //
        // create the new LArPropertiesStandard service provider
        //
        return std::make_unique<detinfo::LArPropertiesStandard>(pset);
      } // setup()
  }; // ProviderSetup<LArPropertiesStandard>
  
  
  /**
   * @brief Environment setup helper for LArPropertiesStandard
   * @tparam TestEnv type of environment to set up
   * @see simpleEnvironmentSetup
   * 
   * A service provider is set up in the environment, associated with the types
   * detinfo::LArPropertiesStandard and detinfo::LArProperties.
   * Its configuration is read from "services.LArPropertiesService".
   * 
   * The environment is expected to expose an interface equivalent to the one
   * of `testing::TesterEnvironment`.
   * 
   * This class specialisation enables the support of `SimpleProviderSetup()`
   * methods of `testing::TesterEnvironment`.
   * It should be possible to set up a testing environment by calling:
   *     
   *     env.SimpleProviderSetup<detinfo::LArPropertiesStandard>();
   *     
   * The provider will be available from any of these two calls:
   *     
   *     env.Provider<detinfo::LArPropertiesStandard>();
   *     env.Provider<detinfo::LArProperties>();
   *     
   *     
   */
  template <typename TestEnv>
  struct SimpleEnvironmentSetupClass<detinfo::LArPropertiesStandard, TestEnv> {
    static detinfo::LArPropertiesStandard* setup(TestEnv& env)
      {
        return SimpleEnvironmentStandardSetupByName
          <detinfo::LArPropertiesStandard, detinfo::LArProperties, TestEnv>
          (env, "LArPropertiesService");
      }
  }; // SimpleEnvironmentSetupClass<detinfo::LArPropertiesStandard>
  

} // namespace testing

  
#endif // LARDATA_DETECTORINFO_LARPROPERTIESSTANDARDTESTHELPERS_H
