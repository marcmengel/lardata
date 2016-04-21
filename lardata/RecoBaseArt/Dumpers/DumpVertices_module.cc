/**
 * @file   DumpVertices_module.cc
 * @brief  Dumps on screen the content of vertices
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   September 29th, 2015
 */

// LArSoft includes
#include "lardata/RecoBase/Vertex.h"

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
   * @brief Prints the content of all the vertices on screen
   *
   * This analyser prints the content of all the vertices into the
   * LogInfo/LogVerbatim stream.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *VertexModuleLabel* (art::InputTag, mandatory): label of the
   *   producer used to create the recob::Vertex collection to be dumped
   * - *OutputCategory* (string, default: "DumpVertices"): the category used
   *   for the output (useful for filtering)
   *
   */
  class DumpVertices: public art::EDAnalyzer {
      public:
    
    /// Default constructor
    explicit DumpVertices(fhicl::ParameterSet const& pset); 
    
    /// Does the printing
    virtual void analyze (const art::Event& evt) override;

      private:

    art::InputTag fInputTag; ///< input tag of the Vertex product
    std::string fOutputCategory; ///< category for LogInfo output

  }; // class DumpVertices
  
} // namespace recob


//==============================================================================
//===  Implementation section
//==============================================================================

// LArSoft includes
#include "lardata/RecoBase/Vertex.h"

// art libraries
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Handle.h"

// support libraries
#include "messagefacility/MessageLogger/MessageLogger.h"

// C//C++ standard libraries
#include <vector>


namespace {
  
  //----------------------------------------------------------------------------
  class VertexDumper {
      public:
    
    /// Constructor; will dump vertices from the specified list
    VertexDumper(std::vector<recob::Vertex> const& vertex_list)
      : vertices(vertex_list)
      {}
    
    
    /// Dump a vertex specified by its index in the input list
    template <typename Stream>
    void DumpVertex
      (Stream&& out, size_t iVertex, std::string indentstr = "") const
      {
        recob::Vertex const& vertex = vertices.at(iVertex);
        
        //
        // intro
        //
        out << "\n" << indentstr
          << "[#" << iVertex << "]";
        
        std::array<double, 3> vtx_pos;
        vertex.XYZ(vtx_pos.data());
        out << " ID=" << vertex.ID() << " at ("
          << vtx_pos[0] << "," << vtx_pos[1] << "," << vtx_pos[2]
          << ")";
        
        //
        // done
        //
        
      } // DumpVertex()
    
    
    /// Dumps all vertices in the input list
    template <typename Stream>
    void DumpAllVertices(Stream&& out, std::string indentstr = "") const
      {
        indentstr += "  ";
        size_t const nVertices = vertices.size();
        for (size_t iVertex = 0; iVertex < nVertices; ++iVertex)
          DumpVertex(out, iVertex, indentstr);
      } // DumpAllVertices()
    
    
    
      protected:
    std::vector<recob::Vertex> const& vertices; ///< input list
    
  }; // VertexDumper
  
  
  //----------------------------------------------------------------------------
  
  
} // local namespace



namespace recob {
  
  //----------------------------------------------------------------------------
  DumpVertices::DumpVertices(fhicl::ParameterSet const& pset) 
    : EDAnalyzer(pset)
    , fInputTag(pset.get<art::InputTag>("VertexModuleLabel"))
    , fOutputCategory(pset.get<std::string>("OutputCategory", "DumpVertices"))
    {}
  
  
  //----------------------------------------------------------------------------
  void DumpVertices::analyze(const art::Event& evt) {
    
    //
    // collect all the available information
    //
    // fetch the data to be dumped on screen
    auto Vertices = evt.getValidHandle<std::vector<recob::Vertex>>(fInputTag);
    
    size_t const nVertices = Vertices->size();
    mf::LogInfo(fOutputCategory)
      << "The event contains " << nVertices << " vertices from '"
      << fInputTag.encode() << "'";
    
    // prepare the dumper
    VertexDumper dumper(*Vertices);
    
    dumper.DumpAllVertices(mf::LogVerbatim(fOutputCategory), "  ");
    
    mf::LogVerbatim(fOutputCategory) << "\n"; // two empty lines
    
  } // DumpVertices::analyze()

  DEFINE_ART_MODULE(DumpVertices)

} // namespace recob
