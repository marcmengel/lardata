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
#include <initializer_list>

// framework libraries
#include "fhiclcpp/ParameterSet.h"
#include "artextensions/SeedService/SeedService.hh"
#include "art/Framework/Principal/Event.h"


namespace lar {
  namespace util {
    
    
    /// Class storing a seed in the valid range
    class ValidSeed {
        public:
      using seed_t = artext::SeedService::seed_t; ///< type of random seed
      
      static constexpr seed_t Min =         1; ///< Smallest allowed seed
      static constexpr seed_t Max = 900000000; ///< Largest allowed seed
      
      /// Forces the specified value into the allowed seed range
      template <typename T>
      static constexpr seed_t MakeValid(T s)
        { return Min + seed_t(s) % (Max - Min + 1); }
      
      /// Constructor: converts from a value
      template <typename T>
      ValidSeed(T s): seed(MakeValid(s)) {}
      
      ValidSeed(const ValidSeed&) = delete;
      ValidSeed(ValidSeed&&) = default;
      ValidSeed& operator= (const ValidSeed&) = delete;
      ValidSeed& operator= (ValidSeed&&) = default;
      
      /// Return the converted seeda
      operator seed_t() const { return seed; }
      
        protected:
      seed_t seed; ///< the converted seed
    }; // class ValidSeed
    
    
    
    //@{
    /** ************************************************************************
     * @brief Retrieves a random seed
     * @param instance engine instance name
     * @param pset parameter set to get the seed from
     * @param seedcfgnames names of possible seed parameter in the confiugration
     * @return a random seed
     * @see FetchRandomSeed(std::string, fhicl::ParameterSet const*, std::string)
     * 
     * This function works as
     * FetchRandomSeed(std::string, fhicl::ParameterSet const*, std::string),
     * except that all strings in seedcfgnames are tried one after another,
     * and the first valid parameter is used to read the seed.
     * The strings must be specified as initializer list, that is as a
     * comma-separated list enclosed in braces.
     */
    artext::SeedService::seed_t FetchRandomSeed(
      std::string instance,
      fhicl::ParameterSet const* pset,
      std::initializer_list<std::string> seedcfgnames
      );
    
    /**
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
     */
    artext::SeedService::seed_t FetchRandomSeed(
      std::string instance,
      fhicl::ParameterSet const* pset,
      std::string seedcfgname = "Seed"
      )
      { return FetchRandomSeed(instance, pset, { seedcfgname }); }
    
    /**
     * @brief Retrieves a random seed
     * @param pset parameter set to get the seed from
     * @param seedcfgnames names of possible seed parameter in the confiugration
     * @return a random seed
     * @see FetchRandomSeed(fhicl::ParameterSet const*, std::string)
     * 
     * This function works as
     * FetchRandomSeed(fhicl::ParameterSet const*, std::string),
     * except that all strings in seedcfgnames are tried one after another,
     * and the first valid parameter is used to read the seed.
     * The strings must be specified as initializer list, that is as a
     * comma-separated list enclosed in braces.
     */
    inline artext::SeedService::seed_t FetchRandomSeed(
      fhicl::ParameterSet const* pset,
      std::initializer_list<std::string> seedcfgnames
      )
      { return FetchRandomSeed("", pset, seedcfgnames); }
    
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
    
    
    /**
     * @brief Creates a seed specific to the specified event and current module
     * @param event the event to extract the seed from
     * @param instance instance name of the random engine
     * @return a valid seed
     * 
     * The seed is a hash value, constrained in the valid seed range
     * (ValidSeed::Min to ValidSeed::Max).
     * The value includes information from the event: run, subrun and event
     * number, and time stamp. It also includes the label of the current module
     * and the optional instance name.
     * 
     * @note Two different processes with the same module label will yield to
     * the same seed for each event.
     * 
     * @note The seed itself is not a good random number, since the lowest seeds
     * have larger probability than the highest, due to the simple method to
     * constraint the hash value into the valid seed range.
     */
    artext::SeedService::seed_t FetchEventRandomSeed
      (art::Event const& event, std::string instance = "");
    
    
  } // namespace util
} // namespace lar

#endif // FETCHRANDOMSEED_H
