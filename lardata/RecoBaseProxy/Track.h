/**
 * @file   Track.h
 * @brief  Offers `proxy::Tracks` and `proxy::Track` class for
 *         `recob::Track` access.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 */

/**
 * @defgroup LArSoftProxyTracks proxy::Tracks (recob::Track proxy)
 * @brief Proxy for a `recob::Track` collection.
 * @ingroup LArSoftProxyReco
 * 
 * 
 * Track proxies is a way to facilitate the navigation of `recob::Track` data
 * objects.
 * The ensemble of fundamental data of a track collection includes:
 * * the tracks themselves, in a `std::vector<recob::Track>` collection
 * * the associated hits, in a `art::Assns<recob::Track, recob::Hit>` data
 *   product
 * 
 * Special customisations are provided for:
 * * the associated hits (automatically pulled in): the
 *   information is provided with `recob::Hit` tag, with the
 *   dedicated accessor `hits()`, `nHits()` and `hitAtPoint()` of the track
 *    proxy and with `hitPtr()` and `hit()` when accessing a single point
 * * the track fit information: include it with `withFitHitInfo()`; the
 *   information is provided with `recob::TrackFitHitInfo` tag, with the
 *   dedicated accessor `fitInfoAtPoint()` of the track proxy and with
 *   `fitInfoPtr()` when accessing a single point
 * 
 * LArSoft prescribes conventions to be followed, which include:
 * * a track has at least two trajectory points
 * * for each track, there is one hit per trajectory point
 * * the association between tracks and hits is created with tracks as first
 *   ("left") element, and hits as second one ("right")
 * * hits in the association are in a well-defined order: first are the hits of
 *   the first track, sorted in the same way as their trajectory points; then
 *   the second track hits come, likewise; and all tracks follow in order;
 *   this is called the "one-to-many sequential association" requirement
 *   documented in `ProxyBase.h`.
 * 
 * For track data products respecting this convention, a track proxy provides
 * an interface to navigate the track information.
 * 
 * @note The interface is experimental, and it is likely not to include all the
 *       features you may need. If you find a missing feature, or find a use
 *       case violating an assumption, or you find the interface cumbersome to
 *       use, please contact the author for a discussion on how to improve this
 *       utility.
 * 
 * 
 * Obtaining a track proxy
 * ========================
 * 
 * Track proxies are created by specifying the tag of the tracks, and the event
 * to read them from.
 * 
 * To create a track proxy:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Here we ask `getCollection()` to create a proxy of category `proxy::Tracks`.
 * Each proxy is a different beast which needs to be explicitly supported:
 * here support for the proxy to `recob::Track` is described.
 * 
 * The additional customizations for track proxy are described above.
 * For example, if the module with label `fitTag` stored the fit information for
 * the track, that information can be merged as:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<proxy::Tracks>
 *   (event, tracksTag, withFitHitInfo(fitTag));
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * while in the more likely case where that information was produced together
 * with the tracks, 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<proxy::Tracks>
 *   (event, tracksTag, withFitHitInfo());
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * will suffice. After this, the interface specific to `recob::TrackFitHitInfo`
 * will be available. Otherwise, any attempt to use it will cause (complicate)
 * compilation errors.
 * 
 * In addition, any type of data can be associated using the generic collection
 * proxy interface (`withAssociated()`, `withParallelData()`, etc.).
 * 
 * 
 * The C++ type of `tracks` object should not matter to the user, and it depends
 * on which additional data are merged in.
 * 
 * 
 * Types of proxies, and what to do with them
 * ===========================================
 * 
 * Currently there are three different type of proxy-like objects for tracks.
 * Each one supports a specific concept:
 * 
 * * `proxy::Tracks` represents the whole collection of tracks; it covers
 *   the tracks themselves, and their associated hits. It is obtained by calling
 *   `getCollection()` as described above.
 * * `proxy::Track` represents a single track; the list of hits and points,
 *   and of course the `recob::Track` object itself, can be accessed through it.
 *   Track proxies are obtained from a track collection proxy (`proxy::Tracks`).
 * * `proxy::TrackPoint` represents a single trajectory point in a track.
 *   It provide access to position and momentum of the track at that point,
 *   associated hit and point flags. Track point proxies are obtained from a
 *   track proxy (`proxy::Track`).
 * 
 * For the details of the interface and the information that is exposed by each
 * of these proxy classes, please refer to each class documentation. In
 * particular, see `proxy::Tracks` documentation for more usage examples.
 * 
 * @note The interface allows by deliberate design only read-only access to the
 *       underlying data.
 * 
 * 
 * @section LArSoftProxyTracksTech Technical details
 * 
 * @subsection LArSoftProxyTracksTechDetails Track collection proxy
 * 
 * The track collection proxy object is derived from
 * `proxy::CollectionProxyBase`, which points to the original (track) data
 * product.
 * In addition, it contains a `proxy::details::AssociatedData` object for
 * the `recob::Track`--`recob::Hit` association list.
 * 
 * The `proxy::Tracks` interface is currently quite limited: it only allows to
 * access a track by index, or to iterate through all of them, in addition to
 * know how many tracks are available (`size()`).
 * 
 * The object returned when accessing the single track information,
 * `proxy::Track`, actually contains enough information so that it is
 * independent of the track collection proxy. This is derived from the generic
 * collection element proxy object (`proxy::CollectionProxyElement`).
 * 
 * The object describing the information of a single point has the interface of
 * `TrackPointWrapper`. The underlying storage includes pointers to the track,
 * the associated hit, and the index of the point in the track. This is
 * track-specific and it is not part of the generic proxy infrastructure.
 * 
 * 
 * @subsection LArSoftProxyTracksCustom Track proxy as an example of proxy customization
 * 
 * The proxy utilities provide the basic functionality of the track proxy,
 * including the track collection proxy, representing all the tracks in the
 * event, and the track proxy, representing a single track. Access to a third
 * tier, the single trajectory point, as proxy is not covered by the basic
 * framework, and it has been implemented here from scratch.
 * 
 * The most relevant customization pertains proxy to the single track. The proxy
 * is derived from `proxy::details::CollectionProxyElement` for basic
 * functionality, which is enriched by a custom interface that does not in fact
 * add functionality, except for the trajectory point proxy described below.
 * The customizations are fairly trivial, overlaying user-friendly names on the
 * existing functionality. A step beyond that is the access to data that might
 * not be present, that is the fit information. The proxy takes the necessary
 * steps to determine (statically) whether that information is present, and to
 * provide a null pointer to the information if that is not the case. This is
 * a functionality that can't be completely implemented by the basic proxy code,
 * since that generic code has no clue about what type of null pointer to return
 * when the tag `Tracks::TrackFitHitInfoTag` is unknown. This information is
 * provided by the track proxy via the `getIf()` call.
 * For convenience, `proxy::Track` is defined as alias of this single track
 * proxy.
 * 
 * The collection proxy customization is way more straightforward.
 * A proxy "tag" is defined, `proxy::Tracks`, with the only purpose of
 * identifying the collection proxy for tracks. For convenience, it hosts a
 * definition used internally to identify one of the optional auxiliary data,
 * and the type of the main data product, but both definitions are contingent
 * and their presence in there is only to centralize some customization in a
 * single place.
 * Traits (`CollectionProxyMakerTraits<Tracks>`) are specialized to inform that
 * this `proxy::Tracks` proxy relies on `std::vector<recob::Track>` as main
 * data product collection type (`std::vector<recob::Track>` is read as
 * `proxy::Tracks`, but again, this is a contingent detail).
 * Then, the creation of the collection proxy is customised by specializing
 * `CollectionProxyMaker` (`CollectionProxyMaker<Tracks>`). The purpose is for
 * it to create a `proxy::CollectionProxyBase` object, and the simple
 * customization just automatically adds the hits, using `withAssociated()`.
 * Finally, some customized functions may be provided for convenience, like
 * `withFitHitInfo()` as alias of `withAssociated<recob::TrackFitHitInfo>()`.
 * 
 * 
 * The trajectory point proxy has been implemented from scratch here, and it is
 * much less refined than the generic proxy code. It is based on a data
 * structure with a selected list of pointers to the actual data. This structure
 * is implemented as a `std::tuple`. On top of it, a wrapper provides the
 * interface (by interface substitution). This choice is non-essential and has
 * been taken to stress the separation between data storage and interface.
 * Point data structures are created by the proxy on demand, and they are
 * designed so that they don't become invalid when the original proxies fall out
 * of scope, at the price of added memory usage (the minimal information would
 * be a pointer to the track proxy and a point index).
 * 
 * Summary of the customization procedure:
 * -# define the tag to identify the proxy (`proxy::Tracks`)
 * -# choose what type of object that will be (`proxy::CollectionProxyBase`
 *      should do for most)
 * -# define the main data product type (`std::vector<recob::Track>`) and set
 *     the traits of the collection proxy (often, deriving them from
 *     `proxy::CollectionProxyMakerTraits` with the main data product type as
 *     template argument is enough)
 * -# (_optional_) customize the element type; deriving it from
 *      `proxy::CollectionProxyElement` is recommended
 * -# customize the creation of the proxy collection, if either:
 *     * the proxy element is being customised; if using
 *       `proxy::CollectionProxyBase` as collection proxy, its first template
 *       argument must be the element type (here, `proxy::Track`)
 *     * special logic or default components are specified for the proxy
 *       (here, `withAssociated<recob::Hit>()`)
 *   In this case, `proxy::CollectionProxyMaker` must be specialized (for
 *   `proxy::Tracks`), and a starting point may be to derive the specialization
 *   from `proxy::CollectionProxyMakerBase` and redefine its `make()` member
 * 
 * 
 * @subsection LArSoftProxyTracksOverhead Overhead
 * 
 * See the notes on @ref LArSoftProxyOverhead "overhead" in `ProxyBase.h`.
 * 
 * 
 */

