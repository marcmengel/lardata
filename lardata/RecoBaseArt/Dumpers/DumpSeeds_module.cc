/**
 * @file   DumpSeeds_module.cc
 * @brief  Dumps on screen the content of seeds
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   September 29th, 2015
 */

// LArSoft includes
#include "lardata/RecoBase/Seed.h"

// art libraries
#include "art/Utilities/InputTag.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"

// support libraries
#include "fhiclcpp/ParameterSet.h"

// C//C++ standard libraries
#include <string>

// ... and more in the implementation part

namespace recob {
  
  /**
   * @brief Prints the content of all the seeds on screen
   *
   * This analyser prints the content of all the seeds into the
   * LogInfo/LogVerbatim stream.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *SeedModuleLabel* (art::InputTag, mandatory): label of the
   *   producer used to create the recob::Seed collection to be dumped
   * - *OutputCategory* (string, default: "DumpSeeds"): the category used
   *   for the output (useful for filtering)
   *
   */
  class DumpSeeds: public art::EDAnalyzer {
      public:
    
    /// Default constructor
    explicit DumpSeeds(fhicl::ParameterSet const& pset); 
    
    /// Does the printing
    virtual void analyze (const art::Event& evt) override;

      private:

    art::InputTag fInputTag; ///< input tag of the Seed product
    std::string fOutputCategory; ///< category for LogInfo output

  }; // class DumpSeeds
  
} // namespace recob


//==============================================================================
//===  Implementation section
//==============================================================================

// LArSoft includes
#include "lardata/RecoBase/Seed.h"
#include "lardata/RecoBase/Hit.h"

// art libraries
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/FindMany.h"
#include "art/Framework/Principal/Handle.h"

// support libraries
#include "messagefacility/MessageLogger/MessageLogger.h"

// C//C++ standard libraries
#include <vector>
#include <array>


namespace {
  
  //----------------------------------------------------------------------------
  class SeedDumper {
      public:
    
    /// Constructor; will dump seeds from the specified list
    SeedDumper(std::vector<recob::Seed> const& seed_list)
      : seeds(seed_list)
      {}
    
    
    /// Sets the hits associated to each seed
    void SetHits(art::FindMany<recob::Hit> const* hit_query)
      { hits = hit_query; }
    
    
    /// Dump a seed specified by its index in the input particle list
    template <typename Stream>
    void DumpSeed(Stream&& out, size_t iSeed, std::string indentstr = "") const
      {
        recob::Seed const& seed = seeds.at(iSeed);
        
        //
        // intro
        //
        out << "\n" << indentstr
          << "[#" << iSeed << "]";
        if (!seed.IsValid()) out << " invalid!";
        else {
          std::array<double, 3> start, dir;
          seed.GetDirection(dir.data(), nullptr);
          seed.GetPoint(start.data(), nullptr);
          out
            << " starts at (" << start[0] << "," << start[1] << "," << start[2]
            << ") to (" << dir[0] << "," << dir[1] << "," << dir[2]
            << "); length: " << seed.GetLength() << " cm"
            ;
        }
        
        //
        // hits
        //
        if (hits) {
          std::vector<recob::Hit const*> myHits = hits->at(iSeed);
          if (!myHits.empty()) {
            out
              << "; " << myHits.size() << " hits:";
            for (recob::Hit const* hit: myHits) {
              out << "\n" << indentstr
                << "  on " << hit->WireID()
                << ", peak at tick " << hit->PeakTime() << ", "
                << hit->PeakAmplitude() << " ADC, RMS: " << hit->RMS()
                << " (channel: "
                << hit->Channel() << ")";
            } // for hits
          } // if we have hits
        } // if we have hit information
        
        //
        // done
        //
        
      } // DumpSeed()
    
    
    /// Dumps all seeds in the input list
    template <typename Stream>
    void DumpAllSeeds(Stream&& out, std::string indentstr = "") const
      {
        indentstr += "  ";
        size_t const nSeeds = seeds.size();
        for (size_t iSeed = 0; iSeed < nSeeds; ++iSeed)
          DumpSeed(out, iSeed, indentstr);
      } // DumpAllSeeds()
    
    
    
      protected:
    std::vector<recob::Seed> const& seeds; ///< input list
    
    /// Associated hits (expected same order as for seeds)
    art::FindMany<recob::Hit> const* hits = nullptr;
    
  }; // SeedDumper
  
  
  //----------------------------------------------------------------------------
  
  
} // local namespace



namespace recob {
  
  //----------------------------------------------------------------------------
  DumpSeeds::DumpSeeds(fhicl::ParameterSet const& pset) 
    : EDAnalyzer(pset)
    , fInputTag(pset.get<art::InputTag>("SeedModuleLabel"))
    , fOutputCategory(pset.get<std::string>("OutputCategory", "DumpSeeds"))
    {}
  
  
  //----------------------------------------------------------------------------
  void DumpSeeds::analyze(const art::Event& evt) {
    
    //
    // collect all the available information
    //
    // fetch the data to be dumped on screen
    auto Seeds = evt.getValidHandle<std::vector<recob::Seed>>(fInputTag);
    
    art::FindMany<recob::Hit> const SeedHits(Seeds, evt, fInputTag);
    
    size_t const nSeeds = Seeds->size();
    mf::LogInfo(fOutputCategory)
      << "The event contains " << nSeeds << " seeds from '"
      << fInputTag.encode() << "'";
    
    // prepare the dumper
    SeedDumper dumper(*Seeds);
    if (SeedHits.isValid()) dumper.SetHits(&SeedHits);
    else mf::LogWarning("DumpSeeds") << "hit information not avaialble";
    
    dumper.DumpAllSeeds(mf::LogVerbatim(fOutputCategory), "  ");
    
    mf::LogVerbatim(fOutputCategory) << "\n"; // two empty lines
    
  } // DumpSeeds::analyze()

  DEFINE_ART_MODULE(DumpSeeds)

} // namespace recob
