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

// framework libraries
#include "canvas/Persistency/Common/Ptr.h"


namespace proxy {
  
  // forward declarations
  class Track;
  
  class TrackPointIterator;
  
  namespace details {
    
    /// Structure for range-for iteration.
    struct TrackPointIteratorBox {
      using const_iterator = TrackPointIterator;
      
      TrackPointIteratorBox(proxy::Track const& track): track(&track) {}
      
      const_iterator begin() const;
      const_iterator end() const;
      
        private:
      proxy::Track const* track = nullptr;
      
    }; // TrackPointIteratorBox
    
  } // namespace details
  
  /**
   * @brief Proxy to an element of a proxy collection of `recob::Track` objects.
   * 
   * This class is the equivalent of `recob::Track`.
   */
  class Track: public ProxyCollectionElement<recob::Track> {
    using Base_t = ProxyCollectionElement<recob::Track>;
    using HitAssns_t = details::AssociatedData<recob::Track, recob::Hit>;
    
      public:
    /// Type of collection of hits associated with the track.
    using Hits_t = HitAssns_t::AuxList_t;
    
    
    /// Constructor: utilizes the specified track and hits set.
    Track(recob::Track const& track, Hits_t const& hits)
      : Base_t(track), fHits(hits) {}
    
    /// Returns the pointed track.
    recob::Track const& track() const { return mainRef(); }
    
    /// Returns a range covering art pointers to hits associated with the track.
    Hits_t const& hits() const { return fHits; }
    
    /// Returns an art pointer to the hit associated with the specified point.
    auto const& HitAtPoint(std::size_t index) const { return hits()[index]; }
    
    /// Returns the number of hits associated to this track.
    auto nHits() const { return hits().size(); }
    
    /// Returns an iterable range with point-by-point information
    /// (`TrackPointWrapper`).
    auto points() const { return details::TrackPointIteratorBox(*this); }
    
    
      private:
    
    Hits_t fHits; ///< Range of hits associated with this track.
    
  }; // class Track
  
  
  /**
   * @brief Proxy to a collection of `recob::Track` objects.
   * 
   * The collection and its elements are immutable.
   */
  class Tracks: public ProxyCollection<proxy::Track> {
      public:
    /// Type of iterator of this class.
    using const_iterator = ProxyCollectionIterator<Tracks>;
    
    /// Return the proxy to the specified element of the track collection.
    auto operator[] (std::size_t index) const -> decltype(auto)
      { return getProxyAt(index); }
    
    /// Returns an iterator to the first track, proxied.
    const_iterator cbegin() const { return { *this, 0U }; }
    
    /// Returns an iterator past the last track.
    const_iterator cend() const { return { *this, size() }; }
    
    /// Returns an iterator to the first track, proxied.
    auto begin() const { return cbegin(); }
    
    /// Returns an iterator past the last track.
    auto end() const { return cend(); }
    
      private:
    friend class details::ProxyCollectionGetterTraits<proxy::Tracks>;
    friend class ProxyCollectionGetter<proxy::Tracks>;
    
    using Base_t = ProxyCollection<proxy::Track>;
    using HitAssns_t = details::AssociatedData<recob::Track, recob::Hit>;
    
    /// Hits associated for each track.
    HitAssns_t hitsPerTrack;
    
    /// Constructor: points to main objects, steals association data.
    Tracks
      (typename Base_t::main_collection_type const& main, HitAssns_t&& hitAssns)
      : ProxyCollection(main), hitsPerTrack(std::move(hitAssns))
      {}
    
    /// Returns a proxy to the specified element.
    proxy::Track getProxyAt(std::size_t index) const;
    
  }; // class Tracks
  
  
  /**
   * @brief Proxy for `std::vector<recob::Track>` data product.
   * 
   * This is the specialization of `ProxyCollectionGetter` for `recob::Track`
   * objects.
   * 
   * It creates an object of type `proxy::Tracks` from the specified tag.
   */
  template <>
  class ProxyCollectionGetter<proxy::Tracks> {
    
    using Traits_t = details::ProxyCollectionGetterTraits<Tracks>;
    
      public:
    using ProductCollection_t = typename Traits_t::ProductCollection_t;
    using ProductElement_t = typename Traits_t::ProductElement_t;
    
    /// Returns the specified proxy object, reading data from `event`.
    template <typename Event>
    proxy::Tracks get(Event const& event, art::InputTag tag) const
      {
        auto trackHandle
          = event.template getValidHandle<ProductCollection_t>(tag);
        return { *trackHandle, { trackHandle, event, tag } };
      }
    
  }; // class ProxyCollectionGetter<Tracks>
  
  
  //----------------------------------------------------------------------------
  //---  track point information
  //---
  /// Type of information pertaining a point on a track.
  using TrackPointData = std::tuple<
    recob::Track const*,
    art::Ptr<recob::Hit> const*,
    std::size_t
    >;
  
  /**
   * @brief Returns an object with information about the specified track point.
   * @param track the track (proxy) the points belong to
   * @param index the index of the point within the track
   * @return a `TrackPointData` object with information on that point
   * 
   * For an interface to the point information, see `TrackPointWrapper`.
   */
  TrackPointData makeTrackPointData
    (proxy::Track const& track, std::size_t index)
    {
      return {
        &(track.track()),
        &(track.HitAtPoint(index)),
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
    
    static constexpr std::size_t TrackIndex = 0;
    static constexpr std::size_t HitIndex      = 1;
    static constexpr std::size_t IndexIndex    = 2;
    static constexpr std::size_t NIndices      = 3;
    
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
  
  
  class TrackPointIterator {
    proxy::Track const* track = nullptr;
    std::size_t index = std::numeric_limits<std::size_t>::max();
    
      public:
    TrackPointIterator() = default;
    TrackPointIterator(proxy::Track const& track, std::size_t index)
      : track(&track), index(index)
      {}
    
    TrackPointIterator& operator++() { ++index; return *this; }
    
    auto operator*() const -> decltype(auto)
      { return TrackPoint(makeTrackPointData(*track, index)); }
    
    bool operator!=(TrackPointIterator const& other) const
      { return (index != other.index) || (track != other.track); }
    
  }; // class TrackPointIterator
  
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_TRACK_H
