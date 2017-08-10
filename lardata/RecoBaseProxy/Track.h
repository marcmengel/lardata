/**
 * @file   Track.h
 * @brief  Offers `proxy::Track` class for `recob::Track` access.
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
 * The proxies have been developed with an eye on minimising the replication of
 * information. The proxies are therefore light-weight objects relying on
 * pointers to the original data. One exception is that an additional structure
 * is created for each one-to-many association (i.e., to hits), which includes
 * a number of entries proportional to the number of tracks.
 * 
 * In general, anyway, copy of any proxies is not recommended, as it is usually
 * better just to pass around a reference to them.
 * 
 * Since this interface (and implementation) is still in development, there
 * might be flaws that make it non-performant. Please report any suspicious
 * behaviour.
 * 
 * 
 * Interface replacement
 * ----------------------
 * 
 * A technique that is used in this implementation is to replace (or extend) the
 * interface of an existing object.
 * A key requirement is that the new interface object must not have any
 * additional state.
 * 
 * The interface class is superimposed to the _existing_ data without
 * replication by _reinterpreting_ its content. This is achieved deriving the
 * new interface class from the data class:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * 
 * class Data {
 *   double chiSq;
 *   double NDF;
 *     public:
 *   Data(double chiSq, double NDF): chiSq(chiSq), NDF(NDF) {}
 *   
 *   double chiSquare() const { return chiSq; }
 *   double DegreesOfFreedom() const { return NDF; }
 *   
 * }; // class Data
 * 
 * 
 * class DataInterface: private Data {
 *   Data const& asData() const { return static_cast<Data const&>(*this); }
 *     public:
 *   
 *   double normalizedChiSquare() const
 *     { return asData().chiSquare() / asData().DegreesOfFreedom(); }
 *   
 *     protected:
 *   DataInterface(Data const& from): Data(data) {}
 *   
 *   friend DataInterface const& makeDataInterface(Data const&);
 *   
 * }; // class DataInterface
 * 
 * 
 * DataInterface const& makeDataInterface(Data const& data)
 *   { return static_const<DataInterface const&>(data); }
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * With this pattern, an interface object can be obtained only by calling
 * `makeDataInterface()` on the base object, and in this way it will be returned
 * only as a reference (in this case, constant). The interface object can't
 * be copied, and it must be passed around as reference. It's not possible to
 * convert it back to `Data`, because the base class is private.
 * There is a single protected constructor. This choice, compared to deleting
 * all constructors, allows for a derived class to acquire the same interface:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * struct DataWithInterface: public DataInterface {
 *   DataWithInterface(Data const& from): DataInterface(from) {}
 * }; // class DataWithInterface
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * This simple class provides the storage for `Data` in addition to exposing
 * `DataInterface`.
 * There are other ways to achieve the same goal (e.g., multiple inheritance).
 * 
 * The presence of a friend function should raise a warning. Friendship is
 * required because the function is attempting a downcast from a private base
 * class. If it is intended that the full `Data` interface is exposed, then
 * the inheritance can be public and no special friendship will be needed.
 * Another way is to replace the `static_cast` with a `reinterpret_cast`.
 * 
 * 
 * Iterator wrappers and "static polymorphism"
 * --------------------------------------------
 * 
 * A widely used interface change is the substitution of the dereference
 * operator of an iterator:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * struct address_iterator: public iterator { // DON'T DO THIS (won't work)
 *   auto operator*() -> decltype(auto)
 *     { return std::addressof(iterator::operator*()); }
 * }; // my_iterator
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * There are two important pitfalls to be aware of in this specific case, well
 * illustrated in this example.
 * 
 * If the caller tries to use e.g. `ait->name()` on a `address_iterator ait`
 * (or other members, like `ait[0]`), they will be picked from the base class,
 * and the overloaded `operator*()` is ignored. This can be avoided with private
 * inheritance, forcing the explicit implementation of everything we want to
 * use, which will be at very least an increment operator and a comparison one.
 * 
 * The second pitfall is that the base class methods return base class
 * references. For example, `*ait++` will call the inherited increment operator,
 * which returns an object of type `iterator`, and the following dereference
 * will be called on it, again bypassing the overridden dereference method.
 * This means that to implement the increment operator is not enough to import
 * the inherited one (`using iterator::operator++;`).
 * 
 * This task of wrapping a `base_iterator` involves a lot of "boilerplate" code:
 * the prefix increment operator will always be
 * `auto& operator++() { base_iterator::operator++(); return *this; }`, the
 * indexing operator will always be
 * `auto operator[](std::size_t i) -> decltype(auto) { return std::addressof(base_iterator::operator[](i)); }`
 * etc. The usual solution is to derive the iterator class from one that
 * implements the boilerplate. Unfortunately part of the boilerplate is from
 * the derived class and so it can't appear in the base class. With run-time
 * polymorphism, the base iterator might define an abstract value transformation
 * method (`transform()`) and use it in its other methods; the linker will take
 * care later on of plugging the right `transform()` method from the derived
 * class. To obtain the same effect at compile time, the base class needs to
 * know in advance the `transform()` function. Plugging it as a templated
 * literal argument (a function pointer) requires quite some gymnastic in
 * predicting the right data type, especially the return type.
 * A weird alternative is to have this base class inherit from the derived
 * class, specified as template argument. The derived iterator looks like:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * struct address_iterator: public iterator_base<address_iterator, iterator> {
 *   using iterator_base_t = iterator_base<address_iterator>;
 *   using iterator_base_t::iterator_base_t;
 *   static auto transform(iterator const& it) { return std::addressof(*it); }
 * };
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * and the weirdness is concentrated in the `iterator_base`:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * template <typename FinalIter, typename WrappedIter>
 * class iterator_base: private WrappedIter {
 *   WrappedIter& asWrapped() const
 *     { return static_const<WrappedIter&>(*this); }
 *   FinalIter& asFinal() { return static_const<FinalIter&>(*this); }
 *     public:
 *   iterator_base() = default;
 *   iterator_base(WrappedIter const& from): WrapperIter(from) {}
 *   FinalIter& operator++() { WrappedIter::operator++(); return asFinal(); }
 *   auto operator*() const -> decltype(auto)
 *     { return asFinal().transform(*asWrapped()); }
 *   bool operator!= (iterator_base const& other) const
 *     { return asWrapped() != other.asWrapped(); }
 * }; // class iterator_base
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * With this class, it's possible to transform an `iterator` into an
 * `address_iterator`, in a similar way to how described in the "Interface
 * replacement" section (there are some workaround needed because of private
 * inheritance and to ensure that the iterator traits are correct).
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
 * `TrackPointWrapper`. The underlying storage includes pointers to all relevant
 * information (flags, position, hit, etc.), and the index of the point in the
 * track.
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
    struct StaticAsserts;
    
    
    template <typename Data>
    struct StaticAsserts<TrackPointWrapper<Data>> {
      using Wrapper_t = TrackPointWrapper<Data>;
      
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
      details::StaticAsserts<TrackPointWrapper<Data>>();
      return reinterpret_cast<TrackPointWrapper<Data> const&>(wrappedData); 
    }
  
  struct TrackPoint
    : private TrackPointData
    , public TrackPointWrapper<TrackPointData>
  {
    using TrackPointData::TrackPointData;
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
