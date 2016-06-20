/**
 * @file   DetectorPropertiesStandard_test.cc
 * @brief  Simple instantiation-only test for DetectorPropertiesStandard
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 6th, 2016
 */

// LArSoft libraries
#include "test/Geometry/geometry_unit_test_base.h"
#include "larcore/Geometry/ChannelMapStandardAlg.h"
#include "lardata/DetectorInfo/LArPropertiesStandardTestHelpers.h"
#include "lardata/DetectorInfo/DetectorClocksStandardTestHelpers.h"
#include "lardata/DetectorInfo/DetectorPropertiesStandard.h"
#include "lardata/DetectorInfo/DetectorPropertiesStandardTestHelpers.h"


//------------------------------------------------------------------------------
//---  The test environment
//---

/*
 * GeometryTesterEnvironment, configured with a geometry-aware configuration
 * object, is used in a non-Boost-unit-test context.
 * It provides:
 * - `detinfo::DetectorProperties const* Provider<detinfo::DetectorProperties>()`
 * - all the other services configured as dependencies
 */
using TesterConfiguration
  = testing::BasicGeometryEnvironmentConfiguration<geo::ChannelMapStandardAlg>;
using TestEnvironment = testing::GeometryTesterEnvironment<TesterConfiguration>;


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
 * 0. name of the executable ("DetectorPropertiesStandard_test")
 * 1. (mandatory) path to the FHiCL configuration file
 * 2. FHiCL path to the configuration of the test
 *    (default: physics.analyzers.larptest)
 * 3. FHiCL path to the configuration of DetectorProperties service
 *    (default: services.DetectorPropertiesService)
 * 
 */
//------------------------------------------------------------------------------
int main(int argc, char const** argv) {
  
  TesterConfiguration config("detp_test");
  
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
  
  // third argument: path of the parameter set for DetectorProperties confi
  // (optional; default: "services.DetectorProperties" from inherited object)
  if (++iParam < argc) {
    config.SetServiceParameterSetPath
      ("DetectorPropertiesService", argv[iParam]);
  }
  
  unsigned int nErrors = 0 /* Tester.Run() */ ;
  
  //
  // testing environment setup
  //
  TestEnvironment TestEnv(config);
  
  // DetectorPropertiesStandard and all its dependencies support the simple set
  // up (see testing::TesterEnvironment::SimpleProviderSetup()), except for
  // Geometry, that has been configured already in the geometry-aware
  // environment. So we invoke a simple set up for each of the dependencies:
  TestEnv.SimpleProviderSetup<detinfo::LArPropertiesStandard>();
  TestEnv.SimpleProviderSetup<detinfo::DetectorClocksStandard>();
  TestEnv.SimpleProviderSetup<detinfo::DetectorPropertiesStandard>();
  
  //
  // run the test algorithm
  // (I leave it here for reference -- there is no test algorithm here)
  //
  
  // 1. we initialize it from the configuration in the environment,
//  MyTestAlgo Tester(TestEnv.TesterParameters());
  
  // 2. we set it up with the geometry from the environment
//  Tester.Setup(*(TestEnv.Provider<detinfo::DetectorProperties>()));
  
  // 3. then we run it!
  mf::LogVerbatim("detp_test")
    << "Electric field in the active volume: "
    << TestEnv.Provider<detinfo::DetectorProperties>()->Efield() << " kV/cm"
    ;
  
  // 4. And finally we cross fingers.
  if (nErrors > 0) {
    mf::LogError("detp_test") << nErrors << " errors detected!";
  }
  
  return nErrors;
} // main()
