/**
 * @file   Track.h
 * @brief  Offers `proxy::Tracks` and `proxy::Track` class for
 *         `recob::Track` access.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 * Track proxies is a way to facilitate the navigation of `recob::Track` data
 * objects.
 * The complex of fundamental data of a track collection are:
 * * the tracks themselves, in a `std::vector<recob::Track>` collection
 * * the associated hits, in a `art::Assns<recob::Track, recob::Hit>` data
 *   product
 * 
 * LArSoft prescribes conventions to be followed, which include:
 * * a track has at least two trajectory points
 * * for each track, there is one hit per trajectory point
 * * the association between tracks and hits is created with tracks as first
 *   ("left") element, and hits as second one ("right")
 * * hits in the association are in a well-defined order: first are the hits of
 *   the first track, sorted in the same way as their trajectory points; then
 *   the second track hits come, likewise; and all tracks follow in order
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
 * Creating a track proxy
 * =======================
 * 
 * Track proxies are created by specifying the tag of the tracks, and the event
 * to read them from. Assumptions:
 * * the tracks are stored in a `std::vector<recob::Track>` data product
 * * the associations of tracks to hits have the same input tag as the tracks
 *   (that means the associations to hits where created by the same module as
 *   the tracks themselves)
 * 
 * With this in mind, to create a track proxy:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Here we ask `getCollection()` to create a proxy of type `proxy::Tracks`.
 * Each proxy is a different beast which needs to be explicitly supported:
 * here support for the proxy to `recob::Track` is described.
 * 
 * The C++ type of `tracks` should not matter, but it is `proxy::Tracks`, as
 * suggested by the function call.
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
 * of these proxy classes, please refer to each class documentation.
 * 
 * @note The interface allows by deliberate design only read-only access to the
 *       underlying data.
 * 
 * 
 * Technical details
 * ==================
 * 
 * Overhead
 * ---------
 * 
 * See the notes on overhead in `ProxyBase.h`.
 * 
 * 
 * Interface replacement
 * ----------------------
 * 
 * A technique that is used in this implementation is to replace (or extend) the
 * interface of an existing object.
 * The documentation of file `CollectionView.h` includes a more in-depth
 * description of it.
 * 
 * 
 * Track collection proxy
 * -----------------------
 * 
 * The track collection proxy object is derived from `proxy::ProxyCollection`,
 * which contains a pointer to the original (track) data product.
 * In addition, it contains a `proxy::details::AssociatedData` object for
 * the `recob::Track`--`recob::Hit` association list.
 * The `AssociatedData` object, on creation, finds the borders surrounding the
 * associated hits for each track, and keep a record of them.
 * The `AssociatedData` object also provides a container-like view of this
 * information, where each element in the container is associated to a single
 * track and it is a container (actually, another view) of hits.
 * Both levels of containers are random access, so that hits associated to a
 * track can be accessed by track index, and the hits within can be accessed
 * with hit index in the track, which is particularly convenient given the
 * prescription that the hit index matches the index of the trajectory point
 * that hit is associated to.
 * 
 * The `proxy::Tracks` interface is currently quite limited: it only allows to
 * access a track by index, or to iterate through all of them, in addition to
 * know how many tracks are available.
 * 
 * The object returned when accessing the single track information,
 * `proxy::Track`, actually contains an iterator to the `proxy::Tracks`
 * collection, and therefore it requires that collection to be still available.
 * It also contains a copy of the range of associated hits. This copy is
 * actually just a single iterator pointing to the relevant information in the
 * associated hit list, again belonging to the `proxy::Tracks` object.
 * 
 * The object describing the information of a single point has the interface of
 * `TrackPointWrapper`. The underlying storage includes pointers to the track,
 * the associated hit, and the index of the point in the track.
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
  namespace details {
    
    template <typename CollProxy>
    class TrackCollectionProxyElement;
    
    template <typename CollProxy>
    struct TrackPointIteratorBox;
    
    template <typename T>
    struct isTrackProxy;
    
  } // namespace details
  
  //----------------------------------------------------------------------------
  
  
  /**
   * @brief Proxy tag for a `recob::Track` collection proxy.
   *
   * This type can be used to get a proxy for `recob::Track` collection:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, tracksTag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * @todo Document how to use that collection.
   * 
   */
  using Tracks = std::vector<recob::Track>;
  
  
  /**
   * @brief Proxy to an element of a proxy collection of `recob::Track` objects.
   * @tparam CollProxy type of the track collection proxy
   * 
   * This class is the equivalent of `recob::Track`, but it exposes data
   * associated with the track.
   */
  template <typename TrackCollProxy>
  using Track = details::TrackCollectionProxyElement<TrackCollProxy>;
  
  
  
  //----------------------------------------------------------------------------
  /// Specialization to create a proxy for `recob::Track` collection.
  template <>
  struct CollectionProxyMaker<Tracks> {
    
    /// Traits of the collection proxy for the collection proxy maker.
    using traits_t = CollectionProxyMakerTraits<Tracks>;
    
    /// Type of main collection proxy.
    using main_collection_proxy_t = typename traits_t::main_collection_proxy_t;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = typename traits_t::main_element_t;
    
    /// Type of element of the main collection.
    using main_collection_t = typename traits_t::main_collection_t;
    
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
     * Associated hits are automatically added to the proxy and must not be
     * explicitly specified.
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
        // automatically add associated hits with the same tag;
        // TODO allow a withAssociated<recob::Hit>() from withArgs to override
        // this one; the pattern may be:
        // - if withArgs contains a withAssociated<recob::Hit>(), produce a new
        //   withArgs with that one pushed first
        // - otherwise, produce a new withArgs with a new
        //   withAssociated<recob::Hit>(tag) as first element
        // In principle there is no need for these hits to be first; code might
        // be simpler when assuming that though.
        auto proxy = makeCollectionProxy(
          *mainHandle,
          withAssociated<recob::Hit>()
            .template createAssnProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag),
          withArgs.template createAssnProxyMaker<main_collection_proxy_t>
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
  
  
  //----------------------------------------------------------------------------
  //---  track point information
  //---
  /// Type of information pertaining a point on a track.
  //
  // NOTE The generic proxy structure we use (from `ProxyBase.h`) does not help
  // in this nested collection. It can help navigate elements of the main data
  // product collections (e.g. tracks), but not elements of elements of the main
  // data (e.g. track points). The main difficulty is to automatically determine
  // which of the auxiliary data is conformly indexed, and with what: there is
  // nothing in recob::Track interface that explicitly states it is a collection
  // of points (no `for(auto&& point: track)`), and there is nothing that
  // indicates a relation in the auxiliary data with it.
  // So we are back to hard-coding here.
  //
  using TrackPointData = std::tuple<
    recob::Track const*,
    art::Ptr<recob::Hit> const*,
    recob::TrackFitHitInfo const*,
    std::size_t
    >;
  
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
  
  namespace details {
    
    template <typename Obj>
    struct StaticAsserts;
    
    // calling this function ensures static assert template is instantiated
    template <typename Obj>
    constexpr bool staticChecks() { return StaticAsserts<Obj>::value; }
    
  }// namespace details
  
  /**
   * @brief Wrapper for a track data proxy.
   * @tparam Data the point data type; requirements are described below
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
    
    /// Returns the position of the trajectory point.
    auto track() const -> decltype(auto) { return *get<TrackIndex>(); }
    
    /// Returns the position of the trajectory point.
    auto position() const -> decltype(auto)
      { return track().Trajectory().LocationAtPoint(index()); }

    /// Returns the momentum vector of the trajectory point.
    auto momentum() const -> decltype(auto)
      { return track().Trajectory().MomentumVectorAtPoint(index()); }
    
    /// Returns the flags associated with the trajectory point.
    auto flags() const -> decltype(auto)
      { return track().Trajectory().FlagsAtPoint(index()); }
    
    /// Returns the hit associated with the trajectory point, as _art_ pointer.
    auto hitPtr() const -> decltype(auto) { return *get<HitIndex     >(); }
    
    /// Returns fit info associated with the trajectory point (nullptr if none).
    recob::TrackFitHitInfo const* fitInfoPtr() const
      { return get<FitHitInfoIndex>(); }
    
    /// Returns the index of this point in the trajectory.
    auto index() const -> decltype(auto) { return get<IndexIndex   >(); }
    
    /// Returns a pointer to the hit on the trajectory point, if any.
    recob::Hit const* hit() const
      { decltype(auto) ptr = hitPtr(); return ptr? ptr.get(): nullptr; }
    
  }; // TrackPointWrapper<>
  
  
  namespace details {
    
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
  
  
  template <typename Data>
  auto wrapTrackPoint(Data const& wrappedData)
    {
      if (!details::StaticAsserts<TrackPointWrapper<Data>>()) return nullptr;
      return reinterpret_cast<TrackPointWrapper<Data> const&>(wrappedData); 
    }
  
  struct TrackPoint
    : private TrackPointData
    , public TrackPointWrapper<TrackPointData>
  {
    using TrackPointData::TrackPointData;
      private:
    static constexpr bool asserts
      = details::StaticAsserts<TrackPointWrapper<TrackPointData>>::value;
  }; // class TrackPoint
  
  
  
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
        { return base_t::template get<recob::Hit>(); }
      
      /// Returns an art pointer to the hit associated with the specified point.
      auto const& hitAtPoint(std::size_t index) const { return hits()[index]; }
      
      /// Returns the number of hits associated with this track.
      std::size_t nHits() const { return hits().size(); }
      
      /// @}
      
      /// Returns fit info for the specified point (`nullptr` if not available).
      recob::TrackFitHitInfo const* fitInfoAtPoint(std::size_t index) const
        {
          if (!base_t::template has<recob::TrackFitHitInfo>()) return nullptr;
          return &(base_t::template getIf
            <recob::TrackFitHitInfo, std::vector<recob::TrackFitHitInfo> const&>
            ()
            [index]);
        }
      
      
      // TODO
      /// Returns an iterable range with point-by-point information
      /// (`TrackPointWrapper`).
      auto points() const { return TrackPointIteratorBox<CollProxy>(*this); }
      
      /// Returns the number of trajectory points in the track.
      std::size_t nPoints() const { return track().NPoints(); }
      
      /// Returns the iterator to the data of the first point.
      point_iterator beginPoint() const { return { *this, 0 }; }
      
      /// Returns the iterator past the last point.
      point_iterator endPoint() const { return { *this, nPoints() }; }
      
      
    }; // TrackCollectionProxyElement<>
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
} // namespace proxy

  
#endif // LARDATA_RECOBASEPROXY_TRACK_H
