/**
 * @file   DumpPFParticles_module.cc
 * @brief  Dumps on screen the content of ParticleFlow particles
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   September 25th, 2015
 */

// LArSoft includes
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardata/RecoBaseArt/Dumpers/hexfloat.h"

// art libraries
#include "canvas/Utilities/InputTag.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"

// support libraries
#include "fhiclcpp/ParameterSet.h"

// C//C++ standard libraries
#include <string>

// ... and more in the implementation part

namespace recob {
  
  /**
   * @brief Prints the content of all the ParticleFlow particles on screen
   *
   * This analyser prints the content of all the ParticleFlow particles into the
   * LogInfo/LogVerbatim stream.
   * 
   * Configuration parameters
   * =========================
   * 
   * - *PFModuleLabel* (art::InputTag, default: `"pandora"`): label of the
   *   producer used to create the recob::Wire collection to be dumped
   * - *OutputCategory* (string, default: `"DumpPFParticles"`): the category
   *   used for the output (useful for filtering)
   * - *PrintHexFloats* (boolean, default: `false`): print all the floating
   *   point numbers in base 16
   *
   */
  class DumpPFParticles: public art::EDAnalyzer {
      public:
    
    /// Default constructor
    explicit DumpPFParticles(fhicl::ParameterSet const& pset); 
    
    /// Does the printing
    virtual void analyze (const art::Event& evt) override;

      private:

    art::InputTag fInputTag; ///< input tag of the PFParticle product
    std::string fOutputCategory; ///< category for LogInfo output
    bool fPrintHexFloats; ///< whether to print floats in base 16

  }; // class DumpPFParticles
  
} // namespace recob


//==============================================================================
//===  Implementation section
//==============================================================================
// LArSoft includes
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Seed.h"
#include "lardataobj/RecoBase/PCAxis.h"

// art libraries
#include "art/Framework/Core/ModuleMacros.h"
#include "canvas/Persistency/Common/FindOne.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "art/Framework/Principal/Handle.h"

// support libraries
#include "messagefacility/MessageLogger/MessageLogger.h"

// C//C++ standard libraries
#include <vector>
#include <utility> // std::swap()
#include <algorithm> // std::count()
#include <limits> // std::numeric_limits<>


namespace {
  /**
   * @brief A container keyed by integer key (size_t).
   * @tparam T type of contained element
   */
  template <typename T>
  class int_map: private std::vector<T> {
    using map_type = std::vector<T>;
      public:
    using this_type = int_map<T>;
    
    using typename map_type::value_type;
    using typename map_type::reference;
    using typename map_type::const_reference;
    using typename map_type::iterator;
    using typename map_type::const_iterator;
    
    using typename map_type::size_type;
    
    
    /// Default constructor: "invalid data" is default-constructed
    int_map() = default;
    
    /// Constructor: sets the value of invalid data
    int_map(value_type invalid_value): invalid(invalid_value) {}
    
    
    /// Resizes the map to accomodate this many items
    void resize(size_t new_size)
      { get_map().resize(new_size, invalid_value()); }
    
    /// Creates the item at specified position with invalid value and returns it
    reference operator[] (size_t pos)
      { if (pos >= size()) resize(pos + 1); return get_map()[pos]; }
    
    /// Returns the item at specified position, or invalid value if not existing
    const_reference operator[] (size_t pos) const
      { return (pos >= size())? invalid: get_map()[pos]; }
    
    using map_type::size;
    using map_type::empty;
    
    using map_type::begin;
    using map_type::end;
    using map_type::cbegin;
    using map_type::cend;
    using map_type::rbegin;
    using map_type::rend;
    using map_type::crbegin;
    using map_type::crend;
    
    /// Swaps the content of data and of this map
    void swap(this_type& data)
      { swap(data.get_map()); std::swap(data.invalid, invalid); }
    
    /// Swaps the content of data and of this map
    void swap(std::vector<T>& data) { map_type::swap(data); }
    
    // non-standard calls follow
    /// Returns the number of invalid elements
    size_type n_invalid() const
      { return std::count(begin(), end(), invalid); }
    