#ifndef LARDATA_RECOBASEPROXY_TRACK_H
#define LARDATA_RECOBASEPROXY_TRACK_H


// LArSoft libraries
#include "lardata/RecoBaseProxy/ProxyBase.h" // proxy namespace
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/TrackFitHitInfo.h"

// framework libraries
#include "canvas/Persistency/Common/Ptr.h"


namespace proxy {
  
  //----------------------------------------------------------------------------
  // forward declarations
  template <typename TrackProxy>
  class TrackPointIterator;

  template <typename Data>
  class TrackPointWrapper;
  
  //----------------------------------------------------------------------------
  namespace details {
    
    template <typename CollProxy>
    struct TrackPointIteratorBox;
    
    template <typename T>
    struct isTrackProxy;
    
    template <typename Obj>
    struct StaticAsserts;
    
    template <typename Data>
    struct StaticAsserts<TrackPointWrapper<Data>>: public std::true_type {
      using Wrapper_t = TrackPointWrapper<Data>;
      
      static_assert(sizeof(Wrapper_t) == 1U, "Wrapper carries data!");
      
      static_assert(std::is_same<
        std::decay_t<decltype(std::declval<Wrapper_t>().position())>,
        recob::Track::Point_t
        >(),
        "position() is not a recob::Track::Point_t"
        );
      static_assert(std::is_same<
        std::decay_t<decltype(std::declval<Wrapper_t>().momentum())>,
        recob::Track::Vector_t
        >(),
        "momentum() is not a recob::Track::Vector_t"
          );
      static_assert(std::is_same<
        std::decay_t<decltype(std::declval<Wrapper_t>().flags())>,
        recob::Track::PointFlags_t
        >(),
        "flags() is not a recob::Track::PointFlags_t"
        );
      static_assert(std::is_same<
        std::decay_t<decltype(std::declval<Wrapper_t>().hitPtr())>,
        art::Ptr<recob::Hit>
        >(),
        "hit() is not a art::Ptr<recob::Hit>"
        );
      static_assert(std::is_same<
        std::decay_t<decltype(std::declval<Wrapper_t>().index())>,
        std::size_t
        >(),
        "index() is not a std::size_t"
        );
      
    }; // StaticAsserts<TrackPointWrapper<Data>>
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  
  /**
   * @brief Proxy tag for a `recob::Track` collection proxy.
   * @see `proxy::TrackCollectionProxyElement`
   * @ingroup LArSoftProxyTracks
   *
   * This type can be used to get a proxy for `recob::Track` collection:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * An example of usage for a simple track processing loop:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * void MyAnalyzer::analyze(art::Event const& event) {
   *   
   *   auto tracks = proxy::getCollection<proxy::Tracks>
   *     (event, tracksTag, proxy::withFitHitInfo());
   *   
   *   if (tracks.empty()) {
   *     mf::LogVerbatim("TrackProxyTest")
   *       << "No tracks in '" << tracksTag.encode() << "'";
   *     return;
   *   }
   *   
   *   mf::LogVerbatim("TrackProxyTest") << "Collection '" << tracksTag.encode()
   *     << "' contains " << tracks.size() << " tracks.";
   *   
   *   for (auto track: tracks) {
   *     
   *     recob::Track const& trackRef = track.track();
   *     
   *     mf::LogVerbatim log("TrackProxyTest");
   *     log << "[#" << track.index() << "] track " << trackRef
   *       << "\n  with " << trackRef.NPoints() << " points and "
   *       << track.nHits() << " hits:";
   *     
   *     for (auto const& point: track.points()) {
   *       log <<
   *         "\n  [#" << point.index() << "] at " << point.position()
   *           << " (momentum: " << point.momentum() << "), flags: "
   *           << point.flags();
   *       
   *       recob::Hit const* hit = point.hit();
   *       if (hit) {
   *         log << " with a Q=" << hit->Integral() << " hit on channel "
   *           << hit->Channel() << " at tick " << hit->PeakTime()
   *           << ", measured: " << point.fitInfoPtr()->hitMeas();
   *       }
   *       else
   *         log << " (no associated hit)";
   *     
   *     } // for points in track
   *     
   *   } // for track
   *   
   * } // MyAnalyzer::analyze()
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this example, the track proxy accesses the track itself, its associated
   * hits (always implicitly present) and the track fit hit information
   * (explicitly requested). Since both those data products are produced by the
   * same module as the track, there is no need to specify their producer module
   * label.
   * 
   * Unfortunately, the proxy object (`tracks` in the example) can be of a
   * different class depending on which data is merged into it: a proxy created
   * by `getCollection<proxy::Tracks>(event, tag, proxy::withFitHitInfo())` has
   * different type than e.g. `getCollection<proxy::Tracks>(event, tag)`.
   * This implies than when passing proxies as arguments to functions, template
   * types must be used. For example, the following code is equivalent to the
   * one above, but with methods processing a single track (a track proxy) and
   * a single trajectory point (a track point proxy):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * template <typename TrackPoint>
   * void MyAnalyzer::processPoint(TrackPoint const& point) const {
   *   
   *   mf::LogVerbatim log("TrackProxyTest");
   *   log <<
   *     "  [#" << point.index() << "] at " << point.position()
   *       << " (momentum: " << point.momentum() << "), flags: "
   *       << point.flags();
   *   
   *   recob::Hit const* hit = point.hit();
   *   if (hit) {
   *     log << " with a Q=" << hit->Integral() << " hit on channel "
   *       << hit->Channel() << " at tick " << hit->PeakTime()
   *       << ", measured: " << point.fitInfoPtr()->hitMeas();
   *   }
   *   else
   *     log << " (no associated hit)";
   *   
   * } // MyAnalyzer::processPoint()
   * 
   * 
   * //------------------------------------------------------------------------------
   * template <typename Track>
   * void MyAnalyzer::processTrack(Track const& track) const {
   *   
   *   recob::Track const& trackRef = track.track();
   *   
   *   mf::LogVerbatim("TrackProxyTest")
   *     << "[#" << track.index() << "] track " << trackRef
   *     << "\n  with " << trackRef.NPoints() << " points and " << track.nHits()
   *     << " hits:";
   *   
   *   for (auto point: track.points()) {
   *     processPoint(point);
   *   } // for points in track
   *   
   * } // MyAnalyzer::processTrack()
   * 
   * 
   * //------------------------------------------------------------------------------
   * void MyAnalyzer::proxyUsageExample(art::Event const& event) {
   *   
   *   auto tracks = proxy::getCollection<proxy::Tracks>
   *     (event, tracksTag, proxy::withFitHitInfo());
   *   
   *   if (tracks.empty()) {
   *     mf::LogVerbatim("TrackProxyTest") << "No tracks in '"
   *       << tracksTag.encode() << "'";
   *     return;
   *   }
   *   
   *   mf::LogVerbatim("TrackProxyTest") << "Collection '" << tracksTag.encode()
   *     << "' contains " << tracks.size() << " tracks.";
   *   
   *   for (auto track: tracks) {
   *     
   *     processTrack(track);
   *     
   *   } // for track
   *   
   * } // MyAnalyzer::proxyUsageExample()
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * As any other proxy object, other data can be merged to the proxy, but no
   * custom interface will be available. For example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<proxy::Tracks>(
   *   event, tracksTag,
   *   proxy::withParallelData<recob::TrackMomentumFit>(momTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * will add a data product `std::vector<recob::TrackMomentumFit>` expected to
   * have one element (of type `recob::TrackMomentumFit`) per track, which will
   * be accessed with the generic proxy interface:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto track: tracks) {
   *   recob::TrackMomentumFit const& momFit
   *     = track.get<recob::TrackMomentumFit>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The other common features of proxy collections are supported, like tagging
   * of different instances of the same data types:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct MCS {};
   * struct Range {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(
   *   event, tracksTag,
   *   proxy::withParallelDataAs<recob::TrackMomentumFit, Range>(rangeMomTag),
   *   proxy::withParallelDataAs<recob::TrackMomentumFit, MCS>(MCStag)
   *   );
   * for (auto track: tracks) {
   *   recob::TrackMomentumFit const& rangeMom = track.get<Range>();
   *   recob::TrackMomentumFit const& MCSmom = track.get<MCS>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * A new, filtered collection of proxies can be created with obvious means and
   * with a less-than-friendly declaration:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * std::vector<decltype(tracks)::element_proxy_t> longTracks;
   * for (auto track: tracks) {
   *   if (track->Length() >= 30.0) longTracks.push_back(track);
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The collection thus created (`longTracks`) is valid also after the
   * collection proxy (`tracks`) has fallen out of scope.
   * 
   * 
   * @note `proxy::Tracks` is *not* the type of the collection proxy returned
   *       by `getCollection()`.
   */
  struct Tracks {
    
