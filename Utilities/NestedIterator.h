/**
 * @file   NestedIterator.h
 * @brief  Iterators recursing though nested collections
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   August 22th, 2014
 * 
 * This header is currently a draft with reduced (essential) functionality.
 * Its ambition is to become general enough to be used transparently.
 */

#ifndef NESTEDITERATOR_H
#define NESTEDITERATOR_H

// interface include
#include <type_traits> // std::true_type, std::false_type, std::conditional
#include <iterator> // std::forward_iterator_tag

namespace lar {
  
  namespace details {
    namespace type_traits {
    
      //------------------------------------------------------------------------
      // declarations of helpers for has_const_iterator() function
      template <typename T, bool>
      struct has_const_iterator_struct;
      
      template <typename T>
      constexpr auto has_const_iterator_helper(T* = nullptr)
        -> decltype(typename T::const_iterator(), bool());
      
      // Used as fallback when SFINAE culls the template method
      constexpr bool has_const_iterator_helper(...);
      //------------------------------------------------------------------------
      
    } // namespace type_traits
  } // namespace details
  
  template <typename T>
  struct has_const_iterator:
    public details::type_traits::has_const_iterator_struct
      <T, details::type_traits::has_const_iterator_helper((T*)(nullptr))>
    {};
  
  /// Internal helper class: actual implementation of nested iterator
  template <typename ITER>
  class deep_const_fwd_iterator_nested;
  
  
  /// Deep iterator
  /// @todo write documentation about how to use it
/*  template <typename CITER>
  using deep_fwd_const_iterator = std::conditional<
    has_const_iterator<typename CITER::value_type>(),
    deep_const_fwd_iterator_nested<CITER>,
    CITER
    >;
*/
  
  template <typename CITER>
  using double_fwd_const_iterator = deep_const_fwd_iterator_nested<CITER>;

} // namespace lar


namespace std {
  template <typename CITER>
  void swap(lar::deep_const_fwd_iterator_nested<CITER>& a,
    lar::deep_const_fwd_iterator_nested<CITER>& b);
  
} // namespace std


//------------------------------------------------------------------------------

namespace lar {

  namespace details {
    namespace type_traits {
    
      //------------------------------------------------------------------------
      // helpers for has_const_iterator() function
      template <typename T, bool>
      struct has_const_iterator_struct: public std::false_type {};
      
      template <typename T>
      struct has_const_iterator_struct<T, true>: public std::true_type {};
      
      // Culled by SFINAE if T::const_iterator does not exist
      // or is not accessible or not default-constructable
      template <typename T>
      constexpr auto has_const_iterator_helper(T* = nullptr)
        -> decltype(typename T::const_iterator(), bool()) 
        { return true; }
      
      // Used as fallback when SFINAE culls the template method
      constexpr bool has_const_iterator_helper(...) { return false; }
      //------------------------------------------------------------------------
      
    } // namespace type_traits
  } // namespace details
  
  
  //---
  //--- deep_const_fwd_iterator_nested declaration
  //---
  template <typename ITER>
  class deep_const_fwd_iterator_nested: public std::forward_iterator_tag {
      public:
    using OuterIterator_t = ITER;
    using InnerIterator_t
  //    = deep_fwd_const_iterator<typename ITER::value_type::const_iterator>;
      = typename ITER::value_type::const_iterator;
    
  //  using iterator_type = deep_fwd_const_iterator<OuterIterator_t>;
    using iterator_type = deep_const_fwd_iterator_nested<OuterIterator_t>;
    
    /// Type of the value pointed by the iterator
    using value_type = typename InnerIterator_t::value_type;
    
    
    struct BeginPositionTag {};
    struct EndPositionTag {};
    
    static constexpr BeginPositionTag begin = {};
    static constexpr EndPositionTag end = {};
    
    /// Default constructor (invalid iterator)
    deep_const_fwd_iterator_nested() = default;
    
    /// Copy constructor (default behaviour)
    deep_const_fwd_iterator_nested(const iterator_type&) = default;
    
