/**
 * @file    StatCollector.h
 * @brief   Clasess gathering simple statistics
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    December 23rd, 2014
 * 
 * @todo Need a unit test!
 */

#ifndef STATCOLLECTOR_H
#define STATCOLLECTOR_H 1

// C/C++ standard libraries
#include <cmath> // std::sqrt()
#include <limits> // std::numeric_limits<>
#include <algorithm> // std::for_each()
#include <initializer_list>


namespace lar {
  namespace util {
    
    /// Returns the square of the specified value
    template <typename T>
    inline T sqr(T const& v) { return v*v; }
    
    
    /** ************************************************************************
     * @brief Collects statistics on a single quantity (weighted)
     * @tparam T type of the quantity (and of weight)
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
     * that should print: "Statistics from 3 entries: 3.8".
     */
    template <typename T>
    class StatCollector {
        public:
      using This_t = StatCollector<T>; ///< this type
      using Data_t = T; ///< type of the data
      using Weight_t = T; ///< type of the weight
      
      // default constructor, destructor and all the rest
      
      /// Adds one entry with specified value and weight
      void add(Data_t value, Weight_t weight = Weight_t(1.0));
      
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
      
      /// Returns the value average (no check performed)
      Weight_t Average() const { return Sum() / Weights(); }
      
      /// Returns the square of the RMS of the values (no check performed)
      Weight_t RMS2() const { return SumSq() / Weights() - sqr(Average()); }
      
      /// Returns the sum of the weights
      Weight_t RMS() const { return std::sqrt(RMS2()); }
      
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
      
      
      /// Returns the accumulated minimum, or a very large number if no values
      Data_t min() const { return minimum; }
      
      /// Returns the accumulated maximum, or a very small number if no values
      Data_t max() const { return maximum; }
      
      
        protected:
      /// the accumulated minimum
      Data_t minimum = std::numeric_limits<Data_t>::max();
      
      /// the accumulated maximum
      Data_t maximum = std::numeric_limits<Data_t>::min();
      
    }; // class MinMaxCollector
    
    
  } // namespace util
} // namespace lar


//==============================================================================
//=== template implementation
//==============================================================================

//******************************************************************************
//***  StatCollector
//***

template <typename T>
void lar::util::StatCollector<T>::add
  (Data_t value, Weight_t weight /* = Weight_t(1.0) */)
{
  ++n;
  w += weight;
  x += weight * value;
  x2 += weight * sqr(value);
} // StatCollector<T>::add()


template <typename T>
void lar::util::StatCollector<T>::clear() {
  n = 0;
  w = 0.;
  x = 0.;
  x2 = 0.;
} // StatCollector<T>::add()



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
  { add(values.begin(), values.end()); }


template <typename T> template <typename Iter>
inline lar::util::MinMaxCollector<T>& lar::util::MinMaxCollector<T>::add
  (Iter begin, Iter end)
{
  std::for_each(begin, end, [this](Data_t value) { this->add(value); });
  return *this;
} // lar::util::MinMaxCollector<T>::add(Iter)

//******************************************************************************


#endif // STATCOLLECTOR_H
