/**
 * @file   FetchRandomSeed.cxx
 * @brief  Returns a random seed - implementation
 * @author prefers to stay unknown
 * @see    FetchRandomSeed.h
 */

#include "Utilities/FetchRandomSeed.h"

// C/C++ standard libraries
#include <chrono>
#include <sstream>
#include <functional> // std::hash<>

// framework libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/System/CurrentModule.h"
#include "art/Utilities/Exception.h"

namespace lar {
  namespace util {
    namespace details {
     
     std::string UniqueEventIDString(art::EventID const& ID) {
       std::ostringstream sstr;
       sstr << "Run: " << ID.run() << " Subrun: " << ID.subRun()
         << " Event: " << ID.event();
       return sstr.str();
     } // lar::util::details::UniqueEventIDString()
     
     
     std::string UniqueEventString(art::Event const& event) {
       std::ostringstream sstr;
       sstr << " Timestamp: " << event.time().value();
       return UniqueEventIDString(event.id()) + sstr.str();
     } // lar::util::details::UniqueEventString()
     
     std::string UniqueEventModuleString
       (art::Event const& event, std::string instance = "")
     {
       art::ServiceHandle<art::CurrentModule> CM;
       std::string s = UniqueEventString(event) + " Module: " + CM->label();
       if (!instance.empty()) s.append(" Instance: ").append(instance);
       return s;
     } // lar::util::details::UniqueEventModuleString()
    
    } // namespace details
  } // namespace util
} // namespace lar


//------------------------------------------------------------------------------
artext::SeedService::seed_t lar::util::FetchRandomSeed(
  std::string instance,
  fhicl::ParameterSet const* pset,
  std::initializer_list<std::string> seedcfgnames
) {
  artext::SeedService::seed_t seed;
  
  // 1. if we have a parameter set to look for... well, look for it
  if (pset) {
    for (std::string seedcfgname: seedcfgnames)
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
  // the increment makes sure that if this function is called twice in a
  // row, at least the seed is not the same
  return ValidSeed
    (increment++ + std::chrono::system_clock::now().time_since_epoch().count());
} // lar::util::FetchRandomSeed()


artext::SeedService::seed_t lar::util::FetchEventRandomSeed
  (art::Event const& event, std::string instance /* = "" */)
{
  return ValidSeed(
    std::hash<std::string>()(details::UniqueEventModuleString(event, instance))
    );
} // lar::util::FetchEventRandomSeed()


//------------------------------------------------------------------------------