    /// Returns the number of valid elements
    size_type n_valid() const { return size() - n_invalid(); }
    
    /// Returns whether the element at specified position is valid
    bool is_valid(size_type pos) const
      { return (pos < size())? is_valid_value(get_map()[pos]): false; }
    
    /// Returns whether the specified value is valid
    bool is_valid_value(value_type const& v) const { return v != invalid; }
    
    /// Returns the invalid value
    value_type const& invalid_value() const { return invalid; }
    
    
      private:
    value_type const invalid; ///< value of invalid data
    
    map_type& get_map() { return static_cast<map_type&>(*this); }
    map_type const& get_map() const
      { return static_cast<map_type const&>(*this); }
    
  }; // int_map<>
  
  
  //----------------------------------------------------------------------------
  int_map<size_t> CreateMap(std::vector<recob::PFParticle> const& particles) {
    
    int_map<size_t> pmap(std::numeric_limits<size_t>::max());
    pmap.resize(particles.size());
    
    size_t const nParticles = particles.size();
    for (size_t iPart = 0; iPart < nParticles; ++iPart)
      pmap[particles[iPart].Self()] = iPart;
    
    return pmap;
  } // CreateMap()
  
  
  //----------------------------------------------------------------------------
  class ParticleDumper {
      public:
    
    /// Collection of available printing style options
    struct PrintOptions_t {
      bool hexFloats = false; ///< print all floating point numbers in base 16
    }; // PrintOptions_t
    
    
    /// Constructor; will dump particles from the specified list
    ParticleDumper(
      std::vector<recob::PFParticle> const& particle_list,
      PrintOptions_t print_options = {}
      )
      : particles(particle_list)
      , options(print_options)
      , visited(particles.size(), 0U)
      , particle_map (CreateMap(particles))
      {
      }
    
    
    /// Sets the vertices associated to each particle
    void SetVertices(art::FindOne<recob::Vertex> const* vtx_query)
      { vertices = vtx_query; }
    
    /// Sets the vertices associated to each particle
    void SetTracks(art::FindMany<recob::Track> const* trk_query)
      { tracks = trk_query; }
    
    /// Sets the clusters associated to each particle
    void SetClusters(art::FindMany<recob::Cluster> const* cls_query)
      { clusters = cls_query; }
    
    /// Sets the seeds associated to each particle
    void SetSeeds(art::FindMany<recob::Seed> const* seed_query)
      { seeds = seed_query; }
    
    /// Sets the 3D points associated to each particle
    void SetSpacePoints(art::FindMany<recob::SpacePoint> const* sp_query)
      { spacepoints = sp_query; }
    
