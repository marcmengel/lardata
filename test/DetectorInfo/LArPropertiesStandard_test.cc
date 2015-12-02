/**
 * @file   LArPropertiesStandard_test.cc
 * @brief  Simple instantiation-only test for LArPropertiesStandard
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 1st, 2015
 */

// LArSoft test libraries
#include "test/DetectorInfo/larproperties_unit_test_base.h"

// LArSoft libraries
#include "DetectorInfo/LArPropertiesStandard.h"

//------------------------------------------------------------------------------
//---  The test environment
//---

// we define here all the configuration that is needed;
// we use an existing class provided for this purpose, since our test
// environment allows us to tailor it at run time.
using LArPropertiesStandardConfiguration
  = testing::BasicLArPropertiesEnvironmentConfiguration<detinfo::LArPropertiesStandard>;

/*
 * LArPropertiesTesterEnvironment, configured with the object above, is used in
 * a non-Boost-unit-test context.
 * It provides:
 * - `detinfo::LArProperties const* LArProperties()`
 * - `detinfo::LArProperties const* GlobalLArProperties()` (static member)
 */
using LArPropertiesStandardTestEnvironment
  = testing::LArPropertiesTesterEnvironment<LArPropertiesStandardConfiguration>;


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
 * 0. name of the executable ("Geometry_test")
 * 1. path to the FHiCL configuration file
 * 2. FHiCL path to the configuration of the geometry test
 *    (default: physics.analysers.geotest)
 * 3. FHiCL path to the configuration of the geometry
 *    (default: services.Geometry)
 * 
 */
//------------------------------------------------------------------------------
int main(int argc, char const** argv) {
  
  LArPropertiesStandardConfiguration config("larp_test");
  
  //
  // parameter parsing
  //
  int iParam = 0;
  
  // first argument: configuration file (mandatory)
  if (++iParam < argc) config.SetConfigurationPath(argv[iParam]);
  
  // second argument: path of the parameter set for geometry test configuration
  // (optional; default: "physics.analysers.geotest")
  config.SetMainTesterParameterSetPath
    ((++iParam < argc)? argv[iParam]: "physics.analyzers.larptest");
  
  // third argument: path of the parameter set for LArProperties configuration
  // (optional; default: "services.LArProperties" from the inherited object)
  if (++iParam < argc) config.SetLArPropertiesParameterSetPath(argv[iParam]);
  
  //
  // testing environment setup
  //
  LArPropertiesStandardTestEnvironment TestEnvironment(config);
  
  //
  // run the test algorithm
  // (I leave it here for reference -- there is no test algorithm here)
  //
  
  // 1. we initialize it from the configuration in the environment,
//  MyTestAlgo Tester(TestEnvironment.TesterParameters());
  
  // 2. we set it up with the geometry from the environment
//  Tester.Setup(*TestEnvironment.LArProperties());
  
  // 3. then we run it!
  mf::LogInfo("larp_test")
    << "The atomic number of liquid argon is "
    << TestEnvironment.LArProperties()->AtomicNumber()
    ;
  unsigned int nErrors = 0 /* Tester.Run() */ ;
  
  // 4. And finally we cross fingers.
  if (nErrors > 0) {
    mf::LogError("larp_test") << nErrors << " errors detected!";
  }
  
  return nErrors;
} // main()
