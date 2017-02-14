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
      struct Dumper;
      
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
      
      /// Visitor to dereference an iterator.
      struct Dereferencer: public boost::static_visitor<value_type> {
        value_type operator() (begin_t const& iter) const { return *iter; }
        [[noreturn]] value_type operator() (end_t const&) const
          { throw std::logic_error("End iterator should not be dereferenced"); }
      }; // Dereferencer
      
      /// Visitor to increment an iterator.
      struct Incrementer: public boost::static_visitor<> {
        void operator() (begin_t& iter) const { ++iter; }
        [[noreturn]] void operator() (end_t&) const
          { throw std::logic_error("End iterator should not be incremented"); }
      }; // Incrementer
      
      /// Visitor to compare iterators.
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
    
    
    
    /// Class defining types and traits for RangeForWrapperBox
    template <typename RangeRef>
    struct RangeForWrapperTraits {
      
      using RangeRef_t = RangeRef; ///< Type of the stored reference.
      
      ///< Type of the stored range (constantness is preserved).
      using Range_t = std::remove_reference_t<RangeRef_t>;
      
      /// Extractor of the begin iterator from a range.
      static auto extractBegin(RangeRef_t range)
        { using namespace std; return begin(static_cast<RangeRef_t>(range)); }
      
      /// Extracts the end iterator from a range object.
      static auto extractEnd(RangeRef_t range)
        { using namespace std; return end(static_cast<RangeRef_t>(range)); }
      
      /// Type of wrapped begin iterator.
      using BeginIter_t = decltype(extractBegin(std::declval<RangeRef_t>()));
      
      /// Type of wrapped end iterator.
      using EndIter_t = decltype(extractEnd(std::declval<RangeRef_t>()));
      
      /// True if the range has iterators of the same type.
      static constexpr bool sameIteratorTypes
        = std::is_same<BeginIter_t, EndIter_t>();
      
      /// Type of wrapper iterators (same for begin and end iterators).
      using Iterator_t = RangeForWrapperIterator<BeginIter_t, EndIter_t>;
      
    }; // class RangeForWrapperTraits<>
    
    
    /**
     * @brief Class offering begin/end iterators of the same type out of a range
     *        of iterators of different types.
     * @tparam RangeRef type of reference to be stored (constantness embedded)
     * 
     * The class steals (moves) the value if `RangeRef` is a rvalue reference
     * type, while it just references the original one otherwise.
     */
    template <typename RangeRef>
    class RangeForWrapperBox {
      
      static_assert(std::is_reference<RangeRef>::value,
        "RangeForWrapperBox requires a reference type.");
      
      using Traits_t = RangeForWrapperTraits<RangeRef>;
      
        public:
      
      // Import traits
      using RangeRef_t = typename Traits_t::RangeRef_t;
      using Range_t = typename Traits_t::Range_t;
      
      /// Type of wrapper iterators (same for begin and end iterators).
      using Iterator_t = typename Traits_t::Iterator_t;
      
      /// Constructor: references the specified range (lvalue reference).
      RangeForWrapperBox(Range_t& range)
        : fRange(range)
        {}
      
      /// Constructor: references the specified range (rvalue reference).
      RangeForWrapperBox(Range_t&& range)
        : fRange(std::move(range))
        {}
      
      /// Returns a begin-of-range iterator.
      Iterator_t begin()
        {
          return Iterator_t
            (Traits_t::extractBegin(static_cast<RangeRef_t>(fRange)));
        }
      
      /// Returns a end-of-range iterator.
      Iterator_t end()
        {
          return Iterator_t
            (Traits_t::extractEnd(static_cast<RangeRef_t>(fRange)));
        }
      
        private:
      
      struct DataBox {
        
        using Stored_t = std::conditional_t<
          std::is_rvalue_reference<RangeRef_t>::value,
          std::remove_reference_t<RangeRef_t>,
          RangeRef_t
          >;
        using Data_t = std::remove_reference_t<Stored_t>;
        
        Stored_t data;
        
        // only one of these is valid...
        DataBox(Data_t& data): data(data) {}
        DataBox(Data_t&& data): data(std::move(data)) {}
        
        operator RangeRef_t() const { return RangeRef_t(data); }
        operator RangeRef_t() { return RangeRef_t(data); }
        
      }; // DataBox
      
      DataBox fRange; ///< A reference to the original range.
      
    }; // class RangeForWrapperBox<>
    
    
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
        <RangeForWrapperTraits<Range>::sameIteratorTypes, SameIterTag>
        = {}
      ) -> decltype(auto)
      { return std::forward<Range>(range); }
    
    
    /// Wraps an object for use in a range-for loop (different iterator types)
    template <
      typename Range,
      typename = std::enable_if_t
        <!RangeForWrapperTraits<Range>::sameIteratorTypes>
      >
    auto wrapRangeFor(
      Range&& range,
      std::enable_if_t
        <!RangeForWrapperTraits<Range>::sameIteratorTypes, DiffIterTag>
        = {}
      )
      { 
        return RangeForWrapperBox<decltype(range)>
          (static_cast<decltype(range)>(range));
      }
    
    
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
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * where data is supposed to gave begin and end iterators of different types.
   */
  template <typename Range>
  auto operator| (Range&& range, RangeForWrapperTag) -> decltype(auto)
    { return details::wrapRangeFor(std::forward<Range>(range)); }
  
  
} // namespace util


#endif // LARDATA_UTILITIES_RANGEFORWRAPPER_H
