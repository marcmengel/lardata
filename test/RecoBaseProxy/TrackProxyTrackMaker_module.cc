/**
 * @file   TrackProxyTrackMaker_module.cc
 * @brief  Test producer creating a few dummy hits.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 31, 2017
 * 
 * This started as a copy of `lar::test::AssnsChainHitMaker` test module.
 */

// LArSoft libraries
#include "lardata/Utilities/PtrMaker.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"

// framework libraries
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"

#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Utilities/InputTag.h"

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/Comment.h"

// C/C++ standard libraries
#include <vector>
#include <utility> // std::move()
#include <memory> // std::make_unique()


namespace lar {
  namespace test {
    
    // -------------------------------------------------------------------------
    /**
    * @brief Creates some dummy hits.
    * 
    * The produced tracks have completely dummy content.
    * 
    * Configuration parameters
    * =========================
    * 
    * * *hits* (input tag, mandatory): the data product to read the hits from
    * * *hitsPerTrack* (list of unsigned integers, mandatory): number of hits
    *     for each produced track. If there are hits left after all the tracks
    *     specified here have been created, an additional track with all those
    *     hits is created. If there are not enough hits, an exception is thrown.
    * 
    */
    class TrackProxyTrackMaker: public art::EDProducer {
        public:
      
      struct Config {
        using Name = fhicl::Name;
        using Comment = fhicl::Comment;
        
        fhicl::Atom<art::InputTag> hitsTag{
          Name("hits"),
          Comment("tag of the recob::Hit data products to produce tracks with.")
          };
        
        fhicl::Sequence<unsigned int> hitsPerTrack{
          Name("hitsPerTrack"),
          Comment("number of hits per track; last takes all remaining ones.")
          };
        
      }; // struct Config
      
      using Parameters = art::EDProducer::Table<Config>;
      
      explicit TrackProxyTrackMaker(Parameters const& config)
        : hitsTag(config().hitsTag())
        , hitsPerTrack(config().hitsPerTrack())
        {
          produces<std::vector<recob::Track>>();
          produces<art::Assns<recob::Track, recob::Hit>>();
        }

      virtual void produce(art::Event& event) override;
      
        private:
      art::InputTag hitsTag; ///< Input hit collection label.
      std::vector<unsigned int> hitsPerTrack; ///< Hits per produced track.

    };  // TrackProxyTrackMaker

    // -------------------------------------------------------------------------
    
    
  } // namespace test
} // namespace lar


// -----------------------------------------------------------------------------
void lar::test::TrackProxyTrackMaker::produce(art::Event& event) {
  
  auto tracks = std::make_unique<std::vector<recob::Track>>();
  auto hitTrackAssn = std::make_unique<art::Assns<recob::Track, recob::Hit>>();
  
  auto hitHandle = event.getValidHandle<std::vector<recob::Hit>>(hitsTag);
  auto const& hits = *hitHandle;
  
  lar::PtrMaker<recob::Track> trackPtrMaker(event, *this);
  unsigned int iTrack = 0;
  unsigned int usedHits = 0;
  while (usedHits < hits.size()) {
    
    // how many hits for this track:
    unsigned int const nTrackHits = (iTrack < hitsPerTrack.size())
      ? std::min(hitsPerTrack[iTrack], (unsigned int)(hits.size() - usedHits))
      : (hits.size() - usedHits)
      ;
    
    //
    // create the track trajectory
    //
    std::size_t const firstHit = usedHits;
    recob::TrackTrajectory::Positions_t pos;
    recob::TrackTrajectory::Momenta_t mom;
    recob::TrackTrajectory::Flags_t flags;
    for (unsigned int iPoint = 0; iPoint < nTrackHits; ++iPoint) {
      pos.emplace_back(iPoint, iPoint, iPoint);
      mom.emplace_back(2.0, 1.0, 0.0);
      flags.emplace_back(usedHits++);
    } // for
    recob::TrackTrajectory traj
      (std::move(pos), std::move(mom), std::move(flags), true);
    
    //
    // create the track
    //
    recob::Track track(std::move(traj), 2112, 1.0, nTrackHits, {}, {}, iTrack);
    tracks->push_back(std::move(track));
    
    //
    // create the track-hit associations
    //
    auto const trackPtr = trackPtrMaker(iTrack);
    for (std::size_t iHit = firstHit; iHit < usedHits; ++iHit)
      hitTrackAssn->addSingle(trackPtr, { hitHandle, iHit });
    
    mf::LogVerbatim("TrackProxyTrackMaker")
      << "New track #" << tracks->back().ID()
      << " with " << nTrackHits << " hits";
    
    //
    // prepare for the next track
    //
    ++iTrack;
    
  } // while
  
  mf::LogInfo("TrackProxyTrackMaker")
    << "Produced " << tracks->size() << " tracks from " << usedHits << " hits.";
  
  event.put(std::move(tracks));
  event.put(std::move(hitTrackAssn));
  
} // lar::test::TrackProxyTrackMaker::produce()

// -----------------------------------------------------------------------------
DEFINE_ART_MODULE(lar::test::TrackProxyTrackMaker)

// -----------------------------------------------------------------------------
