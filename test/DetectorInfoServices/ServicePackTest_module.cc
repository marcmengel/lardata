/**
 * @file   ServicePackTest_module.cc
 * @brief  Tests utilities in ServicePack.h
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 23, 2015
 * @see    ServicePack.h
 */

// LArSoft libraries
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "lardata/DetectorInfoServices/ServicePack.h" // lar::providerFrom<>()
#include "lardataalg/DetectorInfo/LArProperties.h"
#include "lardataalg/DetectorInfo/DetectorProperties.h"
#include "lardataalg/DetectorInfo/DetectorClocks.h"
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcore/CoreUtils/ServiceUtil.h" // lar::providerFrom<>()

// framework libraries
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// C/C++ standard libraries
#include <string>
#include <sstream>

namespace fhicl {
  class ParameterSet; //
} // namespace fhicl


namespace { // local

  template <typename T>
  inline std::string to_string(T const& v) { return std::to_string(v); }

  template <typename T>
  std::string to_string(T const* ptr) {
    std::ostringstream sstr;
    sstr << '<' << ((void*) ptr) << '>';
    return sstr.str();
  } // to_string()

} // local namespace


namespace lar {

  /**
   * @brief Test module for ServicePack.h utilities depending on art farmework
   *
   * Currently exercises:
   * - `lar::extractProviders()`
   *
   * Throws an exception on failure.
   *
   * Service requirements
   * =====================
	*
	* This module requires the following services to be configured:
	* - Geometry
	* - LArPropertiesService
	* - DetectorClocksService
	* - DetectorPropertiesService
   *
   * Configuration parameters
   * =========================
   *
   * Currently none.
   *
   */
  class ServicePackTest: public art::EDAnalyzer {

      public:

    /// Constructor
    explicit ServicePackTest(fhicl::ParameterSet const&);

    /// Run event-independent tests
    virtual void beginJob() override;

    /// Run event-dependent tests (none so far)
    virtual void analyze(const art::Event& /* evt */) override {}

    /// Throws if errors have been accumulated
    virtual void endJob() override;


    /// @{
    /// @name Test functions

    /// Tests lar::extractProviders()
    void extractProviders_test_plain();

    /// Tests lar::extractProviders() and permuted constructor
    void extractProviders_test_permuted();

    /// Tests lar::extractProviders() and assignment to reduced pack
    void extractProviders_test_reduced();

    /// All tests on lar::extractProviders()
    void extractProviders_tests();

    /// @}

      private:
    std::vector<std::string> errors; ///< list of collected errors

  }; // ServicePackTest

  DEFINE_ART_MODULE(ServicePackTest)

} // namespace lar

//------------------------------------------------------------------------------
//--- implementation
//---
namespace lar {

  //----------------------------------------------------------------------------
  ServicePackTest::ServicePackTest(const fhicl::ParameterSet& pset)
    : EDAnalyzer(pset)
    {}


  //----------------------------------------------------------------------------
  void ServicePackTest::beginJob() {
    extractProviders_tests();
  } // ServicePackTest::beginJob()


  //----------------------------------------------------------------------------
  void ServicePackTest::endJob() {
    if (errors.empty()) {
      mf::LogInfo("ServicePackTest") << "All tests were successful.";
      return;
    }
    else {

      mf::LogError log("ServicePackTest");
      log << errors.size() << " errors detected:";

      for (std::string const& error: errors)
        log << "\n - " << error;

      throw art::Exception(art::errors::LogicError)
        << errors.size() << " errors detected";

    } // if ... else

  } // ServicePackTest::endJob()


  //----------------------------------------------------------------------------
  void ServicePackTest::extractProviders_tests() {

    extractProviders_test_plain();
    extractProviders_test_permuted();
    extractProviders_test_reduced();

  } // ServicePackTest::extractProviders_tests()

  //----------------------------------------------------------------------------
  void ServicePackTest::extractProviders_test_plain() {

    /*
     * The test creates a ProviderPack and checks that its element as as
     * expected.
     *
     * The expected value is extracted from the framework in the "traditional"
     * way.
     *
     */

    // these are the "solutions":
    geo::GeometryCore const* geom
      = lar::providerFrom<geo::Geometry>();
    detinfo::LArProperties const* larprop
      = lar::providerFrom<detinfo::LArPropertiesService>();
    detinfo::DetectorClocks const* detclocks
      = lar::providerFrom<detinfo::DetectorClocksService>();
    detinfo::DetectorProperties const* detprop
      = lar::providerFrom<detinfo::DetectorPropertiesService>();

    lar::ProviderPack<
      geo::GeometryCore,
      detinfo::LArProperties,
      detinfo::DetectorClocks,
      detinfo::DetectorProperties
      > providers
      = lar::extractProviders<
        geo::Geometry,
        detinfo::LArPropertiesService,
        detinfo::DetectorClocksService,
        detinfo::DetectorPropertiesService
        >();

    // check time
    if (providers.get<geo::GeometryCore>() != geom) {
      errors.push_back("wrong geometry provider (got "
        + ::to_string(providers.get<geo::GeometryCore>())
        + ", expected " + ::to_string(geom)
        + ")");
    }
    if (providers.get<detinfo::LArProperties>() != larprop) {
      errors.push_back("wrong LAr properties provider (got "
        + ::to_string(providers.get<detinfo::LArProperties>())
        + ", expected " + ::to_string(larprop)
        + ")");
    }
    if (providers.get<detinfo::DetectorClocks>() != detclocks) {
      errors.push_back("wrong detector clocks provider (got "
        + ::to_string(providers.get<detinfo::DetectorClocks>())
        + ", expected " + ::to_string(detclocks)
        + ")");
    }
    if (providers.get<detinfo::DetectorProperties>() != detprop) {
      errors.push_back("wrong detector properties provider (got "
        + ::to_string(providers.get<detinfo::DetectorProperties>())
        + ", expected " + ::to_string(detprop)
        + ")");
    }

  } // ServicePackTest::extractProviders_test_plain()


