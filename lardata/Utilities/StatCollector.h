/**
 * @file    StatCollector.h
 * @brief   Classes gathering simple statistics
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
#include <array>
#include <tuple>
#include <limits> // std::numeric_limits<>
#include <initializer_list>
#include <iterator> // std::begin(), std::end()
#include <utility> // std::forward()
#include <algorithm> // std::for_each()
#include <stdexcept> // std::range_error


namespace lar {
  namespace util {
    
    
    /// A unary functor returning its own argument (any type)
    struct identity {
      template<typename T>
      constexpr auto operator()(T&& v) const noexcept
        -> decltype(std::forward<T>(v))
      {
        return std::forward<T>(v);
      } // operator()
    }; // class identity
    
    
    namespace details {
      
      /// Class tracking the number of entries and their total weight
      /// @tparam W type of the weight
      template <typename W>
      class WeightTracker {
          public:
        using Weight_t = W; ///< type of the weight
        
        /// Adds the specified weight to the statistics
        void add(Weight_t weight) { ++n; w += weight; }
        
        /// Resets the count
        void clear() { n= 0; w = Weight_t(0); }
        
        /// Returns the number of entries added
        int N() const { return n; }
        
        /// Returns the sum of the weights
        Weight_t Weights() const { return w; }
        
        /**
         * @brief Returns the arithmetic average of the weights
         * @return the weight average
         * @throws std::range_error if no entry was added
         */
        Weight_t AverageWeight() const;
        
        
        /// Returns the square of the specified value
        template <typename V>
        inline constexpr V sqr(V const& v) { return v*v; }
        
          protected:
        int n = 0;                ///< number of added entries
        Weight_t w = Weight_t(0); ///< total weight
        
      }; // class WeightTracker<>
      
      
      /**
       * @brief Class tracking sums of variables up to a specified power
       * @tparam Power power up to which to collect statistics
       * @tparam T type of the quantity
       * @tparam W type of the weight (as T by default)
       * 
       * Note that statistics of order 0 (that is, pertaining only weights)
       * are not collected.
       */
      template <unsigned int PWR, typename T, typename W = T>
      class DataTracker {
          public:
        static constexpr unsigned int Power = PWR;
        static_assert(Power >= 1, "DataTracker must have at least power 1");
        
        using Data_t = T; ///< type of data
        using Weight_t = T; ///< type of weight
        
        /// Default constructor
        DataTracker() { clear(); }
        
        /// Adds the specified weight to the statistics
        void add(Data_t v, Weight_t w)
          {
            Weight_t x = w;
            for (size_t i = 0; i < Power; ++i) sums[i] += (x *= v);
          }
        
        /// Resets the count
        void clear() { sums.fill(Data_t(0)); }
        
        /// Returns the sum of the values to the power N (1 <= N <= 2)
        template <unsigned int N>
        Weight_t SumN() const
          {
            static_assert
              ((N > 0) && (N <= Power), "Invalid sum of power requested");
            return sums[N - 1];
          } // SumN()
        
        /// Returns the sum of the values to the power n (1 <= n <= 2, no check)
        Weight_t Sum(unsigned int n) const { return sums[n - 1]; }
        
        /// Returns the weighted sum of the entries
        Weight_t Sum() const { return SumN<1>(); }
        
          protected:
        std::array<Weight_t, Power> sums;
        
      }; // class DataTracker<>
      
      
      /**
       * @brief Class tracking sums of variables up to power 2
       * @tparam T type of the quantity
       * @tparam W type of the weight (as T by default)
       */
      template <typename T, typename W = T, unsigned int PWR = 2>
      class DataTracker2: public DataTracker<PWR, T, W> {
        using Base_t = DataTracker<PWR, T, W>; ///< base class type
          public:
        using Base_t::Power;
        static_assert(Power >= 2, "DataTracker2 must have Power >= 2");
        using Weight_t = typename Base_t::Weight_t;
        
        /// Returns the weighted sum of the square of the entries
        Weight_t SumSq() const
          { return Base_t::sums[1]; }
        //{ return Base_t::SumN<2>(); } // I can't make gcc 4.9.1 digest this...
        
      }; // class DataTracker2
      
      /**
       * @brief Class tracking sums of variables up to power 2
       * @tparam T type of the quantity
       * @tparam W type of the weight (as T by default)
       */
      template <typename T, typename W = T, unsigned int PWR = 3>
      class DataTracker3: public DataTracker2<T, W, PWR> {
        using Base_t = DataTracker2<T, W, PWR>; ///< base class type
          public:
        using Base_t::Power;
        static_assert(Power >= 3, "DataTracker3 must have Power >= 3");
        using Weight_t = typename Base_t::Weight_t;
        
        /// Returns the weighted sum of the square of the entries
        Weight_t SumCube() const
          { return Base_t::sums[2]; }
        //{ return Base_t::SumN<3>(); } // I can't make gcc 4.9.1 digest this...
        
      }; // class DataTracker3
      
    } // namespace details
    
    
    
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
     * 
     * StatCollector::Variance() is known to be very sensitive to rounding
     * errors, since it uses the formula E[x^2] - E^2[x].  If the variance
     * is effectively small the formula can become negative.  In case of 
     * negative value produced by the variance formula, the Variance() 
     * method will round up to zero.
     * If you know roughly the average of the items you are
     * add()ing, you can reduce the rounding error by subtracting it from 
     * the input value; you'll have to shift the average back, while the
     * variance will not be affected;
     * also the sums will be shifted. Example:
     *     
     *     // fill the values, shifted
     *     for (auto element: elements)
     *       sc.add(element - elements[0], (*weight)++);
     *     
     *     auto sum_weights = sc.Weights();
     *     auto sum_values = sc.Sum() - elements[0] * sc.Weights();
     *     auto sum_values2 = sc.SumSq() + sqr(elements[0]) * sc.Weights()
     *       + 2. * elements[0] * sc.Sum();
     *     auto average = sc.Average() + elements[0];
     *     auto variance = sc.Variance();
     *     
     * A small variance implies values of similar magnitude, and therefore
     * subtracting any single one of them (in the example, the first one) is
     * more effective than it seems.
     * As a rule of thumb, if you are collecting statistics for elements with
     * N significant bits, a 2N significant bits is recommended for statistics
     * collection. That means `double` for `float`, and `long double` for
     * `double` (although usually `long double` is only marginally more precise
     * than `double`, typically 25 or 50% more).
     */
    template <typename T, typename W = T>
    class StatCollector: public details::WeightTracker<W> {
      using Base_t = details::WeightTracker<W>;
      using Base_t::sqr;
        public:
      using This_t = StatCollector<T, W>; ///< this type
      using Data_t = T; ///< type of the data
      using Weight_t = typename Base_t::Weight_t; ///< type of the weight
      
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
      int N() const { return Base_t::N(); }
      
      /// Returns the sum of the weights
      Weight_t Weights() const { return Base_t::Weights(); }
      
      /// Returns the weighted sum of the values
      Weight_t Sum() const { return x.Sum(); }
      
      /// Returns the weighted sum of the square of the values
      Weight_t SumSq() const { return x.SumSq(); }
      
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
      Weight_t Variance() const;
      
      /**
       * @brief Returns the root mean square
       * @return the RMS of the values
       * @throws std::range_error if the total weight is 0 (see Variance())
       * @throws std::range_error if Variance() is negative (due to rounding errors)
       */
      Weight_t RMS() const;
      
      
      /**
       * @brief Returns the arithmetic average of the weights
       * @return the weight average
       * @throws std::range_error if no entry was added
       */
      Weight_t AverageWeight() const { return Base_t::AverageWeight(); }
      
      /// @}
      
      
        protected:
      using Variable_t = details::DataTracker2<Data_t, Weight_t>;
      
      Variable_t x; ///< accumulator for variable x
      
    }; // class StatCollector<>
    
    
    /** ************************************************************************
     * @brief Collects statistics on two homogeneous quantities (weighted)
     * @tparam T type of the quantities
     * @tparam W type of the weight (as T by default)
     * 
     * This is a convenience class, as easy to use as:
     *     
     *     lar::util::StatCollector2D<double> stat;
     *     stat.add(3.0, 4.0, 2.0);
     *     stat.add(4.0, 3.0, 2.0);
     *     stat.add(5.0, 5.0, 1.0);
     *     std::cout << "Statistics from " << stat.N() << " entries: "
     *       << stat.AverageX() << ", " << stat.AverageY() << std::endl;
     *     
     * or also
     *     std::vector<std::pair<double, double>> values({
     *       { 3.0, 4.0, 2.0 },
     *       { 4.0, 3.0, 2.0 },
     *       { 5.0, 5.0, 1.0 },
     *       });
     *     lar::util::StatCollector<double> stat;
     *     stat.add_weighted(values.begin(), values.end());
     *     std::cout << "Statistics from " << stat.N() << " entries: "
     *       << stat.AverageX() << ", " << stat.AverageY() << std::endl;
     *     
     * that should both print: "Statistics from 3 entries: 3.8, 3.8".
     * 
     * Other functions are available allowing addition of weighted and
     * unweighted data from collections.
     * For additional examples, see the unit test StatCollector_test.cc .
     */
    template <typename T, typename W = T>
    class StatCollector2D: public details::WeightTracker<W> {
        public:
      using Base_t = details::WeightTracker<W>;
      using Base_t::sqr;
        public:
      using This_t = StatCollector2D<T, W>; ///< this type
      using Data_t = T; ///< type of the data
      using Weight_t = typename Base_t::Weight_t; ///< type of the weight

      using Pair_t = std::tuple<Data_t, Data_t>;
      using WeightedPair_t = std::tuple<Data_t, Data_t, Weight_t>;
      
      // default constructor, destructor and all the rest
      
      /// @{
      /// @name Add elements
      
      //@{
      /// Adds one entry with specified values and weight
      void add(Data_t x, Data_t y, Weight_t weight = Weight_t(1.0));
      
      void add(Pair_t value, Weight_t weight = Weight_t(1.0))
        { add(std::get<0>(value), std::get<1>(value), weight); }
      
      void add(WeightedPair_t value)
        { add(std::get<0>(value), std::get<1>(value), std::get<2>(value)); }
      //@}
      
      /**
       * @brief Adds entries from a sequence with weight 1
       * @tparam Iter forward iterator to the pairs to be added
       * @param begin iterator pointing to the first element to be added
       * @param end iterator pointing after the last element to be added
       * 
       * The value pointed by the iterator must be a tuple with types compatible
       * with Pair_t.
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
       *     Pair_t Pred::operator() (typename Iter::value_type);
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
       *     Pair_t Pred::operator() (typename Cont::value_type);
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
       * @param weight_extractor predicate extracting the weight to be inserted
       * 
       * Each element is added with the weight pointed by the matching element
       * in the list pointed by begin_weight: the element `*(begin_value)` will
       * have weight `*(begin_weight)`, the next element `*(begin_value + 1)`
       * will have weight `*(begin_weight + 1)`, etc.
       * 
       * The predicates are required to react to a call like with:
       *     
       *     Pair_t VPred::operator() (typename VIter::value_type);
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
       * @tparam Iter forward iterator to WeightedPair_t elements to be added
       * @param begin iterator pointing to the first element to be added
       * @param end iterator pointing after the last element to be added
       * 
       * The value pointed by the iterator must be a WeightedPair_t or something
       * with elements convertible to its own (Data_t, Data_t and Weight_t).
       * For more complicate structures, use the version with two predicates
       * (using the weight iterator the same as the value iterator).
       */
      template <typename Iter>
      void add_weighted(Iter begin, Iter end);
      
      /**
       * @brief Adds entries from a sequence with individually specified weights
       * @tparam Cont type of container of WeightedPair_t elements to be added
       * @param cont container of (x, y, weight) pairs to be added
       * 
       * The values in the container must be tuples with first and second
       * element convertible to the Data_t and the third element convertible to
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
      int N() const { return Base_t::N(); }
      
      /// Returns the sum of the weights
      Weight_t Weights() const { return Base_t::Weights(); }
      
      /// Returns the weighted sum of the x values
      Weight_t SumX() const { return x.Sum(); }
      
      /// Returns the weighted sum of the y values
      Weight_t SumY() const { return y.Sum(); }
      
      /// Returns the weighted sum of the square of the x values
      Weight_t SumSqX() const { return x.SumSq(); }
      
      /// Returns the weighted sum of the square of the y values
      Weight_t SumSqY() const { return y.SumSq(); }
      
      /// Returns the weighted sum of the product of x and y values
      Weight_t SumXY() const { return sum_xy; }
      
      /**
       * @brief Returns the x value average
       * @return the x value average
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t AverageX() const;
      
      /**
       * @brief Returns the y value average
       * @return the y value average
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t AverageY() const;
      
      /**
       * @brief Returns the variance of the x values
       * @return the square of the RMS of the x values
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t VarianceX() const;
      
      /**
       * @brief Returns the variance of the y values
       * @return the square of the RMS of the y values
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t VarianceY() const;
      
      /**
       * @brief Returns the covariance of the (x, y) pair
       * @return the square of the RMS of the y values
       * @throws std::range_error if the total weight is 0 (usually: no data)
       */
      Weight_t Covariance() const;
      
      /**
       * @brief Returns the standard deviation of the x sample
       * @return the RMS of the x values
       * @throws std::range_error if the total weight is 0 (see VarianceX())
       * @throws std::range_error if VarianceX() is negative (due to rounding errors)
       */
      Weight_t RMSx() const;
      
      /**
       * @brief Returns the standard deviation of the y sample
       * @return the RMS of the y values
       * @throws std::range_error if the total weight is 0 (see VarianceY())
       * @throws std::range_error if VarianceY() is negative (due to rounding errors)
       */
      Weight_t RMSy() const;
      
      
      /**
       * @brief Returns the linear correlation
       * @return the linear correlation RMS of the y values
       * @throws std::range_error if the total weight is 0 (see Covariance())
       * @throws std::range_error if any variance is non-positive
       */
      Weight_t LinearCorrelation() const;
      
      
      /**
       * @brief Returns the arithmetic average of the weights
       * @return the weight average
       * @throws std::range_error if no entry was added
       */
      Weight_t AverageWeight() const { return Base_t::AverageWeight(); }
      
      /// @}
      
        protected:
      using Variable_t = details::DataTracker2<Data_t, Weight_t>;
      
      Variable_t x; ///< accumulator for variable x
      Variable_t y; ///< accumulator for variable y
      Weight_t sum_xy = Weight_t(0); ///< weighted sum of xy
      
    }; // class StatCollector2D<>
    
    
    
    /** ************************************************************************
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
//***  details::WeightTracker<>
//***

template <typename W>
typename lar::util::details::WeightTracker<W>::Weight_t
  lar::util::details::WeightTracker<W>::AverageWeight() const
{
  if (N() == 0)
    throw std::range_error("WeightTracker<>::AverageWeight(): divide by 0");
  return Weights() / N();
} // details::WeightTracker<W>::AverageWeight()


//******************************************************************************
//***  StatCollector<>
//***

template <typename T, typename W>
void lar::util::StatCollector<T, W>::add
  (Data_t value, Weight_t weight /* = Weight_t(1.0) */)
{
  Base_t::add(weight);
  x.add(value, weight);
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
inline void lar::util::StatCollector<T, W>::clear() {
  Base_t::clear();
  x.clear();
} // StatCollector<T, W>::clear()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::Average() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector<>::Average(): divide by 0");
  return Sum() / Weights();
} // StatCollector<T, W>::Average()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::Variance() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector<>::Variance(): divide by 0");
  return std::max(Weight_t(0), (SumSq() - sqr(Sum()) / Weights()) / Weights());
} // StatCollector<T, W>::Variance()


