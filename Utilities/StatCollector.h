/**
 * @file    StatCollector.h
 * @brief   Clasess gathering simple statistics
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    December 23rd, 2014
 *
 * Currently includes:
 *  - MinMaxCollector to extract data range
 *  - StatCollector to extract simple statistics (average, RMS etc.)
 * 
 */

#ifndef STATCOLLECTOR_H
#define STATCOLLECTOR_H 1

// C/C++ standard libraries
#include <cmath> // std::sqrt()
#include <limits> // std::numeric_limits<>
#include <initializer_list>
#include <iterator> // std::begin(), std::end()
#include <utility> // std::forward()
#include <algorithm> // std::for_each()
#include <stdexcept> // std::range_error


namespace lar {
  namespace util {
    
    
    /// Returns the square of the specified value
    template <typename T>
    inline T sqr(T const& v) { return v*v; }
    
    
    /// A unary functor returning its own argument (any type)
    struct identity {
      template<typename T>
      constexpr auto operator()(T&& v) const noexcept
        -> decltype(std::forward<T>(v))
      {
        return std::forward<T>(v);
      } // operator()
    }; // class identity
    
    
    /** ************************************************************************
     * @brief Collects statistics on a single quantity (weighted)
     * @tparam T type of the quantity
     * @tparam W type of the weight (as T by default)
     * 
     * This is a convenience class, as easy to use as:
     *     
     *     lar::util::StatCollector<double> stat;
     *     stat.add(3.0, 2.0);
     *     stat.add(4.0, 2.0);
     *     stat.add(5.0, 1.0);
     *     std::cout << "Statistics from " << stat.N() << " entries: "
     *       << stat.Average() << std::endl;
     *     
     * or also
     *     std::vector<std::pair<double, double>> values({
     *       { 3.0, 2.0 },
     *       { 4.0, 2.0 },
     *       { 5.0, 1.0 },
     *       });
     *     lar::util::StatCollector<double> stat;
     *     stat.add_weighted(values.begin(), values.end());
     *     std::cout << "Statistics from " << stat.N() << " entries: "
     *       << stat.Average() << std::endl;
     *     
     * that should both print: "Statistics from 3 entries: 3.8".
     * 
     * Other functions are available allowing addition of weighted and
     * unweighted data from collections.
     * For additional examples, see the unit test StatCollector_test.cc .
     */
    template <typename T, typename W = T>
    class StatCollector {
        public:
      using This_t = StatCollector<T>; ///< this type
      using Data_t = T; ///< type of the data
      using Weight_t = W; ///< type of the weight
      
      // default constructor, destructor and all the rest
      
      /// @{
      /// @name Add elements
      
      /// Adds one entry with specified value and weight
      void add(Data_t value, Weight_t weight = Weight_t(1.0));
      
      /**
       * @brief Adds entries from a sequence with weight 1
       * @tparam Iter forward iterator to the elements to be added
       * @param begin iterator pointing to the first element to be added
       * @param end iterator pointing after the last element to be added
       * 
       * The value pointed by the iterator must be convertible to the Data_t
       * type.
       */
      template <typename Iter>
      void add_unweighted(Iter begin, Iter end)
        {  add_unweighted(begin, end, identity()); }
      
      /**
       * @brief Adds entries from a sequence with weight 1
       * @tparam Iter forward iterator to the elements to be added
       * @tparam Pred a predicate to extract the element from iterator value
       * @param begin iterator pointing to the first element to be added
       * @param end iterator pointing after the last element to be added
       * @param extractor the predicate extracting the value to be inserted
       *
       * The predicate is required to react to a call like with:
       *     
       *     Data_t Pred::operator() (typename Iter::value_type);
       *     
       */
      template <typename Iter, typename Pred>
      void add_unweighted(Iter begin, Iter end, Pred extractor);
      
      /**
       * @brief Adds all entries from a container, with weight 1
       * @tparam Cont type of container of the elements to be added
       * @tparam Pred a predicate to extract the element from iterator value
       * @param cont container of the elements to be added
       * @param extractor the predicate extracting the value to be inserted
       * 
       * The predicate is required to react to a call like with:
       *     
       *     Data_t Pred::operator() (typename Cont::value_type);
       *     
       * The container must support the range-based for loop syntax, that is
       * is must have std::begin<Cont>() and std::end<Cont>() defined.
       */
      template <typename Cont, typename Pred>
      void add_unweighted(Cont cont, Pred extractor)
        { add_unweighted(std::begin(cont), std::end(cont), extractor); }
      
