/**
 * @file    StatCollector_test.cc
 * @brief   Tests the classes in StatCollector.h
 * @author  Gianluca Petrillo (petrillo@fnal.gov)
 * @date    20141229
 * @version 1.0
 * @see     StatCollector.h
 *
 * See http://www.boost.org/libs/test for the Boost test library home page.
 * 
 * Timing:
 * not given yet
 */

// C/C++ standard libraries
#include <array>
#include <valarray>
#include <utility> // std::pair<>
#include <memory> // std::unique_ptr<>
#include <initializer_list>

// Boost libraries
/*
 * Boost Magic: define the name of the module;
 * and do that before the inclusion of Boost unit test headers
 * because it will change what they provide.
 * Among the those, there is a main() function and some wrapping catching
 * unhandled exceptions and considering them test failures, and probably more.
 * This also makes fairly complicate to receive parameters from the command line
 * (for example, a random seed).
 */
#define BOOST_TEST_MODULE ( StatCollector_test )
#include <boost/test/auto_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()
#include <boost/test/floating_point_comparison.hpp> // BOOST_CHECK_CLOSE()

// LArSoft libraries
#include "Utilities/StatCollector.h"


//------------------------------------------------------------------------------
//--- Test code
//

template <typename T, typename W>
void CheckStats(
  lar::util::StatCollector<T, W> stats,
  int n,
  W weights,
  W sum,
  W sumsq,
  W rms // might as well compute it myself...
) {
  
  using Data_t = T;
  using Weight_t = W;
  
  BOOST_CHECK_EQUAL(stats.N(),       n);
  if (n == 0) {
    BOOST_CHECK_THROW(stats.AverageWeight(), std::range_error);
  }
  else {
    const Weight_t average = weights / n;
    BOOST_CHECK_CLOSE(double(stats.AverageWeight()), double(average), 0.1);
  }
  if (weights == 0.) {
    BOOST_CHECK_SMALL(double(stats.Weights()), 0.01);
    BOOST_CHECK_SMALL(double(stats.Sum()),     0.01);
    BOOST_CHECK_SMALL(double(stats.SumSq()),   0.01);
    BOOST_CHECK_THROW(stats.Average(), std::range_error);
    BOOST_CHECK_THROW(stats.RMS2(),    std::range_error);
    BOOST_CHECK_THROW(stats.RMS(),     std::range_error);
  }
  else {
    const Weight_t average = sum / weights;
    // check at precision 0.01% or 0.1%
    BOOST_CHECK_CLOSE(double(stats.Weights()), double(weights), 0.01);
    BOOST_CHECK_CLOSE(double(stats.Sum()),     double(sum),     0.01);
    BOOST_CHECK_CLOSE(double(stats.SumSq()),   double(sumsq),   0.01);
    BOOST_CHECK_CLOSE(double(stats.Average()), double(average), 0.1);
    BOOST_CHECK_CLOSE(double(stats.RMS2()),    double(rms*rms), 0.1);
    BOOST_CHECK_CLOSE(double(stats.RMS()),     double(rms),     0.1);
  }
} // CheckStats<>()


/**
 * @brief Tests StatCollector object with a known input
 * @tparam T type of the stat collector data
 */
template <typename T, typename W = T>
void StatCollectorTest() {
  
  using Data_t = T;
  using Weight_t = W;
  
  using WeightedData_t = std::vector<std::pair<Data_t, Weight_t>>;
  
  // prepare input data
  std::valarray<Data_t> unweighted_data{
    Data_t(5), Data_t(7), Data_t(7), Data_t(13)
    };
  WeightedData_t unweighted_data_weight({
    { Data_t(5), Weight_t(1) },
    { Data_t(7), Weight_t(1) },
    { Data_t(7), Weight_t(1) },
    { Data_t(13), Weight_t(1) }
    });
  int      uw_n       =            4;
  Weight_t uw_weights = Weight_t(  4.);
  Weight_t uw_sum     = Weight_t( 32.);
  Weight_t uw_sumsq   = Weight_t(292.);
  Weight_t uw_rms     = Weight_t(  3.);
  
  WeightedData_t weighted_data({
    { Data_t(5), Weight_t(1) },
    { Data_t(7), Weight_t(2) },
    { Data_t(13), Weight_t(1) }
    });
  int      w_n       =            3;
  Weight_t w_weights = Weight_t(  4.);
  Weight_t w_sum     = Weight_t( 32.);
  Weight_t w_sumsq   = Weight_t(292.);
  Weight_t w_rms     = Weight_t(  3.);
  
  //
  // part I: construction
  //
  lar::util::StatCollector<Data_t, Weight_t> stats;
  
  // check that everything is 0 or NaN-like
  CheckStats<Data_t, Weight_t>(stats, 0, 0., 0., 0., 0. /* should not be used */);
  
  //
  // part II: add elements one by one
  //
  // data set: { 5, 7, 7, 13 }
  stats.add(5);    // w =   1   sum =    5   sum2 =     25
  stats.add(7, 2); // w =   3   sum =   19   sum2 =    123
  stats.add(13);   // w =   4   sum =   32   sum2 =    292
  
  CheckStats<Data_t, Weight_t>(stats, w_n, w_weights, w_sum, w_sumsq, w_rms);
  
  
  //
  // part II: add unweighted elements by bulk
  //
  
  // - II.1: clear the statistics
  stats.clear();
  CheckStats<Data_t, Weight_t>(stats, 0, 0., 0., 0., 0. /* should not be used */);
  
  // - II.2: fill by iterators
  stats.add_unweighted(std::begin(unweighted_data), std::end(unweighted_data));
  CheckStats(stats, uw_n, uw_weights, uw_sum, uw_sumsq, uw_rms);
  
  // - II.3: fill by container
  stats.clear();
  stats.add_unweighted(unweighted_data);
  CheckStats(stats, uw_n, uw_weights, uw_sum, uw_sumsq, uw_rms);
  
  // - II.4: fill by iterators and extractor
  stats.clear();
  stats.add_unweighted(
    unweighted_data_weight.begin(), unweighted_data_weight.end(),
    [](std::pair<Data_t, Weight_t> d){ return d.first; }
    );
  CheckStats(stats, uw_n, uw_weights, uw_sum, uw_sumsq, uw_rms);
  
  // - II.5: fill by container and extractor
  stats.clear();
  stats.add_unweighted(unweighted_data_weight,
    [](std::pair<Data_t, Weight_t> d){ return d.first; }
    );
  CheckStats(stats, uw_n, uw_weights, uw_sum, uw_sumsq, uw_rms);
  
  
  //
  // part III: add weighted elements by bulk
  //
  
  // - II.1: fill by iterators
  stats.clear();
  stats.add_weighted(weighted_data.begin(), weighted_data.end());
  CheckStats(stats, w_n, w_weights, w_sum, w_sumsq, w_rms);
  
  // - II.2: fill by container
  stats.clear();
  stats.add_weighted(weighted_data);
  CheckStats(stats, w_n, w_weights, w_sum, w_sumsq, w_rms);
  
  
} // StatCollectorTest()