template <typename T, typename W>
typename lar::util::StatCollector<T, W>::Weight_t
  lar::util::StatCollector<T, W>::RMS() const
{
  const Weight_t rms2 = Variance();
  if (rms2 < Weight_t(0))
    throw std::range_error("StatCollector<>::RMS(): negative RMS^2");
  return std::sqrt(rms2);
} // StatCollector<T, W>::RMS()



//******************************************************************************
//***  StatCollector2D<>
//***

template <typename T, typename W>
void lar::util::StatCollector2D<T, W>::add
  (Data_t x_value, Data_t y_value, Weight_t weight /* = Weight_t(1.0) */)
{
  Base_t::add(weight);
  x.add(x_value, weight);
  y.add(y_value, weight);
  sum_xy += weight * x_value * y_value;
} // StatCollector2D<T, W>::add()


template <typename T, typename W>
template <typename Iter, typename Pred>
void lar::util::StatCollector2D<T, W>::add_unweighted
  (Iter begin, Iter end, Pred extractor)
{
  std::for_each
    (begin, end, [this, extractor](auto item) { this->add(extractor(item)); });
} // StatCollector2D<T, W>::add_unweighted(Iter, Iter, Pred)


template <typename T, typename W>
template <typename VIter, typename WIter, typename VPred, typename WPred>
void lar::util::StatCollector2D<T, W>::add_weighted(
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
} // StatCollector2D<T, W>::add_weighted(VIter, VIter, WIter, VPred, WPred)