    /// Sets the 3D points associated to each particle
    void SetPCAxes(art::FindMany<recob::PCAxis> const* pca_query)
      { pcaxes = pca_query; }
    
    
    /// Dump a particle specified by its index in the input particle list
    template <typename Stream>
    void DumpParticle
      (Stream&& out, size_t iPart, std::string indentstr = "")
      const
      {
        lar::OptionalHexFloat hexfloat(options.hexFloats);
        
        recob::PFParticle const& part = particles.at(iPart);
        bool const isPrimary = part.IsPrimary();
        ++visited[iPart];
        
        //
        // intro
        //
        auto const PartID = part.Self();
        out << "\n" << indentstr << "ID=" << PartID;
        if (iPart != PartID) out << " [#" << iPart << "]";
        out << " (type: ";
        DumpPDGID(out, part.PdgCode());
        out << ")";
        if (isPrimary) out << " (primary)";
        else           out << " from ID=" << part.Parent();
        
        // vertex information
        if (vertices) {
          cet::maybe_ref<recob::Vertex const> VertexRef = vertices->at(iPart);
          if (VertexRef.isValid()) {
            recob::Vertex const& vertex = VertexRef.ref();
            std::array<double, 3> vtx_pos;
            vertex.XYZ(vtx_pos.data());
            out << " [decay at ("
              << hexfloat(vtx_pos[0]) << "," << hexfloat(vtx_pos[1])
              << "," << hexfloat(vtx_pos[2])
              << "), ID=" << vertex.ID() << "]";
          }
          else {
            out << " [no vertex found!]";
          }
        }
        
        if (part.NumDaughters() > 0)
          out << " with " << part.NumDaughters() << " daughters";
        else
          out << " with no daughter";
        
        // axis
        if (pcaxes) {
          std::vector<recob::PCAxis const*> const& myAxes = pcaxes->at(iPart);
          out << "\n" << indentstr;
          if (myAxes.empty())
            out << " [no axis found!]";
          else {
            auto printDirection
              = [&hexfloat](auto& out, recob::PCAxis const& axis)
              {
              out << "axis ID=" << axis.getID() << ", principal: ("
                  << hexfloat(axis.getEigenVectors()[0][0])
                  << ", " << hexfloat(axis.getEigenVectors()[0][1])
                  << ", " << hexfloat(axis.getEigenVectors()[0][2]) << ")";
              };
            if (myAxes.size() == 1) {
              printDirection(out, *(myAxes.front()));
            }
            else {
              out << "  " << myAxes.size() << " axes present:";
              for (recob::PCAxis const* axis: myAxes) {
                out << "\n" << indentstr << "    ";
                if (axis->getSvdOK()) printDirection(out, *axis);
                else out << "axis is invalid";
              } // for axes
            } // else
          } // if has axes
        }
        
        //
        // tracks
        //
        if (tracks) {
          std::vector<recob::Track const*> const& myTracks = tracks->at(iPart);
          if (!myTracks.empty()) {
            out << "\n" << indentstr << "  "
              << myTracks.size() << " tracks:";
            for (recob::Track const* track: myTracks) {
              if (myTracks.size() > 1) out << "\n" << indentstr << "   ";
              out << " length " << hexfloat(track->Length())
                << "cm from (" << hexfloat(track->Vertex().X())
                  << ";" << hexfloat(track->Vertex().Y())
                  << ";" << hexfloat(track->Vertex().Z())
                << ") to (" << hexfloat(track->End().X())
                  << ";" << hexfloat(track->End().Y())
                  << ";" << hexfloat(track->End().Z())
                << ") (ID=" << track->ID() << ")";
            } // for clusters
          }
        } // if we have track information
        
        //
        // seeds
        //
        if (seeds) {
          std::vector<recob::Seed const*> const& mySeeds = seeds->at(iPart);
          if (!mySeeds.empty()) {
            out << "\n" << indentstr << "  "
              << mySeeds.size() << " seeds:";
            for (recob::Seed const* seed: mySeeds) {
              if (!seed->IsValid()) {
                out << "  <invalid>";
                continue;
              }
              std::array<double, 3> start, dir;
              seed->GetDirection(dir.data(), nullptr);
              seed->GetPoint(start.data(), nullptr);
              out << "\n" << indentstr
                << "    (" << hexfloat(start[0]) << "," << hexfloat(start[1])
                << "," << hexfloat(start[2])
                << ")=>(" << hexfloat(dir[0]) << "," << hexfloat(dir[1])
                << "," << hexfloat(dir[2])
                << "), " << hexfloat(seed->GetLength()) << " cm"
                ;
            } // for seeds
          } // if we have seeds
        } // if we have seed information
        
        //
        // space points
        //
        if (spacepoints) {
          std::vector<recob::SpacePoint const*> const& mySpacePoints
            = spacepoints->at(iPart);
          out << "\n" << indentstr << "  ";
          if (!mySpacePoints.empty()) {
            constexpr unsigned int PointsPerLine = 2;
            out << mySpacePoints.size() << " space points:";
            for (size_t iSP = 0; iSP < mySpacePoints.size(); ++iSP) {
              if ((iSP % PointsPerLine) == 0)
                out << "\n" << indentstr << "  ";
              
              recob::SpacePoint const& sp = *(mySpacePoints[iSP]);
              const double* pos = sp.XYZ();
              out << "  ID=" << sp.ID()
                << " at (" << hexfloat(pos[0]) << "," << hexfloat(pos[1])
                << "," << hexfloat(pos[2]) << ") cm"
                ;
            } // for seeds
          } // if we have space points
          else out << "no space points";
        } // if we have space point information
        
        //
        // clusters
        //
        if (clusters) {
          std::vector<recob::Cluster const*> myClusters = clusters->at(iPart);
          if (!myClusters.empty()) {
            out << "\n" << indentstr << "  "
              << myClusters.size() << " clusters:";
            for (recob::Cluster const* cluster: myClusters) {
              out << "  " << cluster->NHits() << " hits on " << cluster->Plane()
                << " (ID=" << cluster->ID() << ")";
            } // for clusters
            
          }
        } // if we have cluster information
        
        //
        // daughters
        //
        if (part.NumDaughters() > 0) {
          std::vector<size_t> const& daughters = part.Daughters();
          out << "\n" << indentstr
            << "  " << part.NumDaughters() << " particle daughters:";
          for (size_t DaughterID: daughters) {
            if (DaughterID == PartID) {
              out << "\n" << indentstr
                << "    oh, just great: this particle is its own daughter!";
            }
            else DumpParticleWithID(out, DaughterID, indentstr + "  ");
          }
        } // if has daughters
        
        //
        // warnings
        //
        if (visited[iPart] == 2) {
          out << "\n" << indentstr << "WARNING: particle ID=" << PartID
            << " connected more than once!";
        }
        
        //
        // done
        //
        
      } // DumpParticle()
    
    
    /// Dump a particle specified by its ID
    template <typename Stream>
    void DumpParticleWithID
      (Stream&& out, size_t pID, std::string indentstr = "")
      const
      {
        size_t const pos = particle_map[pID];
        if (particle_map.is_valid_value(pos)) {
          DumpParticle(out, pos, indentstr);
        }
        else {
          out << "\n" << indentstr << "<ID=" << pID << " not found>";
        }
      } // DumpParticleWithID()
    
    
    /// Dumps all primary particles
    template <typename Stream>
    void DumpAllPrimaries(Stream&& out, std::string indentstr = "") const
      {
        indentstr += "  ";
        size_t const nParticles = particles.size();
        unsigned int nPrimaries = 0;
        for (size_t iPart = 0; iPart < nParticles; ++iPart) {
          if (!particles[iPart].IsPrimary()) continue;
          DumpParticle(out, iPart, indentstr);
        } // for
        if (nPrimaries == 0)
          out << "\n" << indentstr << "No primary particle found";
      } // DumpAllPrimaries()
    
    
    /// Dumps all particles in the input list
    template <typename Stream>
    void DumpAllParticles(Stream&& out, std::string indentstr = "") const
      {
        // first print all the primary particles
        DumpAllPrimaries(out, indentstr);
        // then find out if there are any that are "disconnected"
        unsigned int const nDisconnected
          = std::count(visited.begin(), visited.end(), 0U);
        if (nDisconnected) {
          out << "\n" << indentstr
            << nDisconnected << " particles not coming from primary ones:";
          size_t const nParticles = visited.size();
          for (size_t iPart = 0; iPart < nParticles; ++iPart) {
            if (visited[iPart] > 0) continue;
            DumpParticle(out, iPart, indentstr + "  ");
          } // for unvisited particles
          out << "\n" << indentstr
            << "(end of " << nDisconnected << " particles not from primaries)";
        } // if there are disconnected particles
        // TODO finally, note if there are multiply-connected particles
        
      } // DumpAllParticles()
    
    
    template <typename Stream>
    static void DumpPDGID(Stream&& out, int ID)
      {
        switch (ID) {
          case -11: out << "e+";  break;
          case  11: out << "e-";  break;
          case -13: out << "mu+"; break;
          case  13: out << "mu-"; break;
          default:  out << "MCID=" << ID; break;
        } // switch
      } // DumpPDGID()
    
    
      protected:
    std::vector<recob::PFParticle> const& particles; ///< input list
    
