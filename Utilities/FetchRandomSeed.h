/**
 * @file   FetchRandomSeed.h
 * @brief  Returns a random seed
 * @author prefers to stay unknown
 * @see    FetchRandomSeed.cxx
 */

#ifndef FETCHRANDOMSEED_H
#define FETCHRANDOMSEED_H 1

// C/C++ standard libraries
#include <string>

// framework libraries
#include "fhiclcpp/ParameterSet.h"
#include "artextensions/SeedService/SeedService.hh"


namespace lar {
  namespace util {
    
    //@{
    /** ************************************************************************
     * @brief Retrieves a random seed
     * @param instance engine instance name
     * @param pset parameter set to get the seed from
     * @param seedcfgname name of the seed parameter in the confiugration
     * @return a random seed
     *
     * This function returns a random seed, looking for it in sequence:
     * -# in the specified parameter set: if the parameter set is specified,
     *    we look for a configuration item seedcfgname and we return its
     *    integral value
     * -# ask the seed service: if the SeedService is configured, *as it should*,
     *    it will return the seed we are looking for, for the specified instance
     * -# extract the seed ourself; this is based taken from the clock
     *
     *
     */
    artext::SeedService::seed_t FetchRandomSeed(
      std::string instance,
      fhicl::ParameterSet const* pset,
      std::string seedcfgname = "Seed"
      );
    
    /**
     * @brief Retrieves a random seed
     * @param pset parameter set to get the seed from
     * @param seedcfgname name of the seed parameter in the confiugration
     * @return a random seed
     *
     * This function returns a random seed, looking for it in sequence:
     * -# in the specified parameter set: if the parameter set is specified,
     *    we look for a configuration item seedcfgname and we return its
     *    integral value
     * -# ask the seed service: if the SeedService is configured, *as it should*,
     *    it will return the seed we are looking for
     * -# extract the seed ourself; this is based taken from the clock
     *
     *
     */
    inline artext::SeedService::seed_t FetchRandomSeed(
      fhicl::ParameterSet const* pset,
      std::string seedcfgname = "Seed"
      )
      { return FetchRandomSeed("", pset, seedcfgname); }
    
    /**
     * @brief Retrieves a random seed
     * @param instance engine instance name (default: none)
     * @return a random seed
     *
     * This function returns a random seed, looking for it in sequence:
     * -# ask the seed service: if the SeedService is configured, *as it should*,
     *    it will return the seed we are looking for
     * -# extract the seed ourself; this is based taken from the clock
     *
     *
     */
    inline artext::SeedService::seed_t FetchRandomSeed
      (std::string instance = "")
      { return FetchRandomSeed(instance, nullptr); }
    //@}
    
  } // namespace util
} // namespace lar

#endif // FETCHRANDOMSEED_H
