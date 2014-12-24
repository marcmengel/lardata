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


namespace lar {
  namespace util {
    
    /// Returns the square of the specified value
    template <typename T>
    inline T sqr(T const& v) { return v*v; }
    
    
    /**
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

#endif // STATCOLLECTOR_H