      /**
       * @brief Adds all entries from a container, with weight 1
       * @tparam Cont type of container of the elements to be added
       * @param cont container of the elements to be added
       * 
       * The container must support the range-based for loop syntax, that is
       * is must have std::begin<Cont>() and std::end<Cont>() defined.
       * The value in the container must be convertible to the Data_t type.
       */
      template <typename Cont>
      void add_unweighted(Cont cont)
        { add_unweighted(std::begin(cont), std::end(cont)); }
      
      
      /**
       * @brief Adds entries from a sequence with individually specified weights
       * @tparam VIter forward iterator to the elements to be added
       * @tparam WIter forward iterator to the weights
       * @tparam VPred a predicate to extract the element from iterator value
       * @tparam WPred a predicate to extract the weight from iterator value
       * @param begin_value iterator pointing to the first element to be added
       * @param end_value iterator pointing after the last element to be added
       * @param begin_weight iterator pointing to the weight of first element
       * @param value_extractor predicate extracting the value to be inserted
       * @param weight_extractor predicate extracting the value to be inserted
       * 
       * Each value is added with the weight pointed by the matching element in
       * the list pointed by begin_weight: the element `*(begin_value)` will
       * have weight `*(begin_weight)`, the next element `*(begin_value + 1)`
       * will have weight `*(begin_weight + 1)`, etc.
       * 
       * The predicates are required to react to a call like with:
       *     
       *     Data_t VPred::operator() (typename VIter::value_type);
       *     Weight_t WPred::operator() (typename WIter::value_type);
       *     
       */
      template <
        typename VIter, typename WIter,
        typename VPred, typename WPred = identity
        >
      void add_weighted(
        VIter begin_value, VIter end_value,
        WIter begin_weight,
        VPred value_extractor,
        WPred weight_extractor = WPred()
        );
      
      
      /**
       * @brief Adds entries from a sequence with individually specified weights
       * @tparam Iter forward iterator to (value, weight) pairs to be added
       * @param begin iterator pointing to the first element to be added
       * @param end iterator pointing after the last element to be added
       * 
       * The value pointed by the iterator must be a pair with first element
       * convertible to the Data_t and the second element convertible to
       * Weight_t.
       * For more complicate structures, use the version with two predicates
       * (using the weight iterator the same as the value iterator).
       */
      template <typename Iter>
      void add_weighted(Iter begin, Iter end);
      
      /**
       * @brief Adds entries from a sequence with individually specified weights
       * @tparam Cont type of container of (value, weight) pairs to be added
       * @param cont container of (value, weight) pairs to be added
       * 
       * The values in the container must be pairs with first element
       * convertible to the Data_t and the second element convertible to
       * Weight_t.
       */
      template <typename Cont>
      void add_weighted(Cont cont)
        { add_weighted(std::begin(cont), std::end(cont)); }
      
      ///@}
      
      /// Clears all the statistics
      void clear();
      
      /// @{
      /// @name Statistic retrieval
      
      /// Returns the number of entries added
      int N() const { return n; }
      
      /// Returns the sum of the weights
      Weight_t Weights() const { return w; }
      
      /// Returns the weighted sum of the values
      Weight_t Sum() const { return x; }
      
      /// Returns the weighted sum of the square of the values
      Weight_t SumSq() const { return x2; }
      
      /**
       * @brief Returns the value average
       * @return the value average
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t Average() const;
      
      /**
       * @brief Returns the square of the RMS of the values
       * @return the square of the RMS of the values
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t RMS2() const;
      
      /**
       * @brief Returns the root mean square
       * @return the RMS of the values
       * @throws std::range_error if the total weight is 0 (see RMS2())
       * @throws std::range_error if RMS2() is negative (due to rounding errors)
       */
      Weight_t RMS() const;
      
      
      /**
       * @brief Returns the arithmetic average of the weights
       * @return the weight average
       * @throws std::range_error if no entry was added
       */
      Weight_t AverageWeight() const;
      
      /// @}
      
        protected:
      int n = 0;        ///< number of accumulated entries
      Weight_t w = 0.;  ///< sum of weights
      Weight_t x = 0.;  ///< sum of quantities
      Weight_t x2 = 0.; ///< sum of square quantities
      
    }; // class StatCollector<>
    
    
    
    /**
     * @brief Keeps track of the minimum and maximum value we observed
     * @tparam T type of datum
     * 
     * Implementation note: a similar class with an arbitrary comparison rule
     * would require a careful choice of initial values for minimum and maximum,
     * or a entry count that should be checked at each insertion.
     * We save that slight overhead here.
     */
    template <typename T>
    class MinMaxCollector {
        public:
      using Data_t = T; ///< type of data we collect
      using This_t = MinMaxCollector<T>; ///< this type
      
      //@{
      /// Default constructor: no data collected so far.
      MinMaxCollector() = default;
      
      /// Constructor: starts with parsing the specified data
      MinMaxCollector(std::initializer_list<Data_t> init)
        { add(init); }
      
      /**
       * @brief Include a sequence of values in the statistics
       * @tparam Iter type of an iterator on values
       * @param begin iterator pointing to the first value to be included
       * @param end iterator pointing to the last value to be included
       */
      template <typename Iter>
      MinMaxCollector(Iter begin, Iter end)
        { add(begin, end); }
      //@}
      
      
      // default copy and move constructor and assignment, and destructor
      
      /// @{
      /// @name Inserters
      /**
       * @brief Include a single value in the statistics
       * @param value the value to be added
       * @return this object
       */
      This_t& add(Data_t value);
      
      /**
       * @brief Include a sequence of values in the statistics
       * @param values the values to be added
       * @return this object
       */
      This_t& add(std::initializer_list<Data_t> values);
      