    /// Type of the main collection.
    using TrackDataProduct_t = std::vector<recob::Track>;
    
    /// Tag used for the "standard" track fit information.
    using TrackFitHitInfoTag = recob::TrackFitHitInfo;
    
    /// Tag used for the associated hits.
    using HitTag = recob::Hit;
    
  }; // struct Tracks
  
  
  
  /// Define the traits of `proxy::Tracks` proxy.
  template <>
  struct CollectionProxyMakerTraits<Tracks>
    : public CollectionProxyMakerTraits<Tracks::TrackDataProduct_t>
  {};
  
  
  //----------------------------------------------------------------------------
  //---  track point information
  //---
  /**
   * @brief Container of track point information.
   * @see `proxy::Track`, `proxy::TrackPointWrapper`
   * @ingroup LArSoftProxyTracks
   * 
   * This class contains some information pertaining a single point of a
   * `recob::Track`.
   * 
   * The information is not extensible via the usual proxy mechanisms. The data
   * supported is stored in `proxy::TrackPointData` class, and it currently
   * includes:
   * * the `recob::Track` the point belongs to
   * * the position, momentum and flags of the point
   * * the hit associated to the point
   * * the index of the point in its track
   * * fit information
   * 
   * The access interface is determined by `proxy::TrackPointWrapper`.
   * 
   * An object of this type is returned when accessing a single track point from
   * a track proxy. While the data itself is not copied, this object owns some
   * pointers to the actual data, and once created it is independent of the
   * track proxy which created it.
   * 
   */
  using TrackPointData = std::tuple<
    recob::Track const*,
    art::Ptr<recob::Hit> const*,
    recob::TrackFitHitInfo const*,
    std::size_t
    >;
  
