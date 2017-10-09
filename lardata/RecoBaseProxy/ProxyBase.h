/**
 * @file   lardata/RecoBaseProxy/ProxyBase.h
 * @brief  Base utilities for the implementation of data product facades.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 * This library is header-only.
 */

#ifndef LARDATA_RECOBASEPROXY_PROXYBASE_H
#define LARDATA_RECOBASEPROXY_PROXYBASE_H

// LArSoft libraries
#include "lardata/Utilities/CollectionView.h"
#include "lardata/Utilities/TupleLookupByTag.h" // util::getByTag(), ...
#include "larcorealg/CoreUtils/DebugUtils.h" // lar::debug::demangle()

// framework libraries
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Utilities/InputTag.h"

// C/C++ standard 
#include <vector>
#include <string> // std::to_string()
#include <tuple> // std::tuple_element_t<>, std::get()
#include <iterator> // std::distance(), ...
#include <algorithm> // std::min()
#include <utility> // std::forward()
#include <stdexcept> // std::runtime_error, std::logic_error
#include <type_traits> // std::is_same<>, std::enable_if_t<>, ...
#include <limits> // std::numeric_limits<>
#include <cstdlib> // std::size_t
#include <cassert>


/**
 * @defgroup LArSoftProxies LArSoft data proxies
 * @brief Helper classes for easier access to connected data.
 * 
 * Proxies are objects that expose multiple connected data products with a
 * single interface, implicitly browsing the interconnections.
 * The connections may be explicit (via some type of data product association,
 * like `art::Assns`, or element indices) or implicit, following an agreed rule
 * (like in @ref LArSoftProxyDefinitionParallelData "parallel data products").
 * 
 * More complex proxy implementations are provided for some LArSoft data
 * products.
 * New proxies can be created as well.
 * 
 * 
 * @{
 */
/**
 * @page LArSoftProxiesIntro Introduction to LArSoft data product proxy objects.
 * 
 * 
 * @section LArSoftProxyDefinitions Definitions
 * 
 * * *one-to-many sequential association*:
 *   @anchor LArSoftProxyDefinitionOneToManySeqAssn
 *   an association between `L` and `R` types where:
 *     * `L` objects come from a single data product
 *     * the sequence of associations is such that if `L1` is before `L2` in the
 *       original data product, all `L1`-`Rx` associations of `L1` are listed
 *       before any of the `L2`-`Rx` associations of `L2`; in other words, the
 *       association list follows the original order of the `L` data product;
 *       note that this preclude actual many-to-many associations.
 *   This does _not_ require associations to be one-to-one (it allows one `L` to
 *   many `R`), nor that all `L` be associated to at least one `R`.
 * * *parallel data product*:
 *   @anchor LArSoftProxyDefinitionParallelData
 *   a data product collection of elements extending
 *   the information from another data product collection ("main"), where
 *     * the two data products have the same number of elements
 *     * there is an implicit one-to-one association between the elements of the
 *       two data products, so that an element at position _i_ of the parallel
 *       data product pertains to the element at position _i_ of the main
 *       data product
 * * *one-to-(zero-or-one) sequential association*:
 *   @anchor LArSoftProxyDefinitionOneToZeroOrOneSeqAssn
 *   an association between `L` and `R` types where:
 *     * `L` objects come from a single data product
 *     * there is at most one `R` associated to each single `L`
 *     * the sequence of associations is such that if `L1` is before `L2` in the
 *       original data product, `L1`-`R1` association is listed before the
 *       `L2`-`R2` association; in other words, the association list follows the
 *       original order of the `L` data product.
 *   This does require associations to be one-to-one, bit it does _not_ require
 *   that all `L` be associated to at least one `R`.
 * 
 * 
 */
/**
 * @defgroup LArSoftProxyCustom LArSoft data proxy infrastructure
 * @ingroup LArSoftProxies
 * @brief Classes for implementation and customization of LArsoft proxies.
 * 
 * This documentation section contains hints for the creation or customization
 * of data product proxies. The creation of new proxies is not overly hard, but
 * it is nevertheless not straightforward, and following the example of already
 * implemented proxies may be the best starting point.
 * 
 * It also explains some implementation choices.
 * 
 * @bug The current design is flawed in the support of (sub)proxies as direct
 *      elements of a proxy. The merged elements are implemented as base classes
 *      of the proxies, to allow their potentially customized interface to
 *      percolate to the proxy. Since an indirect base class can't appear also
 *      as direct base class, trying to merge a proxy causes all sorts of
 *      conflicts between base classes. The stub code `withCollectionProxyAs()`
 *      and related is showing that problem on some (all?) usages.
 *      [the author hasn't tried to create a working combination for it]
 * 
 * @section LArSoftProxySimple The simplest new proxy
 * 
 * In its simplest form, a proxy may be created with no customization at all:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<std::vector<recob::Track>>
 *   (event, tracksTag, proxy::withAssociated<recob::Hit>());
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * makes a proxy object called `tracks` which accesses a `recob::Track`
 * collection data product and its associated hits, assuming that tracks and
 * their association with hits be created by the same module (`trackTag`).
 * From this, it is possible to access tracks and their hits:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * for (auto trackInfo: tracks) {
 *   
 *   recob::Track const& track = *trackInfo; // access to the track
 *   double const startTheta = track.Theta();
 *   
 *   double const length = trackInfo->Length(); // access to track members
 * 
 *   // access to associated data (returns random-access collection-like object)
 *   decltype(auto) hits = trackInfo.get<recob::Hit>();
 *   
 *   double charge = 0.0;
 *   for (auto const& hitPtr: hits) {
 *     charge += hitPtr->Integral();
 *   } // for hits
 *   
 *   mf::LogVerbatim("Info")
 *     << "[#" << trackInfo.index() << "] track ID=" << track.ID()
 *     << " (" << length << " cm, starting with theta=" << startTheta
 *     << " rad) deposited charge=" << charge
 *     << " with " << hits.size() << " hits";
 *   
 * } // for tracks
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * 
 * @subsection LArSoftProxyQuirks Quirks of proxy usage (a.k.a. "C++ is not python")
 * 
 * There are a number of things one should remember when using proxies.
 * 
 * First, the type of the proxy collection, and the type of the proxy collection
 * element, are not trivial. That is the reason why we use
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<std::vector<recob::Track>>
 *   (event, tracksTag, proxy::withAssociated<recob::Hit>());
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * instead of
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * proxy::CollectionProxyBase<
 *     proxy::CollectionProxyElement,
 *     std::vector<recob::Track>,
 *     proxy::details::AssociatedData<recob::Track, recob::Hit, recob::Hit>
 *   >
 *   tracks = proxy::getCollection<std::vector<recob::Track>>
 *   (event, tracksTag, proxy::withAssociated<recob::Hit>());
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * and even more so
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * for (auto trackInfo: tracks)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * instead of the full class name
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * for (proxy::CollectionProxyElement<
 *   proxy::CollectionProxyBase<
 *     proxy::CollectionProxyElement,
 *     std::vector<recob::Track>,
 *     proxy::details::AssociatedData<recob::Track, recob::Hit, recob::Hit> >
 *   > trackInfo: tracks)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * More important, the type depends on which elements we merged into the
 * collection proxy (in the example, `proxy::details::AssociatedData` reveals
 * that we merged an associated data). This means that a C++ function in general
 * can't reliably take a proxy argument by specifying its type, and it needs to
 * use templated arguments instead:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * template <typename Track>
 * unsigned int nHitsOnView(Track const& track, geo::View view);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Also remember that template class methods are allowed but they can't be
 * virtual.
 * 
 * Second quirk, which yields a confused compilation message (at least with GCC
 * 6), is that template class methods of objects of a template type need the
 * `template` keyword for C++ to understand what's going on:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * template <typename Track>
 * unsigned int nHitsOnView(Track const& track, geo::View view) {
 *   unsigned int count = 0U;
 *   for (art::Ptr<recob::Hit> const& hitPtr: track.template get<recob::Hit>())
 *     if (hitPtr->View() == view) ++count;
 *   return count;
 * } // nHitsOnView()
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Here, `track` is a constant reference to type `Track`, which is a template
 * type, so that when we ask for `track.get<recob::Hit>()` the compiler does not
 * know that the object `track` of type `Track` has a method `get()` which is a
 * template method, and it gets confused (in fact, it thinks the expression
 * might be a comparison, `track.get < recob::Hit`, and hilarity ensues).
 * This is not true when the type of the object is immediately known:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto tracks = proxy::getCollection<std::vector<recob::Track>>
 *   (event, tracksTag, proxy::withAssociated<recob::Hit>());
 * 
 * for (auto track: tracks) {
 *   unsigned int count = 0U;
 *   for (art::Ptr<recob::Hit> const& hitPtr: track.get<recob::Hit>())
 *     if (hitPtr->View() == view) ++count;
 *   mf::LogVerbatim("") << "Track ID=" << track->ID() << ": " << count
 *     << " hits on view " << view;
 * } // for
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * where `tracks` is a well-known (to the compiler) type, and `track` as well.
 * 
 * 
 * @section LArSoftProxyCustomization Customization of collection proxies
 * 
 * The "customization" of a collection proxy consists of writing classes and
 * functions specific for a use case, to be used as components of a collection
 * proxy in place of the standard ones.
 * 
 * The options of customization are numerous, and it is recommended that
 * customization start from the code of an existing customized proxy
 * implementing functionalities similar to the desired ones.
 * In the same spirit, customization hints are not provided here, but rather
 * in the
 * @ref LArSoftProxyTracksCustom "documentation of the proxy::Tracks collection proxy".
 * 
 * 
 * 
 * @section LArSoftProxyImplementation Technical details
 * 
 * @subsection LArSoftProxyOverhead Overhead
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
 * @subsection LArSoftProxyInterfaceSubstitition Interface substitution
 * 
 * A technique that is used in this implementation is to replace (or extend) the
 * interface of an existing object.
 * The @ref InterfaceSubstitution "documentation of file CollectionView.h"
 * includes a more in-depth description of it.
 * 
 * 
 * @subsection IteratorWrappers Iterator wrappers and "static polymorphism"
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
 * substitution" section (there are some workaround needed because of private
 * inheritance and to ensure that the iterator traits are correct).
 * 
 * @note I learned afterward about the existence of `boost::iterator_adapter`,
 *       which might provide similar functionality and also be dealing correctly
 *       with non-constant iterators. Worth considering.
 * 
 */
/**
 * @defgroup LArSoftProxyReco Reconstructed object data proxy
 * @ingroup LArSoftProxies
 * @brief Data proxies for LArSoft reconstruction data products.
 * 
 */
// LArSoftProxyReco group is only defined here, no content is provided.
// We selectively add to LArSoftProxies group via @ingroup directives.
/**
 * @}
 */


/// Encloses LArSoft data product proxy objects and utilities.
/// @ingroup LArSoftProxies
namespace proxy {
  
  //----------------------------------------------------------------------------
  /// Trait of value contained in the template collection.
  template <typename Coll>
  struct collection_value_type {
    using type = typename Coll::value_type;
    using value_type = type;
  }; // struct collection_value_type
  
  /// Type contained in the collection `Coll`.
  template <typename Coll>
  using collection_value_t = typename collection_value_type<Coll>::type;
  
  
  /// Trait of type returned when accessing an element of collection `Coll`.
  template <typename Coll>
  struct collection_value_access_type {
      private:
    static auto getBegin(Coll&& coll) { using std::begin; return begin(coll); }
    
      public:
    using type = decltype(*getBegin(std::declval<Coll>()));
    using value_type = collection_value_t<Coll>;
    
  }; // struct collection_value_access_type
  
  /// Type returned when accessing an element of collection `Coll`.
  template <typename Coll>
  using collection_value_access_t
    = typename collection_value_access_type<Coll>::type;
  
  
  /// Trait of type returned when accessing an element of collection `Coll`.
  template <typename Coll>
  struct collection_value_constant_access_type {
      private:
    static auto getCBegin(Coll&& coll)
      { using std::cbegin; return cbegin(coll); }
    
      public:
    using type = decltype(*getCBegin(std::declval<Coll>()));
    using value_type = collection_value_t<Coll>;
    
  }; // struct collection_value_constant_access_type
  
  /// Type returned when accessing an element of collection `Coll`.
  template <typename Coll>
  using collection_value_constant_access_t
    = typename collection_value_constant_access_type<Coll>::type;
  
  
  //----------------------------------------------------------------------------
  namespace details {
    
    //--------------------------------------------------------------------------
    template <bool B>
    using YesNoStruct = std::integral_constant<bool, B>;
    
    //--------------------------------------------------------------------------
    // forward declarations
    template <typename MainColl>
    struct MainCollectionProxy;
    
    //--------------------------------------------------------------------------
    template <typename AuxCollTuple>
    struct SubstituteWithAuxList;
    
    //--------------------------------------------------------------------------
    template <
      typename AuxColl,
      typename Aux = collection_value_t<AuxColl>,
      typename Tag = Aux
      >
    class ParallelData;
    
    //--------------------------------------------------------------------------
    template <typename Main, typename Aux, typename Tag = Aux>
    class AssociatedData;
    
    //--------------------------------------------------------------------------
    template <typename Main, typename Aux, typename Tag = Aux>
    class OneTo01Data;
    
    //--------------------------------------------------------------------------
    template <
      typename Aux,
      typename ArgTuple,
      template <typename CollProxy> class ProxyMaker,
      typename AuxTag = Aux
      >
    class WithAssociatedStructBase;
    
    template <typename Aux, typename AuxTag = Aux>
    struct AssociatedDataProxyMakerWrapper;
    
    template <typename Aux, typename ArgTuple, typename AuxTag = Aux>
    using WithAssociatedStruct = WithAssociatedStructBase<
      Aux,
      ArgTuple,
      AssociatedDataProxyMakerWrapper<Aux, AuxTag>::template maker_t,
      AuxTag
      >;

    template <typename Aux, typename AuxTag = Aux>
    struct OneTo01DataProxyMakerWrapper;
    
    template <typename Aux, typename ArgTuple, typename AuxTag = Aux>
    using WithOneTo01AssociatedStruct = WithAssociatedStructBase<
      Aux,
      ArgTuple,
      OneTo01DataProxyMakerWrapper<Aux, AuxTag>::template maker_t,
      AuxTag
      >;

    template <typename Aux, typename AuxTag, typename AuxColl = void>
    struct ParallelDataProxyMakerWrapper;
    
    template <typename Aux, typename ArgTuple, typename AuxTag = Aux>
    using WithParallelCollectionStruct = WithAssociatedStructBase<
      Aux,
      ArgTuple,
      ParallelDataProxyMakerWrapper<Aux, AuxTag>::template maker_t,
      AuxTag
      >;
    
    template
      <typename Aux, typename ArgTuple, typename AuxColl, typename AuxTag = Aux>
    using WithWrappedParallelCollectionStruct = WithAssociatedStructBase<
      Aux,
      ArgTuple,
      ParallelDataProxyMakerWrapper<Aux, AuxTag, AuxColl>::template maker_t,
      AuxTag
      >;
    
    
    //--------------------------------------------------------------------------
    template <
      typename AuxProxyColl,
      typename Aux = collection_value_t<AuxProxyColl>,
      typename Tag = Aux
      >
    struct ProxyAsParallelData;
    
    template <
      typename AuxProxy,
      typename ArgTuple,
      typename AuxTag = AuxProxy
      >
    class WithProxyAsAuxStructBase;
    
    
    //--------------------------------------------------------------------------
    template <typename ProxyElement, typename... AuxData>
    auto makeCollectionProxyElement(
      std::size_t index,
      typename ProxyElement::main_element_t const& main,
      AuxData&&... auxData
      );
    
    //--------------------------------------------------------------------------
    template <typename Cont>
    class IndexBasedIterator;
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  // forward declarations
  template <typename CollProxy, typename Event, typename... OptionalArgs>
  auto getCollection(Event const&, OptionalArgs&&...);
  
  
  //----------------------------------------------------------------------------
  /**
   * @defgroup LArSoftProxiesAssociatedData Associated data infrastructure
   * @ingroup  LArSoftProxyCustom
   * @brief Infrastructure for support of associated data.
   * 
   * Associated data is auxiliary data connected to the main data via _art_
   * associations.
   * The following associated data are currently supported:
   * * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association"
   *   implicitly supporting also one-to-any (one-to-one, one-to-zero/or/one) in
   *   a non-optimized way
   * 
   * @{
   */
  
