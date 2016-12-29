/**
 * @file    RangeForWrapper.h
 * @brief   Utility function to enable range-for on different type iterators
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    December 12, 2016
 *
 * The functionality provided here is implemented in C++ language 2017.
 * Better than here, of course.
 */

#ifndef LARDATA_UTILITIES_RANGEFORWRAPPER_H
#define LARDATA_UTILITIES_RANGEFORWRAPPER_H

// Boost libraries
#include "boost/variant.hpp"

// C/C++ standard libraries
#include <stdexcept> // std::logic_error
#include <utility> // std::move(), std::declval(), ...
#include <type_traits> // std::is_same<>, std::enable_if_t<>, ...


namespace util {
  
  namespace details {
   
    /// Iterator wrapping one of two types of iterators.
    /// @see RangeForWrapperBox
    template <typename BeginIter, typename EndIter>
    class RangeForWrapperIterator {
      struct Dereferencer;
      struct Incrementer;
      struct Comparer;
      
        public:
      using begin_t = BeginIter; ///< Type of begin iterator we can store.
      using end_t = EndIter; ///< Type of end iterator we can store.
      using this_t = RangeForWrapperIterator<begin_t, end_t>; ///< This class.
      
      /// Type returned dereferencing the iterator (may be a reference).
      using value_type = decltype(*(std::declval<begin_t>()));
      
      /// Constructor: initializes with a begin-type iterator.
      explicit RangeForWrapperIterator(begin_t&& begin)
        : fIter(std::move(begin))
        {}
      
      /// Constructor: initializes with a end-type iterator.
      explicit RangeForWrapperIterator(end_t&& end)
        : fIter(std::move(end))
        {}
      
      /// Returns the pointed value (just like the original iterator).
      value_type operator*() const
        { return boost::apply_visitor(Dereferencer(), fIter); }
      
      /// Increments the iterator (prefix operator).
      this_t& operator++()
        { boost::apply_visitor(Incrementer(), fIter); return *this; }
      
      /// Returns whether the other iterator is equal to this one.
      bool operator!=(this_t const& other) const
        { return boost::apply_visitor(Comparer(), fIter, other.fIter); }
      
      
        private: 
      static_assert(!std::is_same<begin_t, end_t>::value,
        "RangeForWrapperIterator requires two different iterator types."
        );
      
      boost::variant<begin_t, end_t> fIter; ///< The actual iterator we store.
      
      /// Visitor to dereference an iterator
      struct Dereferencer: public boost::static_visitor<value_type> {
        value_type operator() (begin_t const& iter) const { return *iter; }
        [[noreturn]] value_type operator() (end_t const&) const
          { throw std::logic_error("End iterator should not be dereferenced"); }
      }; // Dereferencer
      
      /// Visitor to increment an iterator
      struct Incrementer: public boost::static_visitor<> {
        void operator() (begin_t& iter) const { ++iter; }
        [[noreturn]] void operator() (end_t&) const
          { throw std::logic_error("End iterator should not be incremented"); }
      }; // Incrementer
     
      /// Visitor to compare iterators 
      struct Comparer: public boost::static_visitor<bool> {
        bool operator() (begin_t const& left, begin_t const& right) const
          { return left != right; }
        // A range for is supposed to use only this comparison:
        bool operator() (begin_t const& left, end_t const& right) const
          { return left != right; }
        bool operator() (end_t const& left, begin_t const& right) const
          { return left != right; }
        [[noreturn]] bool operator() (end_t const&, end_t const&) const
          { throw std::logic_error("End iterators should not be compared"); }
      }; // Comparer
      
    }; // class RangeForWrapperIterator<>
    
    
    // forward declaration
    template <typename BeginIter, typename EndIter>
    class RangeForWrapperBox;
    
    /// Collection of static functions for type and iterator manipulation
    struct RangeForWrapperBase {
       /// Extracts the begin iterator from a range object
      template <typename Range>
      static auto extractBegin(Range&& range)
        { using namespace std; return begin(std::forward<Range>(range)); }
      
      /// Extracts the end iterator from a range object
      template <typename Range>
      static auto extractEnd(Range&& range)
        { using namespace std; return end(std::forward<Range>(range)); }
      
      /// Type of begin-of-range iterator
      template <typename Range>
      using begin_t = decltype(extractBegin(std::declval<Range>()));
      
      /// Type of end-of-range iterator
      template <typename Range>
      using end_t = decltype(extractEnd(std::declval<Range>()));
      
      /// Type of iterator box for range-for loops
      template <typename Range>
      using iteratorbox_t = RangeForWrapperBox<begin_t<Range>, end_t<Range>>;
      