  /**
   * @brief Wrapper for a track data proxy.
   * @tparam Data the point data type; requirements are described below
   * @ingroup LArSoftProxyTracks
   * 
   * This class provides a user interface to data pertaining a single trajectory
   * point of a `recob::Track`.
   * 
   * 
   * Implementation details
   * -----------------------
   * 
   * This type wraps a generic data structure with tuple interface.
   * It is expected that the following information is returned:
   * 
   * * pointer to point position as `recob::Track::Point_t` from
   *   `std::get<0>(Data const&)`
   * * pointer to momentum at point as `recob::Track::Vector_t` from
   *   `std::get<1>(Data const&)`
   * * pointer to flags as `recob::Track::PointFlags_t` from
   *   `std::get<2>(Data const&)`
   * * pointer to associated hit as `art::Ptr<recob::Hit>` from
   *   `std::get<3>(Data const&)`
   * 
   * The "pointers" can be any object returning the required type when
   * dereferenced.
   * 
   */
  template <typename Data>
  class TrackPointWrapper {
    using This_t = TrackPointWrapper<Data>;
    using Wrapped_t = std::add_const_t<Data>;
    
    static constexpr std::size_t TrackIndex      = 0;
    static constexpr std::size_t HitIndex        = 1;
    static constexpr std::size_t FitHitInfoIndex = 2;
    static constexpr std::size_t IndexIndex      = 3;
    static constexpr std::size_t NIndices        = 4;
    
