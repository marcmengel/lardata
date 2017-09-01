/**
 * @file   TrackProxyTest_module.cc
 * @brief  Tests `proxy::Track` class.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 */


// LArSoft libraries
#include "lardata/RecoBaseProxy/Track.h" // proxy namespace
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/TrackFitHitInfo.h"

#include "larcorealg/CoreUtils/DebugUtils.h" // lar::debug::demangle()
#include "larcorealg/CoreUtils/UncopiableAndUnmovableClass.h"

// framework libraries
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
  
  /// Single-track processing function example.
  template <typename Track>
  void processTrack(Track const& track) const;
  
  /// Single-track-point processing function example.
  template <typename TrackPoint>
  void processPoint(TrackPoint const& point) const;
  
}; // class TrackProxyTest


//------------------------------------------------------------------------------
void TrackProxyTest::analyze(art::Event const& event) {
  
  proxyUsageExample(event);
  
  testTracks(event);
  
} // TrackProxyTest::analyze()


//------------------------------------------------------------------------------
template <typename TrackPoint>
void TrackProxyTest::processPoint(TrackPoint const& point) const {
  
  mf::LogVerbatim log("TrackProxyTest");
  log <<
    "  [#" << point.index() << "] at " << point.position()
      << " (momentum: " << point.momentum() << "), flags: "
      << point.flags();
  
  recob::Hit const* hit = point.hit();
  if (hit) {
    log << " with a Q=" << hit->Integral() << " hit on channel "
      << hit->Channel() << " at tick " << hit->PeakTime()
      << ", measured: " << point.fitInfoPtr()->hitMeas();
  }
  else
    log << " (no associated hit)";
  
} // TrackProxyTest::processPoint()


//------------------------------------------------------------------------------
template <typename Track>
void TrackProxyTest::processTrack(Track const& track) const {
  
  recob::Track const& trackRef = track.track();
  
  mf::LogVerbatim("TrackProxyTest")
    << "[#" << track.index() << "] track " << trackRef
    << "\n  with " << trackRef.NPoints() << " points and " << track.nHits()
    << " hits:";
  
  for (auto point: track.points()) {
    processPoint(point);
  } // for points in track
  
} // TrackProxyTest::processTrack()


//------------------------------------------------------------------------------
void TrackProxyTest::proxyUsageExample(art::Event const& event) {
  
  auto tracks = proxy::getCollection<proxy::Tracks>
    (event, tracksTag, proxy::withFitHitInfo());
  
  if (tracks.empty()) {
    mf::LogVerbatim("TrackProxyTest") << "No tracks in '" << tracksTag.encode()
      << "'";
    return;
  }
  
  mf::LogVerbatim("TrackProxyTest") << "Collection '" << tracksTag.encode()
    << "' contains " << tracks.size() << " tracks.";
  
  for (auto track: tracks) {
    
    processTrack(track);
    
  } // for track
  
} // TrackProxyTest::proxyUsageExample()

namespace tag {
  struct SpecialHits {};
}

