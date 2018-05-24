/**
 * @file   DetectorClocksStandard_test.cc
 * @brief  Simple instantiation-only test for DetectorClocksStandard
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 6th, 2016
 */

// LArSoft libraries
#include "larcorealg/TestUtils/unit_test_base.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandard.h"
#include "lardataalg/DetectorInfo/DetectorClocksStandardTestHelpers.h"


//------------------------------------------------------------------------------
//---  The test environment
//---

/*
 * TesterEnvironment, configured with a "standard" configuration object, is used
 * in a non-Boost-unit-test context.
 * It provides:
 * - `detinfo::DetectorClocks const* Provider<detinfo::DetectorClocks>()`
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
 * 0. name of the executable ("DetectorClocksStandard_test")
 * 1. (mandatory) path to the FHiCL configuration file
 * 2. FHiCL path to the configuration of the test
 *    (default: physics.analyzers.larptest)
 * 3. FHiCL path to the configuration of DetectorClocks service
 *    (default: services.DetectorClocksService)
 * 
 */
//------------------------------------------------------------------------------
int main(int argc, char const** argv) {
  
  testing::BasicEnvironmentConfiguration config("clocks_test");
  
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
  
  // third argument: path of the parameter set for DetectorClocks configuration
  // (optional; default: "services.DetectorClocks" from the inherited object)
  if (++iParam < argc)
    config.SetServiceParameterSetPath("DetectorClocksService", argv[iParam]);
  
  
  unsigned int nErrors = 0 /* Tester.Run() */ ;
  
  //
  // testing environment setup
  //
  TestEnvironment TestEnv(config);
  
  // DetectorClocksStandard supports the simple set up; so we invoke it
  TestEnv.SimpleProviderSetup<detinfo::DetectorClocksStandard>();
  
  // we print some information from the service itself;
  // this is specific to the implementation, so we need to talk
  // to the concrete type; this is not the way we would normally operate:
  TestEnv.Provider<detinfo::DetectorClocksStandard>()->debugReport();
  
  //
  // run the test algorithm
  // (I leave it here for reference -- there is no test algorithm here)
  //
  
  // 1. we initialize it from the configuration in the environment,
//  MyTestAlgo Tester(TestEnv.TesterParameters());
  
  // 2. we set it up with the geometry from the environment
//  Tester.Setup(*(TestEnv.Provider<detinfo::DetectorClocks>()));
  
  // 3. then we run it!
  // Note that here we are querying the abstract DetectorClocks interface;
  // this is the right way to go.
  detinfo::DetectorClocks const* detClocks
    = TestEnv.Provider<detinfo::DetectorClocks>();
  mf::LogVerbatim("clocks_test")
    << "TPC clock period: " << detClocks->TPCClock().FramePeriod() << " us";
  
  // here we cheat and use the knowledge of which implementation we are using
  // (need to use pointers to use the feature of nullptr on conversion failure)
  auto const* detClocksStd
    = dynamic_cast<detinfo::DetectorClocksStandard const*>(detClocks);
  if (detClocksStd) {
    detClocksStd->debugReport();
  }
  else {
    mf::LogWarning("clocks_test")
      << "Can't run DetectorClocksStandard-specific diagnostics.";
  }

  // 4. And finally we cross fingers.
  if (nErrors > 0) {
    mf::LogError("clocks_test") << nErrors << " errors detected!";
  }
  
  return nErrors;
} // main()