template <typename T, typename W>
template <typename Iter>
void lar::util::StatCollector2D<T, W>::add_weighted(Iter begin, Iter end) {
  
  std::for_each(begin, end, [this](auto p) { this->add(p); });
  
} // StatCollector2D<T, W>::add_weighted(Iter, Iter)



template <typename T, typename W>
inline void lar::util::StatCollector2D<T, W>::clear() {
  Base_t::clear();
  x.clear();
  y.clear();
  sum_xy = Weight_t(0);
} // StatCollector<T, W>::clear()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::AverageX() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector2D<>::AverageX(): divide by 0");
  return SumX() / Weights();
} // StatCollector2D<T, W>::AverageX()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::VarianceX() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector2D<>::VarianceX(): divide by 0");
  return (SumSqX() - sqr(SumX()) / Weights()) / Weights();
} // StatCollector2D<T, W>::VarianceX()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::RMSx() const
{
  const Weight_t rms2 = VarianceX();
  if (rms2 < Weight_t(0))
    throw std::range_error("StatCollector2D<>::RMSx(): negative RMS^2");
  return std::sqrt(rms2);
} // StatCollector2D<T, W>::RMSx()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::AverageY() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector2D<>::AverageY(): divide by 0");
  return SumY() / Weights();
} // StatCollector2D<T, W>::AverageY()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::VarianceY() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector2D<>::VarianceY(): divide by 0");
  return (SumSqY() - sqr(SumY()) / Weights()) / Weights();
} // StatCollector2D<T, W>::VarianceY()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::Covariance() const
{
  if (Weights() == Weight_t(0))
    throw std::range_error("StatCollector2D<>::Covariance(): divide by 0");
  return (SumXY() - SumX() * SumY() / Weights()) / Weights();
} // StatCollector2D<T, W>::VarianceY()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::RMSy() const
{
  const Weight_t rms2 = VarianceY();
  if (rms2 < Weight_t(0))
    throw std::range_error("StatCollector2D<>::RMSy(): negative RMS^2");
  return std::sqrt(rms2);
} // StatCollector2D<T, W>::RMSy()


template <typename T, typename W>
typename lar::util::StatCollector2D<T, W>::Weight_t
  lar::util::StatCollector2D<T, W>::LinearCorrelation() const
{
  if (Weights() == Data_t(0))
    throw std::range_error("StatCollector2D<>::LinearCorrelation(): divide by 0");
  
  const Weight_t var_prod = VarianceX() * VarianceY();
  if (var_prod <= Weight_t(0))
    throw std::range_error("StatCollector2D<>::LinearCorrelation(): variance is 0");
  
  return Covariance() / std::sqrt(var_prod);
} // StatCollector2D<T, W>::LinearCorrelation()



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