    /// Constructor, starts from the container at the specified iterator
    deep_const_fwd_iterator_nested(OuterIterator_t src);
    
    /// Constructor, starts from the beginning of the specified container
    template <class CONT>
    deep_const_fwd_iterator_nested(CONT src, BeginPositionTag);
    
    /// Constructor, initializes with past-to-end of the specified container
    template <class CONT>
    deep_const_fwd_iterator_nested(CONT src, EndPositionTag);
    
    
    /// Prefix increment operator: returns this operator, incremented
    iterator_type& operator++();
    
    /// Postfix increment operator: returns this operator, then increments it
    iterator_type operator++(int)
      { iterator_type old(*this); this->operator++(); return old; }
    
    ///@{
    /// @name Dereference operators
    const value_type& operator*() const { return *inner_iter; }
    const value_type& operator->() const { return *inner_iter; }
    ///@}
    
    
    /// Assignment operator (default behaviour)
    iterator_type& operator= (const iterator_type&) = default;
    
    ///@{
    /// @name Comparison operators
    /// Returns true if the two iterators are equivalent
    bool operator== (const iterator_type& as) const 
      { return (as.outer_iter == outer_iter) && (as.inner_iter == inner_iter); }
    /// Returns true if the two iterators are not equivalent
    bool operator!= (const iterator_type& as) const 
      { return (as.outer_iter != outer_iter) || (as.inner_iter != inner_iter); }
    ///@}
    
    
    /// Swaps this with the specified iterator
    void swap(iterator_type& with);
    
      protected:
    OuterIterator_t outer_iter; ///< points to current inner container
    
    InnerIterator_t inner_iter; ///< points to the current element
    InnerIterator_t inner_end; ///< stores the end of current inner container
    
    
      private:
    void init_inner();
  }; // class deep_const_fwd_iterator_nested<>
  
  
  //---
  //--- deep_const_fwd_iterator_nested implementation
  //---
  
  template <typename ITER>
  deep_const_fwd_iterator_nested<ITER>::deep_const_fwd_iterator_nested
    (OuterIterator_t src):
    outer_iter(src)
    { init_inner(); }
  
  
  template <typename ITER> template <class CONT>
  deep_const_fwd_iterator_nested<ITER>::deep_const_fwd_iterator_nested
    (CONT cont, BeginPositionTag):
    outer_iter(std::begin(cont))
    { if (outer_iter != std::end(cont)) init_inner(); }
  
  
  template <typename ITER> template <class CONT>
  deep_const_fwd_iterator_nested<ITER>::deep_const_fwd_iterator_nested
    (CONT cont, EndPositionTag):
    outer_iter(std::end(cont))
    {}
  
  
  template <typename ITER>
  void deep_const_fwd_iterator_nested<ITER>::init_inner() {
    inner_iter = std::begin(*outer_iter);
    inner_end = std::end(*outer_iter);
  } // deep_const_fwd_iterator_nested<>::init_inner()
  
  
  template <typename ITER>
  deep_const_fwd_iterator_nested<ITER>&
  deep_const_fwd_iterator_nested<ITER>::operator++() {
    while(++inner_iter == inner_end) {
      ++outer_iter;
      init_inner();
    } // while inner iterator ended
    return *this;
  } // deep_const_fwd_iterator_nested<>::operator++()
  
  
  template <typename ITER>
  void deep_const_fwd_iterator_nested<ITER>::swap(iterator_type& with) {
    std::swap(outer_iter, with.outer_iter);
    std::swap(inner_iter, with.inner_iter);
    std::swap(inner_end, with.inner_end);
  } // deep_const_fwd_iterator_nested<>::swap()
  
  
} // namespace lar


namespace std {
  template <typename CITER>
  inline void swap(lar::deep_const_fwd_iterator_nested<CITER>& a,
    lar::deep_const_fwd_iterator_nested<CITER>& b)
    { a.swap(b); }
} // namespace std



#endif // NESTEDITERATOR_H