/**
 * @brief Tests MinMaxCollector object with a known input
 * 
 */
template <typename T>
void MinMaxCollectorTest() {
  
  using Data_t = T;
  
  std::initializer_list<Data_t> more_data{ 7, -20,  44, 78, 121 }; // [-20,121]
  std::array<Data_t, 5> even_more_data   { 7,  -2, 123, 78, 121 }; // [-2,123]
  
  // for easier notation
  std::unique_ptr<lar::util::MinMaxCollector<Data_t>> collector;
  
  //
  // 1. from default constructor
  //
  collector.reset(new lar::util::MinMaxCollector<Data_t>);
  
  // there should be no data now
  BOOST_CHECK(!collector->has_data());
  
  collector->add(Data_t(10));
  // there should be some data now
  BOOST_CHECK(collector->has_data());
  
  BOOST_CHECK_EQUAL(collector->min(), Data_t(  10));
  BOOST_CHECK_EQUAL(collector->max(), Data_t(  10));
  
  collector->add(more_data);
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -20));
  BOOST_CHECK_EQUAL(collector->max(), Data_t( 121));
  
  collector->add(even_more_data.begin(), even_more_data.end());
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -20));
  BOOST_CHECK_EQUAL(collector->max(), Data_t( 123));
  
  //
  // 2. from initializer list constructor
  //
  collector.reset(new lar::util::MinMaxCollector<Data_t>{ -25, 3, 1 });
  
  // there should be data already
  BOOST_CHECK(collector->has_data());
  
  collector->add(Data_t(10));
  // there should still be some data
  BOOST_CHECK(collector->has_data());
  
  BOOST_CHECK_EQUAL(collector->min(), Data_t(  -25));
  BOOST_CHECK_EQUAL(collector->max(), Data_t(  10));
  
  collector->add(more_data);
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -25));
  BOOST_CHECK_EQUAL(collector->max(), Data_t( 121));
  
  collector->add(even_more_data.begin(), even_more_data.end());
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -25));
  BOOST_CHECK_EQUAL(collector->max(), Data_t( 123));
  
  
  //
  // 3. from initializer list constructor
  //
  std::array<Data_t, 3> init_data{ -25, 3, 1 };
  collector.reset(
    new lar::util::MinMaxCollector<Data_t>(init_data.begin(), init_data.end())
    );
  
  // there should be data already
  BOOST_CHECK(collector->has_data());
  
  collector->add(Data_t(10));
  // there should still be some data
  BOOST_CHECK(collector->has_data());
  
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -25));
  BOOST_CHECK_EQUAL(collector->max(), Data_t(  10));
  
  collector->add(more_data);
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -25));
  BOOST_CHECK_EQUAL(collector->max(), Data_t( 121));
  
  collector->add(even_more_data.begin(), even_more_data.end());
  BOOST_CHECK_EQUAL(collector->min(), Data_t( -25));
  BOOST_CHECK_EQUAL(collector->max(), Data_t( 123));
  
} // MinMaxCollectorTest()


//------------------------------------------------------------------------------
//--- registration of tests
//
// Boost needs now to know which tests we want to run.
// Tests are "automatically" registered, hence the BOOST_AUTO_TEST_CASE()
// macro name. The argument is a name for the test; each test will have a
// number of checks and it will fail if any of them does.
//

//
// stat collector tests
//
BOOST_AUTO_TEST_CASE(StatCollectorPureIntegerTest) {
  StatCollectorTest<int, int>();
}

BOOST_AUTO_TEST_CASE(StatCollectorIntegerTest) {
  StatCollectorTest<int, float>();
}

BOOST_AUTO_TEST_CASE(StatCollectorIntegerWeightsTest) {
  StatCollectorTest<float, int>();
}

BOOST_AUTO_TEST_CASE(StatCollectorRealTest) {
  StatCollectorTest<double, double>();
}


//
// Minimum/maximum collector tests
//
BOOST_AUTO_TEST_CASE(MinMaxCollectorIntegerTest) {
  MinMaxCollectorTest<int>();
}

BOOST_AUTO_TEST_CASE(MinMaxCollectorRealTest) {
  MinMaxCollectorTest<double>();
}