      /// Evaluates to true if the range has iterators of the same type
      template <typename Range>
      using same_iterator_types = std::integral_constant
        <bool, std::is_same<begin_t<Range>, end_t<Range>>::value>;
      
    }; // struct RangeForWrapperBase
    
    
    /// Class offering begin/end iterators of the same type out of a range of
    /// iterators of different types.
    template <typename BeginIter, typename EndIter>
    class RangeForWrapperBox {
      /// Type of iterator stored.
      using iterator_t = RangeForWrapperIterator<BeginIter, EndIter>;
      
      iterator_t fBegin; ///< Begin-of-range iterator.
      iterator_t fEnd; ///< End-of-range iterator
      
        public:
      
      /// Type of begin iterator.
      using begin_t = typename iterator_t::begin_t;
      /// Type of end iterator.
      using end_t = typename iterator_t::end_t;
      
      /// Constructor: extracts begin and end iterator from a range
      template <typename Range>
      RangeForWrapperBox(Range&& range)
        : fBegin(RangeForWrapperBase::extractBegin(std::forward<Range>(range)))
        , fEnd(RangeForWrapperBase::extractEnd(std::forward<Range>(range)))
        {}
      
      /// Returns a copy of the begin-of-range iterator.
      iterator_t begin() const { return fBegin; }
      
      /// Returns a copy of the end-of-range iterator.
      iterator_t end() const { return fEnd; }
      
      
    }; // class RangeForWrapperBox
    
    
    /// Tag for internal use.
    struct SameIterTag {};
    
    /// Tag for internal use.
    struct DiffIterTag {};
    
    /// Wraps an object for use in a range-for loop
    /// (same iterator types: pass through)
    // the return type decltype(auto) is necessary to preserve the forwarded
    // referenceness
    template <typename Range>
    auto wrapRangeFor(
      Range&& range,
      std::enable_if_t
        <RangeForWrapperBase::same_iterator_types<Range>::value, SameIterTag>
        = {}
      ) -> decltype(auto)
      { return range; }
    
    
    /// Wraps an object for use in a range-for loop (different iterator types)
    template <
      typename Range,
      typename = std::enable_if_t
        <!RangeForWrapperBase::same_iterator_types<Range>::value>
      >
    auto wrapRangeFor(
      Range&& range,
      std::enable_if_t
        <!RangeForWrapperBase::same_iterator_types<Range>::value, DiffIterTag>
        = {}
      )
      {
        return RangeForWrapperBase::iteratorbox_t<Range>
          (std::forward<Range>(range));
      } // wrapRangeFor()
    
    
    
  } // namespace details
  
  
  /**
   * @brief Wraps an object for use in a range-for loop
   * @tparam Range type of range object (anything with begin() and end())
   * @param range instance of the range object to be wrapped
   *
   * This is necessary only when the argument provides different types for
   * the begin-of-range and end-of-range iterators.
   * This is also superfluous for compilers adhering to C++ 2017 standard,
   * which accepts iterators of different types by requirement.
   * Example of usage:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * Range data; // initialization
   * for (auto&& value: util::wrapRangeFor(data)) // ...
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * where data is supposed to gave begin and end iterators of different types.
   */
  template <typename Range>
  auto wrapRangeFor(Range&& range) -> decltype(auto)
    { return details::wrapRangeFor(std::forward<Range>(range)); }
  
  
  /// Tag marking the use of RangeForWrapperBox
  struct RangeForWrapperTag {};
  
  /// Constant to be used with
  /// `operator|(Range&&, details::RangeForWrapperTag)`.
  constexpr RangeForWrapperTag range_for;
  
  /**
   * @brief Transforms a range so that it can be used in a range-for loop
   * @tparam Range the type of range to be transformed
   * @param range the range to be transformed
   * @return an equivalent range object to be used in a range-for loop
   * 
   * This is necessary only when the argument provides different types for
   * the begin-of-range and end-of-range iterators.
   * This is also superfluous for compilers adhering to C++ 2017 standard,
   * which accepts iterators of different types by requirement.
   * Example of usage:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * Range data; // initialization
   * for (auto&& value: data | util::range_for) // ...
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * where data is supposed to gave begin and end iterators of different types.
   */
  template <typename Range>
  auto operator| (Range&& range, RangeForWrapperTag) -> decltype(auto)
    { return details::wrapRangeFor(std::forward<Range>(range)); }
  
  
} // namespace util


#endif // LARDATA_UTILITIES_RANGEFORWRAPPER_H
