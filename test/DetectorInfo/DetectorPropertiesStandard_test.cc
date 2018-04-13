/**
 * @file   DetectorPropertiesStandard_test.cc
 * @brief  Simple instantiation-only test for DetectorPropertiesStandard
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 6th, 2016
 */

// LArSoft libraries
#include "lardata/DetectorInfo/LArPropertiesStandardTestHelpers.h"
#include "lardata/DetectorInfo/DetectorClocksStandardTestHelpers.h"
#include "lardata/DetectorInfo/DetectorPropertiesStandard.h"
#include "lardata/DetectorInfo/DetectorPropertiesStandardTestHelpers.h"
#include "larcorealg/Geometry/ChannelMapStandardAlg.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcorealg/CoreUtils/RealComparisons.h"
#include "test/Geometry/geometry_unit_test_base.h"

// C/C++ standard libraries
#include <iomanip>
#include <vector>
#include <array>

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
  auto const& geom = *(TestEnv.Provider<geo::GeometryCore>());
  auto const& detp = *(TestEnv.Provider<detinfo::DetectorProperties>());
  
  auto const driftVelocity = detp.DriftVelocity();
  auto const TDCtick = detp.SamplingRate();

  mf::LogVerbatim("detp_test")
    <<   "Electric field in the active volume: " << detp.Efield() << " kV/cm"
    << "\nSampling rate:     " << TDCtick << " ns"
    << "\nArgon temperature: " << detp.Temperature() << " K"
    << "\nDrift velocity:    " << driftVelocity << " cm/us";
  
  // accumulate the plane IDs; needed just for table formatting
  unsigned int const colWidth = 9U;
  unsigned int headerColWidth = 0U;
  for (auto planeID: geom.IteratePlaneIDs()) {
    auto const l = std::string(planeID).length();
    if (headerColWidth < l) headerColWidth = l;
  }
  
  // print drift distances
  // collect all drift distances, and check whether they are all equal
  bool allSameDrift = true;
  lar::util::RealComparisons<double> check(1.0); // 1 cm tolerance
  auto driftDistances = geom.makeTPCData<double>();
  for (auto const& TPC: geom.IterateTPCs()) {
    auto const driftDistance = TPC.DriftDistance();
    driftDistances[TPC.ID()] = driftDistance;
    allSameDrift
      = allSameDrift & check.equal(driftDistance, driftDistances.first());
  } // for
  
  if (allSameDrift) {
    // print drift distance
    auto const driftDistance = driftDistances.first();
    auto const driftTime = driftDistance / driftVelocity;
    mf::LogVerbatim("detp_test")
      <<   "Drift distance:    " << driftDistance << " cm"
      << "\nDrift time:        " << driftTime << " us, "
                                 << (driftTime / (TDCtick / 1000.)) << " ticks"
      ;
  }
  else {
    // print drift distance table header
    mf::LogVerbatim log("detp_test");
    std::array<unsigned int, 4U> const columnSizes
      = {{ headerColWidth, 9U, 13U, 7U }};
    log << std::setw(columnSizes[0]) << "Drift:"
      << " | " << std::setw(columnSizes[1]) << "time [us]"
      << " | " << std::setw(columnSizes[2]) << "distance [cm]"
      << " | " << std::setw(columnSizes[3]) << "ticks";
    
    // print drift distances by TPC
    for (auto const& TPCID: geom.IterateTPCIDs()) {
      auto const driftDistance = driftDistances[TPCID];
      auto const driftTime = driftDistance / driftVelocity;
      log << "\n" << std::setw(columnSizes[0]) << TPCID
        << " | " << std::setw(columnSizes[1]) << driftTime
        << " | " << std::setw(columnSizes[2]) << driftDistance
        << " | " << std::setw(columnSizes[3]) << (driftTime / (TDCtick / 1000.))
        ;
    } // for TPC
  }
  
  // print tick number table header
  // number of ticks
  unsigned int const nTicks = detp.NumberTimeSamples();
  constexpr unsigned int nPrintedTicks = 11;
  mf::LogVerbatim("detp_test")
    << "Conversion of tick number to x position [cm]"
    " (detinfo::DetectorProperties::ConvertTicksToX()):";
  
  std::array<double, nPrintedTicks> ticks;
  for (unsigned int iTick = 0; iTick < nPrintedTicks; ++iTick)
    ticks[iTick] = iTick * nTicks / (nPrintedTicks - 1);
  
  {
    mf::LogVerbatim log("detp_test");
    log << std::setw(headerColWidth) << "tick #:";
    for (double tick: ticks) log << " | " << std::setw(colWidth) << tick;
  }
  
  for (auto planeID: geom.IteratePlaneIDs()) {
    mf::LogVerbatim log("detp_test");
    log << std::setw(headerColWidth) << std::string(planeID);
    
    for (double tick: ticks) {
      double const x = detp.ConvertTicksToX(tick, planeID);
      log << " | " << std::setw(colWidth) << x;
    } // for tick
  } // for planes
  
  // 4. And finally we cross fingers.
  if (nErrors > 0) {
    mf::LogError("detp_test") << nErrors << " errors detected!";
  }
  
  return nErrors;
} // main()