  //----------------------------------------------------------------------------
  //--- one-to-(zero/one) associations
  //----------------------------------------------------------------------------
  /**
   * @brief Processes and returns an one-to-(zero/one) associated data object.
   * @tparam Tag the tag labelling this associated data
   *             (if omitted: second type of the association: `right_t`)
   * @tparam Assns type of association to be processed
   * @param assns association object to be processed
   * @param minSize minimum number of entries in the produced association data
   * @return a new `OneTo01Data` filled with associations from `tag`
   * 
   * The content of the association object must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero or one) sequential association".
   * The `Assns` type is expected to be a `art::Assns` instance. At least,
   * the `Assns` type is required to have `left_t` and `right_t` definitions
   * representing respectively the main data type and the associated one, and
   * to respond to `begin()` and `end()` functions. The iterated object must
   * also respond to `std::get<0>()` with a `art::Ptr<left_t>` and to
   * `std::get<1>()` with a `art::Ptr<right_t>`.
   * 
   * Elements in the main collection not associated with any object will present
   * an invalid _art_ pointer (`isNull()` true). If there is information for
   * less than `minSize` main objects, more records will be added to mark the
   * missing objects as not associated to anything.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * art::Assns<recob::Track, recob::Vertex> trackVertexAssns;
   * // ...
   * auto assData = makeAssociatedTo01data(assns);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * will have `assData` tagged as `recob::Vertex`.
   */
  template <typename Tag, typename Assns>
  auto makeAssociatedTo01data(Assns const& assns, std::size_t minSize = 0);
  
  template <typename Assns>
  auto makeAssociatedTo01data(Assns const& assns, std::size_t minSize = 0)
    { return makeAssociatedTo01data<typename Assns::right_t>(assns, minSize); }
  
  /**
   * @brief Creates and returns an one-to-(zero/one) associated data object.
   * @tparam Main type of main object to be associated
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Event type of event to read associations from
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @param minSize minimum number of entries in the produced association data
   * @return a new `OneTo01Data` filled with associations from `tag`
   * @see `makeAssociatedTo01data(Assns, std::size_t)`
   * 
   * The association being retrieved must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero or one) sequential association".
   * 
   * Two template types must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeAssociatedTo01data<recob::Track, recob::Vertex>(event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <typename Main, typename Aux, typename Tag, typename Event>
  auto makeAssociatedTo01data
    (Event const& event, art::InputTag tag, std::size_t minSize = 0);
  
  template <typename Main, typename Aux, typename Event>
  auto makeAssociatedTo01data
    (Event const& event, art::InputTag tag, std::size_t minSize = 0)
    { 
      return makeAssociatedTo01data<Main, Aux, Aux, Event>(event, tag, minSize);
    }
  
  /**
   * @brief Creates and returns an one-to-(zero/one) associated data object.
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Handle type of handle to the main collection object
   * @tparam Event type of event to read associations from
   * @param handle handle to the main collection object
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @return a new `OneTo01Data` wrapping the information in `assns`
   * @see `makeAssociatedData(Event const&, art::InputTag, std::size_t)`
   * 
   * This function operates like
   * `makeAssociatedTo01data(Event const&, art::InputTag, std::size_t)`, but it
   * extracts the information about the type of main object and the minimum
   * number of them from a handle.
   * The handle object is expected to behave as a smart pointer to a
   * collection of elements of the associated type.
   * 
   * One template type must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeAssociatedTo01data<recob::Vertex>(handle, event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <typename Aux, typename Tag, typename Handle, typename Event>
  auto makeAssociatedTo01data
    (Handle&& handle, Event const& event, art::InputTag tag);
  
  template <typename Aux, typename Handle, typename Event>
  auto makeAssociatedTo01data
    (Handle&& handle, Event const& event, art::InputTag tag)
    {
      return makeAssociatedTo01data<Aux, Aux, Handle, Event>
        (std::forward<Handle>(handle), event, tag);
    }
  
  
  /**
   * @brief Creates and returns an one-to-(zero/one) associated data object.
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam MainColl type of the main collection object
   * @tparam Assns type of the association object
   * @param mainColl the main collection object
   * @param assns association data object
   * @return a new `OneTo01Data` wrapping the information in `assns`
   * @see `makeAssociatedTo01data(Assns const&, std::size_t)`
   * 
   * This function operates like
   * `makeAssociatedTo01data(Assns const&, std::size_t)`, where the size is
   * extracted from the main data collection.
   */
  template <typename Tag, typename MainColl, typename Assns>
  auto makeAssociatedTo01data(MainColl const& mainColl, Assns const& assns)
    { return makeAssociatedTo01data<Tag>(assns, mainColl.size()); }
  
  template <typename MainColl, typename Assns>
  auto makeAssociatedTo01data(MainColl const& mainColl, Assns const& assns)
    { return makeAssociatedTo01data<typename Assns::right_t>(mainColl, assns); }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an one-to-(zero-or-one) wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam AuxTag tag labelling this association
   * 
   * Usually, `AuxTag` is also the type of datum (element) to associate to
   * ("right").
   * 
   * This class works as a base class for `OneTo01DataProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <typename Main, typename Aux, typename AuxTag = Aux>
  struct OneTo01DataProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum ("left").
    using main_element_t = Main;
    
    /// Type of the auxiliary associated datum ("right").
    using aux_element_t = Aux;
    
    /// Type of associated data proxy being created.
    using aux_collection_proxy_t
       = details::OneTo01Data<main_element_t, aux_element_t, data_tag>;
    
    /// Type of _art_ association being used as input.
    using assns_t = typename aux_collection_proxy_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param mainArgs an object describing the main data product
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The `mainArgs` object is of an arbitrary type that must be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
      {
        return createFromTag
          (event, std::forward<Handle>(mainHandle), art::InputTag(mainArgs));
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param auxInputTag the tag of the association to be read
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make(
      Event const& event, Handle&& mainHandle,
      MainArgs const&, art::InputTag auxInputTag
      )
      {
        return
          createFromTag(event, std::forward<Handle>(mainHandle), auxInputTag);
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from (unused)
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param handle handle to the main data collection
     * @param assns the associations to be wrapped
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs, typename Assns>
    static auto make
      (Event const&, Handle&& handle, MainArgs const&, Assns const& assns)
      {
        static_assert(
          std::is_convertible<typename Assns::right_t, aux_element_t>(),
          "Improper right type for one-to-(zero-or-one) association."
          );
        return makeAssociatedTo01data<data_tag>(assns, handle->size());
      }
    
    
      private:
    template<typename Event, typename Handle>
    static auto createFromTag
      (Event const& event, Handle&& mainHandle, art::InputTag auxInputTag)
      {
        return makeAssociatedTo01data<main_element_t, aux_element_t, data_tag>
          (event, auxInputTag, mainHandle->size());
      }
    
  }; // struct OneTo01DataProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an one-to-(zero-or-one) wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @see `withZeroOrOne()`
   * This class is (indirectly) called when using `proxy::withZeroOrOne()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the proxy to the data
   * association between the main data type and an auxiliary one.
   * The interface required by `withZeroOrOne()` includes:
   * * a static `make()` method creating and returning the associated data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the associated proxy (coming from
   *   `withZeroOrOne()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withAssociated()` for an example
   * of the general procedure).
   * The default implementation just wraps a one-to-(zero-or-one)
   * `art::Assns<Main, Aux>` data product fulfilling
   * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero or one) sequential association"
   * requirement.
   * 
   * The last template argument is designed for specialization of associations
   * in the context of a specific proxy type.
   */
  template
    <typename Main, typename Aux, typename CollProxy, typename Tag = Aux>
  class OneTo01DataProxyMaker
    : public OneTo01DataProxyMakerBase<Main, Aux, Tag>
  {
    //
    // Note that this implementation is here only to document how to derive
    // a AssociatedDataProxyMaker (specialization) from
    // AssociatedDataProxyMakerBase. It's just mirroring the base class.
    //
    using base_t = OneTo01DataProxyMakerBase<Main, Aux, Tag>;
    
      public:
    
    /// Type of the main datum ("left").
    using typename base_t::main_element_t;
    
    /// Type of the auxiliary associated datum ("right").
    using typename base_t::aux_element_t;
    
    /// Type of associated data proxy being created.
    using typename base_t::aux_collection_proxy_t;
    
    /// Type of _art_ association being used as input.
    using typename base_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of data product handle
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @tparam Args optional single type (`art::InputTag` required)
     * @param event the event to read associations from
     * @param mainHandle handle of the main collection data product
     * @param margs an object describing the main data product
     * @param args input tag for associated data, if different from main
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * This implementation requires `margs` object to be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... Args>
    static auto make(
      Event const& event, Handle&& mainHandle, MainArgs const& margs,
      Args&&... args
      )
      {
        return base_t::make(
          event,
          std::forward<Handle>(mainHandle),
          margs,
          std::forward<Args>(args)...
          );
      }
    
  }; // struct OneTo01DataProxyMaker<>
  
  
  //----------------------------------------------------------------------------
  //--- one-to-many associations
  //----------------------------------------------------------------------------
  /**
   * @brief Processes and returns an associated data object.
   * @tparam Tag the tag labelling this associated data
   *             (if omitted: second type of the association: `right_t`)
   * @tparam Assns type of association to be processed
   * @param assns association object to be processed
   * @param minSize minimum number of entries in the produced association data
   * @return a new `AssociatedData` filled with associations from `tag`
   * 
   * The content of the association object must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association".
   * The `Assns` type is expected to be a `art::Assns` instance. At least,
   * the `Assns` type is required to have `left_t` and `right_t` definitions
   * representing respectively the main data type and the associated one, and
   * to respond to `begin()` and `end()` functions. The iterated object must
   * also respond to `std::get<0>()` with a `art::Ptr<left_t>` and to
   * `std::get<1>()` with a `art::Ptr<right_t>`.
   * 
   * Elements in the main collection not associated with any object will be
   * recorded as such. If there is information for less than `minSize` main
   * objects, more records will be added to mark the missing objects as not
   * associated to anything.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * art::Assns<recob::Track, recob::Hit> trackHitAssns;
   * // ...
   * auto assData = makeAssociatedData(assns);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * will have `assData` tagged as `recob::Hit`.
   */
  template <typename Tag, typename Assns>
  auto makeAssociatedData(Assns const& assns, std::size_t minSize = 0);
  
  template <typename Assns>
  auto makeAssociatedData(Assns const& assns, std::size_t minSize = 0)
    { return makeAssociatedData<typename Assns::right_t>(assns, minSize); }
  
  /**
   * @brief Creates and returns an associated data object.
   * @tparam Main type of main object to be associated
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Event type of event to read associations from
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @param minSize minimum number of entries in the produced association data
   * @return a new `AssociatedData` filled with associations from `tag`
   * 
   * The association being retrieved must fulfill the requirements of
   * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association".
   * 
   * Elements in the main collection not associated with any object will be
   * recorded as such. If there is information for less than `minSize` main
   * objects, more records will be added to mark the missing objects as not
   * associated to anything.
   * 
   * Two template types must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeAssociatedData<recob::Track, recob::Hit>(event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <typename Main, typename Aux, typename Tag, typename Event>
  auto makeAssociatedData
    (Event const& event, art::InputTag tag, std::size_t minSize = 0);
  
  template <typename Main, typename Aux, typename Event>
  auto makeAssociatedData
    (Event const& event, art::InputTag tag, std::size_t minSize = 0)
    { return makeAssociatedData<Main, Aux, Aux, Event>(event, tag, minSize); }
  
  /**
   * @brief Creates and returns an associated data object.
   * @tparam Aux type of data to be associated to the main objects
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam Handle type of handle to the main collection object
   * @tparam Event type of event to read associations from
   * @param handle handle to the main collection object
   * @param event event to read associations from
   * @param tag input tag of the association object
   * @return a new `AssociatedData` filled with associations from `tag`
   * @see `makeAssociatedData(Event const&, art::InputTag, std::size_t)`
   * 
   * This function operates like
   * `makeAssociatedData(Event const&, art::InputTag, std::size_t)`, but it
   * extracts the information about the type of main object and the minimum
   * number of them from a handle.
   * The handle object is expected to behave as a smart pointer to a
   * collection of elements of the associated type.
   * 
   * One template type must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto assData = makeAssociatedData<recob::Hit>(handle, event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  template <typename Aux, typename Tag, typename Handle, typename Event>
  auto makeAssociatedData
    (Handle&& handle, Event const& event, art::InputTag tag);
  
  template <typename Aux, typename Handle, typename Event>
  auto makeAssociatedData
    (Handle&& handle, Event const& event, art::InputTag tag)
    {
      return makeAssociatedData<Aux, Aux, Handle, Event>
        (std::forward<Handle>(handle), event, tag);
    }
  
  
  /**
   * @brief Creates and returns an associated data object.
   * @tparam Tag the tag labelling this associated data (if omitted: `Aux`)
   * @tparam MainColl type of the main collection object
   * @tparam Assns type of the association object
   * @param mainColl the main collection object
   * @param assns association data object
   * @return a new `AssociatedData` wrapping the information in `assns`
   * @see `makeAssociatedData(Assns const&, std::size_t)`
   * 
   * This function operates like
   * `makeAssociatedData(Assns const&, std::size_t)`, where the size is
   * extracted from the main data collection.
   */
  template <typename Tag, typename MainColl, typename Assns>
  auto makeAssociatedData(MainColl const& mainColl, Assns const& assns)
    { return makeAssociatedData<Tag>(assns, mainColl.size()); }
  