    static_assert(std::tuple_size<Data>::value == NIndices,
      "Unexpected data size.");
    
    Wrapped_t const& base() const
      { return reinterpret_cast<Wrapped_t const&>(*this); }
    
    template <std::size_t N>
    auto get() const -> decltype(auto) { return std::get<N>(base()); }
    
      protected:
    TrackPointWrapper() = default;
    TrackPointWrapper(TrackPointWrapper const&) = default;
    TrackPointWrapper(TrackPointWrapper&&) = default;
    TrackPointWrapper& operator=(TrackPointWrapper const&) = default;
    TrackPointWrapper& operator=(TrackPointWrapper&&) = default;
    
      public:
    
    /// Returns the track this point belongs to.
    recob::Track const& track() const
      { return *get<TrackIndex>(); }
    
    /// Returns the position of the trajectory point.
    /// @see `recob::Track::LocationAtPoint()`
    auto position() const -> decltype(auto)
      { return track().Trajectory().LocationAtPoint(index()); }

    /// Returns the momentum vector of the trajectory point.
    /// @see `recob::Track::MomentumVectorAtPoint()`
    auto momentum() const -> decltype(auto)
      { return track().Trajectory().MomentumVectorAtPoint(index()); }
    
    /// Returns the flags associated with the trajectory point.
    /// @see `recob::Track::FlagsAtPoint()`
    auto flags() const -> decltype(auto)
      { return track().Trajectory().FlagsAtPoint(index()); }
    
