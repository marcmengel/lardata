/**
 * @file   TrackProxyTest_module.cc
 * @brief  Tests `proxy::Track` class.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 */

// LArSoft libraries
#include "lardata/RecoBaseProxy/Track.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"

// framework libraries
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "canvas/Utilities/InputTag.h"

// utility libraries
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Name.h"
#include "fhiclcpp/types/Comment.h"
#include "fhiclcpp/ParameterSet.h"

// Boost libraries
#include <boost/test/test_tools.hpp> // BOOST_CHECK()

// C/C++ libraries
#include <vector>
#include <initializer_list>
#include <memory> // std::unique_ptr<>
#include <cstring> // std::strlen(), std::strcpy()


//------------------------------------------------------------------------------
/**
 * @brief Runs a test of `proxy::Tracks` interface.
 * 
 * This module is that it uses Boost unit test library, and as such it must be
 * run with `lar_ut` instead of `lar`.
 */
class TrackProxyTest : public art::EDAnalyzer {
    public:
  
  struct Config {
    using Name = fhicl::Name;
    using Comment = fhicl::Comment;
    
    fhicl::Atom<art::InputTag> tracksTag{
      Name("tracks"),
      Comment("tag of the recob::Track data products to run the test on.")
      };
    
  }; // struct Config
  
  using Parameters = art::EDAnalyzer::Table<Config>;
  
  explicit TrackProxyTest(Parameters const& config)
    : art::EDAnalyzer(config)
    , tracksTag(config().tracksTag())
    {}
  
  // Plugins should not be copied or assigned.
  TrackProxyTest(TrackProxyTest const &) = delete;
  TrackProxyTest(TrackProxyTest &&) = delete;
  TrackProxyTest& operator= (TrackProxyTest const &) = delete;
  TrackProxyTest& operator= (TrackProxyTest &&) = delete;
  
  virtual void analyze(art::Event const& event) override;
  
    private:
  art::InputTag tracksTag; ///< Tag for the input tracks.
  
  /// An example of how to access the information via track proxy.
  void proxyUsageExample(art::Event const& event);
  
  /// Performs the actual test.
  void testTracks(art::Event const& event);
  
}; // TrackProxyTest


//------------------------------------------------------------------------------
void TrackProxyTest::analyze(art::Event const& event) {
  
  proxyUsageExample(event);
  
  testTracks(event);
  
} // TrackProxyTest::analyze()


//------------------------------------------------------------------------------
void TrackProxyTest::proxyUsageExample(art::Event const& event) {
  
  auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag);
  
  if (tracks.empty()) {
    mf::LogVerbatim("TrackProxyTest") << "No tracks in '" << tracksTag.encode()
      << "'";
    return;
  }
  
  mf::LogVerbatim("TrackProxyTest") << "Collection '" << tracksTag.encode()
    << "' contains " << tracks.size() << " tracks.";
  
  for (auto track: tracks) {
    
    recob::Track const& trackRef = track.track();
    
    mf::LogVerbatim log("TrackProxyTest");
    log << "Track " << trackRef
      << "\n  with " << trackRef.NPoints() << " points and " << track.nHits()
        << " hits:";
    
    for (auto const& point: track.points()) {
      log <<
        "\n  [#" << point.index() << "] at " << point.position()
          << " (momentum: " << point.momentum() << "), flags: "
          << point.flags();
      
      recob::Hit const* hit = point.hit();
      if (hit) {
        log << " with a Q=" << hit->Integral() << " hit on channel "
          << hit->Channel() << " at tick " << hit->PeakTime();
      }
      else
        log << " (no associated hit)";
      
    } // for points in track
    
  } // for track
  
} // TrackProxyTest::proxyUsageExample()


void TrackProxyTest::testTracks(art::Event const& event) {
  
  auto expectedTracksHandle
    = event.getValidHandle<std::vector<recob::Track>>(tracksTag);
  auto const& expectedTracks = *expectedTracksHandle;
  
  mf::LogInfo("TrackProxyTest")
    << "Starting test on " << expectedTracks.size() << " tracks from '"
    << tracksTag.encode() << "'";
  
  art::FindManyP<recob::Hit> hitsPerTrack
    (expectedTracksHandle, event, tracksTag);
  
  auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag);
  
  BOOST_CHECK_EQUAL(tracks.empty(), expectedTracks.empty());
  BOOST_CHECK_EQUAL(tracks.size(), expectedTracks.size());
  
  std::size_t iExpectedTrack = 0;
  for (auto trackProxy: tracks) {
    auto const& expectedTrack = expectedTracks[iExpectedTrack];
    auto const& expectedHits = hitsPerTrack.at(iExpectedTrack);
    
    recob::Track const& trackRef = *trackProxy;
    
    BOOST_CHECK_EQUAL(&trackRef, &expectedTrack);
    BOOST_CHECK_EQUAL(&(trackProxy.track()), &expectedTrack);
    BOOST_CHECK_EQUAL(trackProxy.nHits(), expectedHits.size());
    
    // direct interface to recob::Track
    BOOST_CHECK_EQUAL(trackProxy->NPoints(), expectedTrack.NPoints());
    
    std::size_t iPoint = 0;
    for (auto const& pointInfo: trackProxy.points()) {
      
      decltype(auto) expectedPointFlags = expectedTrack.FlagsAtPoint(iPoint);
      
      BOOST_CHECK_EQUAL(pointInfo.index(), iPoint);
      BOOST_CHECK_EQUAL(
        pointInfo.position(),
        expectedTrack.Trajectory().LocationAtPoint(iPoint)
        );
      BOOST_CHECK_EQUAL
        (pointInfo.momentum(), expectedTrack.MomentumVectorAtPoint(iPoint));
      BOOST_CHECK_EQUAL(pointInfo.flags(), expectedPointFlags);
      if (expectedPointFlags.hasOriginalHitIndex()) {
        BOOST_CHECK_EQUAL
          (pointInfo.hitPtr().key(), expectedPointFlags.fromHit());
      }
      else {
        BOOST_CHECK(!pointInfo.hitPtr());
      }
      
      ++iPoint;
    } // for
    BOOST_CHECK_EQUAL(iPoint, expectedTrack.NPoints());
    
    
    ++iExpectedTrack;
  } // for
  BOOST_CHECK_EQUAL(iExpectedTrack, expectedTracks.size());
  
} // TrackProxyTest::testTracks()


//------------------------------------------------------------------------------

DEFINE_ART_MODULE(TrackProxyTest)