void TrackProxyTest::testTracks(art::Event const& event) {
  
  auto expectedTracksHandle
    = event.getValidHandle<std::vector<recob::Track>>(tracksTag);
  auto const& expectedTracks = *expectedTracksHandle;
  
  mf::LogInfo("TrackProxyTest")
    << "Starting test on " << expectedTracks.size() << " tracks from '"
    << tracksTag.encode() << "'";
  
  art::FindManyP<recob::Hit> hitsPerTrack
    (expectedTracksHandle, event, tracksTag);
  
  auto const& expectedTrackFitHitInfo
    = *(event.getValidHandle<std::vector<std::vector<recob::TrackFitHitInfo>>>
    (tracksTag));
  
  auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag
    , proxy::withAssociatedAs<recob::Hit, tag::SpecialHits>()
    , proxy::withFitHitInfo()
    );
  
  //
  // we try to access something we did not "register" in the proxy: space points
  //
  static_assert(!tracks.has<recob::SpacePoint>(),
    "Track proxy does NOT have space points available!!!"); 
  BOOST_CHECK_THROW(tracks.getIf<recob::SpacePoint>(), std::logic_error);
  
  static_assert(
    tracks.has<recob::TrackFitHitInfo>(),
    "recob::TrackFitHitInfo not found!!!"
    );
  
  
  BOOST_CHECK_EQUAL(tracks.empty(), expectedTracks.empty());
  BOOST_CHECK_EQUAL(tracks.size(), expectedTracks.size());
  
  BOOST_CHECK_EQUAL(tracks.size(), expectedTrackFitHitInfo.size());
  decltype(auto) allFitHitInfo = tracks.get<recob::TrackFitHitInfo>();
  static_assert(
    std::is_lvalue_reference<decltype(allFitHitInfo)>(),
    "Copy of parallel data!"
    );
  BOOST_CHECK_EQUAL
    (allFitHitInfo.data(), std::addressof(expectedTrackFitHitInfo));
  
  auto fitHitInfoSize
    = std::distance(allFitHitInfo.begin(), allFitHitInfo.end());
  BOOST_CHECK_EQUAL(fitHitInfoSize, expectedTrackFitHitInfo.size());
  
  std::size_t iExpectedTrack = 0;
  for (auto trackProxy: tracks) {
    auto const& expectedTrack = expectedTracks[iExpectedTrack];
    auto const& expectedHits = hitsPerTrack.at(iExpectedTrack);
    auto const& expectedFitHitInfo = expectedTrackFitHitInfo[iExpectedTrack];
    
    recob::Track const& trackRef = *trackProxy;
    
    BOOST_CHECK_EQUAL
      (std::addressof(trackRef), std::addressof(expectedTrack));
    BOOST_CHECK_EQUAL
      (std::addressof(trackProxy.track()), std::addressof(expectedTrack));
    BOOST_CHECK_EQUAL(trackProxy.nHits(), expectedHits.size());
    BOOST_CHECK_EQUAL(trackProxy.index(), iExpectedTrack);
    
    decltype(auto) fitHitInfo = trackProxy.get<recob::TrackFitHitInfo>();
    static_assert(
      std::is_lvalue_reference<decltype(fitHitInfo)>(),
      "Copy of parallel data element!"
      );
    BOOST_CHECK_EQUAL
      (std::addressof(fitHitInfo), std::addressof(expectedFitHitInfo));
    BOOST_CHECK_EQUAL(fitHitInfo.size(), expectedFitHitInfo.size());
    
    BOOST_CHECK_EQUAL
      (trackProxy.get<tag::SpecialHits>().size(), expectedHits.size());
    
    // direct interface to recob::Track
    BOOST_CHECK_EQUAL(trackProxy->NPoints(), expectedTrack.NPoints());
    
    std::size_t iPoint = 0;
    for (auto pointInfo: trackProxy.points()) {
      
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
      
      BOOST_CHECK_EQUAL
        (fitHitInfo[iPoint].WireId(), expectedFitHitInfo[iPoint].WireId());
      BOOST_CHECK_EQUAL
        (pointInfo.fitInfoPtr(), std::addressof(expectedFitHitInfo[iPoint]));
      BOOST_CHECK_EQUAL(
        std::addressof(fitHitInfo[iPoint]),
        std::addressof(expectedFitHitInfo[iPoint])
        );
      
      ++iPoint;
    } // for
    BOOST_CHECK_EQUAL(iPoint, expectedTrack.NPoints());
    
    ++iExpectedTrack;
  } // for
  BOOST_CHECK_EQUAL(iExpectedTrack, expectedTracks.size());
  
} // TrackProxyTest::testTracks()


//------------------------------------------------------------------------------

DEFINE_ART_MODULE(TrackProxyTest)