    /**
     * @brief Returns the hit associated with the trajectory point
     * @return an _art_ pointer to the hit associated to this point
     */
    art::Ptr<recob::Hit> const& hitPtr() const { return *get<HitIndex>(); }
    
    /**
     * @brief Returns fit info associated with the trajectory point.
     * @return a pointer to the fit info, or `nullptr` if not merged in proxy
     * 
     * If the track proxy this point comes from had no fit information,
     * `nullptr` is returned.
     * The fit information is extracted using the tag in
     * `proxy::Tracks::TrackFitHitInfoTag`.
     */
    recob::TrackFitHitInfo const* fitInfoPtr() const
      { return get<FitHitInfoIndex>(); }
    
    /// Returns the index of this point in the trajectory.
    auto index() const -> decltype(auto) { return get<IndexIndex   >(); }
    
    /// Returns a pointer to the hit on the trajectory point, if any.
    recob::Hit const* hit() const
      { decltype(auto) ptr = hitPtr(); return ptr? ptr.get(): nullptr; }
    
  }; // TrackPointWrapper<>
  
  
  /**
   * @brief Type of track point information.
   * @ingroup LArSoftProxyTracks
   * 
   * For its interface, see `proxy::TrackPointWrapper`.
   */
  struct TrackPoint
    : private TrackPointData
    , public TrackPointWrapper<TrackPointData>
  {
    using TrackPointData::TrackPointData;
      private:
    static constexpr bool asserts
      = details::StaticAsserts<TrackPointWrapper<TrackPointData>>::value;
  }; // class TrackPoint
  
  
  /**
   * @brief Returns an object with information about the specified track point.
   * @tparam TrackProxy an instance of proxy::Track template
   * @param track the track (proxy) the points belong to
   * @param index the index of the point within the track
   * @return a `TrackPointData` object with information on that point
   * 
   * For an interface to the point information, see `TrackPointWrapper`.
   */
  template <typename TrackProxy>
  TrackPointData makeTrackPointData
    (TrackProxy const& track, std::size_t index)
    {
      static_assert(details::isTrackProxy<TrackProxy>(), "Not a proxy::Track!");
      return {
        &(track.track()),
        &(track.hitAtPoint(index)),
        track.fitInfoAtPoint(index),
        index
      };
    } // makeTrackPointData()
  
  
  //--------------------------------------------------------------------------
  /**
    * @brief Class for track proxy elements.
    * @tparam CollProxy type of track proxy collection to get data from
    * @ingroup LArSoftProxyTracks
    */
  template <typename CollProxy>
  struct TrackCollectionProxyElement
    : public CollectionProxyElement<CollProxy>
  {
    using base_t = CollectionProxyElement<CollProxy>; ///< Base type.
    using base_t::base_t; // inherit constructors
    
    ///< This type.
    using track_proxy_t = TrackCollectionProxyElement<CollProxy>;
    
      public:
    /// Iterator for trajectory point information.
    using point_iterator = TrackPointIterator<track_proxy_t>;
    
    /// Returns the pointed track.
    recob::Track const& track() const { return base_t::operator*(); }
    
    /// @{
    /// @name Direct hit interface.
    
    /// Returns a collection-like range of hits of this track, at point order.
    auto hits() const -> decltype(auto)
      { return base_t::template get<Tracks::HitTag>(); }
    
    /// Returns an art pointer to the hit associated with the specified point.
    auto const& hitAtPoint(std::size_t index) const { return hits()[index]; }
    
    /// Returns the number of hits associated with this track.
    std::size_t nHits() const { return hits().size(); }
    
    /// @}
    
    /// Returns fit info for the specified point (`nullptr` if not available).
    recob::TrackFitHitInfo const* fitInfoAtPoint(std::size_t index) const
      {
        if (!base_t::template has<Tracks::TrackFitHitInfoTag>())
          return nullptr;
        auto&& fitInfo = base_t::template getIf<
          Tracks::TrackFitHitInfoTag,
          std::vector<recob::TrackFitHitInfo> const&
          >();
        return &(fitInfo[index]);
      }
    
    
    /// @{
    /// @name Point-by-point iteration interface
    /// 
    /// The points on track can be accessed individually with a special,
    /// non-extensible proxy.
    /// 
    
    /// Returns an iterable range with point-by-point information
    /// (`TrackPointWrapper`).
    auto points() const
      { return details::TrackPointIteratorBox<CollProxy>(*this); }
    
    /// Returns the number of trajectory points in the track.
    std::size_t nPoints() const { return track().NPoints(); }
    
    /// Returns the iterator to the data of the first point.
    point_iterator beginPoint() const { return { *this, 0 }; }
    
    /// Returns the iterator past the last point.
    point_iterator endPoint() const { return { *this, nPoints() }; }
    
    /// @{
    /// Extracts information from the specified point.
    TrackPoint point(std::size_t index) const
      { return { makeTrackPointData(track(), index) }; }
    
    TrackPoint operator[](std::size_t index) const
      { return point(index); }
    
    /// @}
    
  }; // TrackCollectionProxyElement<>
  
  
  /**
   * @brief Proxy to an element of a proxy collection of `recob::Track` objects.
   * @tparam TrackCollProxy type of the track collection proxy
   * @ingroup LArSoftProxyTracks
   * 
   * This class is the proxy equivalent of `recob::Track`, which exposes data
   * associated with the track.
   * An object of this type is returned when accessing a single track from a
   * track collection proxy. While the data itself is not copied, this object
   * owns some pointers to the actual data, and once created it is independent
   * of the track collection proxy which created it.
   * 
   * The interface is currently defined by
   * `proxy::TrackCollectionProxyElement`.
   */
  template <typename TrackCollProxy>
  using Track = TrackCollectionProxyElement<TrackCollProxy>;
  
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Adds `recob::TrackFitHitInfo` information to the proxy.
   * @param inputTag the data product label to read the data from
   * @return an object driving `getCollection()` to use `recob::TrackFitHitInfo`
   * @ingroup LArSoftProxyTracks
   * 
   * The collection of `recob::TrackFitHitInfo` is required to be a
   * `std::vector<std::vector<recob::TrackFitHitInfo>>`, where the first index
   * addresses which track the information is about, and the second index which
   * point within that track.
   * 
   * The data is avaialble through the regular interface via tag
   * `recob::TrackFitHitInfo`.
   * 
   * The data must satisfy the "parallel data product" requirement described in
   * `ProxyBase.h`.
   * 
   */
  inline auto withFitHitInfo(art::InputTag inputTag)
    {
      return proxy::withParallelDataAs
        <std::vector<recob::TrackFitHitInfo>, Tracks::TrackFitHitInfoTag>
        (inputTag);
    }
  
