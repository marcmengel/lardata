/**
 * @file   DetectorClocksStandardTestHelpers.h
 * @brief  Helper functions for support of DetectorClocksService in LArSoft tests
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 6th, 2016
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

#ifndef LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTESTHELPERS_H
#define LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTESTHELPERS_H 1

// LArSoft libraries
#include "lardata/DetectorInfo/DetectorClocksStandard.h"
#include "larcorealg/TestUtils/ProviderTestHelpers.h"

// framework and utility libraries
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>
#include <memory> // std::unique_ptr<>


namespace testing {
  
  /**
   * @brief Set up a detinfo::DetectorClocksStandard from a parameter set.
   * @return pointer to a newly created and set up DetectorClocksService
   * 
   * This function specialization enables the support of `SetupProvider()`
   * methods of `testing::TesterEnvironment`.
   */
  template <>
  struct ProviderSetupClass<detinfo::DetectorClocksStandard> {
    
    static std::unique_ptr<detinfo::DetectorClocksStandard> setup
      (fhicl::ParameterSet const& pset)
      {
        // some feedback about whether we are using the right configuration
        std::string ServiceProviderPath;
        if (pset.get_if_present("service_provider", ServiceProviderPath)) {
          std::string ServiceProviderName = ServiceProviderPath;
          size_t iSlash = ServiceProviderPath.rfind('/');
          if (iSlash != std::string::npos)
            ServiceProviderName.erase(0, iSlash + 1);
          
          if (ServiceProviderName == "DetectorClocksServiceStandard") {
            LOG_TRACE("setupProvider")
              << "Verified service implementation for DetectorClocksService: '"
              << ServiceProviderPath << "'";
          }
          else {
            mf::LogWarning("setupProvider")
              << "This set up is for a DetectorClocksStandard provider.\n"
              "Your configuration specifies a '" << ServiceProviderPath
              << "' service implementation"
              " that is not known to use that provider.";
          }
        }
        
        //
        // create the new DetectorClocksStandard service provider
        //
        auto detClocks
          = std::make_unique<detinfo::DetectorClocksStandard>(pset);
        
        //
        // do the actual setup (when issue #19446 is solved, this should become redundant)
        //
        detClocks->SetDefaultTriggerTime();
        
        //
        // all done
        //
        return detClocks;
      } // setup()
    
  }; // ProviderSetupClass<DetectorClocksStandard>
  
  
  /**
   * @brief Environment setup helper for DetectorClocksStandard.
   * @tparam TestEnv type of environment to set up
   * @see simpleEnvironmentSetup
   * 
   * A service provider is set up in the environment, associated with the types
   * detinfo::DetectorClocksStandard and detinfo::DetectorClocks.
   * Its configuration is read from "services.DetectorClocksService".
   * 
   * The environment is expected to expose an interface equivalent to the one
   * of `testing::TesterEnvironment`.
   * 
   * This class specialisation enables the support of `SimpleProviderSetup()`
   * methods of `testing::TesterEnvironment`.
   * It should be possible to set up a testing environment by calling:
   *     
   *     env.SimpleProviderSetup<detinfo::DetectorClocksStandard>();
   *     
   * The provider will be available from any of these two calls:
   *     
   *     env.Provider<detinfo::DetectorClocksStandard>();
   *     env.Provider<detinfo::DetectorClocks>();
   *     
   *     
   */
  template <typename TestEnv>
  struct SimpleEnvironmentSetupClass<detinfo::DetectorClocksStandard, TestEnv> {
    static detinfo::DetectorClocksStandard* setup(TestEnv& env)
      {
        return SimpleEnvironmentStandardSetupByName
          <detinfo::DetectorClocksStandard, detinfo::DetectorClocks, TestEnv>
          (env, "DetectorClocksService");
      }
  };
  

} // namespace testing

  
#endif // LARDATA_DETECTORINFO_DETECTORCLOCKSSTANDARDTESTHELPERS_H
