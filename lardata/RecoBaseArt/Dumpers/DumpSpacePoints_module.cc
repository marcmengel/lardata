/**
 * @file   DumpSpacePoints_module.cc
 * @brief  Dumps on screen the content of space points
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 18th, 2015
 */

// LArSoft includes
#include "lardata/RecoBaseArt/Dumpers/NewLine.h" // recob::dumper::makeNewLine()
#include "lardata/RecoBaseArt/Dumpers/SpacePointDumpers.h"
#include "lardata/RecoBase/SpacePoint.h"

// art libraries
#include "art/Utilities/InputTag.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"

// support libraries
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h" // also pulls in fhicl::Name and fhicl::Comment

// C//C++ standard libraries
#include <string>

// ... and more in the implementation part

namespace recob {
  
  /**
   * @brief Prints the content of all the space points on screen
   *
   * This analyser prints the content of all the space points into the
   * LogInfo/LogVerbatim stream.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *SpacePointModuleLabel* (art::InputTag, mandatory): label of the
   *   producer used to create the recob::SpacePoint collection to be dumped
   * - *OutputCategory* (string, default: "DumpSpacePoints"): the category used
   *   for the output (useful for filtering)
   *
   */
  class DumpSpacePoints: public art::EDAnalyzer {
      public:
    
    /// Configuration parameters
    struct Config {
      using Name = fhicl::Name;
      using Comment = fhicl::Comment;
      
      fhicl::Atom<art::InputTag> SpacePointModuleLabel {
        Name   ("SpacePointModuleLabel"),
        Comment("label of the producer used to create the recob::SpacePoint collection to be dumped")
        };
      fhicl::Atom<std::string> OutputCategory {
        Name   ("OutputCategory"),
        Comment("the category used for the output (useful for filtering) [\"DumpSpacePoints\"]"),
        "DumpSpacePoints" /* default value */
        };
      
    }; // struct Config
    
    /// Default constructor
    explicit DumpSpacePoints(art::EDAnalyzer::Table<Config> const& config); 
    
    /// Does the printing
    virtual void analyze (const art::Event& evt) override;

      private:

    art::InputTag fInputTag; ///< input tag of the SpacePoint product
    std::string fOutputCategory; ///< category for LogInfo output

  }; // class DumpSpacePoints
  
} // namespace recob


//==============================================================================
//===  Implementation section
//==============================================================================

// LArSoft includes
#include "lardata/RecoBase/SpacePoint.h"
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
  class SpacePointDumper {
      public:
    
    /// Constructor; will dump space points from the specified list
    SpacePointDumper(std::vector<recob::SpacePoint> const& point_list)
      : points(point_list)
      {}
    
    
    /// Sets the hits associated to each space point
    void SetHits(art::FindMany<recob::Hit> const* hit_query)
      { hits = hit_query; }
    
    
    /// Dump a space point specified by its index in the input list
    template <typename Stream>
    void DumpSpacePoint
      (Stream&& out, size_t iPoint, std::string indentstr = "") const
      {
        recob::SpacePoint const& point = points.at(iPoint);
        
        //
        // intro
        //
        auto first_nl = recob::dumper::makeNewLine(out, indentstr);
        first_nl()
          << "[#" << iPoint << "] ";
        
        auto nl = recob::dumper::makeNewLine
          (out, indentstr + "  ", true /* follow */);
        recob::dumper::DumpSpacePoint(out, point, nl);
        
        //
        // hits
        //
        if (hits) {
          std::vector<recob::Hit const*> myHits = hits->at(iPoint);
          if (myHits.empty()) {
            out << "; no associated hits";
          }
          else {
            out
              << "; " << myHits.size() << " hits:";
            for (recob::Hit const* hit: myHits) {
              nl()
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
        
      } // DumpSpacePoints()
    
    
    /// Dumps all space points in the input list
    template <typename Stream>
    void DumpAllSpacePoints(Stream&& out, std::string indentstr = "") const
      {
        indentstr += "  ";
        size_t const nPoints = points.size();
        for (size_t iPoint = 0; iPoint < nPoints; ++iPoint)
          DumpSpacePoint(std::forward<Stream>(out), iPoint, indentstr);
      } // DumpAllSpacePoints()
    
    
    
      protected:
    std::vector<recob::SpacePoint> const& points; ///< input list
    
    /// Associated hits (expected same order as for space points)
    art::FindMany<recob::Hit> const* hits = nullptr;
    
  }; // SpacePointDumper
  
  
  //----------------------------------------------------------------------------
  
  
} // local namespace



namespace recob {
  
  //----------------------------------------------------------------------------
  DumpSpacePoints::DumpSpacePoints(art::EDAnalyzer::Table<Config> const& config)
    : EDAnalyzer(config)
    , fInputTag(config().SpacePointModuleLabel())
    , fOutputCategory(config().OutputCategory())
    {}
  
  
  //----------------------------------------------------------------------------
  void DumpSpacePoints::analyze(const art::Event& evt) {
    
    //
    // collect all the available information
    //
    // fetch the data to be dumped on screen
    auto SpacePoints
      = evt.getValidHandle<std::vector<recob::SpacePoint>>(fInputTag);
    
    art::FindMany<recob::Hit> const PointHits(SpacePoints, evt, fInputTag);
    
    size_t const nPoints = SpacePoints->size();
    mf::LogInfo(fOutputCategory)
      << "The event contains " << nPoints << " space points from '"
      << fInputTag.encode() << "'";
    
    // prepare the dumper
    SpacePointDumper dumper(*SpacePoints);
    if (PointHits.isValid()) dumper.SetHits(&PointHits);
    else mf::LogWarning("DumpSpacePoints") << "hit information not avaialble";
    
    dumper.DumpAllSpacePoints(mf::LogVerbatim(fOutputCategory), "  ");
    
    mf::LogVerbatim(fOutputCategory) << "\n"; // two empty lines
    
  } // DumpSpacePoints::analyze()

  DEFINE_ART_MODULE(DumpSpacePoints)

} // namespace recob
