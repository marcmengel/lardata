/**
 * @file   ProxyBase.h
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
#include <stdexcept> // std::runtime_error
#include <type_traits> // std::is_same<>, std::enable_if_t<>, ...
#include <limits> // std::numeric_limits<>
#include <cstdlib> // std::size_t
#include <cassert>


/// Encloses LArSoft data product proxy objects and utilities.
namespace proxy {
  
  namespace details {
    template <typename Proxy>
    struct ProxyCollectionGetterTraits;
  } // namespace details
  
  
  /**
   * @brief Object to create the proxy of a collection.
   * @tparam Proxy the type of proxy to be created
   * 
   * This class can't be used directly and must be specialized.
   * Its content reflects the interface that the infrastructure expects to find.
   * 
   * The interface it made of:
   * - traits:
   *     * `ProductCollection_t` is the type of the collection of the main data
   *       product
   *     * `ProductElement_t` is the type of the main data product object
   * - `get()` method doing the work, using an event and a tag
   * 
   * This class is used by `proxy::getCollection()` function.
   * Overloading that function is another way to customize the creation of a
   * proxy.
   */
  template <typename Proxy>
  class ProxyCollectionGetter {
    
    using Traits_t = details::ProxyCollectionGetterTraits<Proxy>;
    
      public:
    using ProductCollection_t = typename Traits_t::ProductCollection_t;
    using ProductElement_t = typename Traits_t::ProductElement_t;
    
    /// Returns the specified proxy object, reading data from `event`.
    template <typename Event>
    auto get(Event const& event, art::InputTag tag) const;
    
      private:
    ProxyCollectionGetter() = delete; // this class must be specialized
  }; // class ProxyCollectionGetter
  
  
  /**
   * @brief Creates and returns a proxy of the specified data type.
   * @tparam Proxy type of proxy object to be created
   * @tparam Event type of event object to be used (let C++ autodetect it!)
   * @param event the event object to read the information from
   * @param tag the tag of the main object collection
   * @return a Proxy object pointing to the specified data product
   * 
   * This functions creates and returns a collection-like object where each
   * element is a proxy to an element of a data product collection.
   * For example, `proxy::getCollection<proxy::Tracks>(event, "pandora")` will
   * return a random-access object where each element is a `proxy::Track`,
   * proxy object of a track of type `recob::Track` in a data product with tag
   * `"pandora"`.
   * 
   * The supported proxy objects are normally defined in the `proxy` namespace
   * and they share the class name with the data product they are proxy of.
   * For example, the proxy object of `recob::Track` is `proxy::Track`.
   * A collection of tracks, `std::vector<recob::Track>`, is proxied by a
   * `proxy::Tracks` object (note the plural "s").
   * 
   */
  template <typename Proxy, typename Event>
  auto getCollection(Event const& event, art::InputTag tag)
    { return ProxyCollectionGetter<Proxy>().get(event, tag); }
  
  
  //----------------------------------------------------------------------------
  /// Traits for a collection element proxy.
  template <typename Data>
  struct ProxyCollectionElementTraits {
    
    using main_value_type = Data; ///< Type of main data product content.
    
    using value_type = main_value_type; ///< Type of main data product content.
    
  }; // struct ProxyCollectionElementTraits<>
  
  
  /// Traits for a collection proxy.
  template<
    typename Proxy,
    template <typename T, typename...> class Coll = std::vector
    >
  struct ProxyCollectionTraits {
      private:
    using proxy_traits_t
      = ProxyCollectionElementTraits<typename Proxy::value_type>;
    
      public:
    using proxy_value_type = Proxy; ///< Type of proxy.
    
    /// Type of proxy collection (that is, this class).
    using proxy_collection_type = ProxyCollectionTraits<Proxy, Coll>;
    
    /// Type exposed by the collection (the proxy).
    using value_type = proxy_value_type;
    
    /// Type of the main object contained in the value proxy.
    using main_value_type = typename proxy_value_type::value_type;
    
    /// Type of data product collection.
    using main_collection_type = Coll<main_value_type>;
    
  }; // struct ProxyCollectionTraits<>
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Proxy to a collection of objects.
   * @tparam Proxy type of proxy to the element of the collection.
   * 
   * The collection and its elements are immutable.
   */
  template <typename Proxy>
  class ProxyCollection: public ProxyCollectionTraits<Proxy> {
    using Base_t = ProxyCollectionTraits<Proxy>;
    
      public:
    /// Returns whether the track collection is empty.
    bool empty() const { return main->empty(); }
    
    /// Returns the number of tracks in the collection.
    std::size_t size() const { return main->size(); }
    
      protected:
    using main_collection_type = typename Base_t::main_collection_type;
    
    ProxyCollection(main_collection_type const& main): main(&main) {}
    
    /// Returns the main element at the specified index. No boundary checked.
    auto getMainAt(std::size_t index) const -> decltype(auto)
      { return main->operator[](index); }
    
      private:
    
    main_collection_type const* main; ///< Pointer to the main data collection.
    
  }; // class ProxyCollection
  
  
  //----------------------------------------------------------------------------
  /**
   * @brief Iterator for a proxy collection.
   * @tparam ProxyColl type of proxy collection to iterate
   * 
   * `ProxyColl` is a collection derived from `ProxyCollection`, which must
   * provide a public `operator[](std::size_t)` method returning a proxy.
   * A new proxy object is generated at each dereferenciation, that means
   * the expression `&*it == &*it` will be false (while generally `*it == *it`
   * would be true).
   */
  template <typename ProxyColl>
  class ProxyCollectionIterator {
    using Collection_t = ProxyColl;
    
      public:
    using value_type = typename Collection_t::value_type;
    using const_iterator = ProxyCollectionIterator;
    
    /// Default constructor (required by iterator protocol): an unusable iterator.
    ProxyCollectionIterator() = default;
    
    /// Constructor: initializes from an iterator of the proxy main collection.
    ProxyCollectionIterator(Collection_t const& coll, std::size_t index = 0)
      : fColl(&coll), fIndex(index) {}
    
    /// Returns the value pointed by this iterator.
    auto operator* () const -> decltype(auto)
      { return fColl->operator[](fIndex); }
    
    /// Returns the value pointed by this iterator.
    const_iterator& operator++ () { ++fIndex; return *this; }
    
    /// Returns whether the iterators point to the same element.
    bool operator!= (const_iterator const& other) const
      { return (other.fIndex != fIndex) || (other.fColl != fColl); }
    
      protected:
    /// Pointer to the original collection.
    Collection_t const* fColl = nullptr;
    
    /// Current index in the main collection.
    std::size_t fIndex = std::numeric_limits<std::size_t>::max();
    
  }; // ProxyCollectionIterator<>


  //----------------------------------------------------------------------------
  /**
   * @brief Proxy to an element of a proxy collection.
   * @tparam Data type of element being proxied
   * 
   * The element points to a "main" object (of type `Data`) via an immutable
   * pointer.
   * 
   * Given the point of a proxy, there is no use of this class directly, but it
   * should be extended with additional data and interface.
   * 
   * As a proxy base class, it provides access to the original object via a
   * dereference `operator->`, smart-pointer-like. The proxy is expected always
   * to point to a main object.
   * 
   */
  template <typename Data>
  class ProxyCollectionElement: public ProxyCollectionElementTraits<Data> {
    using Base_t = ProxyCollectionElementTraits<Data>;
    using CollTraits_t = Base_t;
      public:
    
    // this is a "regular" object
    ProxyCollectionElement(ProxyCollectionElement const&) = default;
    ProxyCollectionElement(ProxyCollectionElement&&) = default;
    ProxyCollectionElement& operator=(ProxyCollectionElement const&) = default;
    ProxyCollectionElement& operator=(ProxyCollectionElement&&) = default;
    
    
    /// Access to the main object of the proxy: `recob::Track`.
    auto operator->() const { return mainPtr(); }
    
    /// Access to the main object of the proxy: `recob::Track`.
    auto operator*() const -> decltype(auto) { return mainRef(); }
    
    
      protected:
    using main_value_type = typename Base_t::main_value_type;
    
    /// Constructor: points to the specified main value.
    ProxyCollectionElement(main_value_type const& main): fMain(&main) {}
    
    /// Access to the main data element.
    auto mainPtr() const { return fMain; }
    
    /// Access to the main data element.
    auto mainRef() const -> decltype(auto) { return *mainPtr(); }
    
  //  void setMain(Base_t::main_value_type const* main) { fMain = main; }
    
      private:
    ///< Pointer to the proxied object. It should never be nullptr.
    main_value_type const* fMain;
    
  }; // class ProxyCollectionElement
  
  
  //----------------------------------------------------------------------------
  namespace details {
    
    /// Constant expressing whether `Iter` type implements random access.
    template <typename Iter>
    constexpr bool is_random_access_iterator_v = std::is_same<
      typename std::iterator_traits<Iter>::iterator_category,
      std::random_access_iterator_tag
      >::value;
    
    
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
      // BoundaryListRangeBase<data_iterator_t> const&
      using range_t = typename range_iterator_t::value_type;
      
      
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
      /// Returns the begin iterator of the `i`-th range (unchecked).
      range_t range(std::size_t i) const
        { return { std::next(boundaries.begin(), i) }; }
      /// Returns the begin iterator of the `i`-th range (unchecked).
      range_t operator[](std::size_t i) const
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
    
    
    /// Modified iterator returning the second element out of the pointed tuple.
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
    
    
    /**
     * @brief Object to draft associated data interface.
     * @tparam Main type of the main associated object (one)
     * @tparam Aux type of the additional associated objects (many)
     * 
     * Allows:
     *  * random access (no index check guarantee)
     *  * forward iteration
     * 
     * Construction is not part of the interface.
     */
    template <typename Main, typename Aux>
    class AssociatedData {
      using This_t = AssociatedData<Main, Aux>; ///< This type.
      using Assns_t = art::Assns<Main, Aux>; ///< Type of _art_ association.
      
      using associated_data_iterator_t
        = tuple_element_iterator<1U, typename Assns_t::assn_iterator>;
      
      using GroupRanges_t = BoundaryList<associated_data_iterator_t>;
      
      GroupRanges_t groups;
      
        public:
      using range_iterator_t = typename GroupRanges_t::range_iterator_t;
      
      
      // constructor is not part of the interface
      template <typename Handle, typename Event>
      AssociatedData(Handle&& handle, Event&& event, art::InputTag tag)
        : groups(
            makeAssociatedGroups
              (std::forward<Handle>(handle), std::forward<Event>(event), tag)
          )
        {}
      
      
      /// Returns an iterator pointing to the first associated data range.
      auto begin() const -> decltype(auto)
        { return groups.begin(); }
      
      /// Returns an iterator pointing past the last associated data range.
      auto end() const -> decltype(auto)
        { return groups.end(); }
      
      /// Returns the range with the specified index (no check performed).
      auto getRange(std::size_t i) const -> decltype(auto)
        { return groups.range(i); }
      
      /// Returns the range with the specified index (no check performed).
      auto operator[] (std::size_t index) const -> decltype(auto)
        { return getRange(index); }
      
      /// Type of collection of auxiliary data associated with a main item.
      using AuxList_t = decltype(std::declval<This_t>().operator[](0U));
      
        private:
      template <typename Handle, typename Event>
      GroupRanges_t makeAssociatedGroups
        (Handle&& handle, Event&& event, art::InputTag tag);
      
    }; // class AssociatedData<>
    
    
  } // namespace details
  
  
} // namespace proxy


