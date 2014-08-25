/**
 * @file   CountersMap.h
 * @brief  Map of counters, stored compactly
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   August 22th, 2014
 */

#ifndef COUNTERSMAP_H
#define COUNTERSMAP_H

// interface include
#include <map>
#include <array>
#include <memory> // std::allocator<>
#include <iterator> // std::forward_terator_tag

namespace lar {
  
  /// Returns true if the argument is a power of 2
  constexpr bool IsPowerOf2(unsigned long long int v)
    { return v & 1? v == 1: IsPowerOf2(v >> 1); }
  
  /// Returns the position of the first set bit (0 for LSB)
  constexpr int LowestSetBit(unsigned long long int v);
  
  
  
  /**
   * @brief Map storing counters in a compact way
   * @param KEY the type of the key of the counters map
   * @param COUNTER the type of a basic counter (can be signed or unsigned)
   * @param BLOCKSIZE the number of counters in a cluster
   * @param ALLOC allocator for the underlying STL map
   * @param SUBCOUNTERS split each counter in subcounters (not implemented yet)
   *
   * This class is designed for the need of a vast number of counters with
   * a integral numerical key, when the counter keys are usually clustered.
   * This can be more or less efficient than a straight counters STL map
   * according to how often the counters are clustered (if that does not happen
   * often, CountersMap can have considerable memory overhead).
   * 
   * Counters are allocated in contiguous blocks of BLOCKSIZE counters.
   * The selling point here is that a map node has some overhead (typically
   * at least 3 pointers) and dynamically allocating it costs a lot (40 bytes
   * have been observed). If you need a counter with a range of 100 (1 byte),
   * that's far from optimal in many aspects (memory allocation also takes
   * time).
   * 
   * The requirement of the key to be numerical is so that the concept of
   * "next counter" is well defined and we can store contiguous counters
   * in a fixed structure.
   * 
   * <h3>Subcounters</h3>
   * The idea behind subcounters is that you migt want to split a counter into
   * subcounters to save memory if the maximum counter value is smaller than
   * the range of the counter type.
   * <strong>The implementation is delayed</strong> since in the end the same
   * effect can be achieved by using a small counter type (e.g. signed char),
   * unless the range is smaller that 16 (or 4, or 2), in which case the
   * character can be split into bits. That will cause some overhead for
   * increment and decrement instructions.
   */
  template <
    typename KEY,
    typename COUNTER,
    size_t SIZE,
    typename ALLOC = std::allocator<std::pair<KEY,std::array<COUNTER, SIZE>>>,
    unsigned int SUBCOUNTERS=1
    >
  class CountersMap {
    static_assert(SUBCOUNTERS == 1, "subcounters not implemented yet");
    static_assert(IsPowerOf2(SIZE),
      "the size of the cluster of counters must be a power of 2");
    
      public:
    using Key_t = KEY; ///< type of counter key in the map
    using Counter_t = COUNTER; ///< type of the single counter
    using Allocator_t = ALLOC; ///< type of the single counter

    /// This class
    using CounterMap_t = CountersMap<KEY, COUNTER, SIZE, ALLOC, SUBCOUNTERS>;
    
    /// Number of counters in one counter block
    static constexpr size_t NCounters = SIZE;
    
    /// Type of the subcounter (that is, the actual counter)
    using SubCounter_t = Counter_t;
    
    /// type of block of counters (just a STL array until SUBCOUNTERS are in)
    class CounterBlock_t: std::array<Counter_t, NCounters> {
        public:
      using Array_t = std::array<Counter_t, NCounters>; ///< type of base class
      
      /// Default constructor: initializes the array to 0
      CounterBlock_t(): Array_t() { Array_t::fill(0); }
      
      /// Convenience constructor: initializes all counters to 0, except one
      CounterBlock_t(size_t index, Counter_t value): CounterBlock_t()
        { Array_t::operator[](index) = value; }
    }; // CounterBlock_t
    
    /// type of the map used in the implementation
    using BaseMap_t = std::map<Key_t, CounterBlock_t, Allocator_t>;
    
    
    ///@{
    /// @name STL type definitions
    using value_type = typename BaseMap_t::value_type;
    using mapped_type = typename BaseMap_t::mapped_type;
    // TODO others are missing
    ///@}
    
    
    /// Default constructor (empty map)
    CountersMap() {}
    
    /// Constructor, specifies an allocator
    CountersMap(Allocator_t alloc): BaseMap_t(alloc) {}
    
    
    /// Read-only access to an element; returns 0 if no counter is present
    SubCounter_t operator[] (Key_t key) const { return GetSubCounter(key); }
    
    /**
     * @brief Increments by 1 the specified counter
     * @param keykey of the counter to be increased
     * @return new value of the counter
     */
    SubCounter_t increment(Key_t key);
    
