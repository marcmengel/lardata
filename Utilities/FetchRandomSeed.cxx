/**
 * @file   FetchRandomSeed.cxx
 * @brief  Returns a random seed - implementation
 * @author prefers to stay unknown
 * @see    FetchRandomSeed.h
 */

#include "Utilities/FetchRandomSeed.h"

// C/C++ standard libraries
#include <chrono>

// framework libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Utilities/Exception.h"


//------------------------------------------------------------------------------
artext::SeedService::seed_t lar::util::FetchRandomSeed(
  std::string instance,
  fhicl::ParameterSet const* pset,
  std::string seedcfgname /* = "Seed" */
) {
  artext::SeedService::seed_t seed;
  
  // 1. if we have a parameter set to look for... well, look for it
  if (pset) {
    if (pset->get_if_present(seedcfgname, seed)) return seed;
  }
  
  // 2. check if SeedService is present... the hard way
  artext::SeedService* pSeedService = nullptr;
  try {
    pSeedService = art::ServiceHandle<artext::SeedService>().operator->();
  }
  catch (art::Exception const& e) {
    if (e.categoryCode() != art::errors::NotFound)
      throw; // whatever happened, it's not expected
    
    mf::LogError("FetchRandomSeed")
      << std::string(80, '*')
      << "\nSeedService SHOULD BE CONFIGURED!! please update your configuration."
      << "\nThe random seeds produced in this run will be of bad quality, just to punish you."
      << "\n" << std::string(80, '*');
  }
  if (pSeedService) return pSeedService->getSeed(instance);
  
  // 3. extract a number anew;
  static unsigned int increment = 0;
  constexpr artext::SeedService::seed_t MagicMaxSeed = 900000000;
  // the increment makes sure that if this function is called twice in a
  // row, at least the seed is not the same
  return (increment++ +
    std::chrono::system_clock::now().time_since_epoch().count())
    % MagicMaxSeed + 1;
} // lar::util::FetchRandomSeed()


//------------------------------------------------------------------------------
