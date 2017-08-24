/**
 * @file   ProxyBase.h
 * @brief  Base utilities for the implementation of data product facades.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   July 27, 2017
 * 
 * This library is header-only.
 * 
 * 
 * Definitions
 * ============
 * 
 * * *one-to-many sequential association*: an association between `L` and `R`
 *   types where:
 *     * `L` objects come from a single data product
 *     * the sequence of associations is such that if `L1` is before `L2` in the
 *       original data product, all `L1`-`Rx` associations of `L1` are listed
 *       before any of the `L2`-`Rx` associations of `L2`; in other words, the
 *       association list follows the original order of the `L` data product;
 *       note that this preclude actual many-to-many associations.
 *   This does _not_ require associations to be one-to-one (it allows one `L` to
 *   many `R`), nor that all `L` be associated to at least one `R`.
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
 * The documentation of file `CollectionView.h` includes a more in-depth
 * description of it.
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
 * @note I learned afterward about the existence of `boost::iterator_adapter`,
 *       which might provide similar functionality and also be dealing correctly
 *       with non-constant iterators. Worth considering.
 * 
 * 
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
 * @defgroup LArSoftProxies Helper classes for easier access to connected data.
 */

/// Encloses LArSoft data product proxy objects and utilities.
namespace proxy {
  
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
    template <typename Main, typename Aux, typename Tag = Aux>
    class AssociatedData;
    
    //--------------------------------------------------------------------------
    template <typename Aux, typename ArgTuple, typename AuxTag = Aux>
    class WithAssociatedStruct;
    
    //--------------------------------------------------------------------------
    template <typename ProxyElement, typename... AuxData>
    auto makeCollectionProxyElement(
      std::size_t index,
      typename ProxyElement::main_element_t const& main,
      AuxData&&... auxData
      );
    
    //--------------------------------------------------------------------------
    template <typename Coll>
    class IndexBasedIterator;
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
  
  /// @{
  /// @name Associated data infrastructure
  /// @ingroup LArSoftProxies
  