    /// Increments by 1 the specified counter, returning its new value
    SubCounter_t decrement(Key_t key);
    
    
    ///@{
    /// @name Iterators (experimental)
    // this stuff needs serious work to be completed
    
    /// Returns an iterator to the begin of the counters
    NestedForwardIterator<CounterMap_t> begin();
    
    /// Returns an iterator past-the-end of the counters
    NestedForwardIterator<CounterMap_t> end();
    ///@}
    
      private:
    
    using BlockKey_t = Key_t; ///< type of block key
    using CounterIndex_t = Key_t; ///< type of index in the block
    
    
    /// Structure with the index of the counter, split as needed
    struct CounterKey_t {
      BlockKey_t block; ///< key of the counter block
      CounterIndex_t counter; ///< index of the counter in the block
      // there should be a subcluster index here too
      
      CounterKey_t(): block(0), counter(0) {}
      
      /// Constructor from a pair
      CounterKey_t(BlockKey_t major, CounterIndex_t minor):
        block(major), counter(minor) {}
      
      /// Initialize from a mangled key
      CounterKey_t(Key_t key):
        CounterKey_t(key >> MinorKeyBits, key & MinorKeyMask) {}
      
      /// Number of bits for the minor key
      static constexpr Key_t MinorKeyBits = LowestSetBit(NCounters) + 1;
      
      /// Bit mask for the minor key
      static constexpr Key_t MinorKeyMask = Key_t(1) << MinorKeyBits - 1;
    
    }; // struct CounterKey_t
    
    
    BaseMap_t counter_map; ///< the actual data structure for counters
    
    
    /// Returns the value of the counter at the specified split key
    Counter_t GetCounter(CounterKey_t key) const;
    
    /// Returns the value of the subcounter at the specified split key
    // Since subcounters are not implemented yet, this is a no-op
    SubCounter_t GetSubCounter(CounterKey_t key) const
      { return GetCounter(key); }
    
    /// Returns the counter at the specified split key
    Counter_t& GetOrCreateCounter(CounterKey_t key);
    
    
    /// Returns a split key corresponding to the specified key
    static CounterKey_t SplitKey(Key_t key);
    
    /// Adds a delta to the specified counter
    SubCounter_t unchecked_add(CounterKey_t key, SubCounter_t delta);
    
  }; // class CountersMap
  
  
  /// Namespace hiding implementation details
  namespace details {
    /// Namespace specific to counter map
    namespace counters_map {
    } // namespace counters_map
  } // namespace details
  
} // namespace lar


//------------------------------------------------------------------------------
#include <algorithm> // std::max()
#include <vector>
#include <iostream>
#include <array>

namespace lar {
  
  namespace details {
    /// Internally used by LowestSetBit
    inline constexpr int LowestSetBitScaler(unsigned long long int v, int b)
      { return (v & 1)? b: LowestSetBitScaler(v >> 1, b + 1); }
    
    namespace counters_map {
      static constexpr bool bDebug = true;

      
    } // namespace counters_map
  } // namespace details
  
  
  inline constexpr int LowestSetBit(unsigned long long int v)
    { return v == 0? -1: details::LowestSetBitScaler(v, 0); }
  
  
  
  template <typename K, typename C, size_t S, typename A, unsigned int SUB>
  inline typename CountersMap<K, C, S, A, SUB>::Counter_t
    CountersMap<K, C, S, A, SUB>::GetCounter(CounterKey_t key) const
  {
    typename CounterBlock_t::const_iterator iBlock
      = counter_map.find(key.block);
    return (iBlock == counter_map.end())? 0: iBlock->second[key.counter];
  } // CountersMap<>::GetCounter() const
  
  
  template <typename K, typename C, size_t S, typename A, unsigned int SUB>
  typename CountersMap<K, C, S, A, SUB>::SubCounter_t
    CountersMap<K, C, S, A, SUB>::unchecked_add
    (CounterKey_t key, SubCounter_t delta)
  {
    // take it low level here
    // first get the iterator to the block
    typename CounterBlock_t::iterator iBlock
      = counter_map.lower_limit(key.block);
    if (iBlock->first == key.block) { // sweet, we have that counter already
      return iBlock->second[key.counter] += delta;
    }
    
    // no counter there yet: create one;
    // hint to insert before the block just after what we found
    // (this is optimal in STL map for C++11);
    // unless we are at the end already
    if (iBlock != counter_map.end()) ++iBlock;
    // create a block using the special constructor
    counter_map.emplace_hint(iBlock, { key.counter, delta });
    return delta;
  } // CountersMap<>::unchecked_add() const
  
  
  template <typename K, typename C, size_t S, typename A, unsigned int SUB>
  inline typename CountersMap<K, C, S, A, SUB>::Counter_t&
    CountersMap<K, C, S, A, SUB>::GetOrCreateCounter(CounterKey_t key)
    { return counter_map[key.block][key.counter]; }
  
  
} // namespace lar


#endif // BULKALLOCATOR_H