  /// Like `withFitHitInfo(art::InputTag)`, using the same label as for tracks.
  /// @ingroup LArSoftProxyTracks
  inline auto withFitHitInfo()
    {
      return proxy::withParallelDataAs
        <std::vector<recob::TrackFitHitInfo>, Tracks::TrackFitHitInfoTag>();
    }
  
  //----------------------------------------------------------------------------
  /// Specialization to create a proxy for `recob::Track` collection.
  template <>
  struct CollectionProxyMaker<Tracks>
    : public CollectionProxyMakerBase<Tracks>
  {
    
    /// Traits of the collection proxy for the collection proxy maker.
    using maker_base_t = CollectionProxyMakerBase<Tracks>;
    
    /// Type of main collection proxy.
    using typename maker_base_t::main_collection_proxy_t;
    
    /// Type of element of the main collection.
    using typename maker_base_t::main_collection_t;
    
    /**
     * @brief Creates and returns a collection proxy for `recob::Track` based on
     *        `proxy::Tracks` tag and with the requested associated data.
     * @tparam Event type of the event to read the information from
     * @tparam WithArgs type of arguments for associated data
     * @param event event to read the information from
     * @param tag input tag of the `recob::Track` collection data product
     * @param withArgs optional associated objects to be included
     * @return a collection proxy to `recob::Track` collection with `tag`
     * 
     * For each argument in `withArgs`, an action is taken. Usually that is to
     * add an association to the proxy.
     * Associated hits (tag: `recob::Hit`) are automatically added to the proxy
     * and must not be explicitly specified.
     * 
     * Only a few associated data collections are supported:
     * * `withAssociated<Aux>()` (optional argument: hit-track association
     *     tag, as track by default): adds to the proxy an association to the
     *     `Aux` data product (see `withAssociated()`)
     * 
     */
    template <typename Event, typename... WithArgs>
    static auto make
      (Event const& event, art::InputTag tag, WithArgs&&... withArgs)
      {
        auto mainHandle = event.template getValidHandle<main_collection_t>(tag);
        // automatically add associated hits with the same input tag;
        // IDEA: allow a withAssociated<recob::Hit>() from withArgs to override
        // this one; the pattern may be:
        // - if withArgs contains a withAssociated<recob::Hit>(), produce a new
        //   withArgs with that one pushed first
        // - otherwise, produce a new withArgs with a new
        //   withAssociated<recob::Hit>(tag) as first element
        // In principle there is no need for these hits to be first; code might
        // be simpler when assuming that though.
        auto proxy = makeCollectionProxy(
          *mainHandle,
          withAssociatedAs<recob::Hit, Tracks::HitTag>()
            .template createAuxProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag),
          withArgs.template createAuxProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag)...
          );
        return proxy;
      } // make()
    
