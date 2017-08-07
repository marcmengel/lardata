/**
 * @file   Track.h
 * @brief  Offers `proxy::Track` class for `recob::Track` access.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
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
    /// (`TrackPointProxyWrapper`)
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
  using TrackPointProxyData = std::tuple<
    recob::Track::Point_t const*,
    recob::Track::Vector_t const*,
    recob::Track::PointFlags_t const*,
    art::Ptr<recob::Hit> const*,
    std::size_t
    >;
  
  /**
   * @brief Returns an object with information about the specified track point.
   * @param track the track (proxy) the points belong to
   * @param index the index of the point within the track
   * @return a `TrackPointProxyData` object with information on that point
   * 
   * For an interface to the point information, see `TrackPointProxyWrapper`.
   */
  TrackPointProxyData makeTrackPointProxyData
    (proxy::Track const& track, std::size_t index)
    {
      return {
        &(track->Trajectory().LocationAtPoint(index)),
        &(track->Trajectory().MomentumVectorAtPoint(index)),
        &(track->FlagsAtPoint(index)),
        &(track.HitAtPoint(index)),
        index
      };
    } // makeTrackPointProxyData()
  
  
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
  class TrackPointProxyWrapper {
    using This_t = TrackPointProxyWrapper<Data>;
    using Wrapped_t = std::add_const_t<Data>;
    
    static constexpr std::size_t PositionIndex = 0;
    static constexpr std::size_t MomentumIndex = 1;
    static constexpr std::size_t FlagsIndex    = 2;
    static constexpr std::size_t HitIndex      = 3;
    static constexpr std::size_t IndexIndex    = 4;
    static constexpr std::size_t NIndices      = 5;
    
    Wrapped_t const& base() const
      { return reinterpret_cast<Wrapped_t const&>(*this); }
    
    template <std::size_t N>
    auto get() const -> decltype(auto) { return std::get<N>(base()); }
    
      protected:
    TrackPointProxyWrapper() = default;
    TrackPointProxyWrapper(TrackPointProxyWrapper const&) = default;
    TrackPointProxyWrapper(TrackPointProxyWrapper&&) = default;
    TrackPointProxyWrapper& operator=(TrackPointProxyWrapper const&) = default;
    TrackPointProxyWrapper& operator=(TrackPointProxyWrapper&&) = default;
    
      public:
    
    /// Returns the position of the trajectory point.
    auto position() const -> decltype(auto) { return *get<PositionIndex>(); }
    
    /// Returns the momentum vector of the trajectory point.
    auto momentum() const -> decltype(auto) { return *get<MomentumIndex>(); }
    
    /// Returns the flags associated with the trajectory point.
    auto flags   () const -> decltype(auto) { return *get<FlagsIndex   >(); }
    
    /// Returns the hit associated with the trajectory point, as _art_ pointer.
    auto hitPtr  () const -> decltype(auto) { return *get<HitIndex     >(); }
    
    /// Returns the index of this point in the trajectory.
    auto index   () const -> decltype(auto) { return get<IndexIndex   >(); }
    
    /// Returns a pointer to the hit on the trajectory point, if any.
    recob::Hit const* hit() const
      { decltype(auto) ptr = hitPtr(); return ptr? ptr.get(): nullptr; }
    
  }; // TrackPointProxyWrapper<>
  
  
  namespace details {
    
    template <typename Data>
    struct StaticAsserts;
    
    
    template <typename Data>
    struct StaticAsserts<TrackPointProxyWrapper<Data>> {
      using Wrapper_t = TrackPointProxyWrapper<Data>;
      
      static_assert(sizeof(Wrapper_t) == 0U, "Wrapper carries data!");
      
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
        std::decay_t<decltype(std::declval<Wrapper_t>().hit())>,
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
      
    }; // StaticAsserts<TrackPointProxyWrapper<Data>>
  } // namespace details
  
  
  template <typename Data>
  auto wrapTrackPointProxy(Data const& wrappedData)
    {
      details::StaticAsserts<TrackPointProxyWrapper<Data>>();
      return reinterpret_cast<TrackPointProxyWrapper<Data> const&>(wrappedData); 
    }
  
  struct TrackPointProxy
    : private TrackPointProxyData
    , public TrackPointProxyWrapper<TrackPointProxyData>
  {
    using TrackPointProxyData::TrackPointProxyData;
  }; // class TrackPointProxy
  
  
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
      { return TrackPointProxy(makeTrackPointProxyData(*track, index)); }
    
    bool operator!=(TrackPointIterator const& other) const
      { return (index != other.index) || (track != other.track); }
    
  }; // class TrackPointIterator
  
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_TRACK_H