      /**
       * @brief Include a sequence of values in the statistics
       * @tparam Iter type of an iterator on values
       * @param begin iterator pointing to the first value to be included
       * @param end iterator pointing to the last value to be included
       * @return this object
       */
      template <typename Iter>
      This_t& add(Iter begin, Iter end);
      /// @}
      
      
      /// Returns whether at least one datum has been added
      bool has_data() const { return minimum <= maximum; }
      
      /// Returns the accumulated minimum, or a very large number if no values
      Data_t min() const { return minimum; }
      
      /// Returns the accumulated maximum, or a very small number if no values
      Data_t max() const { return maximum; }
      
      
      /// Removes all statistics and reinitializes the object
      void clear();
      
        protected:
      /// the accumulated minimum
      Data_t minimum = std::numeric_limits<Data_t>::max();
      
      /// the accumulated maximum
      Data_t maximum = std::numeric_limits<Data_t>::min();
      
    }; // class MinMaxCollector<>
    
    
  } // namespace util
} // namespace lar


//==============================================================================
//=== template implementation
//==============================================================================

//******************************************************************************
//***  StatCollector
//***

template <typename T, typename W>
void lar::util::StatCollector<T, W>::add
  (Data_t value, Weight_t weight /* = Weight_t(1.0) */)
{
  ++n;
  w += weight;
  x += weight * value;
  x2 += weight * sqr(value);
} // StatCollector<T, W>::add()


template <typename T, typename W>
template <typename Iter, typename Pred>
void lar::util::StatCollector<T, W>::add_unweighted
  (Iter begin, Iter end, Pred extractor)
{
  std::for_each
    (begin, end, [this, extractor](auto item) { this->add(extractor(item)); });
} // StatCollector<T, W>::add_unweighted(Iter, Iter, Pred)


template <typename T, typename W>
template <typename VIter, typename WIter, typename VPred, typename WPred>
void lar::util::StatCollector<T, W>::add_weighted(
  VIter begin_value, VIter end_value,
  WIter begin_weight,
  VPred value_extractor,
  WPred weight_extractor /* = WPred() */
) {
  while (begin_value != end_value) {
    add(value_extractor(*begin_value), weight_extractor(*begin_weight));
    ++begin_value;
    ++begin_weight;
  } // while
} // StatCollector<T, W>::add_weighted(VIter, VIter, WIter, VPred, WPred)


template <typename T, typename W>
template <typename Iter>
void lar::util::StatCollector<T, W>::add_weighted(Iter begin, Iter end) {
  
  std::for_each(begin, end, [this](auto p) { this->add(p.first, p.second); });
  
} // StatCollector<T, W>::add_weighted(Iter, Iter)



template <typename T, typename W>
void lar::util::StatCollector<T, W>::clear() {
  n = 0;
  w = 0.;
  x = 0.;
  x2 = 0.;
} // StatCollector<T, W>::clear()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::Average() const
{
  if (Weights() == Data_t(0))
    throw std::range_error("StatCollector<>::Average(): divide by 0");
  return Sum() / Weights();
} // StatCollector<T, W>::Average()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::RMS2() const
{
  if (Weights() == Data_t(0))
    throw std::range_error("StatCollector<>::RMS2(): divide by 0");
  /// Returns the sum of the weights
  return SumSq() / Weights() - sqr(Average());
} // StatCollector<T, W>::RMS2()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::RMS() const
{
  const Weight_t rms2 = RMS2();
  if (rms2 < Data_t(0))
    throw std::range_error("StatCollector<>::RMS(): negative RMS^2");
  return std::sqrt(rms2);
} // StatCollector<T, W>::RMS()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::AverageWeight() const
{
  if (N() == 0)
    throw std::range_error("StatCollector<>::AverageWeight(): divide by 0");
  return Weights() / N();
} // StatCollector<T, W>::AverageWeight()



//******************************************************************************
//*** MinMaxCollector
//***

template <typename T>
lar::util::MinMaxCollector<T>& lar::util::MinMaxCollector<T>::add(Data_t value)
{
  if (value < minimum) minimum = value;
  if (value > maximum) maximum = value;
  return *this;
} // lar::util::MinMaxCollector<T>::add


template <typename T>
inline lar::util::MinMaxCollector<T>& lar::util::MinMaxCollector<T>::add
  (std::initializer_list<Data_t> values)
  { return add(values.begin(), values.end()); }


template <typename T> template <typename Iter>
inline lar::util::MinMaxCollector<T>& lar::util::MinMaxCollector<T>::add
  (Iter begin, Iter end)
{
  std::for_each(begin, end, [this](Data_t value) { this->add(value); });
  return *this;
} // lar::util::MinMaxCollector<T>::add(Iter)


template <typename T>
inline void lar::util::MinMaxCollector<T>::clear() {
  minimum = std::numeric_limits<Data_t>::max();
  maximum = std::numeric_limits<Data_t>::min();
} // lar::util::MinMaxCollector<T>::clear()

//******************************************************************************


#endif // STATCOLLECTOR_H
