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

// LArSoft libraries
#include "Utilities/StatCollector.h"


//------------------------------------------------------------------------------
//--- Test code
//

/**
 * @brief Tests StatCollector object with a known input
 * @tparam T type of the stat collector data
 */
template <typename T>
void StatCollectorTest() {
  
  using Data_t = T;
  
  lar::util::StatCollector<Data_t> stats;
  
  // check that everything is 0 or NaN-like
  BOOST_CHECK_EQUAL(stats.N(),       Data_t(0));
  BOOST_CHECK_EQUAL(stats.Weights(), Data_t(0));
  BOOST_CHECK_EQUAL(stats.Sum(),     Data_t(0));
  BOOST_CHECK_EQUAL(stats.SumSq(),   Data_t(0));
  BOOST_CHECK_THROW(stats.Average(), std::range_error);
  BOOST_CHECK_THROW(stats.RMS2(),    std::range_error);
  BOOST_CHECK_THROW(stats.RMS(),     std::range_error);
  
  // data set: { 5, 7, 7, 13 }
  stats.add(5);    // w =   1   sum =    5   sum2 =     25
  stats.add(7, 2); // w =   3   sum =   19   sum2 =    123
  stats.add(13);   // w =   4   sum =   32   sum2 =    292
  
  BOOST_CHECK_EQUAL(stats.N(),       Data_t(  3));
  BOOST_CHECK_EQUAL(stats.Weights(), Data_t(  4));
  BOOST_CHECK_EQUAL(stats.Sum(),     Data_t( 32));
  BOOST_CHECK_EQUAL(stats.SumSq(),   Data_t(292));
  BOOST_CHECK_EQUAL(stats.Average(), Data_t(  8));
  BOOST_CHECK_EQUAL(stats.RMS2(),    Data_t(  9));
  BOOST_CHECK_EQUAL(stats.RMS(),     Data_t(  3));
  
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
BOOST_AUTO_TEST_CASE(StatCollectorIntegerTest) {
  StatCollectorTest<int>();
}

BOOST_AUTO_TEST_CASE(StatCollectorRealTest) {
  StatCollectorTest<double>();
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