  //----------------------------------------------------------------------------
  void ServicePackTest::extractProviders_test_permuted() {

    /*
     * The test creates a ProviderPack and checks that its element as as
     * expected.
     *
     * The expected value is extracted from the framework in the "traditional"
     * way.
     *
     * The order of the providers is different from the order of the services;
     * in this way a "wrong" ProviderPack will be (deliberately) created,
     * and the code will have to convert it to the right pack.
     */

    // these are the "solutions":
    geo::GeometryCore const* geom
      = lar::providerFrom<geo::Geometry>();
    detinfo::LArProperties const* larprop
      = lar::providerFrom<detinfo::LArPropertiesService>();
    detinfo::DetectorClocks const* detclocks
      = lar::providerFrom<detinfo::DetectorClocksService>();
    detinfo::DetectorProperties const* detprop
      = lar::providerFrom<detinfo::DetectorPropertiesService>();

    lar::ProviderPack<
      detinfo::LArProperties,
      detinfo::DetectorClocks,
      detinfo::DetectorProperties,
      geo::GeometryCore
      > providers
      = lar::extractProviders<
        geo::Geometry,
        detinfo::LArPropertiesService,
        detinfo::DetectorClocksService,
        detinfo::DetectorPropertiesService
        >();

    // check time
    if (providers.get<geo::GeometryCore>() != geom) {
      errors.push_back("wrong geometry provider (got "
        + ::to_string(providers.get<geo::GeometryCore>())
        + ", expected " + ::to_string(geom)
        + ") [permuted]");
    }
    if (providers.get<detinfo::LArProperties>() != larprop) {
      errors.push_back("wrong LAr properties provider (got "
        + ::to_string(providers.get<detinfo::LArProperties>())
        + ", expected " + ::to_string(larprop)
        + ") [permuted]");
    }
    if (providers.get<detinfo::DetectorClocks>() != detclocks) {
      errors.push_back("wrong detector clocks provider (got "
        + ::to_string(providers.get<detinfo::DetectorClocks>())
        + ", expected " + ::to_string(detclocks)
        + ") [permuted]");
    }
    if (providers.get<detinfo::DetectorProperties>() != detprop) {
      errors.push_back("wrong detector properties provider (got "
        + ::to_string(providers.get<detinfo::DetectorProperties>())
        + ", expected " + ::to_string(detprop)
        + ") [permuted]");
    }

  } // ServicePackTest::extractProviders_test_permuted()


  //----------------------------------------------------------------------------
  void ServicePackTest::extractProviders_test_reduced() {

    /*
     * The test creates a ProviderPack and checks that its element as as
     * expected.
     *
     * The expected value is extracted from the framework in the "traditional"
     * way.
     *
     * We use a smaller provider pack to store the providers;
     * DetectorProperties will be dropped.
     *
     */

    // these are the "solutions":
    geo::GeometryCore const* geom
      = lar::providerFrom<geo::Geometry>();
    detinfo::LArProperties const* larprop
      = lar::providerFrom<detinfo::LArPropertiesService>();
    detinfo::DetectorClocks const* detclocks
      = lar::providerFrom<detinfo::DetectorClocksService>();

    lar::ProviderPack<
      detinfo::LArProperties,
      detinfo::DetectorClocks,
      geo::GeometryCore
      > providers
      = lar::extractProviders<
        geo::Geometry,
        detinfo::LArPropertiesService,
        detinfo::DetectorClocksService,
        detinfo::DetectorPropertiesService
        >();

    // check time
    if (providers.get<geo::GeometryCore>() != geom) {
      errors.push_back("wrong geometry provider (got "
        + ::to_string(providers.get<geo::GeometryCore>())
        + ", expected " + ::to_string(geom)
        + ") [reduced]");
    }
    if (providers.get<detinfo::LArProperties>() != larprop) {
      errors.push_back("wrong LAr properties provider (got "
        + ::to_string(providers.get<detinfo::LArProperties>())
        + ", expected " + ::to_string(larprop)
        + ") [reduced]");
    }
    if (providers.get<detinfo::DetectorClocks>() != detclocks) {
      errors.push_back("wrong detector clocks provider (got "
        + ::to_string(providers.get<detinfo::DetectorClocks>())
        + ", expected " + ::to_string(detclocks)
        + ") [reduced]");
    }
    if (providers.has<detinfo::DetectorProperties>()) {
      errors.push_back("detector properties provider should not be there!");
    }


  } // ServicePackTest::extractProviders_test_reduced()


  //----------------------------------------------------------------------------

} // namespace lar
