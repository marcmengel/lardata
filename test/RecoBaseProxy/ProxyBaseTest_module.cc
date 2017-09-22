/**
 * @file   ProxyBaseTest_module.cc
 * @brief  Tests feattures of `ProxyBase.h`.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   September 8, 2017
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
#include <algorithm> // std::for_each()
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
class ProxyBaseTest : public art::EDAnalyzer {
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
  
  explicit ProxyBaseTest(Parameters const& config)
    : art::EDAnalyzer(config)
    , tracksTag(config().tracksTag())
    {}
  
  // Plugins should not be copied or assigned.
  ProxyBaseTest(ProxyBaseTest const &) = delete;
  ProxyBaseTest(ProxyBaseTest &&) = delete;
  ProxyBaseTest& operator= (ProxyBaseTest const &) = delete;
  ProxyBaseTest& operator= (ProxyBaseTest &&) = delete;
  
  virtual void analyze(art::Event const& event) override;
  
    private:
  art::InputTag tracksTag; ///< Tag for the input tracks.
  
  /// An example of how to access the information via track proxy.
  void proxyUsageExample(art::Event const& event) const;
  
  /// Returns proxies to tracks longer than a certain length.
  auto getLongTracks(art::Event const& event, double minLength) const;
  
  /// Tests proxy composition.
  void testProxyComposition(art::Event const& event) const;
  
  /// Performs the actual test.
  void testTracks(art::Event const& event) const;
  
  /// Single-track processing function example.
  template <typename Track>
  void processTrack(Track const& track) const;
  
}; // class ProxyBaseTest


//------------------------------------------------------------------------------
template <typename Track>
void ProxyBaseTest::processTrack(Track const& track) const {
  
  recob::Track const& trackRef = *track;
  
  mf::LogVerbatim("ProxyBaseTest")
    << "[#" << track.index() << "] track " << trackRef
    << "  " << track->Length() << " cm long, with "
    << trackRef.NPoints() << " points and "
    << track.template get<recob::Hit>().size()
    << " hits:";
  
} // ProxyBaseTest::processTrack()


//------------------------------------------------------------------------------
namespace tag {
  struct SpecialHits {};
  struct DirectHitAssns {};
  struct DirectFitInfo {};
  struct TrackSubproxy {};
  struct FitInfoProxy {};
}


//------------------------------------------------------------------------------
void ProxyBaseTest::proxyUsageExample(art::Event const& event) const {
  
  auto tracks = proxy::getCollection<std::vector<recob::Track>>
    (event, tracksTag, proxy::withAssociated<recob::Hit>());
  
  if (tracks.empty()) {
    mf::LogVerbatim("ProxyBaseTest") << "No tracks in '" << tracksTag.encode()
      << "'";
    return;
  }
  
  mf::LogVerbatim("ProxyBaseTest") << "Collection '" << tracksTag.encode()
    << "' contains " << tracks.size() << " tracks.";
  
  for (auto trackInfo: tracks) {
    
    recob::Track const& track = *trackInfo; // access to the track
    double const startTheta = track.Theta();
    
    double const length = trackInfo->Length(); // access to track members
  
    // access to associated data (returns random-access collection-like object)
    decltype(auto) hits = trackInfo.get<recob::Hit>();
    
    double charge = 0.0;
    for (auto const& hitPtr: hits) {
      charge += hitPtr->Integral();
    } // for hits
    
    mf::LogVerbatim("Info")
      << "[#" << trackInfo.index() << "] track ID=" << track.ID()
      << " (" << length << " cm, starting with theta=" << startTheta
      << " rad) deposited charge=" << charge
      << " with " << hits.size() << " hits";
    
  } // for tracks
 
} // ProxyBaseTest::proxyUsageExample()


//------------------------------------------------------------------------------
auto ProxyBaseTest::getLongTracks
  (art::Event const& event, double minLength) const
{
  //
  // this code is not a particularly good practice, but it is aimed to check
  // that after the proxy collection is out of scope, elements copied from it
  // are still valid
  //
  auto tracks = proxy::getCollection<std::vector<recob::Track>>
    (event, tracksTag, proxy::withAssociated<recob::Hit>() );
  
  std::vector<decltype(tracks)::element_proxy_t> longTracks;
  for (auto track: tracks) {
    if (track->Length() >= minLength) longTracks.push_back(track);
  } // for track
  return longTracks;
  
} // ProxyBaseTest::proxyUsageExample()


//------------------------------------------------------------------------------
void ProxyBaseTest::testProxyComposition(art::Event const& event) const {
  
  auto const& expectedTracks
    = *(event.getValidHandle<std::vector<recob::Track>>(tracksTag));
  
  mf::LogInfo("ProxyBaseTest")
    << "Starting test on " << expectedTracks.size() << " tracks from '"
    << tracksTag.encode() << "'";
  
  auto directTracks = proxy::getCollection<std::vector<recob::Track>>(
    event, tracksTag
    , proxy::withParallelData<std::vector<recob::TrackFitHitInfo>>()
    );

  auto tracks = proxy::getCollection<std::vector<recob::Track>>(
    event, tracksTag
    , proxy::withParallelData<std::vector<recob::TrackFitHitInfo>>()
    , proxy::wrapParallelDataAs<tag::TrackSubproxy>(directTracks)
    /*
    , proxy::withCollectionProxyAs
      <std::vector<recob::TrackFitHitInfo>, tag::FitInfoProxy>
      (tracksTag, proxy::withParallelData<std::vector<recob::Track>>())
    */
    );
  BOOST_CHECK_EQUAL
    (tracks.get<tag::TrackSubproxy>().data(), std::addressof(directTracks));
  
  static_assert(
    std::is_lvalue_reference<decltype(tracks.get<tag::TrackSubproxy>())>(),
    "Not reference!"
    );
  
  auto const& expectedFitHitInfo
    = *(event.getValidHandle<std::vector<std::vector<recob::TrackFitHitInfo>>>
    (tracksTag));
  
  std::size_t iExpectedTrack = 0;
  for (auto trackProxy: tracks) {
    auto const& expectedTrack = expectedTracks[iExpectedTrack];
    auto const& expectedTrackFitInfo = expectedFitHitInfo[iExpectedTrack];
    
    auto directTrackProxy = trackProxy.get<tag::TrackSubproxy>();
    BOOST_CHECK_EQUAL
      (std::addressof(*directTrackProxy), std::addressof(expectedTrack));
    BOOST_CHECK_EQUAL(directTrackProxy->ID(), expectedTrack.ID());
    BOOST_CHECK_EQUAL(directTrackProxy->Length(), expectedTrack.Length());
    
    BOOST_CHECK_EQUAL(
      std::addressof
        (directTrackProxy.get<std::vector<recob::TrackFitHitInfo>>()),
      std::addressof(expectedTrackFitInfo)
      );
    /*
    auto fitInfoProxy = trackProxy.get<tag::FitInfoProxy>();
    BOOST_CHECK_EQUAL
      (std::addressof(*fitInfoProxy), std::addressof(expectedFitHitInfo));
    BOOST_CHECK_EQUAL(
      std::addressof(fitInfoProxy.get<recob::Track>()),
      std::addressof(expectedTrack)
      );
    */
    ++iExpectedTrack;
  } // for
  
  BOOST_CHECK_EQUAL(iExpectedTrack, expectedTracks.size());
  
} // ProxyBaseTest::testProxyComposition()