  template <typename MainColl, typename Assns>
  auto makeAssociatedData(MainColl const& mainColl, Assns const& assns)
    { return makeAssociatedData<typename Assns::right_t>(mainColl, assns); }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam AuxTag tag labelling this association
   * 
   * Usually, `AuxTag` is also the type of datum (element) to associate to
   * ("right").
   * 
   * This class works as a base class for `AssociatedDataProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <typename Main, typename Aux, typename AuxTag = Aux>
  struct AssociatedDataProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum ("left").
    using main_element_t = Main;
    
    /// Type of the auxiliary associated datum ("right").
    using aux_element_t = Aux;
    
    /// Type of associated data proxy being created.
    using aux_collection_proxy_t
       = details::AssociatedData<main_element_t, aux_element_t, data_tag>;
    
    /// Type of _art_ association being used as input.
    using assns_t = typename aux_collection_proxy_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param mainArgs an object describing the main data product
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The `mainArgs` object is of an arbitrary type that must be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
      {
        return createFromTag
          (event, std::forward<Handle>(mainHandle), art::InputTag(mainArgs));
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param event the event to read associations from
     * @param mainHandle handle to the main collection data product
     * @param auxInputTag the tag of the association to be read
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make(
      Event const& event, Handle&& mainHandle,
      MainArgs const&, art::InputTag auxInputTag
      )
      {
        return
          createFromTag(event, std::forward<Handle>(mainHandle), auxInputTag);
      }
    
    /**
     * @brief Create a association proxy collection using the specified tag.
     * @tparam Event type of the event to read associations from (unused)
     * @tparam Handle type of handle to the main data product (unused)
     * @tparam MainArgs any type convertible to `art::InputTag` (unused)
     * @param assns the associations to be wrapped
     * @return a auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs, typename Assns>
    static auto make
      (Event const&, Handle&&, MainArgs const&, Assns const& assns)
      {
        static_assert(
          std::is_convertible<typename Assns::right_t, aux_element_t>(),
          "Improper right type for association."
          );
        return makeAssociatedData<data_tag>(assns);
      }
    
    
      private:
    template<typename Event, typename Handle>
    static auto createFromTag
      (Event const& event, Handle&& mainHandle, art::InputTag auxInputTag)
      {
        return makeAssociatedData<main_element_t, aux_element_t, data_tag>
          (event, auxInputTag, mainHandle->size());
      }
    
  }; // struct AssociatedDataProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @see `withAssociated()`
   * This class is (indirectly) called when using `proxy::withAssociated()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the proxy to the data
   * association between the main data type and an auxiliary one.
   * The interface required by `withAssociated()` includes:
   * * a static `make()` method creating and returning the associated data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the associated proxy (coming from
   *   `withAssociated()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withAssociated()` for an example).
   * The default implementation just wraps a one-to-many
   * `art::Assns<Main, Aux>` data product fulfilling "one-to-many sequential
   * association" requirement (see the "Definitions" section in `ProxyBase.h`
   * documentation).
   * 
   * The last template argument is designed for specialization of associations
   * in the context of a specific proxy type.
   */
  template
    <typename Main, typename Aux, typename CollProxy, typename Tag = Aux>
  class AssociatedDataProxyMaker
    : public AssociatedDataProxyMakerBase<Main, Aux, Tag>
  {
    //
    // Note that this implementation is here only to document how to derive
    // a AssociatedDataProxyMaker (specialization) from
    // AssociatedDataProxyMakerBase. It's just mirroring the base class.
    //
    using base_t = AssociatedDataProxyMakerBase<Main, Aux, Tag>;
    
      public:
    
    /// Type of the main datum ("left").
    using typename base_t::main_element_t;
    
    /// Type of the auxiliary associated datum ("right").
    using typename base_t::aux_element_t;
    
    /// Type of associated data proxy being created.
    using typename base_t::aux_collection_proxy_t;
    
    /// Type of _art_ association being used as input.
    using typename base_t::assns_t;
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of data product handle
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @tparam Args optional single type (`art::InputTag` required)
     * @param event the event to read associations from
     * @param mainHandle handle of the main collection data product
     * @param margs an object describing the main data product
     * @param args input tag for associated data, if different from main
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * This implementation requires `margs` object to be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... Args>
    static auto make(
      Event const& event, Handle&& mainHandle, MainArgs const& margs,
      Args&&... args
      )
      {
        return base_t::make(
          event,
          std::forward<Handle>(mainHandle),
          margs,
          std::forward<Args>(args)...
          );
      }
    
  }; // struct AssociatedDataProxyMaker<>
  
  /// @}
  // end Associated data
  
  
  /**
   * @defgroup LArSoftProxiesParallelData Parallel data infrastructure
   * @ingroup LArSoftProxyCustom
   * @brief Infrastructure for support of parallel data structures.
   * 
   * This infrastructure provides support for merging to a proxy data products
   * fulfilling the
   * @ref LArSoftProxyDefinitionParallelData "parallel data product"
   * requirements.
   * 
   * The typical pattern is to merge a parallel data product with
   * `withParallelData()`:
   * 
   * 
   * @{
   */

  //----------------------------------------------------------------------------
  /**
   * @brief Wraps a collection into a parallel data collection object.
   * @tparam AuxColl type of parallel data data product container
   * @tparam Aux type of parallel data to be associated to the main objects
   *             (if omitted: `AuxColl::value_type`)
   * @tparam Tag the tag labelling this associated data (if omitted: as `Aux`)
   * @param data data collection to be wrapped
   * @return a new `ParallelData` wrapping the information in `data`
   * 
   * The data collection must be non-temporary and it is treated as fulfilling
   * @ref LArSoftProxyDefinitionParallelData "parallel data product"
   * requirements.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * std::vector<recob::TrackFitHitInfo> trackData;
   * // ...
   * auto auxData = makeParallelData(trackData);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * where the tag of the parallel data is now `recob::TrackFitHitInfo` and
   * `auxData` behaviour becomes undefined as soon as `trackData` falls out of
   * scope.
   */
  template <
    typename AuxColl,
    typename Aux = collection_value_t<AuxColl>,
    typename Tag = Aux
    >
  auto makeParallelData(AuxColl const& data);
  
  
  /**
   * @brief Creates and returns a parallel data collection object.
   * @tparam AuxColl type of parallel data data product container
   * @tparam Aux type of parallel data to be associated to the main objects
   *             (if omitted: `AuxColl::value_type`)
   * @tparam Tag the tag labelling this associated data (if omitted: as `Aux`)
   * @tparam Event type of event to read the data product from
   * @param event event to read the data product from
   * @param tag input tag of the parallel data product
   * @return a new `ParallelData` filled with data from `tag`
   * 
   * The data product being retrieved must fulfill the requirements of
   * @ref LArSoftProxyDefinitionParallelData "parallel data product".
   * 
   * At least one template type must be explicitly specified, e.g.
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto auxData
   *   = makeParallelData<std::vector<recob::TrackFitHitInfo>>(event, tag);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this case, the `Aux` type is defined as `recob::TrackFitHitInfo`, as is
   * the tag.
   */
  template <typename AuxColl, typename Aux, typename Tag, typename Event>
  auto makeParallelData(Event const& event, art::InputTag tag);
  
  template <typename AuxColl, typename Aux, typename Event>
  auto makeParallelData(Event const& event, art::InputTag tag)
    { return makeParallelData<AuxColl, Aux, Aux, Event>(event, tag); }
  
  template <typename AuxColl, typename Event>
  auto makeParallelData(Event const& event, art::InputTag tag)
    {
      return makeParallelData<AuxColl, collection_value_t<AuxColl>, Event>
        (event, tag);
    }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an parallel data wrapper for the specified types.
   * @tparam Main type of main datum (element)
   * @tparam AuxColl type of the parallel data collection
   * @tparam Aux type of the parallel data element
   * @tparam AuxTag tag labelling the parallel data collection
   * 
   * Usually, `AuxTag` is also the type of datum (element) in the parallel
   * collection.
   * 
   * This class works as a base class for `ParallelDataProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <
    typename Main,
    typename AuxColl,
    typename Aux,
    typename AuxTag = collection_value_t<AuxColl>
    >
  struct ParallelDataProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum.
    using main_element_t = Main;
    
    /// Type of the auxiliary data product.
    using aux_collection_t = AuxColl;
    
    /// Type of the auxiliary datum.
    using aux_element_t = Aux;
    
    /// Type of associated data proxy being created.
    using aux_collection_proxy_t
       = details::ParallelData<aux_collection_t, aux_element_t, data_tag>;
    
    /**
     * @brief Create a parallel data proxy collection using main collection tag.
     * @tparam Event type of the event to read data from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs _(unused)_ any type convertible to `art::InputTag`
     * @param event the event to read data from
     * @param mainHandle _(unused)_ handle to the main collection data product
     * @param mainArgs an object describing the main data product
     * @return a parallel data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The `mainArgs` object is of an arbitrary type that must be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the parallel data collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
      {
        return createFromTag
          (event, std::forward<Handle>(mainHandle), art::InputTag(mainArgs));
      }
    
    /**
     * @brief Create a parallel data proxy collection using the specified tag.
     * @tparam Event type of the event to read data from
     * @tparam Handle type of handle to the main data product
     * @tparam MainArgs (_unused_) any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param mainHandle (_unused_) handle to the main collection data product
     * @param auxInputTag the tag of the data to be read
     * @return a parallel data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make(
      Event const& event, Handle&& mainHandle,
      MainArgs const&, art::InputTag auxInputTag
      )
      {
        return
          createFromTag(event, std::forward<Handle>(mainHandle), auxInputTag);
      }
    
    /**
     * @brief Create a parallel data proxy collection using the specified tag.
     * @tparam Event (_unused_) type of the event to read data from
     * @tparam Handle (_unused_) type of handle to the main data product
     * @tparam MainArgs (_unused_) any type convertible to `art::InputTag`
     * @param auxColl the collection to be wrapped
     * @return a parallel data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     */
    template<typename Event, typename Handle, typename MainArgs>
    static auto make
      (Event const&, Handle&&, MainArgs const&, aux_collection_t const& auxColl)
      {
        return
          makeParallelData<aux_collection_t, aux_element_t, data_tag>(auxColl);
      }
    
    
      private:
    template<typename Event, typename Handle>
    static auto createFromTag
      (Event const& event, Handle&&, art::InputTag auxInputTag)
      {
        return makeParallelData<aux_collection_t, aux_element_t, data_tag>
          (event, auxInputTag);
      }
    
  }; // struct ParallelDataProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam Aux type of datum (element) to associate to ("right")
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @tparam AuxColl type of the auxiliary data (default: `std::vector<Aux>`)
   * @see `withParallelDataAs()`, `wrapParallelDataAs()`
   * 
   * This class is (indirectly) called when using `proxy::withParallelData()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the proxy to the auxiliary data
   * parallel to the main data type.
   * The interface required by `withParallelData()` includes:
   * * a static `make()` method creating and returning the auxiliary data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the associated proxy (coming from
   *   `withParallelData()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withParallelData()` for an example).
   * The default implementation just wraps a one-to-many
   * `std::vector<Aux>` data product fulfilling "parallel data product"
   * requirement (see the "Definitions" section in `ProxyBase.h` documentation).
   * 
   * The last template argument is designed for specialization of auxiliary data
   * in the context of a specific proxy type.
   */
  template<
    typename Main, typename Aux, typename CollProxy, typename Tag = Aux,
    typename AuxColl = std::vector<Aux>
    >
  class ParallelDataProxyMaker
    : public ParallelDataProxyMakerBase<Main, AuxColl, Aux, Tag>
  {
    //
    // Note that this implementation is here only to document how to derive
    // a ParallelDataProxyMaker (specialization) from
    // ParallelDataProxyMakerBase. It's just mirroring the base class.
    //
    using base_t = ParallelDataProxyMakerBase<Main, AuxColl, Aux, Tag>;
    
      public:
    
    /// Type of the main datum.
    using typename base_t::main_element_t;
    
    /// Type of the auxiliary data product.
    using typename base_t::aux_collection_t;
    
    /// Type of the auxiliary datum.
    using typename base_t::aux_element_t;
    
    /// Type of collection data proxy being created.
    using typename base_t::aux_collection_proxy_t;
    
    
    /**
     * @brief Create a association proxy collection using main collection tag.
     * @tparam Event type of the event to read associations from
     * @tparam Handle type of data product handle
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @tparam Args optional single type (`art::InputTag` required)
     * @param event the event to read associations from
     * @param mainHandle handle of the main collection data product
     * @param margs an object describing the main data product
     * @param args input tag for parallel data, if different from main
     * @return an auxiliary data proxy object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * This implementation requires `margs` object to be convertible
     * by explicit type cast into a `art::InputTag`; that input tag will be
     * used to fetch the association.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... Args>
    static auto make(
      Event const& event, Handle&& mainHandle, MainArgs const& margs,
      Args&&... args
      )
      {
        return base_t::make(
          event,
          std::forward<Handle>(mainHandle),
          margs,
          std::forward<Args>(args)...
          );
      }
    
  }; // struct ParallelDataProxyMaker<>
  
  
  
  /// @}
  // end Parallel data
  
  
  /**
   * @defgroup LArSoftProxiesAuxProxy Infrastructure for proxies as auxiliary
   *           data.
   * @ingroup LArSoftProxyCustom
   * @brief Infrastructure to use a collection proxy as auxiliary data for
   *        another proxy.
   * @bug Broken in many ways. Do not use.
   * 
   * @{
   */
  
  //----------------------------------------------------------------------------
  template <
    typename Tag /* = Aux */,
    typename Aux /* = collection_value_t<AuxProxyColl>*/,
    typename AuxProxyColl
    >
  auto makeProxyAsParallelData(AuxProxyColl&& auxProxy)
    {
      return details::ProxyAsParallelData<AuxProxyColl, Aux, Tag>
        (std::move(auxProxy));
    } // makeProxyAsParallelData()
  
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates a proxy wrapper for merging into another proxy ("main").
   * @tparam Main type of main datum (element) of the main proxy
   * @tparam AuxProxy type ("proxy name") of the proxy being wrapped
   * @tparam AuxTag tag of the auxiliary proxy in the context of the main one
   * 
   * By default, `AuxTag` is the same as the proxy name.
   * 
   * This class works as a base class for `ProxyAsAuxProxyMaker` so that
   * the specializations of the latter can still inherit from this one if they
   * its facilities.
   */
  template <
    typename Main,
    typename AuxProxy,
    typename AuxTag = AuxProxy
    >
  struct ProxyAsAuxProxyMakerBase {
    
    /// Tag labelling the associated data we are going to produce.
    using data_tag = AuxTag;
    
    /// Type of the main datum.
    using main_element_t = Main;
    
    /// Tag-type of the auxiliary proxy (not the type of the proxy!).
    using aux_proxy_t = AuxProxy;
    
    /**
     * @brief Create a parallel data proxy collection using the specified tag.
     * @tparam Event type of the event to read data from
     * @tparam Handle (_unused_) type of handle to the main data product
     * @tparam MainArgs (_unused_) any type convertible to `art::InputTag`
     * @tparam AuxArgs type of arguments for the creation of the auxiliary proxy
     * @param event event to create the proxy from
     * @param auxProxyTag tag for the creation of the auxiliary collection proxy
     * @param args other arguments for the creation of the auxiliary proxy
     * @return a auxiliary proxy data object
     * 
     * The returned object exposes a random access container interface, with
     * data indexed by the index of the corresponding object in the main
     * collection.
     * 
     * The tag of the main collection proxy is ignored even if present, and
     * the caller must specify it.
     */
    template
      <typename Event, typename Handle, typename MainArgs, typename... AuxArgs>
    static auto make(
      Event const& event, Handle&&, MainArgs const&, art::InputTag auxProxyTag,
      AuxArgs&&... args
      )
      {
        auto auxProxy = makeAuxiliaryProxy
          (event, auxProxyTag, std::forward<AuxArgs>(args)...);
        return makeProxyAsParallelData
          <data_tag, collection_value_t<decltype(auxProxy)>>
          (std::move(auxProxy));
      }
    
    
      private:
    
    /// Creates the proxy to be used as parallel data.
    template <typename Event, typename... AuxArgs>
    static auto makeAuxiliaryProxy(
      Event const& event,
      art::InputTag auxProxyTag,
      AuxArgs&&... args
    )
      { 
        return getCollection<aux_proxy_t>
          (event, auxProxyTag, std::forward<AuxArgs>(args)...);
      }
    
    
  }; // struct ProxyAsAuxProxyMakerBase<>
  
  
  //--------------------------------------------------------------------------
  /**
   * @brief Creates an auxiliary proxy wrapper for the specified proxy.
   * @tparam Main type of main datum (element) to associate from ("left")
   * @tparam AuxProxy type of proxy collection to be associated
   * @tparam CollProxy type of proxy this associated data works for
   * @tparam Tag tag for the association proxy to be created
   * @see `withCollectionProxy()`
   * 
   * This class is (indirectly) called when using `proxy::withCollectionProxy()`
   * in `getCollection()`.
   * Its task is to supervise the creation of the collection proxy that is used
   * as auxiliary data for the main data type.
   * The interface required by `withCollectionProxy()` includes:
   * * a static `make()` method creating and returning the auxiliary data
   *   proxy with arguments an event, the main data product handle, a template
   *   argument representing the main collection information, and all the
   *   arguments required for the creation of the auxiliary collection proxy
   *   (coming from `withCollectionProxy()`); equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename Handle, typename MainArg, typename... Args>
   *   auto make(Event const&, Handle&&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized.
   * The default implementation just uses `getCollection()` to create the
   * auxiliary proxy, and merges it to the main collection proxy in a fashion
   * similar to parallel data.
   * 
   * The template argument `CollProxy` is designed for specialization of
   * auxiliary data in the context of a specific proxy type.
   */
  template <
    typename Main,
    typename AuxProxy,
    typename CollProxy,
    typename Tag = collection_value_t<AuxProxy>
    >
  class ProxyAsAuxProxyMaker
    : public ProxyAsAuxProxyMakerBase<Main, AuxProxy, Tag>
    {};
  
  
  /// @}
  // end Proxy as auxiliary data
  
  
  /**
   * @defgroup LArSoftProxiesElement Proxy element infrastructure
   * @ingroup LArSoftProxyCustom
   * @brief Infrastructure to describe the element of a proxy
   * 
   * A collection proxy element is a complex entity in that it has to figure out
   * all the data it can return, and their type.
   * 
   * The default type of a collection proxy element is
   * `proxy::CollectionProxyElement`.
   * 
   * @{
   */
  
  //----------------------------------------------------------------------------
  /**
   * @brief An element of a collection proxy.
   * @tparam CollProxy the type of collection proxy this is the element of
   * 
   * The collection proxy element represents, unsurprisingly, a single element
   * of the collection proxy.
   * It exposes all the connections between the components merged into the
   * collection proxy, for a specific element.
   * 
   * The interface of a proxy element allows it to access the main and auxiliary
   * data as follows:
   * * main data element (always present!):
   *     * the whole object, by dereference method (`*track`)
   *     * methods and fields, by member access (`track->Length()`)
   * * auxiliary data: accessed by tag (usually it's the default, that is the
   *   same as the data type):
   *     * check if available: `has()` method, static
   *       (`track.has<recob::Hit>()`)
   *     * get the data:
   *         * `get()` method; the data *must* be available or this
   *           call will not compile (`track.get<recob::Hit>()`)
   *         * `getIf()` method; if the data is not available, an exception is
   *           thrown
   * * _index_ of the element in the collection (a bonus!): `index()` method
   * 
   * The for loop block illustrates how to use some of them:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<std::vector<recob::Track>>
   *   (event, tracksTag, proxy::withAssociated<recob::Hit>());
   * 
   * for (auto track: tracks) {
   *   
   *   recob::Track const& trackObj = *track; // access to the track
   *   
   *   double const length = track->Length(); // access to track members
   * 
   *   // access to associated data
   *   // (returns random-access collection-like object)
   *   decltype(auto) hits = track.get<recob::Hit>();
   *   double charge = 0.0;
   *   for (auto const& hitPtr: hits) charge += hitPtr->Integral();
   *   
   *   std::vector<recob::TrackFitHitInfo> const* fitInfo
   *     = track.has<std::vector<recob::TrackFitHitInfo>>()
   *     ? &(track.getIf<std::vector<recob::TrackFitHitInfo>>())
   *     : nullptr
   *     ;
   *   
   *   mf::LogVerbatim("Info")
   *     << "[#" << trackInfo.index() << "] track ID=" << trackObj.ID()
   *     << " (" << length << " cm) deposited charge=" << charge
   *     << " with " << hits.size() << " hits and"
   *     << (fitInfo? "": " no") << " fit information";
   *   
   * } // for tracks
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Please note that `getIf()` method is _not easy to use correctly_.
   * In this example, we rely on the knowledge that `track.getIf()` would return
   * a reference to a (non-temporary) object, and we take the address of that
   * object: this is not necessarily the case (e.g., it is not the case for
   * `getIf<recob::Hit>()`).
   * 
   * 
   * Customization
   * ==============
   * 
   * A proxy element class can (and should) be derived from
   * `proxy::CollectionProxyElement`.
   * On top of it, the derived class can extend or completely rewrite the
   * interface:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * template <typename CollProxy>
   * class TrackProxy: public proxy::CollectionProxyElement<CollProxy> {
   *   using base_t = proxy::CollectionProxyElement<CollProxy>;
   *     public:
   *   unsigned int nHits() const
   *     { return base_t::template get<recob::Hit>().size(); }
   * }; // class TrackProxy
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Specialization is also possible, but discouraged.
   * 
   * A `proxy::CollectionProxyElement` has knowledge of the type of collection
   * proxy it's an element of, but it has no connection to a collection proxy
   * object and in fact it never interacts with any, not even during
   * construction.
   * 
   * Once the element is customized, a `proxy::CollectionProxy` can use it by
   * having it as first template argument:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * template <typename MainColl, typename... AuxColl>
   * using Proxy_t
   *   = proxy::CollectionProxyBase<TrackProxy, MainColl, AuxColl...>;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * A `CollectionProxyMaker` specialization will have to `make()` such objects.
   */
  template <typename CollProxy>
  struct CollectionProxyElement {
    
      public:
    using collection_proxy_t = CollProxy;
    using main_element_t = typename collection_proxy_t::main_element_t;
    
    /// Tuple of elements (expected to be tagged types).
    using aux_elements_t = typename details::SubstituteWithAuxList
      <typename collection_proxy_t::aux_collections_t>::type;
    
    /// Constructor: sets the element index, the main element and steals
    /// auxiliary data.
    CollectionProxyElement
      (std::size_t index, main_element_t const& main, aux_elements_t&& auxData)
      : fIndex(index), fMain(&main) , fAuxData(std::move(auxData))
      {}
    
    /// Returns a pointer to the main element.
    main_element_t const* operator->() const { return fMain; }
    
    /// Returns a reference to the main element.
    main_element_t const& operator*() const { return *fMain; }
    
    /// Returns the index of this element in the collection.
    std::size_t index() const { return fIndex; };
    
    /// Returns the auxiliary data specified by type (`Tag`).
    template <typename Tag>
    auto get() const -> decltype(auto)
      { return std::get<util::index_of_tag_v<Tag, aux_elements_t>>(fAuxData); }
    
    
    /**
     * @brief Returns the auxiliary data specified by type (`Tag`).
     * @tparam Tag tag of the data to fetch (usually, its type)
     * @tparam T type to return (by default, a constant reference to `Tag`)
     * @return the auxiliary data specified by type (`Tag`).
     * @throw std::logic_error if the tag is not available.
     * @see get(), has()
     * 
     * This method is a `get()` which forgives when the requested type is not
     * available (because this proxy was configured not to hold it).
     * 
     * The difference with `get()` is the following:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * decltype(auto) elem = tracks[0];
     * if (elem.has<recob::Hit>()) {
     *   auto hits = elem.get<recob::Hit>();
     *   // ...
     * }
     * if (elem.has<recob::SpacePoint>()) {
     *   auto spacepoints = elem.getIf<recob::SpacePoint>();
     *   // ...
     * }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * If the proxy `tracks` has _not_ been coded with `recob::Hit` data,
     * the code snippet will _not_ compile, because `get()` will not compile
     * for tags that were not coded in. On the other end, if `recob::Hit`
     * is coded in `tracks` but `recob::SpacePoint` is not, the snippet _will_
     * compile. In that case, if the `has()` check had been omitted, `getIt()`
     * would throw a `std::logic_error` exception when executed.
     * With C++17, this will not be necessary any more by using "constexpr if":
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * decltype(auto) elem = tracks[0];
     * if constexpr (elem.has<recob::Hit>()) {
     *   auto hits = elem.get<recob::Hit>();
     *   // ...
     * }
     * if constexpr (elem.has<recob::SpacePoint>()) {
     *   auto spacepoints = elem.get<recob::SpacePoint>();
     *   // ...
     * }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * 
     * @note The second template argument contains the *exact* type returned by
     *       the function. That information is needed because this method needs
     *       to return the same type (or compatible types) whether the required
     *       tag is present or not, in order for user code like
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     *       if (elem.has<recob::SpacePoint>()) {
     *         recob::SpacePoint const* spacepoints
     *           = elem.getIf<recob::SpacePoint>(); // won't do
     *         // ...
     *       }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *       to work; it becomes:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     *       if (elem.has<recob::SpacePoint>()) {
     *         recob::SpacePoint const* spacepoints
     *           = elem.getIf<recob::SpacePoint, recob::SpacePoint const*>();
     *         // ...
     *       }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *       This is necessary because when the tag (in the example,
     *       `recob::SpacePoint`) is not registered in the proxy, `getIf()` has
     *       no clue of what the return value should be ("which is the type of
     *       the data that does not exist?").
     * 
     */
    template <typename Tag, typename T = Tag const&>
    auto getIf() const -> decltype(auto);
    
    
    /// Returns whether this class knowns about the specified type (`Tag`).
    template <typename Tag>
    static constexpr bool has() { return util::has_tag_v<Tag, aux_elements_t>; }
    
      private:
    
    std::size_t fIndex; ///< Index of this element in the proxy.
    main_element_t const* fMain; ///< Pointer to the main object of the element.
    
    // note that the auxiliary data is not tagged, we need to learn which
    // the tags are from the collection.
    aux_elements_t fAuxData; ///< Data associated to the main object.
    
    template <typename Tag, typename>
    auto getIfHas(details::YesNoStruct<true>) const -> decltype(auto);
    
    template <typename Tag, typename T>
    [[noreturn]] auto getIfHas(details::YesNoStruct<false>) const -> T;
    
  }; // CollectionProxyElement<>


  /// @}
  // end Collection proxy element infrastructure
  
  
  /**
   * @defgroup LArSoftProxyCollections Collection proxy infrastructure
   * @ingroup LArSoftProxyCustom
   * @brief Infrastructure to define a proxy of collection data product.
   * 
   * A data collection proxy connects a main collection data product with other
   * data products whose elements have relation with one of the main product
   * elements.
   * 
   * A collection proxy is created via a call to `getCollection()`, in a fashion
   * non dissimilar from calling, e.g., `art::Event::getValidHandle()`.
   * 
   * A proxy is characterized by a proxy tag, which is the type used in the
   * `getCollection()` call, but that is not necessarily in direct relation with
   * the type of the collection proxy itself. In other words, calling
   * `proxy::getCollection<proxy::Tracks>(event, trackTag)` will return a proxy
   * object whose type is likely not `proxy::Tracks`.
   * 
   * Proxy collections are created by _merging_ auxiliary data into a main
   * collection data product. See `getCollection()` for more details.
   * 
   * Proxies can be customized, meaning that collection proxies may be written
   * to have a specific interface. There are different levels of customization,
   * of the collection proxy itself, of its element type, and of the auxiliary
   * data merged. Customization may require quite a bit of coding, and the
   * easiest approach is to start from an already customized proxy implementing
   * features similar to the desired ones.
   * 
   * It's worth stressing @ref LArSoftProxyQuirks "again" that collection
   * proxies composed by merging different auxiliary data have different C++
   * types, and that if such proxies need to be propagated as function arguments
   * those arguments need to be of template type.
   * 
   * @{
   */
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Base representation of a collection of proxied objects.
   * @tparam Element type of element of the collection proxy
   * @tparam MainColl type of the collection of the main data product
   * @tparam AuxColls type of all included auxiliary data proxies
   * @see proxy::CollectionProxyElement
   * 
   * This object exposes a collection interface.
   * The collection proxy is driven by a data product containing the main
   * objects. The size of the collection proxy is the same as the one of this
   * main data product, and all associated data is referring to its elements.
   * 
   * Thus, the elements of this collection proxy are objects that collect the
   * information of a single element in the main data product and all the data
   * associated with it.
   * 
   * The `AuxColls` types are tagged types: all must define a `tag` type.
   * Their data is accessed specifying that tag, i.e. via `get<Tag>()`.
   * Therefore, tags must be unique.
   * 
   * The type `Element` is expected to expose the same interface of
   * `CollectionProxyElement`, from which it can derive. It is a template
   * that needs to take as only argument the type of collection proxy it is the
   * element of. This is a way to customize the interface of access to single
   * element of proxy.
   * 
   * 
   */
  template <
    template <typename CollProxy> class Element,
    typename MainColl,
    typename... AuxColls
    >
  class CollectionProxyBase
    : public details::MainCollectionProxy<MainColl>, public AuxColls...
  {
    /// This type.
    using collection_proxy_t
      = CollectionProxyBase<Element, MainColl, AuxColls...>;
    
    /// Type of wrapper used for the main data product.
    using main_collection_proxy_t = details::MainCollectionProxy<MainColl>;
    
      public:
    /// Type of element of the main data product.
    using typename main_collection_proxy_t::main_element_t;
    
    /// Type of collection in the main data product.
    using typename main_collection_proxy_t::main_collection_t;
    
    /// Type of element of this collection proxy.
    using element_proxy_t = Element<collection_proxy_t>;
    
    /// Tuple of all auxiliary data collections (wrappers).
    using aux_collections_t = std::tuple<AuxColls...>;
    
    /// Type of element of this collection proxy.
    using value_type = element_proxy_t;
    
    /// Type of iterator to this collection (constant).
    using const_iterator = details::IndexBasedIterator<collection_proxy_t>;
    
    /// Type of iterator to this collection (still constant).
    using iterator = const_iterator;
    
    /**
     * @brief Constructor: uses the specified data.
     * @param main the original main data product collection
     * @param aux all auxiliary data collections and structures
     * 
     * The auxiliary data structures are stolen (moved) from the arguments.
     * They are expected to be wrappers around the original associated data,
     * not owning the auxiliary data itself.
     */
    CollectionProxyBase(main_collection_t const& main, AuxColls&&... aux)
      : main_collection_proxy_t(main), AuxColls(std::move(aux))...
      {}
    
    /**
     * @brief Returns the element of this collection with the specified index.
     * @param i the index in the collection
     * @return a value representing an element of the collection
     * 
     * The returned value is an object created on the spot, not a reference to
     * an existing structure.
     * The structure exposes the `i`-th element in the main collection, plus all
     * objects that are associated to it.
     */
    element_proxy_t const operator[] (std::size_t i) const
      {
        return details::makeCollectionProxyElement<element_proxy_t>
          (i, getMainAt(i), aux<AuxColls>().operator[](i)...);
      }
    
    /// Returns an iterator to the first element of the collection.
    const_iterator begin() const { return makeIterator(0); }
    
    /// Returns an iterator past the last element of the collection.
    const_iterator end() const { return makeIterator(size()); }
    
    /// Returns whether this collection is empty.
    bool empty() const { return main().empty(); }
    
    /// Returns the size of this collection.
    std::size_t size() const { return main().size(); }
    
    
    /// Returns the associated data proxy specified by `AuxTag`.
    template <typename AuxTag>
    auto get() const -> decltype(auto) { return auxByTag<AuxTag>(); }
    
    
    /**
     * @brief Returns the auxiliary data specified by type (`Tag`).
     * @tparam Tag tag of the data to fetch (usually, its type)
     * @tparam T exact type returned by the method (by default a vector of tags)
     * @return the auxiliary data specified by type (`Tag`).
     * @throw std::logic_error if the tag is not available.
     * @see get(), has()
     * 
     * This method is a `get()` which forgives when the requested type is not
     * available (because this proxy was configured not to hold it).
     * 
     * The difference with `get()` is the following:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * if (tracks.has<recob::Hit>()) {
     *   auto hits = tracks.get<recob::Hit>();
     *   // ...
     * }
     * if (tracks.has<recob::SpacePoint>()) {
     *   auto spacepoints = tracks.getIf<recob::SpacePoint>();
     *   // ...
     * }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * If the proxy `tracks` has _not_ been coded with `recob::Hit` data,
     * the code snippet will _not_ compile, because `get()` will not compile
     * for tags that were not coded in. On the other end, if `recob::Hit`
     * is coded in `tracks` but `recob::SpacePoint` is not, the snippet _will_
     * compile. In that case, if the `has()` check had been omitted, `getIt()`
     * would throw a `std::logic_error` exception when executed.
     * With C++17, this will not be necessary any more by using "constexpr if":
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * if constexpr (tracks.has<recob::Hit>()) {
     *   auto hits = tracks.get<recob::Hit>();
     *   // ...
     * }
     * if constexpr (tracks.has<recob::SpacePoint>()) {
     *   auto spacepoints = tracks.get<recob::SpacePoint>();
     *   // ...
     * }
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * 
     * @note If the wrapped data product is something different than a vector
     *       of space points (which in the example is likely, if space points
     *       are associated to tracks), the almost-correct type of return value
     *       needs to be specified as second template parameter `T`.
     * 
     * @warning This functionality is not trivial to use!
     *          It's mostly meant for implementation of higher level wrappers.
     */
    template <typename Tag, typename T = std::vector<Tag> const&>
    auto getIf() const -> decltype(auto);
    
    
    /// Returns whether this class knowns about the specified type (`Tag`).
    template <typename Tag>
    static constexpr bool has()
      { return util::has_tag_v<Tag, aux_collections_t>; }
    
    
      protected:
    using main_collection_proxy_t::main;
    using main_collection_proxy_t::mainProxy;
    using main_collection_proxy_t::getMainAt;
    
    /// Returns the auxiliary data specified by type.
    template <typename AuxColl>
    AuxColl const& aux() const { return static_cast<AuxColl const&>(*this); }
    
    /// Returns the auxiliary data specified by type.
    template <typename AuxTag>
    auto auxByTag() const -> decltype(auto)
      { return aux<util::type_with_tag_t<AuxTag, aux_collections_t>>(); }
    
    
    template <typename Tag, typename>
    auto getIfHas(details::YesNoStruct<true>) const -> decltype(auto);
    template <typename Tag, typename T>
    [[noreturn]] auto getIfHas(details::YesNoStruct<false>) const -> T;
    
    /// Returns an iterator pointing to the specified index of this collection.
    const_iterator makeIterator(std::size_t i) const { return { *this, i }; }
    
    
    static_assert(!util::has_duplicate_tags<aux_collections_t>(),
      "Some auxiliary data collections share the same tag. They should not.");
    
  }; // struct CollectionProxyBase
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Base representation of a collection of proxied objects.
   * @tparam MainColl type of the collection of the main data product
   * @tparam AuxColls type of all included auxiliary data proxies
   * @see proxy::CollectionProxyElement
   * 
   * This object is a "specialization" of `proxy::CollectionProxyBase` using
   * `proxy::CollectionProxyElement` as element type.
   */
  template <typename MainColl, typename... AuxColls>
  using CollectionProxy
    = CollectionProxyBase<CollectionProxyElement, MainColl, AuxColls...>;
  
  
  //----------------------------------------------------------------------------
  /// The same as `withParallelData()`, but it also specified a tag.
  template <typename Aux, typename AuxTag, typename... Args>
  auto withParallelDataAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return details::WithParallelCollectionStruct<Aux, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withParallelDataAs()
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge an auxiliary data product into the proxy.
   * @tparam Aux type of parallel data product requested
   * @tparam Args types of constructor arguments for parallel data proxy
   * @param args constructor arguments for the parallel data collection proxy
   * @return a temporary object that `getCollection()` knows to handle
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for merging a collection proxy to carry auxiliary data structured as a
   * collection parallel to the main collection.
   * The function also bridges the information required to create a proxy to
   * that auxiliary data.
   * 
   * This data will be tagged with the type `Aux`. To use a different type as
   * tag, use `withParallelDataAs()` instead, specifying the tag as second
   * template argument, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct MCS {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withParallelData<recob::TrackMomentum>(defaultMomTag),
   *   withParallelDataAs<recob::TrackMomentum, MCS>(MCSmomTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first momentum association (`"defaultMomTag"`) will be accessed by
   * using the type `recob::TrackMomentum` as tag, while the second one will
   * be accessed by the `MCS` tag (which is better not be defined in a local
   * scope):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto&& track: tracks) {
   *   decltype(auto) trackMom = track.get<recob::TrackMomentum>();
   *   decltype(auto) trackMCSmom = track.get<MCS>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * The default implementation of parallel data proxy returns for each element
   * query an object with the same interface as the element of the parallel data
   * collection. In the previous examples, that would be a constant reference to
   * an object with `recob::TrackMomentum` interface.
   * 
   * 
   * Customization of the parallel data proxy
   * =========================================
   * 
   * To have a call like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = getCollection<SpecialTracks>
   *   (event, tag, withParallelData<recob::TrackMomentum>(momTag, "special"));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * to create something different than the standard parallel data proxy,
   * one needs to specialize `proxy::ParallelDataProxyMaker`, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace proxy {
   *   template <>
   *   struct ParallelDataProxyMaker<recob::Track, recob::TrackMomentum, SpecialTracks>
   *     : public ParallelDataProxyMakerBase<recob::Track, std::vector<recob::TrackMomentum>, recob::TrackMomentum>
   *   {
   *     
   *     template<typename Event, typename MainArgs>
   *     static auto make(
   *       Event const& event,
   *       MainArgs const&,
   *       art::InputTag assnTag,
   *       std::string quality = "default"
   *       )
   *       {
   *         ::SpecialTrackHitsProxy myAuxProxy;
   *         // ... make it, and make it right
   *         return myAuxProxy;
   *       }
   *     
   *   }; // struct ParallelDataProxyMaker<recob::Track, recob::TrackMomentum, SpecialTracks>
   *   
   * } // namespace proxy
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   */
  template <typename Aux, typename... Args>
  auto withParallelData(Args&&... args)
    { return withParallelDataAs<Aux, Aux>(std::forward<Args>(args)...); }
  
  
  //----------------------------------------------------------------------------
  /// Like `withParallelDataAs()`, but directly using the specified collection.
  template <typename AuxTag, typename AuxColl>
  auto wrapParallelDataAs(AuxColl const& auxColl) {
    std::tuple<AuxColl const&> args = { auxColl };
    return details::WithWrappedParallelCollectionStruct
      <collection_value_t<AuxColl>, decltype(args), AuxColl, AuxTag>
      (std::move(args));
  } // wrapParallelDataAs()
  /*
  /// Like `withParallelData()`, but directly using the specified collection.
  template <typename AuxTag, typename AuxColl>
  auto wrapParallelData(AuxColl const& auxColl)
    { return wrapParallelDataAs<AuxTag>(auxColl); }
  */
  /// Like `withParallelData()`, but directly using the specified collection.
  template <typename AuxColl>
  auto wrapParallelData(AuxColl const& auxColl)
    { return wrapParallelDataAs<collection_value_t<AuxColl>>(auxColl); }
  
  
  //----------------------------------------------------------------------------
  /// The same as `withZeroOrOne()`, but it also specified a tag for the data.
  template <typename Aux, typename AuxTag, typename... Args>
  auto withZeroOrOneAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return details::WithOneTo01AssociatedStruct<Aux, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withZeroOrOneAs()
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge one-to-(zero-or-one) associated data.
   * @tparam Aux type of associated data requested
   * @tparam Args types of constructor arguments for associated data collection
   * @param args constructor arguments for the associated data collection
   * @return a temporary object that `getCollection()` knows to handle
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for the delivered collection proxy to carry auxiliary data from an
   * association fulfilling the
   * @ref LArSoftProxyDefinitionOneToManySeqAssn "one-to-many sequential association"
   * requirements.
   * 
   * This data will be tagged with the type `Aux`. To use a different type as
   * tag, use `withZeroOrOneAs()` instead, specifying the tag as second
   * template argument, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct QuestionableVertex {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withZeroOrOne<recob::Vertex>(defaultVertexTag),
   *   withZeroOrOneAs<recob::Vertex, QuestionableVertex>(stinkyVertexTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first vertex association (`"defaultVertexTag"`) will be accessed by
   * using the type `recob::Vertex` as tag, while the second one will be
   * accessed by the `QuestionableVertex` tag (which is better not be defined
   * in a local scope):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto&& track: tracks) {
   *   decltype(auto) vertex = track.get<recob::Vertex>();
   *   decltype(auto) maybeVertex = track.get<QuestionableVertex>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Customization of the association proxy
   * =======================================
   * 
   * See the technical details about `withAssociated()`, which hold for this
   * function and related classes too.
   * 
   * 
   * Technical details
   * ==================
   * 
   * See the technical details about `withAssociated()`, which hold for this
   * function and related classes too.
   */
  template <typename Aux, typename... Args>
  auto withZeroOrOne(Args&&... args)
    { return withZeroOrOneAs<Aux, Aux>(std::forward<Args>(args)...); }
  
  
  //----------------------------------------------------------------------------
  /// The same as `withAssociated()`, but it also specified a tag for the data.
  template <typename Aux, typename AuxTag, typename... Args>
  auto withAssociatedAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return details::WithAssociatedStruct<Aux, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withAssociatedAs()
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge associated data.
   * @tparam Aux type of associated data requested
   * @tparam Args types of constructor arguments for associated data collection
   * @param args constructor arguments for the associated data collection
   * @return a temporary object that `getCollection()` knows to handle
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for the delivered collection proxy to carry auxiliary data.
   * The function also transfers the information required to create a proxy to
   * that auxiliary data.
   * 
   * This data will be tagged with the type `Aux`. To use a different type as
   * tag, use `withAssociatedAs()` instead, specifying the tag as second
   * template argument, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct DubiousClusters {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withAssociated<recob::Cluster>(defaultClusterTag),
   *   withAssociatedAs<recob::Cluster, DubiousClusters>(maybeClusterTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first cluster association (`"defaultClusterTag"`) will be accessed by
   * using the type `recob::Cluster` as tag, while the second one will be
   * accessed by the `DubiousClusters` tag (which is better not be defined in a
   * local scope):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * for (auto&& track: tracks) {
   *   decltype(auto) clusters = track.get<recob::Clusters>();
   *   decltype(auto) maybeClusters = track.get<DubiousClusters>();
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Customization of the association proxy
   * =======================================
   * 
   * To have a call like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = getCollection<SpecialTracks>
   *   (event, tag, withAssociated<recob::Hit>(hitAssnTag, "special"));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * create something different than the standard association proxy, specialize
   * `proxy::AssociatedDataProxyMaker`, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * namespace proxy {
   *   template <>
   *   struct AssociatedDataProxyMaker<recob::Track, recob::Hit, SpecialTracks>
   *     : public AssociatedDataProxyMakerBase<recob::Track, recob::Hit>
   *   {
   *     
   *     template<typename Event, typename MainArgs>
   *     static auto make(
   *       Event const& event,
   *       MainArgs const&,
   *       art::InputTag assnTag,
   *       std::string quality
   *       )
   *       {
   *         ::SpecialTrackHitsProxy myAuxProxy;
   *         // ... make it, and make it right
   *         return myAuxProxy;
   *       }
   *     
   *   }; // struct AssociatedDataProxyMaker<recob::Track, recob::Hit>
   *   
   * } // namespace proxy
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * 
   * Technical details
   * ==================
   * 
   * The main purpose of this function and the related `WithAssociatedData`
   * class is to save the user from specifying the main type the auxiliary data
   * is associated with, when using it as `getCollection()` argument:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = getCollection<proxy::Tracks>
   *   (event, tag, withAssociated<recob::Hit>(hitAssnTag));
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * While parsing the `withAssociated()` argument (or any argument), the
   * information of which is the proxy collection type (`proxy::Tracks` in the
   * example) is not known. In principle, to fully define the association, two
   * template arguments are needed, e.g.
   * `withAssociated<recob::Track, recob::Hit>(hitAssnTag)`.
   * The class `WithAssociatedData` holds the information of which associated
   * type is requested (`recob::Hit`) and the information needed to create a
   * proxy to such association (all arguments, here just `hitAssnTag`).
   * The function `getCollection()` will have this object as argument, and when
   * executing will be able to supply the missing information, that
   * `recob::Track` is the main data product element we are associating to.
   */
  template <typename Aux, typename... Args>
  auto withAssociated(Args&&... args)
    { return withAssociatedAs<Aux, Aux>(std::forward<Args>(args)...); }
  
  
  //----------------------------------------------------------------------------
  /// Like `withAssociatedAs()`, but directly using the specified association.
  template <typename AuxTag, typename Assns>
  auto wrapAssociatedAs(Assns const& assns) {
    using Aux_t = typename Assns::right_t;
    return details::WithAssociatedStruct<Aux_t, std::tuple<>, AuxTag>({});
  } // wrapAssociatedAs()
  
  
  /// Like `withAssociated()`, but directly using the specified association.
  template <typename AuxTag, typename Assns>
  auto wrapAssociated(Assns const& assns)
    { return wrapAssociatedAs<AuxTag>(assns); }
  
  /// Like `withAssociated()`, but directly using the specified association.
  template <typename Assns>
  auto wrapAssociated(Assns const& assns)
    { return wrapAssociatedAs<typename Assns::right_t>(assns); }
  
  
  //----------------------------------------------------------------------------
  /// The same as `withCollectionProxy()`, but it also specified a tag.
  /// @bug Broken in many ways. Do not use.
  template <typename AuxProxy, typename AuxTag, typename... Args>
  auto withCollectionProxyAs(Args&&... args) {
    using ArgTuple_t = std::tuple<Args&&...>;
    static_assert(
      std::is_convertible
        <std::decay_t<std::tuple_element_t<0U, ArgTuple_t>>, art::InputTag>(),
      "The first argument of withCollectionProxyAs() must be art::InputTag."
      );
    ArgTuple_t argsTuple(std::forward<Args>(args)...);
    return details::WithProxyAsAuxStructBase<AuxProxy, ArgTuple_t, AuxTag>
      (std::move(argsTuple));
  } // withCollectionProxyAs()
  
  //----------------------------------------------------------------------------
  /**
   * @brief Helper function to merge an auxiliary proxy into the proxy.
   * @tparam AuxProxy type (proxy tag) of auxiliary collection proxy requested
   * @tparam Args types of constructor arguments for parallel data proxy
   * @param args constructor arguments for the parallel data collection proxy
   * @return a temporary object that `getCollection()` knows to handle
   * @bug Broken in many ways. Do not use.
   * 
   * This function is meant to convey to `getCollection()` function the request
   * for merging a collection proxy to carry auxiliary data structured as
   * another collection proxy, parallel to the main collection.
   * The function also bridges the information required to create a proxy to
   * that auxiliary data.
   * 
   * This data will be tagged with the type `AuxProxy`. To use a different type
   * as tag, use `withCollectionProxyAs()` instead, specifying the tag as second
   * template argument.
   * 
   * 
   * Customization of the auxiliary collection proxy
   * ================================================
   * 
   * The customization of auxiliary collection proxy happens in a fashion
   * similar to the customization presented in `withParallelData()`.
   * The customization point here is `ProxyAsAuxProxyMaker`.
   */
  template <typename AuxProxy, typename... Args>
  auto withCollectionProxy(Args&&... args)
    {
      return
        withCollectionProxyAs<AuxProxy, AuxProxy>(std::forward<Args>(args)...);
    }
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Collection of data type definitions for collection proxies.
   * @tparam Proxy type of proxy the traits refer to
   * @tparam Selector specialization helper type
   * 
   * Expected traits:
   * * `main_collection_t`: type of the main data product collection
   * * `main_element_t`: type contained in the main data product collection
   * * `main_collection_proxy_t`: type wrapping the main data product collection
   * 
   * Note that the `Proxy` type is expected to be the same type as used in
   * `getCollection()` calls, and does not need to match the actual type of a
   * proxy collection.
   */
  template <typename Proxy, typename Selector = Proxy>
  struct CollectionProxyMakerTraits {
    static_assert
      (util::always_true_type<Proxy>(), "This class requires specialization.");
  };
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Class to assemble the required proxy.
   * @tparam CollProxy a type characterizing the produced proxy
   * 
   * This is a base class suitable for deriving `CollectionProxyMaker`,
   * which is used by `getCollection()` to create the requested proxy.
   * Deriving a specialization of `CollectionProxyMaker` from
   * `CollectionProxyMakerBase` provides some basic definitions and
   * functionality that might be tedious to copy.
   * 
   * In this, which can be considered as the default implementation of
   * `CollectionProxyMaker`, the template argument `CollProxy` is expected to
   * have the interface of `CollectionProxy`.
   * The arguments required are documented in the implementation of the `make()`
   * function. Note that the type of proxy returned does not need to be
   * `CollProxy`, but is in fact an instance of `proxy::CollectionProxy`.
   * 
   */
  template <typename CollProxy>
  struct CollectionProxyMakerBase {
    
    /// Traits of the collection proxy for the collection proxy maker.
    using traits_t = CollectionProxyMakerTraits<CollProxy>;
    
    /// Type of main collection proxy.
    using main_collection_proxy_t = typename traits_t::main_collection_proxy_t;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = typename traits_t::main_element_t;
    
    /// Type of element of the main collection.
    using main_collection_t = typename traits_t::main_collection_t;
    
    /**
     * @brief Creates and returns a collection proxy based on `CollProxy` and
     *        with the requested associated data.
     * @tparam Event type of the event to read the information from
     * @tparam WithArgs type of arguments for associated data
     * @param event event to read the information from
     * @param tag input tag of the main data product
     * @param withArgs optional associated objects to be included
     * @return a collection proxy from data as retrieved via `tag`
     * 
     * For each argument in `withArgs`, an action is taken. Usually that is to
     * add an association to the proxy.
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
        return makeCollectionProxy(
          *mainHandle,
          withArgs.template createAuxProxyMaker<main_collection_proxy_t>
            (event, mainHandle, tag)...
          );
      } // make()
    
      private:
    // helper function to avoid typing the exact types of auxiliary collections
    template <typename MainColl, typename... AuxColl>
    static auto makeCollectionProxy(MainColl const& main, AuxColl&&... aux)
      {
        return CollectionProxy<MainColl, AuxColl...>
          (main, std::forward<AuxColl>(aux)...);
      }
    
  }; // struct CollectionProxyMakerBase<>
  
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Class to assemble the required proxy.
   * @tparam CollProxy a type characterizing the produced proxy
   * 
   * This class is used by `getCollection()` to create the requested proxy.
   * The required interface for this class is:
   * * `make()`: a static method returning the collection proxy, matching the
   *   signature
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, typename... Args>
   *   auto make(Event const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *   where the first argument will be the event to read the information from.
   * 
   * A default implementation is provided as `proxy::CollectionProxyMakerBase`.
   * Specializations of `proxy::CollectionProxyMaker` may choose to derive from
   * `proxy::CollectionProxyMakerBase` as well, for convenience.
   */
  template <typename CollProxy>
  struct CollectionProxyMaker: CollectionProxyMakerBase<CollProxy> {};
  
  
  
  /**
   * @brief Creates a proxy to a data product collection.
   * @tparam CollProxy type of target main collection proxy
   * @tparam Event type of event to read data from
   * @tparam OptionalArgs type of optional arguments
   * @param event event to read data from
   * @param optionalArgs optional arguments for construction of the proxy
   * @return a collection proxy object
   * 
   * This function delivers a collection proxy related to `CollProxy`.
   * 
   * The @ref LArSoftProxyQuirks "type of proxy delivered is arbitrary"
   * and usually not `CollProxy`.
   * The type of the collection proxy must be explicitly specified, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<proxy::Tracks>
   *   (event, tag, withAssociated<recob::Hits>());
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this case, two optional arguments are passed: the input tag to the main
   * collection, and then `withAssociated<recob::Hits>()`. The meaning of both
   * is decided depending on the collection proxy being created, but it's common
   * to have the first argument be the input tag to the main collection, as in
   * the example.
   * 
   * The collection proxy name is arbitrary, but it's custom to have it live in
   * `proxy` namespace and have the same name as the base object, made plural:
   * a proxy to a `recob::Track` collection data product will have a proxy
   * called `proxy::Tracks`.
   * 
   * Note that a proxy need to be explicitly supported in order to be available.
   * 
   * In practice, this function does very little apart from invoking the proper
   * `CollectionProxyMaker` class. Each proxy has its own, and there is where
   * the meaning of the optional arguments is assigned.
   * 
   * 
   * Customization
   * ==============
   * 
   * To control which type of collection proxy is produced for the type
   * `CollProxy`, the class `CollectionProxyMaker` may be specialised.
   * 
   */
  template <typename CollProxy, typename Event, typename... OptionalArgs>
  auto getCollection(Event const& event, OptionalArgs&&... optionalArgs)
  {
    return CollectionProxyMaker<CollProxy>::make
      (event, std::forward<OptionalArgs>(optionalArgs)...);
  }
  
  
  /// @}
  // end group Data product proxy infrastructure
  
  
  //----------------------------------------------------------------------------
  //--- specializations of CollectionProxyMakerTraits
  //----------------------------------------------------------------------------
  template <typename T>
  struct CollectionProxyMakerTraits<std::vector<T>> {
    
    /// Type of element of the main collection.
    using main_collection_t = std::vector<T>;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = collection_value_t<main_collection_t>;
    
    /// Type of main collection proxy.
    using main_collection_proxy_t
      = details::MainCollectionProxy<main_collection_t>;
    
  }; // CollectionProxyMakerTraits<CollectionProxy>
  
  
  template <typename MainColl>
  struct CollectionProxyMakerTraits<CollectionProxy<MainColl>> {
    /// Type of main collection proxy.
    using main_collection_proxy_t = CollectionProxy<MainColl>;
    
    /// Type returned by the main collection indexing operator.
    using main_element_t = typename main_collection_proxy_t::main_element_t;
    
    /// Type of element of the main collection.
    using main_collection_t
      = typename main_collection_proxy_t::main_collection_t;
  }; // CollectionProxyMakerTraits<CollectionProxy>
  
  
  //----------------------------------------------------------------------------
  namespace details {
    
    //--------------------------------------------------------------------------
    //---  general infrastructure
    //--------------------------------------------------------------------------
    /// Constant expressing whether `Iter` type implements random access.
    template <typename Iter>
    constexpr bool is_random_access_iterator_v = std::is_same<
      typename std::iterator_traits<Iter>::iterator_category,
      std::random_access_iterator_tag
      >::value;
    
    
    //--------------------------------------------------------------------------
    /**
     * @brief Iterator to random access collection storing a current index.
     * @tparam Cont type of random-access container to iterate
     * 
     * `Cont` is a type providing a public `operator[](std::size_t)` method.
     */
    template <typename Cont>
    class IndexBasedIterator {
      
        public:
      using container_t = Cont;
      
      using value_type = collection_value_t<container_t>;
      using const_iterator = IndexBasedIterator;
      
      /// Default constructor (required by iterator protocol): an unusable iterator.
      IndexBasedIterator() = default;
      
      /// Constructor: initializes from an iterator of the proxy main collection.
      IndexBasedIterator(container_t const& cont, std::size_t index = 0)
        : fCont(&cont), fIndex(index) {}
      
      /// Returns the value pointed by this iterator.
      auto operator* () const -> decltype(auto)
        { return fCont->operator[](fIndex); }
      
      /// Returns the value pointed by this iterator.
      const_iterator& operator++ () { ++fIndex; return *this; }
      
      /// Returns whether the iterators point to the same element.
      bool operator!= (const_iterator const& other) const
        { return (other.fIndex != fIndex) || (other.fCont != fCont); }
      
        protected:
      container_t const* fCont = nullptr; ///< Pointer to the original container.
      
      /// Current index in the main collection.
      std::size_t fIndex = std::numeric_limits<std::size_t>::max();
      
    }; // IndexBasedIterator<>
    
    
    /**
     * @brief Simple iterator wrapper for manipulation of dereferenced result.
     * @tparam Iter final iterator
     * @tparam BaseIter base iterator
     * @tparam ValueType type returned by the new dereference operator
     * 
     * This class is designed to be used as base class for an iterator that
     * redefines the dereference operations without changing anything else.
     * An example of such iterator is:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * using int_iterator_t = std::vector<int>::const_iterator;
     * 
     * class twice_iterator
     *   : public IteratorWrapperBase<twice_iterator, int_iterator_t, float>
     * {
     *   using base_iterator_t
     *     = IteratorWrapperBase<twice_iterator, int_iterator_t, float>;
     *   
     *     public:
     *   
     *   using base_iterator_t::base_iterator_t; // inherit constructors
     *   
     *   float operator*() const -> decltype(auto)
     *     { return 2.0F * base_iterator_t::operator*(); }
     *   
     * }; // twice_iterator
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * This new iterator `twice_iterator` effectively inherits `int_iterator_t`
     * constructors (via `base_iterator_t`). Its `operator++()` returns a
     * `twice_iterator`, so that operations like `*++it` are correctly handled.
     * 
     * Note that the derived classes need to redefine all the dereferencing
     * operators that are supported:
     * 
     * * operator*()
     * 
     * The `operator->()` is not supported. Typically, `operator*()` returns a
     * reference and `operator->()` a pointer to an existing structure.
     * The `operator*()` is not required to return a reference when the iterator
     * does not allow to modify the pointed data, and the wrapper can manipulate
     * values and return temporary results. To have a similar feature for
     * `operator->()` requires more work, as it should return a smart pointer
     * that would perform the transformation on dereference.
     * 
     */
    template <
      typename Iter,
      typename DataIter,
      typename ValueType = typename DataIter::value_type
      >
    class IteratorWrapperBase: private DataIter {
      using data_iterator_t = DataIter;
      
        public:
      using iterator = Iter; ///!< The type of base iterator wrapper.
      
      /// @{
      /// @name Iterator traits
      using typename data_iterator_t::difference_type;
      using value_type = ValueType;
      using pointer = std::add_pointer_t<value_type>;
      using reference = std::add_lvalue_reference_t<value_type>;
      using iterator_category = std::forward_iterator_tag;
      /// @}
      
      /// Default constructor: default-constructs the underlying iterator.
      IteratorWrapperBase() = default;
      
      /// Copy-from-base constructor.
      IteratorWrapperBase(data_iterator_t const& from): data_iterator_t(from) {}
      
      /// Prefix increment operator.
      iterator& operator++()
        { data_iterator_t::operator++(); return asIterator(); }
      
      /// Comparison with a data iterator (makes unnecessary to wrap end iterators).
      bool operator!=(data_iterator_t const& other) const
        { return other != asDataIterator(); }
      
      /// Comparison with another iterator.
      bool operator!=(iterator const& other) const
        { return operator!=(other.asDataIterator()); }
      
      auto operator[](std::size_t index) const -> decltype(auto)
        { return asIterator().transform(asDataIterator() + index); }
      
      /// Dereference operator; need to be redefined by derived classes.
      auto operator*() const -> decltype(auto)
        { return asIterator().transform(asDataIterator()); }
      
      /// Dereference operator; need to be redefined by derived classes.
      auto operator->() const -> decltype(auto)
        { return makeValuePointer(operator*()); }
      
      
        protected:
      /// Transforms and returns the value at the specified data iterator.
      static auto transform(data_iterator_t const&) -> decltype(auto)
        { return data_iterator_t::operator*(); }
      
      data_iterator_t const& asDataIterator() const
        { return static_cast<data_iterator_t const&>(*this); }
      
        private:
      /// Value box for use with pointer dereference `operator->()`.
      template <typename Value>
      class ValuePtr {
        Value value; ///< Value to return the address of (may be reference).
          public:
        ValuePtr(Value const& value): value(value) {}
        /// Access the contained value via its pointer.
        auto operator->() const -> decltype(auto)
          { return std::addressof(value); }
      }; // class ValuePtr<>
      template <typename Value>
      static ValuePtr<Value> makeValuePointer(Value&& value)
        { return { std::forward<Value>(value) }; }
      
      iterator const& asIterator() const
        { return static_cast<iterator const&>(*this); }
      iterator& asIterator() { return static_cast<iterator&>(*this); }
      
    }; // IteratorWrapperBase<>
    
    
    /// Modified iterator returning the `N`-th element out of the pointed tuple.
    template <std::size_t N, typename TupleIter>
    class tuple_element_iterator:
      public IteratorWrapperBase<
        tuple_element_iterator<N, TupleIter>,
        TupleIter,
        std::tuple_element_t<N, typename TupleIter::value_type>
        >
    {
      using base_iterator_t = IteratorWrapperBase<
        tuple_element_iterator<N, TupleIter>,
        TupleIter,
        std::tuple_element_t<N, typename TupleIter::value_type>
        >;
      
        public:
      using base_iterator_t::base_iterator_t;
      
      /// Constructor from a base iterator (explicitly allowed).
      tuple_element_iterator(base_iterator_t const& from)
        : base_iterator_t(from) {}
      
      static auto transform(TupleIter const& v) -> decltype(auto)
        {return std::get<N>(*v); }
      
    }; // tuple_element_iterator
    
    
    //--------------------------------------------------------------------------
    //--- stuff for the main collection
    //--------------------------------------------------------------------------
    /**
     * @brief Wrapper for the main collection of a proxy.
     * @tparam MainColl type of the collection being wrapped
     * 
     * The wrapper contains a pointer to the original collection, which must
     * persist. The original collection is not modified.
     * 
     * The `MainColl` type must expose a random access container interface.
     */
    template <typename MainColl>
    struct MainCollectionProxy {
      
      /// Type of the original collection.
      using main_collection_t = MainColl;
      
      /// Type of the elements in the original collection.
      using main_element_t = collection_value_t<MainColl>;
      
      /// Constructor: wraps the specified collection.
      MainCollectionProxy(main_collection_t const& main): fMain(&main) {}
      
      /// Returns the wrapped collection.
      main_collection_t const& main() const { return mainRef(); }
      
      /// Returns a reference to the wrapped collection.
      main_collection_t const& mainRef() const { return *fMain; }
      
      /// Returns a pointer to the wrapped collection.
      main_collection_t const* mainPtr() const { return fMain; }
      
      
        protected:
      /// This type.
      using this_t = MainCollectionProxy<main_collection_t>;
      
      /// Return this object as main collection proxy.
      this_t& mainProxy() { return *this; }
      
      /// Return this object as main collection proxy.
      this_t const& mainProxy() const { return *this; }
      
      /// Returns the specified item in the original collection.
      auto getMainAt(std::size_t i) const -> decltype(auto)
        { return fMain->operator[](i); }
      
        private:
      main_collection_t const* fMain; /// Pointer to the original collection.
      
    }; // struct MainCollectionProxy
    
    
    
    //--------------------------------------------------------------------------
    //--- stuff for auxiliary data
    //--------------------------------------------------------------------------
    // Trait replacing each element of the specified tuple with its
    // `auxiliary_data_t`
    template <typename Tuple>
    struct SubstituteWithAuxList {
      static_assert
        (util::always_true_type<Tuple>(), "Template argument must be a tuple");
    }; // SubstituteWithAuxList<>
    
    template <typename... T>
    struct SubstituteWithAuxList<std::tuple<T...>> {
      using type = std::tuple<typename T::auxiliary_data_t...>;
    }; // SubstituteWithAuxList<tuple>
    
    
    //--------------------------------------------------------------------------
    /**
     * @brief Helper to create associated data proxy.
     * @tparam Aux type of data associated to the main one
     * @tparam ArgTuple type of arguments required for the creation of proxy
     * @tparam ProxyMaker template type of the proxy maker class
     * @tparam AuxTag tag for the associated data (default: as `Aux`)
     * 
     * This class stores user arguments for the construction of a proxy to
     * associated data of type `Aux`.
     * It can use that information plus some additional one to create the
     * associated data itself. This additional information is provided by
     * `getCollection()`.
     * 
     * The association will be identified by type `AuxTag`.
     * 
     * This is not a customization point: to have a custom associated data
     * produced, choose and then specialize the `ProxyMaker` class.
     */
    template <
      typename Aux,
      typename ArgTuple,
      template <typename CollProxy> class ProxyMaker,
      typename AuxTag /* = Aux */
      >
    class WithAssociatedStructBase {
      
      /// Type of main data product element from a proxy of type `CollProxy`.
      template <typename CollProxy>
      using main_t = typename CollProxy::main_element_t;
      
      /// Type of associated data.
      using aux_t = Aux;
      
      /// Tag for the associated data (same as the data type itself).
      using tag = AuxTag;
      
      /// Class to create the data proxy associated to a `CollProxy`.
      template <typename CollProxy>
      using proxy_maker_t = ProxyMaker<CollProxy>;
      
        public:
      
      /// Type of association proxy created for the specified `CollProxy`.
      template <typename CollProxy>
      using aux_collection_proxy_t
        = typename proxy_maker_t<CollProxy>::aux_collection_proxy_t;
      
      /// Constructor: steals the arguments, to be used by
      /// `createAuxProxyMaker()`.
      WithAssociatedStructBase(ArgTuple&& args): args(std::move(args)) {}
      
      /// Creates the associated data proxy by means of `ProxyMaker`.
      template
        <typename CollProxy, typename Event, typename Handle, typename MainArgs>
      auto createAuxProxyMaker
        (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
        { 
          return createAssnProxyMaker<CollProxy>(
            event, std::forward<Handle>(mainHandle), mainArgs,
            std::make_index_sequence<NArgs>()
            );
        } // construct()
      
      
        protected:
      
      ArgTuple args; ///< Argument construction storage as tuple.
      
      /// Number of arguments stored.
      static constexpr std::size_t NArgs = std::tuple_size<ArgTuple>();
      
      // this method allows unpacking the arguments from the tuple
      template<
        typename CollProxy, typename Event, typename Handle, typename MainArgs,
        std::size_t... I
        >
      auto createAssnProxyMaker(
        Event const& event, Handle&& mainHandle, MainArgs const& mainArgs,
        std::index_sequence<I...>
        )
        {
          return proxy_maker_t<CollProxy>::make(
            event, mainHandle, mainArgs,
            std::get<I>(std::forward<ArgTuple>(args))...
            );
        }
      
    }; // struct WithAssociatedStructBase
    
    
    
    //--------------------------------------------------------------------------
    //--- stuff for parallel data collection (a form of auxiliary data)
    //--------------------------------------------------------------------------
    template <typename Aux, typename AuxTag, typename AuxColl /* = void */>
    struct ParallelDataProxyMakerWrapper {
      template <typename CollProxy>
      using maker_t = ParallelDataProxyMaker
        <typename CollProxy::main_element_t, Aux, CollProxy, AuxTag, AuxColl>;
    }; // struct ParallelDataProxyMakerWrapper<Aux, AuxColl>
    
    template <typename Aux, typename AuxTag>
    struct ParallelDataProxyMakerWrapper<Aux, AuxTag, void> {
      template <typename CollProxy>
      using maker_t = ParallelDataProxyMaker
        <typename CollProxy::main_element_t, Aux, CollProxy, AuxTag>;
    }; // struct ParallelDataProxyMakerWrapper<Aux>
    
    
    
    /**
     * @brief Object to draft parallel data interface.
     * @tparam AuxColl type of the parallel data collection
     * @tparam Aux type of the associated object
     * @tparam Tag tag this data is labeled with
     * 
     * Allows:
     *  * random access (no index check guarantee)
     *  * forward iteration
     * 
     * Construction is not part of the interface.
     */
    template <
      typename AuxColl,
      typename Aux /* = collection_value_t<AuxColl> */,
      typename Tag /* = Aux */
      >
    class ParallelData {
      using This_t = ParallelData<AuxColl, Aux, Tag>; ///< This type.
      
      /// Type of auxiliary collection.
      using parallel_data_t = AuxColl;
      
      /// Type of the value of auxiliary collection element.
      using aux_t = Aux; // unused
      
      /// Type returned when accessing an auxiliary collection element.
      using aux_element_t = collection_value_constant_access_t<AuxColl>;
      
      using parallel_data_iterator_t = typename parallel_data_t::const_iterator;
      
        public:
      using tag = Tag; ///< Tag of this association proxy.
      
      /// Type returned when accessing auxiliary data.
      using auxiliary_data_t
        = decltype(util::makeTagged<tag>(std::declval<aux_element_t>()));
      
      /// Constructor: points to the specified data collection.
      ParallelData(parallel_data_t const& data)
        : fData(&data)
        {}
      
      /// Returns an iterator pointing to the first data element.
      auto begin() const -> decltype(auto)
        { return fData->begin(); }
      
      /// Returns an iterator pointing past the last data element.
      auto end() const -> decltype(auto)
        { return fData->end(); }
      
      /// Returns the element with the specified index (no check performed).
      auto operator[] (std::size_t index) const -> decltype(auto)
        {
          static_assert(
            std::is_convertible<decltype(getElement(index)), auxiliary_data_t>(),
            "Inconsistent data types."
            );
          return getElement(index);
        }
      
      /// Returns whether this data is labeled with the specified tag.
      template <typename TestTag>
      static constexpr bool hasTag() { return std::is_same<TestTag, tag>(); }
      
      /// Returns a pointer to the whole data collection.
      parallel_data_t const* data() const { return fData; }
      
        private:
      
      parallel_data_t const* fData; ///< Reference to the original data product.
      
      auto getElement(std::size_t index) const -> decltype(auto)
        { return util::makeTagged<tag>(fData->operator[](index)); }
      
    }; // class ParallelData<>
    
    
    
    
    //--------------------------------------------------------------------------
    //--- stuff for associated data (a form of auxiliary data)
    //--------------------------------------------------------------------------
    template <typename Aux, typename AuxTag /* = Aux */>
    struct AssociatedDataProxyMakerWrapper {
      template <typename CollProxy>
      using maker_t = AssociatedDataProxyMaker
        <typename CollProxy::main_element_t, Aux, CollProxy, AuxTag>;
    };
    
    
    /**
     * @class WithAssociatedStruct
     * @brief Helper to create associated data proxy.
     * @tparam Aux type of data associated to the main one
     * @tparam ArgTuple type of arguments required for the creation of proxy
     * @tparam AuxTag tag for the associated data (default: as `Aux`)
     * 
     * This class stores user arguments for the construction of a proxy to
     * associated data of type `Aux`.
     * It can use that information plus some additional one to create the
     * associated data itself. This additional information is provided by
     * `getCollection()`.
     * 
     * The association will be identified by type `AuxTag`.
     * 
     * This is not a customization point: to have a custom associated data
     * produced, specialize `proxy::AssociatedDataProxyMaker` class
     */
    // the class is actually an alias whose definition was above
    
    
    /// Interface providing begin and end iterator of a range.
    /// @tparam BoundaryIter iterator to the first of the range iterators.
    template <typename BoundaryIter>
    class BoundaryListRangeBase: private BoundaryIter {
      using boundary_iterator_t = BoundaryIter;
      
      /// Returns the iterator to the begin iterator.
      auto boundaryIter() const
        { return static_cast<BoundaryIter const&>(*this); }
      
        public:
      /// Constructor: copies the specified base iterator.
      BoundaryListRangeBase(boundary_iterator_t const& it)
        : boundary_iterator_t(it) {}
      
      /// Returns the begin iterator of the range.
      auto begin() const -> decltype(auto)
        { return *(boundaryIter()); }
      
      /// Returns the end iterator of the range (next to the begin iterator).
      auto end() const -> decltype(auto)
        { return *(std::next(boundaryIter())); }
      
    }; // BoundaryListRangeBase<>
    
    
    /// A `BoundaryListRangeBase` with a full container interface.
    template <typename BoundaryIter>
    class BoundaryListRange
      : public lar::CollectionView<BoundaryListRangeBase<BoundaryIter>>
    {
      // A CollectionView can't be constructed except from deriver classes;
      // we define here such a class.
      
      using rangebase_t = BoundaryListRangeBase<BoundaryIter>;
      using base_t = lar::CollectionView<rangebase_t>;
        public:
      using boundary_iterator_t = BoundaryIter;
      
      /// Constructor: from an iterator to the begin iterator.
      BoundaryListRange(boundary_iterator_t const& iBegin)
        : base_t(rangebase_t(iBegin))
        {}
      
    }; // class BoundaryListRange<>
    
    
    /**
     * @brief Reinterprets a iterator to boundaries list as a range collection.
     * @tparam BoundaryIter type of iterator to boundary collection
     * @param iBegin iterator to the begin iterator of a range
     * @return a collection view (`lar::CollectionView`) of the range
     * 
     * A range is conceptually defined as a sequence of data between a begin and
     * and end iterator.
     * The argument of this function is an iterator to the begin iterator of the
     * range. The begin iterator itself is obtained by dereferencing the
     * argument: `*iBegin`. The end iterator of the range is required to be
     * immediately after the begin iterator (@code *std::next(iBegin) @endcode).
     * This pair of iterators is exposed via the `lar::CollectionView` view,
     * that presents a vector-like interface.
     * For this to fully work, the data iterators (e.g., `*iBegin`) must comply
     * with the random-access iterator requirements.
     * 
     * An example of `BoundaryIter` is the iterator to the list of boundaries
     * in `BoundaryList`: `BoundaryList::boundaries_t::const_iterator`.
     */
    template <typename BoundaryIter>
    BoundaryListRange<BoundaryIter> makeBoundaryListRange
      (BoundaryIter const& iBegin)
      { return { iBegin }; }
    
    
    /**
     * @brief Iterator exposing elements of a boundary list as ranges.
     * @tparam BoundaryIter type of iterator to boundary iterators
     * 
     * This iterator wraps an iterator (`BoundaryIter`) to a sequence of
     * iterators representing ranges of data.
     * As an example, let the data be a collection of real numbers:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * std::vector<float> data;
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * and we have a list of iterators that define subranges within that data:
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     * using data_iterator_t = std::vector<data>::const_iterator;
     * std::vector<data_iterator_t> rangeStart;
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * Here `rangeStart` holds the begin iterator of each of the subranges,
     * plus a "global" end iterator, as in the convention described in
     * `lar::makeBoundaryListRange()` (which takes an iterator to `rangeStart`
     * as argument).
     * The `BoundaryIter` type in this example would be
     * @code std::vector<data_iterator_t>::const_iterator @endcode.
     * 
     * When dereferenced, this iterator returns a view of the range currently
     * pointed. This view has a STL-vector-like interface (again, see
     * `lar::makeBoundaryListRange()`).
     */
    template <typename BoundaryIter>
    class BoundaryListRangeIterator
      : public IteratorWrapperBase
        <BoundaryListRangeIterator<BoundaryIter>, BoundaryIter>
    {
      using base_t = IteratorWrapperBase
        <BoundaryListRangeIterator<BoundaryIter>, BoundaryIter>;
      
        public:
      using boundary_iterator_t = BoundaryIter; ///< Type of boundary iterator.
      
      /// Type of range returned when dereferencing.
      using rangeview_t
        = decltype(makeBoundaryListRange(std::declval<boundary_iterator_t>()));
      
      using value_type = rangeview_t;
      using pointer = std::add_pointer_t<std::decay_t<value_type>>;
      using reference = std::add_lvalue_reference_t<std::decay_t<value_type>>;
      
      
      using base_t::base_t; // import constructors (explicitly allowed)
      
      /**
       * @brief Returns the pointed range.
       * @return the pointed range, as a view
       * 
       * The returned value may be a view object, that is a pure interface
       * overlaid on a different data structure.
       * As such, it may be not copiable and need to be propagated by reference.
       */
      static auto transform(BoundaryIter const& iter)
        { return makeBoundaryListRange(iter); }
      
    }; // class BoundaryListRangeIterator<>
    
    
    /**
     * @brief Builds and keeps track of internal boundaries in a sequence.
     * @tparam Iter type of iterators to the original sequence
     * 
     * This class manages a sequence of boundary iterators defining the
     * beginning of contiguous subsequences. Each iterator marks the begin of a
     * subsequence, whose end is marked by the beginning of the next one.
     * The last iterator in the boundary list marks the end of the last
     * subsequence, but it does not mark the beginning of a following one.
     * Therefore, for a list of _N_ subsequences there will be _N + 1_ boundary
     * iterators in the list: _N_ marking the beginning of the respective
     * subsequences, plus another marking the end of the last subsequence.
     * 
     * It is likely that the first iterator in the boundary list is the begin
     * iterator of the underlying sequence (usually a container) being
     * partitioned, while the last one is the end iterator of that sequence.
     * 
     * This is a data class which does not contain any logic to define the
     * subsequences, but rather acquires the result of an algorithm which is
     * expected to have established which the boundaries are.
     * 
     * The underlying representation of the class is a random-access sequence
     * of boundaries. The exposed value, `range_t`, is a range of data elements
     * (a view with the interface of a random access container), which is
     * internally represented as a single iterator pointing to the begin
     * iterator of the range.
     */
    template <typename Iter>
    class BoundaryList {
      using boundarylist_t = BoundaryList<Iter>;
        public:
      using data_iterator_t = Iter;
      using boundaries_t = std::vector<data_iterator_t>;
      
      /// Iterator on the ranges contained in the collection.
      using range_iterator_t
        = BoundaryListRangeIterator<typename boundaries_t::const_iterator>;
      
      /// Structure holding begin and end iterator for a single range.
      // BoundaryListRange<data_iterator_t> const&
      using range_ref_t = typename range_iterator_t::value_type;
      
      /// Range object directly containing the boundary iterators.
      using range_t = lar::RangeAsCollection_t<data_iterator_t>;
      
      
      /// Constructor: steals the specified boundary list.
      explicit BoundaryList(boundaries_t&& boundaries)
        : boundaries(std::move(boundaries))
        { assert(this->boundaries.size() >= 1); }
      
      /// Returns the number of ranges contained in the list.
      std::size_t nRanges() const
        { return boundaries.size() - 1; }
      /// Returns the begin iterator of the `i`-th range (end if overflow).
      data_iterator_t const& rangeBegin(std::size_t i) const
        { return boundaries[std::min(i, nRanges())]; }
      /// Returns the end iterator of the `i`-th range (end if overflow).
      data_iterator_t const& rangeEnd(std::size_t i) const
        { return rangeBegin(i + 1); }
      
      /// Returns the number of ranges contained in the list.
      std::size_t size() const { return nRanges(); }
      /// Returns the begin iterator of the first range.
      range_iterator_t begin() const
        { return { boundaries.begin() }; }
      /// Returns the end iterator of the last range.
      range_iterator_t end() const
        { return { std::prev(boundaries.end()) }; }
      /**
       * @brief Returns the specified range.
       * @param i index of the range to be returned
       * @return a proxy object with container interface
       * @see `range()`
       * 
       * The returned object exposes the range as a random access container
       * interface.
       * 
       * Internally, it refers to the relevant information from this
       * `BoundaryList` object, and therefore it becomes invalid when this 
       * `BoundaryList` object is destroyed.
       * If this is not acceptable, use `range()` instead.
       */
      range_ref_t rangeRef(std::size_t i) const
        { return { std::next(boundaries.begin(), i) }; }
      /**
       * @brief Returns the specified range in an object holding the iterators.
       * @param i index of the range to be returned
       * @return a new object with container interface
       * @see `rangeRef()`
       * 
       * The returned object contains copies of the begin and end iterators of
       * the range. This object is self-contained and valid even after this
       * BoundaryList object is destroyed.
       * 
       * Note the content of the range itself is _not_ copied: just the boundary
       * iterators of the range are.
       */
      range_t range(std::size_t i) const
        { return lar::makeCollectionView(rangeBegin(i), rangeEnd(i)); }
      
      /// Returns the begin iterator of the `i`-th range (unchecked).
      /// @see `range()`
      auto operator[](std::size_t i) const -> decltype(auto)
        { return range(i); }
      
        private:
      /// Begin iterator of each range, plus end iterator of whole sequence.
      boundaries_t boundaries;
      
    }; // class BoundaryList
    
    
    /// Algorithm implementation for `associationRanges()` functions.
    template <std::size_t GroupKey, typename Iter>
    typename BoundaryList<Iter>::boundaries_t associationRangesImpl
      (Iter begin, Iter end, std::size_t expectedSize = 0);
    
    /**
     * @brief Groups associations by the first key.
     * @tparam GroupKey index of the key in the tuple pointed by the iterator
     * @tparam Iter type of iterators delimiting the data (same type required)
     * @param begin iterator to the first association in the list
     * @param end iterator past the last association in the list
     * @return a list of range boundaries marking the different groups.
     * @throw std::runtime_error if input key is not monotonic
     * 
     * The input iterators are expected to point to a tuple-like structure whose
     * key element can be accessed as `std::get<GroupKey>()` and is an _art_
     * pointer of some sort.
     * 
     * The index of the grouping key is expected to be monotonically increasing.
     * Gaps are supported except that at the end: if e.g. an association of 5
     * keys associates objects to only elements #0, #1 and #3, the resulting
     * list will cover 4 ranges for elements #0 to #3 included, but excluding
     * the end elements, the existence of which can't be inferred from the
     * association list in input. In this example, the range #2 will exist and
     * be empty. To enforce a minimum number of elements, use
     * `associationRanges(Iter, Iter, std::size_t)`.
     */
    template <std::size_t GroupKey, typename Iter>
    BoundaryList<Iter> associationRanges(Iter begin, Iter end);
    
    /**
     * @brief Groups associations by the first key.
     * @tparam GroupKey index of the key in the tuple pointed by the iterator
     * @tparam Iter type of iterators delimiting the data (same type required)
     * @param begin iterator to the first association in the list
     * @param end iterator past the last association in the list
     * @param n minimum number of ranges to be produced.
     * @return a list of range boundaries marking the different groups.
     * @throw std::runtime_error if input key is not monotonic
     * @see `associationRanges(Iter, Iter)`
     * 
     * This function operates almost like `associationRanges(Iter, Iter)`.
     * The only difference is that at least `n` ranges are guaranteed to be
     * produced: if the input defines less than `n`, the missing ones will be
     * added at the end, empty.
     * This allows to work around the possibility of empty ranges at the end,
     * which the `associationRanges(Iter, Iter)` algorithm does not support.
     */
    template <std::size_t GroupKey, typename Iter>
    BoundaryList<Iter> associationRanges(Iter begin, Iter end, std::size_t n);
    
    template <typename Assns>
    using AssociatedGroupRanges
      = BoundaryList<typename Assns::assn_iterator>;
    
    
    /**
     * @brief Object to draft associated data interface.
     * @tparam Main type of the main associated object (one)
     * @tparam Aux type of the additional associated objects (many)
     * @tparam Tag tag this data is labeled with
     * 
     * Allows:
     *  * random access (no index check guarantee)
     *  * forward iteration
     * 
     * Construction is not part of the interface.
     * 
     * The `AssociatedData` object, on creation, finds the borders surrounding
     * the associated `Aux` objects for each `Main` one, and keep a record of
     * them (this is actually delegated to `BoundaryList` class).
     * The `AssociatedData` object also provides a container-like view of this
     * information, where each element in the container is associated to a
     * single `Main` and it is a container (actually, another view) of `Right`.
     * Both levels of containers are random access, so that the set of `Right`
     * objects associated to a `Left` can be accessed by index, and the `Right`
     * objects within can be accessed with `Right` index in the `Left`.
     */
    template <typename Main, typename Aux, typename Tag /* = Aux */>
    class AssociatedData {
      using This_t = AssociatedData<Main, Aux, Tag>; ///< This type.
      
        public:
      using assns_t = art::Assns<Main, Aux>; ///< Type of _art_ association.
      
        private:
      using associated_data_iterator_t
        = tuple_element_iterator<1U, typename assns_t::assn_iterator>;
      
        public:
      using tag = Tag; ///< Tag of this association proxy.
      
      using group_ranges_t = BoundaryList<associated_data_iterator_t>;
      
      /// Type of collection of auxiliary data associated with a main item.
      using auxiliary_data_t
        = util::add_tag_t<typename group_ranges_t::range_t, tag>;
      
      // constructor is not part of the interface
      AssociatedData(group_ranges_t&& groups)
        : fGroups(std::move(groups))
        {}
      
      /// Returns an iterator pointing to the first associated data range.
      auto begin() const -> decltype(auto)
        { return fGroups.begin(); }
      
      /// Returns an iterator pointing past the last associated data range.
      auto end() const -> decltype(auto)
        { return fGroups.end(); }
      
      /// Returns the range with the specified index (no check performed).
      auto getRange(std::size_t i) const -> decltype(auto)
        { return util::makeTagged<tag>(fGroups.range(i)); }
      
      /// Returns the range with the specified index (no check performed).
      auto operator[] (std::size_t index) const -> decltype(auto)
        {
          static_assert(
            std::is_convertible<decltype(getRange(index)), auxiliary_data_t>(),
            "Inconsistent data types."
            );
          return getRange(index);
        }
      
      /// Returns whether this data is labeled with the specified tag
      template <typename TestTag>
      static constexpr bool hasTag() { return std::is_same<TestTag, tag>(); }
      
        private:
      group_ranges_t fGroups;
      
    }; // class AssociatedData<>
    
    
    
    //--------------------------------------------------------------------------
    //---  Stuff for one-to-(zero or one) associated data
    //--------------------------------------------------------------------------
    template <typename Aux, typename AuxTag /* = Aux */>
    struct OneTo01DataProxyMakerWrapper {
      template <typename CollProxy>
      using maker_t = OneTo01DataProxyMaker
        <typename CollProxy::main_element_t, Aux, CollProxy, AuxTag>;
    };
    
    
    /**
     * @brief Object for one-to-zero/or/one associated data interface.
     * @tparam Main type of the main associated object (one)
     * @tparam Aux type of the additional associated objects (zero or one)
     * @tparam Tag tag this data is labeled with
     * 
     * Allows:
     *  * random access (no index check guarantee)
     *  * forward iteration
     * 
     * Construction is not part of the interface.
     * 
     * The `OneTo01Data` object acquires a vector of _art_ pointers, one for
     * each element in the main collection.
     * It is an implementation detail for associations fulfilling the
     * @ref LArSoftProxyDefinitionOneToZeroOrOneSeqAssn "one-to-(zero-or-one) sequential association"
     * requirement.
     * 
     * @note This data structure marks the main elements which have no
     *       associated data with an invalid _art_ pointer (default-constructed)
     *       and it does not distinguish that from the element being actually
     *       associated to a default-constructed _art_ pointer.
     * 
     * The `OneTo01Data` object also provides a container-like view of this
     * information, where each element in the container is associated to a
     * single `Main` and it is an _art_ pointer to the `Right` element.
     * 
     * Association metadata is not accessible from this object.
     */
    template <typename Main, typename Aux, typename Tag /* = Aux */>
    class OneTo01Data {
      using This_t = OneTo01Data<Main, Aux, Tag>; ///< This type.
      
        public:
      /// Type of associated datum.
      using aux_t = Aux;
      
      /// Type of tag.
      using tag = Tag;
      
      /// Type of main datum.
      using main_t = Main;
      
      /// Type of _art_ pointer to associated datum.
      using aux_ptr_t = art::Ptr<aux_t>;
      
      /// Type of auxiliary data associated with a main item.
      using auxiliary_data_t = util::add_tag_t<aux_ptr_t, tag>;
      
      /// Type of collection of auxiliary data for all main elements.
      using aux_coll_t = std::vector<aux_ptr_t>;
      
      /// Type of the source association.
      using assns_t = art::Assns<main_t, aux_t>;
      
      
      OneTo01Data(aux_coll_t&& data): auxData(std::move(data)) {}
      
      /// Returns whether the element `i` is associated with auxiliary datum.
      bool has(std::size_t i) const
        { return get(i) == aux_ptr_t(); }
      
      /// Returns a copy of the pointer to data associated with element `i`.
      auxiliary_data_t get(std::size_t i) const
        { return auxiliary_data_t(auxData[i]); }
      
      
      /// Returns the range with the specified index (no check performed).
      auto operator[] (std::size_t index) const -> decltype(auto)
        {
          static_assert(
            std::is_convertible<decltype(get(index)), auxiliary_data_t>(),
            "Inconsistent data types."
            );
          return get(index);
        }
      
        private:
      aux_coll_t auxData; ///< Data associated to the main collection.
      
    }; // class OneTo01Data<>
    
    
    
    //--------------------------------------------------------------------------
    //---  Stuff for collection proxy as auxiliary data
    //--------------------------------------------------------------------------
    /**
     * @brief Object presenting a proxy as parallel data for another one.
     * @tparam AuxProxyColl type of the parallel data collection
     * @tparam Aux type of the associated object
     * @tparam Tag tag this data is labeled with
     * 
     * This object inherits its interface from `proxy::ParallelData`.
     * In addition, it owns the proxy it wraps.
     */
    template <
      typename AuxProxyColl,
      typename Aux /* = collection_value_t<AuxProxyColl> */,
      typename Tag /* = Aux */
      >
    struct ProxyAsParallelData
      : private AuxProxyColl
      , public ParallelData<AuxProxyColl, Aux, Tag>
    {
      /// Steals and wraps collection `proxy`.
      ProxyAsParallelData(AuxProxyColl&& proxy)
        : AuxProxyColl(std::move(proxy))
        , ParallelData<AuxProxyColl, Aux, Tag>
          (static_cast<AuxProxyColl const*>(this))
        {}
      
      // explicitly select the tag from the parallel data (same as Tag)
      using typename ParallelData<AuxProxyColl, Aux, Tag>::tag;
    }; // class ProxyAsParallelData<>
    
    
    //--------------------------------------------------------------------------
    /**
     * @brief Helper to create a proxy as auxiliary data for another proxy.
     * @tparam AuxProxy type of collection proxy associated to the main one
     * @tparam ArgTuple type of arguments required for the creation of proxy
     * @tparam AuxTag tag for the associated data (default: as `Aux`)
     * 
     * This class stores user arguments for the construction of a collection
     * proxy to be used as auxiliary data for another proxy.
     * It can use those arguments plus some additional one to create the
     * collection proxy data itself. This additional information is provided by
     * `getCollection()`.
     * 
     * The auxiliary data will be identified by type `AuxTag`.
     * 
     * This is not a customization point: to have a custom associated data
     * produced, choose and then specialize the `ProxyAsAuxProxyMaker` class.
     */
    template <
      typename AuxProxy,
      typename ArgTuple,
      typename AuxTag /* = AuxProxy */
      >
    class WithProxyAsAuxStructBase {
      
      static_assert(
        std::is_convertible
          <std::decay_t<std::tuple_element_t<0U, ArgTuple>>, art::InputTag>(),
        "The first argument of WithProxyAsAuxStructBase must be art::InputTag."
        );
      
      /// Type of main data product element from a proxy of type `CollProxy`.
      template <typename CollProxy>
      using main_t = typename CollProxy::main_element_t;
      
      /// Type of auxiliary proxy.
      using aux_proxy_t = AuxProxy;
      
      /// Tag for the associated data (same as the data type itself).
      using tag = AuxTag;
      
      /// Class to create the data proxy associated to a `CollProxy`.
      template <typename CollProxy>
      using proxy_maker_t
        = ProxyAsAuxProxyMaker<main_t<CollProxy>, aux_proxy_t, CollProxy, tag>;
      
        public:
      
      /// Constructor: steals the arguments, to be used by
      /// `createAuxProxyMaker()`.
      WithProxyAsAuxStructBase(ArgTuple&& args): args(std::move(args)) {}
      
      /// Creates the associated data proxy by means of `ProxyAsAuxProxyMaker`.
      template
        <typename CollProxy, typename Event, typename Handle, typename MainArgs>
      auto createAuxProxyMaker
        (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
        {
          return createAuxProxyImpl<CollProxy>(
            event, std::forward<Handle>(mainHandle), mainArgs,
            std::make_index_sequence<NArgs>()
            );
        } // construct()
      
      
        protected:
      
      ArgTuple args; ///< Argument construction storage as tuple.
      
      /// Number of arguments stored.
      static constexpr std::size_t NArgs = std::tuple_size<ArgTuple>();
      
      // this method allows unpacking the arguments from the tuple
      template<
        typename CollProxy, typename Event, typename Handle, typename MainArgs,
        std::size_t... I
        >
      auto createAuxProxyImpl(
        Event const& event, Handle&& mainHandle, MainArgs const& mainArgs,
        std::index_sequence<I...>
        )
        {
          return proxy_maker_t<CollProxy>::make(
            event, mainHandle, mainArgs,
            std::get<I>(std::forward<ArgTuple>(args))...
            );
        }
      
    }; // struct WithProxyAsAuxStructBase
    
    
    
    //--------------------------------------------------------------------------
    //---  Stuff for the whole collection proxy
    //--------------------------------------------------------------------------
    template <typename ProxyElement, typename... AuxData>
    auto makeCollectionProxyElement(
      std::size_t index,
      typename ProxyElement::main_element_t const& main,
      AuxData&&... auxData
    ) {
      return ProxyElement(
        index, main,
        typename ProxyElement::aux_elements_t(std::forward<AuxData>(auxData)...)
        );
    } // makeCollectionProxyElement()
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
} // namespace proxy


//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
namespace proxy {
  
  namespace details {
    
    //--------------------------------------------------------------------------
    // Extends vector v with default-constructed data
    // and executes v[index]=value
    template <typename T>
    void extendAndAssign(
      std::vector<T>& v,
      typename std::vector<T>::size_type index, 
      typename std::vector<T>::value_type const& value
    ) {
      if (index >= v.size()) {
        v.reserve(index + 1);
        v.resize(index);
        v.push_back(value);
      }
      else v[index] = value;
    } // extendAndAssign()
    
    // Extends vector v with default-constructed data
    // and executes v[index]=move(value)
    template <typename T>
    void extendAndAssign(
      std::vector<T>& v,
      typename std::vector<T>::size_type index, 
      typename std::vector<T>::value_type&& value
    ) {
      if (index >= v.size()) {
        v.reserve(index + 1);
        v.resize(index);
        v.push_back(std::move(value));
      }
      else v[index] = std::move(value);
    } // extendAndAssign()
    
    
    //--------------------------------------------------------------------------
    //--- associationRanges() implementation
    //--------------------------------------------------------------------------
    template <std::size_t GroupKey, typename Iter>
    typename BoundaryList<Iter>::boundaries_t associationRangesImpl
      (Iter begin, Iter end, std::size_t expectedSize /* = 0 */)
    {
      constexpr auto KeyIndex = GroupKey;
      
      auto extractKey
        = [](auto const& assn){ return std::get<KeyIndex>(assn).key(); };
      
      typename BoundaryList<Iter>::boundaries_t boundaries;
      boundaries.reserve(expectedSize + 1);
      boundaries.push_back(begin);
      std::size_t current = 0;
      for (auto it = begin; it != end; ++it) {
        auto const key = extractKey(*it);
        if (key == current) continue;
        if (key < current) {
          auto index = std::distance(begin, it);
          throw std::runtime_error("associationRanges() got input element #"
            + std::to_string(index - 1) + " with key " + std::to_string(current)
            + " and the next with key " + std::to_string(key) + "!"
            );
        }
        boundaries.insert(boundaries.end(), key - current, it);
        current = key;
      } // for
      boundaries.push_back(end);
      return boundaries;
    } // associationRangesImpl()
    
    
    //--------------------------------------------------------------------------
    template <std::size_t GroupKey, typename Iter>
    auto associationRangeBoundaries(Iter begin, Iter end)
      { return associationRangesImpl<GroupKey, Iter>(begin, end); }
    
    
    //--------------------------------------------------------------------------
    template <std::size_t GroupKey, typename Iter>
    auto associationRangeBoundaries(Iter begin, Iter end, std::size_t n) {
      auto boundaries = associationRangesImpl<GroupKey, Iter>(begin, end, n);
      if (boundaries.size() <= n) {
        boundaries.insert
          (boundaries.end(), n + 1 - boundaries.size(), boundaries.back()); 
        assert(boundaries.size() == (n + 1));
      }
      return boundaries;
    } // associationRangeBoundaries(Iter, Iter, std::size_t)
    
    
    //--------------------------------------------------------------------------
    template <std::size_t GroupKey, typename Iter>
    BoundaryList<Iter> associationRanges(Iter begin, Iter end)
      {
        return BoundaryList<Iter>
          (associationRangeBoundaries<GroupKey>(begin, end));
      }
    
    
    //--------------------------------------------------------------------------
    template <std::size_t GroupKey, typename Iter>
    BoundaryList<Iter> associationRanges(Iter begin, Iter end, std::size_t n)
      {
        return BoundaryList<Iter>
          (associationRangeBoundaries<GroupKey>(begin, end, n));
      }
    
    
    //--------------------------------------------------------------------------
    template <std::size_t Key, std::size_t Data, typename Iter>
    auto associationOneToOneFullSequence(Iter begin, Iter end, std::size_t n) {
      //
      // Here we are actually not using the assumption that the keys are in
      // increasing order; which is just as good as long as we use a fast random
      // access container as STL vector.
      // We do assume the key side of the association to be valid, though.
      //
      using value_type = typename Iter::value_type;
      using data_t = std::tuple_element_t<Data, value_type>;
      std::vector<data_t> data(n); // all default-constructed
      for (auto it = begin; it != end; ++it) {
        auto const& keyPtr = std::get<Key>(*it);
        details::extendAndAssign(data, keyPtr.key(), std::get<Data>(*it));
      }
      return data;
    } // associationOneToOneFullSequence(Iter, Iter, std::size_t)
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  //--- makeAssociatedData() implementations
  //----------------------------------------------------------------------------
  template <typename Tag, typename Assns>
  auto makeAssociatedData(Assns const& assns, std::size_t minSize /* = 0 */)
  {
    using Main_t = typename Assns::left_t;
    using Aux_t = typename Assns::right_t;
    using AssociatedData_t = details::AssociatedData<Main_t, Aux_t, Tag>;
    
    // associationRangeBoundaries() produces iterators to association elements,
    // (i.e. tuples)
    using std::begin;
    using std::end;
    auto ranges = details::associationRangeBoundaries<0U>
      (begin(assns), end(assns), minSize);
    // we convert those iterators into iterators to the right associated item
    // (it takes a few steps)
    using group_ranges_t = typename AssociatedData_t::group_ranges_t;
    return AssociatedData_t(
      group_ranges_t
        (typename group_ranges_t::boundaries_t(ranges.begin(), ranges.end()))
      );
  } // makeAssociatedData(assns)
  
  
  //----------------------------------------------------------------------------
  template <typename Main, typename Aux, typename Tag, typename Event>
  auto makeAssociatedData
    (Event const& event, art::InputTag tag, std::size_t minSize /* = 0 */)
  {
    using Main_t = Main;
    using Aux_t = Aux;
    using AssociatedData_t = details::AssociatedData<Main_t, Aux_t, Tag>;
    using Assns_t = typename AssociatedData_t::assns_t;
    
    return
      makeAssociatedData<Tag>(*(event.template getValidHandle<Assns_t>(tag)));
    
  } // makeAssociatedData(tag)
  
  
  //----------------------------------------------------------------------------
  template <typename Aux, typename Tag, typename Handle, typename Event>
  auto makeAssociatedData
    (Handle&& handle, Event const& event, art::InputTag tag)
  {
    // Handle::value_type is the main data product type (a collection)
    using Main_t = collection_value_t<typename Handle::value_type>;
    using Aux_t = Aux;
    return makeAssociatedData<Main_t, Aux_t, Tag>(event, tag, handle->size());
  } // makeAssociatedData(handle)
  
  
  
  //----------------------------------------------------------------------------
  //--- makeAssociatedData() implementations
  //----------------------------------------------------------------------------
  template <typename Tag, typename Assns>
  auto makeAssociatedTo01data(Assns const& assns, std::size_t minSize /* = 0 */)
  {
    using Main_t = typename Assns::left_t;
    using Aux_t = typename Assns::right_t;
    using AssociatedData_t = details::OneTo01Data<Main_t, Aux_t, Tag>;
    
    using std::cbegin;
    using std::cend;
    return AssociatedData_t(
      details::associationOneToOneFullSequence<0U, 1U>
        (cbegin(assns), cend(assns), minSize)
      );
  } // makeAssociatedTo01data(assns)
  
  
  //----------------------------------------------------------------------------
  template <typename Main, typename Aux, typename Tag, typename Event>
  auto makeAssociatedTo01data
    (Event const& event, art::InputTag tag, std::size_t minSize /* = 0 */)
  {
    using Main_t = Main;
    using Aux_t = Aux;
    using AssociatedData_t = details::OneTo01Data<Main_t, Aux_t, Tag>;
    using Assns_t = typename AssociatedData_t::assns_t;
    
    return makeAssociatedTo01data<Tag>
      (*(event.template getValidHandle<Assns_t>(tag)), minSize);
    
  } // makeAssociatedTo01data(tag)
  
  
  //----------------------------------------------------------------------------
  template <typename Aux, typename Tag, typename Handle, typename Event>
  auto makeAssociatedTo01data
    (Handle&& handle, Event const& event, art::InputTag tag)
  {
    // Handle::value_type is the main data product type (a collection)
    using Main_t = collection_value_t<typename Handle::value_type>;
    using Aux_t = Aux;
    return makeAssociatedTo01data<Main_t, Aux_t, Tag>
      (event, tag, handle->size());
  } // makeAssociatedTo01data(handle)
  
  
  
  //----------------------------------------------------------------------------
  //--- makeParallelData() implementations
  //----------------------------------------------------------------------------
  template <
    typename AuxColl,
    typename Aux /* = collection_value_t<AuxColl>*/,
    typename Tag /* = Aux */
    >
  auto makeParallelData(AuxColl const& data) {
    
    // Ahh, simplicity.
    return details::ParallelData<AuxColl, Aux, Tag>(data);
    
  } // makeParallelData(AuxColl)
  
  
  //----------------------------------------------------------------------------
  template <typename AuxColl, typename Aux, typename Tag, typename Event>
  auto makeParallelData(Event const& event, art::InputTag tag) {
    return makeParallelData<AuxColl, Aux, Tag>
      (*(event.template getValidHandle<AuxColl>(tag)));
  } // makeParallelData()
  
  
  //----------------------------------------------------------------------------
  //---  CollectionProxyElement
  //----------------------------------------------------------------------------
  template <typename CollProxy>
  template <typename Tag, typename T>
  auto CollectionProxyElement<CollProxy>::getIf() const -> decltype(auto)
    { return getIfHas<Tag, T>(details::YesNoStruct<has<Tag>()>{}); }
  
  
  template <typename CollProxy>
  template <typename Tag, typename>
  auto CollectionProxyElement<CollProxy>::getIfHas
    (details::YesNoStruct<true>) const -> decltype(auto)
    { return get<Tag>(); }
  
  template <typename CollProxy>
  template <typename Tag, typename T>
  auto CollectionProxyElement<CollProxy>::getIfHas
    (details::YesNoStruct<false>) const -> T
    {
      throw std::logic_error
        ("Tag '" + lar::debug::demangle<Tag>() + "' not available."); 
    }
  
  
  //----------------------------------------------------------------------------
  //---  CollectionProxyBase
  //----------------------------------------------------------------------------
  template <
    template <typename CollProxy> class Element,
    typename MainColl,
    typename... AuxColls
    >
  template <typename Tag, typename T>
  auto CollectionProxyBase<Element, MainColl, AuxColls...>::getIf() const
    -> decltype(auto)
    { return getIfHas<Tag, T>(details::YesNoStruct<has<Tag>()>{}); }
  
  
  template <
    template <typename CollProxy> class Element,
    typename MainColl,
    typename... AuxColls
    >
  template <typename Tag, typename>
  auto CollectionProxyBase<Element, MainColl, AuxColls...>::getIfHas
    (details::YesNoStruct<true>) const -> decltype(auto)
    { return get<Tag>(); }
  
  template <
    template <typename CollProxy> class Element,
    typename MainColl,
    typename... AuxColls
    >
  template <typename Tag, typename T>
  auto CollectionProxyBase<Element, MainColl, AuxColls...>::getIfHas
    (details::YesNoStruct<false>) const -> T
    {
      throw std::logic_error
        ("Tag '" + lar::debug::demangle<Tag>() + "' not available."); 
    }
  
  
  //----------------------------------------------------------------------------
  //---  CollectionProxyMaker specializations
  //----------------------------------------------------------------------------
  // none so far
  
  //----------------------------------------------------------------------------
  
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_H