  //----------------------------------------------------------------------------
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
   * "one-to-many sequential association" requirement (see the "Definitions"
   * section in `ProxyBase.h` documentation).
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
   * @see `makeAssociatedData(Handle&&, Event&&, art::InputTag)`
   * 
   * This function operates like
   * `makeAssociatedData(Handle&&, Event&&, art::InputTag)`, but it extracts
   * the information about the type of main object and the minimum number of
   * them from a handle.
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
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Creates an associated data wrapper for the specified types.
   * @tparam Main type of main datum (element) to associate from ("left")
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
   *   proxy with arguments an event, a template argument representing the
   *   main collection information, and all the arguments required for the
   *   creation of the associated proxy (coming from `withAssociated()`);
   *   equivalent to the signature:
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   template <typename Event, template MainArg, template... Args>
   *   auto make(Event const&, MainArg const&, Args&&...);
   *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   * This class can be specialized (see `withAssociated()` for an example).
   * The default implementation just wraps a one-to-many
   * `art::Assns<Main, Aux>` data product fulfilling "one-to-many sequential
   * association" requirement (see the "Definitions" section in `ProxyBase.h`
   * documentation).
   * 
   * The third template argument is designed for specialization of associations
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
     * @tparam MainArgs any type convertible to `art::InputTag`
     * @param event the event to read associations from
     * @param margs an object describing the main data product
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
  
  
  /// @{
  /// @name Proxy element infrastructure
  /// @ingroup LArSoftProxies
  
  //----------------------------------------------------------------------------
  /**
   * @brief An element of a collection proxy.
   * @tparam CollProxy the type of collection proxy this is the element of
   * 
   * 
   * @todo The interface of this objects need to be developed
   * @todo Document this class
   */
  template <typename CollProxy>
  struct CollectionProxyElement {
    
      public:
    using collection_proxy_t = CollProxy;
    using main_element_t = typename collection_proxy_t::main_element_t;
    
    /// Tuple of elements.
    using aux_elements_t = typename details::SubstituteWithAuxList
      <typename CollProxy::aux_collections_t>::type;
    
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
     *       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     *       if (elem.has<recob::SpacePoint>()) {
     *         recob::SpacePoint const* spacepoints
     *           = elem.getIf<recob::SpacePoint>(); // won't do
     *         // ...
     *       }
     *       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *       to work; it becomes:
     *       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
     *       if (elem.has<recob::SpacePoint>()) {
     *         recob::SpacePoint const* spacepoints
     *           = elem.getIf<recob::SpacePoint, recob::SpacePoint const*>();
     *         // ...
     *       }
     *       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
  
  
  /// @{
  /// @name Data product proxy infrastructure
  /// @ingroup LArSoftProxies
  
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
    element_proxy_t operator[] (std::size_t i) const
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
      { return aux<util::index_of_tag_v<AuxTag, aux_collections_t>>(); }
    
    
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
   * @brief Helper function to request associated data.
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
   * struct MCS {};
   * auto tracks = proxy::getCollection<proxy::Tracks>(event, trackTag,
   *   withAssociated<recob::TrackMomentum>(defaultMomTag),
   *   withAssociatedAs<recob::TrackMomentum, MCS>(MCSmomTag)
   *   );
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The first momentum association (`"defaultMomTag"`) will be accessed by
   * using the type `recob::TrackMomentum` as tag, while the second one will
   * be accessed by the `MCS` tag (which is better not be defined in a local
   * scope).
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
   * `proxy::AssociatedDataProxyMaker`:
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
   * is * associated with, when using it as `getCollection()` argument:
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
   * A default implementation is provided. In it, the template argument
   * `CollProxy` is expected to have the interface of `CollectionProxy`.
   * The arguments required are documented in the implementation of the `make()`
   * function. Note that the type of proxy returned does not need to be
   * `CollProxy`, but is in fact an instance of `proxy::CollectionProxy`.
   * 
   */
  template <typename CollProxy>
  struct CollectionProxyMaker {
    
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
          withArgs.template createAssnProxyMaker<main_collection_proxy_t>
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
    
  }; // struct CollectionProxyMaker<>
  
  

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
   * 
   *  The type
   * of proxy delivered is arbitrary and usually not `CollProxy`.
   * The type of the collection proxy must be explicitly specified, e.g.:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tracks = proxy::getCollection<proxy::Tracks>
   *   (event, tag, withAssociated<recob::Hits>());
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * In this case, two optional arguments arre passed: the input tag to the main
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
  auto getCollection(Event const& event, OptionalArgs&&... args)
  {
    return CollectionProxyMaker<CollProxy>::make
      (event, std::forward<OptionalArgs>(args)...);
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
    using main_element_t = typename main_collection_t::value_type;
    
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
      
      using value_type = typename container_t::value_type;
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
      using main_element_t = typename MainColl::value_type;
      
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
    // `associated_data_t`
    template <typename Tuple>
    struct SubstituteWithAuxList {
      static_assert
        (util::always_true_type<Tuple>(), "Template argument must be a tuple");
    }; // SubstituteWithAuxList<>
    
    template <typename... T>
    struct SubstituteWithAuxList<std::tuple<T...>> {
      using type = std::tuple<typename T::associated_data_t...>;
    }; // SubstituteWithAuxList<tuple>
    
    
    //--------------------------------------------------------------------------
    /**
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
    template <typename Aux, typename ArgTuple, typename AuxTag /* = Aux */>
    class WithAssociatedStruct {
      
      /// Type of main data product element from a proxy of type `CollProxy`.
      template <typename CollProxy>
      using main_t = typename CollProxy::main_element_t;
      
      /// Type of associated data.
      using aux_t = Aux;
      
      /// Tag for the associated data (same as the data type itself).
      using tag = AuxTag;
      
      /// Class to create the data proxy associated to a `CollProxy`.
      template <typename CollProxy>
      using proxy_maker_t = AssociatedDataProxyMaker
        <main_t<CollProxy>, aux_t, CollProxy, tag>;
      
        public:
      
      /// Type association proxy type created for the specified `CollProxy`.
      template <typename CollProxy>
      using aux_collection_proxy_t
        = typename proxy_maker_t<CollProxy>::aux_collection_proxy_t;
      
      /// Constructor: steals the arguments, to be used by
      /// `createAssnProxyMaker()`.
      WithAssociatedStruct(ArgTuple&& args): args(std::move(args)) {}
      
      /// Creates the associated data proxy by means of
      /// `AssociatedDataProxyMaker`.
      template
        <typename CollProxy, typename Event, typename Handle, typename MainArgs>
      auto createAssnProxyMaker
        (Event const& event, Handle&& mainHandle, MainArgs const& mainArgs)
        { 
          return createAssnProxyMakerImpl<CollProxy>(
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
      auto createAssnProxyMakerImpl(
        Event const& event, Handle&& mainHandle, MainArgs const& mainArgs,
        std::index_sequence<I...>
        )
        {
          return proxy_maker_t<CollProxy>::make(
            event, mainHandle, mainArgs,
            std::get<I>(std::forward<ArgTuple>(args))...
            );
        }
      
    }; // struct WithAssociatedStruct
    
    
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
      using associated_data_t
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
        { return getRange(index); }
      
      /// Returns whether this data is labeled with the specified tag
      template <typename TestTag>
      static constexpr bool hasTag() { return std::is_same<TestTag, tag>(); }
      
        private:
      group_ranges_t fGroups;
    }; // class AssociatedData<>
    
    
    
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
    
  } // namespace details
  
  
  //----------------------------------------------------------------------------
  //--- makeAssociatedData() implementations
  //----------------------------------------------------------------------------
  template <typename Main, typename Aux, typename Tag, typename Event>
  auto makeAssociatedData
    (Event const& event, art::InputTag tag, std::size_t minSize /* = 0 */)
  {
    using Main_t = Main;
    using Aux_t = Aux;
    using AssociatedData_t = details::AssociatedData<Main_t, Aux_t, Tag>;
    using Assns_t = typename AssociatedData_t::assns_t;
    
    auto const& assns = *(event.template getValidHandle<Assns_t>(tag));
    // associationRangeBoundaries() produces iterators to association elements,
    // (i.e. tuples)
    auto ranges = details::associationRangeBoundaries<0U>
      (assns.begin(), assns.end(), minSize);
    // we convert those iterators into iterators to the right associated item
    // (it takes a few steps)
    using group_ranges_t = typename AssociatedData_t::group_ranges_t;
    return AssociatedData_t(
      group_ranges_t
        (typename group_ranges_t::boundaries_t(ranges.begin(), ranges.end()))
      );
  } // makeAssociatedData(size)
  
  
  //----------------------------------------------------------------------------
  template <typename Aux, typename Tag, typename Handle, typename Event>
  auto makeAssociatedData
    (Handle&& handle, Event const& event, art::InputTag tag)
  {
    // Handle::value_type is the main data product type (a collection)
    using Main_t = typename Handle::value_type::value_type;
    using Aux_t = Aux;
    return makeAssociatedData<Main_t, Aux_t, Tag>(event, tag, handle->size());
  } // makeAssociatedData(handle)
  
  
  
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