      private:
    template <typename MainColl, typename... AuxColl>
    using coll_proxy_t = CollectionProxyBase<Track, MainColl, AuxColl...>;
    
    // helper function to avoid typing the exact types of auxiliary collections
    template <typename MainColl, typename... AuxColl>
    static auto makeCollectionProxy(MainColl const& main, AuxColl&&... aux)
      {
        return coll_proxy_t<MainColl, AuxColl...>
          (main, std::forward<AuxColl>(aux)...);
      }
    
  }; // struct CollectionProxyMaker<>
  
  
  /// "Converts" point data into a `proxy::TrackPointWrapper`.
  template <typename Data>
  auto wrapTrackPoint(Data const& wrappedData)
    {
      (void) details::StaticAsserts<TrackPointWrapper<Data>>();
      return reinterpret_cast<TrackPointWrapper<Data> const&>(wrappedData); 
    }
  
  /// Iterator for points of a track proxy. Only supports range-for loops.
  /// @ingroup LArSoftProxyReco
  template <typename TrackProxy>
  class TrackPointIterator {
    
    using track_proxy_t = TrackProxy;
    
    track_proxy_t const* track = nullptr;
    std::size_t index = std::numeric_limits<std::size_t>::max();
    
      public:
    TrackPointIterator() = default;
    TrackPointIterator(track_proxy_t const& track, std::size_t index)
      : track(&track), index(index)
      {}
    
    TrackPointIterator& operator++() { ++index; return *this; }
    
    auto operator*() const -> decltype(auto)
      { return TrackPoint(makeTrackPointData(*track, index)); }
    
    bool operator!=(TrackPointIterator const& other) const
      { return (index != other.index) || (track != other.track); }
    
  }; // class TrackPointIterator
  
  
} // namespace proxy


namespace proxy {
  
  //----------------------------------------------------------------------------
  namespace details {
    
    //--------------------------------------------------------------------------
    template <typename T>
    struct isTrackProxy: public std::false_type {};
    
    template <typename TrackCollProxy>
    struct isTrackProxy<Track<TrackCollProxy>>: public std::true_type {};
    
    
    //--------------------------------------------------------------------------
    /// Structure for range-for iteration.
    template <typename CollProxy>
    struct TrackPointIteratorBox {
      using track_proxy_t = Track<CollProxy>;
      using const_iterator = typename track_proxy_t::point_iterator;
      
      TrackPointIteratorBox(track_proxy_t const& track): track(&track) {}
      
      const_iterator begin() const
        { return track->beginPoint(); }
      
      const_iterator end() const
        { return track->endPoint(); }
      
        private:
      track_proxy_t const* track = nullptr;
      
    }; // TrackPointIteratorBox<>
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
} // namespace proxy

  
#endif // LARDATA_RECOBASEPROXY_TRACK_H
