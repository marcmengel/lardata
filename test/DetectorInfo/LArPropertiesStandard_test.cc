/**
 * @file   LArPropertiesStandard_test.cc
 * @brief  Simple instantiation-only test for LArPropertiesStandard
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 1st, 2015
 */

// LArSoft libraries
#include "larcorealg/TestUtils/unit_test_base.h"
#include "lardataalg/DetectorInfo/LArPropertiesStandard.h"
#include "lardataalg/DetectorInfo/LArPropertiesStandardTestHelpers.h"


//------------------------------------------------------------------------------
//---  The test environment
//---

/*
 * TesterEnvironment, configured with a "standard" configuration object, is used
 * in a non-Boost-unit-test context.
 * It provides:
 * - `detinfo::LArProperties const* Provider<detinfo::LArProperties>()`
 * 
 */
using TestEnvironment
  = testing::TesterEnvironment<testing::BasicEnvironmentConfiguration>;


//------------------------------------------------------------------------------
//---  The tests
//---


/** ****************************************************************************
 * @brief Runs the test
 * @param argc number of arguments in argv
 * @param argv arguments to the function
 * @return number of detected errors (0 on success)
 * @throw cet::exception most of error situations throw
 * 
 * The arguments in argv are:
 * 0. name of the executable ("LArPropertiesStandard_test")
 * 1. (mandatory) path to the FHiCL configuration file
 * 2. FHiCL path to the configuration of the test
 *    (default: physics.analyzers.larptest)
 * 3. FHiCL path to the configuration of LArProperties service
 *    (default: services.LArPropertiesService)
 * 
 */
//------------------------------------------------------------------------------
int main(int argc, char const** argv) {
  
  testing::BasicEnvironmentConfiguration config("larp_test");
  
  //
  // parameter parsing
  //
  int iParam = 0;
  
  // first argument: configuration file (mandatory)
  if (++iParam < argc) config.SetConfigurationPath(argv[iParam]);
  else {
    std::cerr << "FHiCL configuration file path required as first argument!"
      << std::endl;
    return 1;
  }
  
  // second argument: path of the parameter set for geometry test configuration
  // (optional; default does not have any tester)
  if (++iParam < argc) config.SetMainTesterParameterSetPath(argv[iParam]);
  
  // third argument: path of the parameter set for LArProperties configuration
  // (optional; default: "services.LArProperties" from the inherited object)
  if (++iParam < argc)
    config.SetServiceParameterSetPath("LArPropertiesService", argv[iParam]);
  
  
  unsigned int nErrors = 0 /* Tester.Run() */ ;
  
  //
  // testing environment setup
  //
  TestEnvironment TestEnv(config);
  
  // LArPropertiesStandard supports the simple set up; so we invoke it
  TestEnv.SimpleProviderSetup<detinfo::LArPropertiesStandard>();
  
  //
  // run the test algorithm
  // (I leave it here for reference -- there is no test algorithm here)
  //
  
  // 1. we initialize it from the configuration in the environment,
//  MyTestAlgo Tester(TestEnv.TesterParameters());
  
  // 2. we set it up with the geometry from the environment
//  Tester.Setup(*(TestEnv.LArProperties()));
  
  // 3. then we run it!
  mf::LogInfo("larp_test")
    << "The atomic number of liquid argon is "
    << TestEnv.Provider<detinfo::LArProperties>()->AtomicNumber()
    ;
  
  // 4. And finally we cross fingers.
  if (nErrors > 0) {
    mf::LogError("larp_test") << nErrors << " errors detected!";
  }
  
  return nErrors;
} // main()