//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
namespace proxy {
  
  namespace details {
    template <typename Proxy>
    struct ProxyCollectionGetterTraits {
      using ProductCollection_t = typename Proxy::main_collection_type;
      using ProductElement_t = typename Proxy::main_value_type;
    }; // struct ProxyCollectionGetterTraits
    
    
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
    //--- AssociatedData<Main,Aux> implementation
    //--------------------------------------------------------------------------
    template <typename Main, typename Aux>
    template <typename Handle, typename Event>
    auto AssociatedData<Main,Aux>::makeAssociatedGroups
      (Handle&& handle, Event&& event, art::InputTag tag)
      -> GroupRanges_t
    {
      auto const& assns = *(event.template getValidHandle<Assns_t>(tag));
      // associationRanges() produces iterators to association elements,
      // (i.e. tuples)
      auto ranges = associationRangeBoundaries<0U>
        (assns.begin(), assns.end(), handle->size());
      // we convert those iterators into iterators to the right associated item
      return GroupRanges_t
        (typename GroupRanges_t::boundaries_t(ranges.begin(), ranges.end()));
    } // AssociatedData<Main,Aux>::makeAssociatedGroups()
    
    
    //--------------------------------------------------------------------------
    
  } // namespace details
  
} // namespace proxy


#endif // LARDATA_RECOBASEPROXY_PROXYBASE_H