    PrintOptions_t options; ///< printing and formatting options
    
    /// Associated vertices (expected same order as for particles)
    art::FindOne<recob::Vertex> const* vertices = nullptr;
    
    /// Associated tracks (expected same order as for particles)
    art::FindMany<recob::Track> const* tracks = nullptr;
    
    /// Associated clusters (expected same order as for particles)
    art::FindMany<recob::Cluster> const* clusters = nullptr;
    
    /// Associated seeds (expected same order as for particles)
    art::FindMany<recob::Seed> const* seeds = nullptr;
    
    /// Associated space points (expected same order as for particles)
    art::FindMany<recob::SpacePoint> const* spacepoints = nullptr;
    
    /// Associated principal component axes (expected same order as particles)
    art::FindMany<recob::PCAxis> const* pcaxes = nullptr;
    
    /// Number of dumps on each particle
    mutable std::vector<unsigned int> visited;
    
    int_map<size_t> const particle_map; ///< fast lookup index by particle ID
    
  }; // ParticleDumper
  
  
  //----------------------------------------------------------------------------
  
  
} // local namespace



namespace recob {
  
  //----------------------------------------------------------------------------
  DumpPFParticles::DumpPFParticles(fhicl::ParameterSet const& pset) 
    : EDAnalyzer(pset)
    , fInputTag(pset.get<art::InputTag>("PFModuleLabel", "pandora"))
    , fOutputCategory
        (pset.get<std::string>("OutputCategory", "DumpPFParticles"))
    , fPrintHexFloats(pset.get<bool>("PrintHexFloats", false))
    {}
  
  
  //----------------------------------------------------------------------------
  void DumpPFParticles::analyze(const art::Event& evt) {
    
    //
    // collect all the available information
    //
    // fetch the data to be dumped on screen
    art::ValidHandle<std::vector<recob::PFParticle>> PFParticles
      = evt.getValidHandle<std::vector<recob::PFParticle>>(fInputTag);
    
    art::FindOne<recob::Vertex> const ParticleVertices
      (PFParticles, evt, fInputTag);
    art::FindMany<recob::Track> const ParticleTracks
      (PFParticles, evt, fInputTag);
    art::FindMany<recob::Cluster> const ParticleClusters
      (PFParticles, evt, fInputTag);
    art::FindMany<recob::Seed> const ParticleSeeds
      (PFParticles, evt, fInputTag);
    art::FindMany<recob::SpacePoint> const ParticleSpacePoints
      (PFParticles, evt, fInputTag);
    art::FindMany<recob::PCAxis> const ParticlePCAxes
      (PFParticles, evt, fInputTag);
    
    size_t const nParticles = PFParticles->size();
    mf::LogVerbatim(fOutputCategory) << "Event " << evt.id()
      << " contains " << nParticles << " particles from '"
      << fInputTag.encode() << "'";
    
    // prepare the dumper
    ParticleDumper::PrintOptions_t options;
    options.hexFloats = fPrintHexFloats;
    ParticleDumper dumper(*PFParticles, options);
    if (ParticleVertices.isValid()) dumper.SetVertices(&ParticleVertices);
    else mf::LogPrint("DumpPFParticles") << "WARNING: vertex information not available";
    if (ParticleTracks.isValid()) dumper.SetTracks(&ParticleTracks);
    else mf::LogPrint("DumpPFParticles") << "WARNING: track information not available";
    if (ParticleClusters.isValid()) dumper.SetClusters(&ParticleClusters);
    else mf::LogPrint("DumpPFParticles") << "WARNING: cluster information not available";
    if (ParticleSeeds.isValid()) dumper.SetSeeds(&ParticleSeeds);
    else mf::LogPrint("DumpPFParticles") << "WARNING: seed information not avaialble";
    if (ParticleSpacePoints.isValid())
      dumper.SetSpacePoints(&ParticleSpacePoints);
    else {
      mf::LogPrint("DumpPFParticles")
        << "WARNING: space point information not available";
    }
    if (ParticlePCAxes.isValid())
      dumper.SetPCAxes(&ParticlePCAxes);
    else {
      mf::LogPrint("DumpPFParticles")
        << "WARNING: principal component axis not available";
    }
    
    dumper.DumpAllParticles(mf::LogVerbatim(fOutputCategory), "  ");
    
    mf::LogVerbatim(fOutputCategory) << "\n"; // two empty lines
    
  } // DumpPFParticles::analyze()

  DEFINE_ART_MODULE(DumpPFParticles)

} // namespace recob