//------------------------------------------------------------------------------
void ProxyBaseTest::testTracks(art::Event const& event) const {
  
  auto expectedTracksHandle
    = event.getValidHandle<std::vector<recob::Track>>(tracksTag);
  auto const& expectedTracks = *expectedTracksHandle;
  
  auto const& expectedTrackHitAssns
    = *(event.getValidHandle<art::Assns<recob::Track, recob::Hit>>(tracksTag));
  
  mf::LogInfo("ProxyBaseTest")
    << "Starting test on " << expectedTracks.size() << " tracks from '"
    << tracksTag.encode() << "'";
  
  art::FindManyP<recob::Hit> hitsPerTrack
    (expectedTracksHandle, event, tracksTag);
  
  auto const& expectedTrackFitHitInfo
    = *(event.getValidHandle<std::vector<std::vector<recob::TrackFitHitInfo>>>
    (tracksTag));
  
  auto directTracks = proxy::getCollection<std::vector<recob::Track>>(
    event, tracksTag,
    proxy::withParallelData<std::vector<recob::TrackFitHitInfo>>()
    );
    
  auto tracks = proxy::getCollection<std::vector<recob::Track>>(
    event, tracksTag
    , proxy::withAssociated<recob::Hit>()
    , proxy::withAssociatedAs<recob::Hit, tag::SpecialHits>()
    , proxy::withParallelData<std::vector<recob::TrackFitHitInfo>>()
//     , proxy::withCollectionProxyAs
//         <std::vector<recob::Track>, tag::TrackSubproxy>
//         (tracksTag, proxy::withParallelData<std::vector<recob::TrackFitHitInfo>>())
    , proxy::wrapAssociatedAs<tag::DirectHitAssns>(expectedTrackHitAssns)
    , proxy::wrapParallelDataAs<tag::DirectFitInfo>(expectedTrackFitHitInfo)
    , proxy::wrapParallelDataAs<tag::TrackSubproxy>(directTracks)
    );
  
  //
  // we try to access something we did not "register" in the proxy: space points
  //
  static_assert(!tracks.has<recob::SpacePoint>(),
    "Track proxy does NOT have space points available!!!"); 
  BOOST_CHECK_THROW(tracks.getIf<recob::SpacePoint>(), std::logic_error);
  
  static_assert(
    tracks.has<std::vector<recob::TrackFitHitInfo>>(),
    "recob::TrackFitHitInfo not found!!!"
    );
  
  
  BOOST_CHECK_EQUAL(tracks.empty(), expectedTracks.empty());
  BOOST_CHECK_EQUAL(tracks.size(), expectedTracks.size());
  
  BOOST_CHECK_EQUAL(tracks.size(), expectedTrackFitHitInfo.size());
  decltype(auto) allFitHitInfo
    = tracks.get<std::vector<recob::TrackFitHitInfo>>();
  static_assert(
    std::is_lvalue_reference<decltype(allFitHitInfo)>(),
    "Copy of parallel data!"
    );
  BOOST_CHECK_EQUAL
    (allFitHitInfo.data(), std::addressof(expectedTrackFitHitInfo));
  
  BOOST_CHECK_EQUAL(
    tracks.get<tag::DirectFitInfo>().data(),
    std::addressof(expectedTrackFitHitInfo)
    );
  
  BOOST_CHECK_EQUAL(
    directTracks.get<std::vector<recob::TrackFitHitInfo>>().data(),
    std::addressof(expectedTrackFitHitInfo)
    );
  
  BOOST_CHECK_EQUAL(
    tracks.get<tag::TrackSubproxy>().data(),
    std::addressof(directTracks)
    );
  
  auto fitHitInfoSize
    = std::distance(allFitHitInfo.begin(), allFitHitInfo.end());
  BOOST_CHECK_EQUAL(fitHitInfoSize, expectedTrackFitHitInfo.size());
  
  std::size_t iExpectedTrack = 0;
  for (auto trackProxy: tracks) {
    auto const& expectedTrack = expectedTracks[iExpectedTrack];
    auto const& expectedHits = hitsPerTrack.at(iExpectedTrack);
    auto const& expectedFitHitInfo = expectedTrackFitHitInfo[iExpectedTrack];
    
    // proxies deliver temporary objects as elements, each time a new one
    // (although an exceedingly smart compiler might decide otherwise)
    BOOST_CHECK_NE(
      std::addressof(tracks[iExpectedTrack]),
      std::addressof(tracks[iExpectedTrack])
      );
    
    recob::Track const& trackRef = *trackProxy;
    
    auto const trackProxyCopy = trackProxy;
    BOOST_CHECK_NE(std::addressof(trackProxyCopy), std::addressof(trackProxy));
    
    BOOST_CHECK_EQUAL
      (std::addressof(trackRef), std::addressof(expectedTrack));
    BOOST_CHECK_EQUAL
      (std::addressof(*trackProxy), std::addressof(expectedTrack));
    BOOST_CHECK_EQUAL(trackProxy.get<recob::Hit>().size(), expectedHits.size());
    BOOST_CHECK_EQUAL(trackProxy.index(), iExpectedTrack);
    
    std::vector<recob::TrackFitHitInfo> const& fitHitInfo
      = trackProxy.get<std::vector<recob::TrackFitHitInfo>>();
    static_assert(
      std::is_lvalue_reference<decltype(fitHitInfo)>(),
      "Copy of parallel data element!"
      );
    BOOST_CHECK_EQUAL
      (std::addressof(fitHitInfo), std::addressof(expectedFitHitInfo));
    BOOST_CHECK_EQUAL(fitHitInfo.size(), expectedFitHitInfo.size());
    
    BOOST_CHECK_EQUAL(
      std::addressof(trackProxy.get<tag::DirectFitInfo>()),
      std::addressof(expectedTrackFitHitInfo[iExpectedTrack])
      );
    
    BOOST_CHECK_EQUAL(
      std::addressof(trackProxyCopy.get<tag::DirectFitInfo>()),
      std::addressof(trackProxy.get<tag::DirectFitInfo>())
      );
    
    // subproxy elements are typically temporaries
    BOOST_CHECK_NE(
      std::addressof(trackProxyCopy.get<tag::TrackSubproxy>()),
      std::addressof(trackProxy.get<tag::TrackSubproxy>())
      );
    
    auto directTrackProxy = trackProxy.get<tag::TrackSubproxy>();
    BOOST_CHECK_EQUAL
      (std::addressof(*directTrackProxy), std::addressof(expectedTrack));
    BOOST_CHECK_EQUAL(directTrackProxy->ID(), expectedTrack.ID());
    BOOST_CHECK_EQUAL(directTrackProxy->Length(), expectedTrack.Length());
    BOOST_CHECK_EQUAL(
      std::addressof
        (directTrackProxy.get<std::vector<recob::TrackFitHitInfo>>()),
      std::addressof(fitHitInfo)
      );
    
    BOOST_CHECK_EQUAL
      (trackProxy.get<tag::SpecialHits>().size(), expectedHits.size());
    
    BOOST_CHECK_EQUAL
      (trackProxy.get<tag::DirectHitAssns>().size(), expectedHits.size());
    
    // direct interface to recob::Track
    BOOST_CHECK_EQUAL(trackProxy->NPoints(), expectedTrack.NPoints());
    
    ++iExpectedTrack;
  } // for
  BOOST_CHECK_EQUAL(iExpectedTrack, expectedTracks.size());
  
} // ProxyBaseTest::testTracks()


//------------------------------------------------------------------------------
void ProxyBaseTest::analyze(art::Event const& event) {
  
  // "test" that track proxies survive their collection (part I)
  const double minLength = 30.0;
  auto const longTracks = getLongTracks(event, minLength);
  
  // usage example (supposed to be educational)
  proxyUsageExample(event);
  
  // actual test
  testTracks(event);
  
  // test proxy composition
  testProxyComposition(event);
  
  // "test" that track proxies survive their collection (part II)
  mf::LogVerbatim("ProxyBaseTest")
    << longTracks.size() << " tracks are longer than " << minLength << " cm:";
  std::for_each(longTracks.begin(), longTracks.end(),
    [this](auto const& track){ this->processTrack(track); });
  
} // ProxyBaseTest::analyze()


//------------------------------------------------------------------------------

DEFINE_ART_MODULE(ProxyBaseTest)
