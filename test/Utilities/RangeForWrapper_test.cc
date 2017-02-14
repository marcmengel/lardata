/**
 * @file   RangeForWrapper_test.cc
 * @brief  Test for RangeForWrapper utilities
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   December 29, 2016
 * @see    RangeForWrapper.h
 * 
 * The test is run with no arguments.
 * 
 */

// LArSoft libraries
#include "lardata/Utilities/RangeForWrapper.h"

// Boost libraries
#define BOOST_TEST_MODULE ( RangeForWrapper_test )
#include <cetlib/quiet_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()

// C/C++ standard libraries
#include <iterator>
#include <algorithm> // std::accumulate()
#include <vector>
#include <type_traits> // std::is_same<>, std::is_lvalue_reference<>, ...
#include <initializer_list>
#include <iostream>

//------------------------------------------------------------------------------
//--- make up a class with begin and end iterators with different types
//--- (and provide it with begin() and end() free functions)
//--- 
template <typename Value>
struct base_iterator {
  using value_type = Value;
  
  value_type* ptr = nullptr;
  
  base_iterator(value_type* ptr = nullptr): ptr(ptr) {}
  
  value_type& operator* () const { return *ptr; }
  value_type* operator-> () const { return ptr; }
  
  base_iterator& operator++ () { ++ptr; return *this; }
};

template <typename Value>
struct begin_iterator_base: public base_iterator<Value> {
  using base_iterator<Value>::base_iterator;
  begin_iterator_base<Value>& operator++ ()
    { base_iterator<Value>::operator++(); return *this; }
};

template <typename Value>
struct end_iterator_base: public base_iterator<Value> {
  using base_iterator<Value>::base_iterator;
  end_iterator_base<Value>& operator++ () = delete;
};

template <typename ValueL, typename ValueR>
bool operator!=(base_iterator<ValueL> const& a, base_iterator<ValueR> const& b)
  { return a.ptr != b.ptr; }


template <typename Value>
struct Data {
  using value_type = Value;
  
  std::vector<value_type> data;
  
  using begin_iterator = begin_iterator_base<value_type>;
  using end_iterator = end_iterator_base<value_type>;
  using begin_const_iterator = begin_iterator_base<value_type const>;
  using end_const_iterator = end_iterator_base<value_type const>;
  
  Data(std::initializer_list<value_type> const& data): data(data) {}
  
  begin_const_iterator do_begin() const { return { &*data.cbegin() }; }
  begin_iterator do_begin() { return { &*data.begin() }; }
  end_const_iterator do_end() const { return { &*data.cend() }; }
  end_iterator do_end() { return { &*data.end() }; }
}; // class Data

template <typename Value>
auto begin(Data<Value> const& data) { return data.do_begin(); }

template <typename Value>
auto end(Data<Value> const& data) { return data.do_end(); }

template <typename Value>
auto begin(Data<Value>&& data) { return data.do_begin(); }

template <typename Value>
auto end(Data<Value>&& data) { return data.do_end(); }

template <typename Value>
auto begin(Data<Value>& data) { return data.do_begin(); }

template <typename Value>
auto end(Data<Value>& data) { return data.do_end(); }

template <typename T>
T copy(T const& v) { return v; }


//------------------------------------------------------------------------------

template <typename DataColl>
void const_test
  (DataColl const& data, typename DataColl::value_type expected_total)
{
  using value_type = typename DataColl::value_type;

  static_assert(
    !std::is_lvalue_reference
      <decltype(copy(data) | util::range_for)>::value,
    "util::range_for on a rvalue should return a rvalue"
    );

//  for (double& d: data); // this should fail compilation
  
  value_type total = 0;
  for (value_type d: data | util::range_for) total += d;
  
  BOOST_CHECK_EQUAL(total, expected_total);
  
  // from a temporary
  total = 0;
  for (value_type d: copy(data) | util::range_for) total += d;
  
  BOOST_CHECK_EQUAL(total, expected_total);
  
} // const_test()


template <typename DataColl>
void test(DataColl& data, typename DataColl::value_type expected_total) {
  using value_type = typename DataColl::value_type;
  
  static_assert(
    !std::is_lvalue_reference
      <decltype(copy(data) | util::range_for)>::value,
    "util::range_for on a rvalue should return a rvalue"
    );

  //
  // from a lvalue
  //
  value_type total = 0;
  for (value_type d: data | util::range_for) total += d;
  
  BOOST_CHECK_EQUAL(total, expected_total);
  
  //
  // from a rvalue (temporary)
  //
  total = 0;
  for (value_type d: copy(data) | util::range_for) total += d;
  
  BOOST_CHECK_EQUAL(total, expected_total);
  
  //
  // from a temporary, which is changed
  //
  total = 0;
  for (value_type& d: copy(data) | util::range_for) total += (d *= 3);
  
  BOOST_CHECK_EQUAL(total, 3 * expected_total);
  
  // original value is still unchanged
  total = 0;
  for (value_type d: data | util::range_for) total += d;
  
  BOOST_CHECK_EQUAL(total, expected_total);
  
  //
  // from a lvalue, which is changed
  //
  for (value_type& d: data | util::range_for) d *= 3;
  
  total = 0;
  for (value_type d: data | util::range_for) total += d;
  
  BOOST_CHECK_EQUAL(total, 3 * expected_total);
  
} // test()


BOOST_AUTO_TEST_CASE(RangeForWrapperSameIterator_test) {
  
  using value_type = int;
  std::vector<value_type> vdata = { 2, 3, 4 };
  
  // static checks
  static_assert(std::is_same<
      std::decay_t<decltype(vdata)>,
      std::decay_t<decltype(vdata | util::range_for)>
    >::value,
    "util::range_for should be pass-through!"
    );
  static_assert(
    std::is_lvalue_reference<decltype(vdata | util::range_for)>::value,
    "Pass-through on a lvalue should return a lvalue reference"
    );

  static_assert(
    !std::is_lvalue_reference
      <decltype(copy(vdata) | util::range_for)>::value,
    "Pass-through on a rvalue should return a rvalue (or its reference)"
    );
  
  BOOST_CHECK_EQUAL(&vdata, &(vdata | util::range_for));
  
  auto expected_total = std::accumulate(vdata.begin(), vdata.end(), 0);
  
  const_test(vdata, expected_total);
  
  test(vdata, expected_total);
  
} // BOOST_AUTO_TEST_CASE(RangeForWrapperSameIterator_test_test)


BOOST_AUTO_TEST_CASE(RangeForWrapperDiffIterator_test) {
  
  using value_type = int;
  std::vector<value_type> vdata = { 2, 3, 4 };
  
  Data<value_type> data = { 2, 3, 4 };
  
  auto expected_total = std::accumulate(vdata.begin(), vdata.end(), 0);
  
  // static check
  static_assert(!std::is_same<
      std::decay_t<decltype(data)>,
      std::decay_t<decltype(data | util::range_for)>
    >::value,
    "util::range_for should generate a wrapper!"
    );
  
//  for (double d: data); // this should fail compilation
  
  const_test(data, expected_total);

  test(data, expected_total);
  
} // BOOST_AUTO_TEST_CASE(RangeForWrapperDiffIterator_test_test)
